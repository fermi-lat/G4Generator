// $Header: /nfs/slac/g/glast/ground/cvs/G4Generator/src/G4Generator.cxx,v 1.20 2002/04/08 14:59:07 riccardo Exp $

// Include files

// Geant4
#include "G4Generator.h"
#include "G4UImanager.hh"
#include "G4ParticleDefinition.hh"

#include "RunManager.h"
#include "PrimaryGeneratorAction.h"
#include "McParticleManager.h"

// Gaudi
#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/AlgFactory.h"
#include "GaudiKernel/IDataProviderSvc.h"
#include "GaudiKernel/Algorithm.h"
#include "GaudiKernel/IAlgManager.h"
#include "GaudiKernel/ISvcLocator.h"
#include "GaudiKernel/SmartDataPtr.h"

// special to setup the TdGlastData structure
#include "GlastSvc/GlastDetSvc/IGlastDetSvc.h"

#include "GlastEvent/MonteCarlo/McPositionHit.h"
#include "GlastEvent/MonteCarlo/McParticle.h"

//flux
#include "FluxSvc/IFluxSvc.h"
#include "FluxSvc/IFlux.h"


//gui
#include "GuiSvc/GuiSvc.h"
#include "gui/DisplayControl.h"
#include "gui/GuiMgr.h"
#include "gui/SimpleCommand.h"
#include "DisplayManager.h"

#include "CLHEP/Geometry/Point3D.h"
#include "CLHEP/Geometry/Vector3D.h"

static const AlgFactory<G4Generator>  Factory;
const IAlgFactory& G4GeneratorFactory = Factory;

G4Generator::G4Generator(const std::string& name, ISvcLocator* pSvcLocator) 
:Algorithm(name, pSvcLocator) 
{
// set defined properties
     declareProperty("source_name",  m_source_name="default");
     declareProperty("UIcommands", m_UIcommands);
}
    
////////////////////////////////////////////////////////////////////////////
StatusCode G4Generator::initialize()
{
    MsgStream log(msgSvc(), name());
    log << MSG::INFO << "initialize" << endreq;

    // Use the Job options service to set the Algorithm's parameters
    setProperties();

    if ( service("FluxSvc", m_fluxSvc).isFailure() ){
        log << MSG::ERROR << "Couldn't find the FluxSvc!" << endreq;
        return StatusCode::FAILURE;
    }
    
    if ( m_fluxSvc->source(m_source_name, m_flux).isFailure() ){
        log << MSG::ERROR << "Couldn't find the source \"" 
            << m_source_name << "\"" << endreq;
        return StatusCode::FAILURE;
    }
    log << MSG::INFO << "Source: "<< m_flux->title() << endreq;

    setupGui();

    if( !m_UIcommands.value().empty() ) {
        G4UImanager* UI = G4UImanager::GetUIpointer();
        for( std::vector<std::string>::const_iterator k = m_UIcommands.value().begin(); 
            k!=m_UIcommands.value().end(); ++k){
            UI->ApplyCommand(*k);
            log << MSG::INFO << "UI command: " << (*k) << endreq;
        }
    }  

    // Get the Glast detector service 
    IGlastDetSvc* gsv=0;
    if( service( "GlastDetSvc", gsv).isFailure() ) {
        log << MSG::ERROR << "Couldn't set up GlastDetSvc!" << endreq;
        return StatusCode::FAILURE;
    }

    // Init the McParticle hierarchy 
    McParticleManager::getPointer()->initialize(eventSvc());


    // The geant4 manager
    if (!(m_runManager = RunManager::GetRunManager()))
    {
        m_runManager = new RunManager(gsv,eventSvc());

        // Initialize Geant4
        m_runManager->Initialize();

        log << MSG::DEBUG << "G4 RunManager ready" << endreq;
    }

    // Initialize Geant4
    m_runManager->Initialize();

    log << MSG::DEBUG << "G4 RunManager ready" << endreq;
    return StatusCode::SUCCESS;

}

//------------------------------------------------------------------------------
void G4Generator::setupGui()
{
    //
    // get the (optional) Gui service
    //
    MsgStream log(msgSvc(), name());

    IGuiSvc* guiSvc=0;
    
    if ( service("GuiSvc", guiSvc).isFailure() ){
        log << MSG::WARNING << "No GuiSvc: so, no event display " << endreq;
        return;
    } 
    m_guiMgr= guiSvc->guiMgr();
    new DisplayManager(&m_guiMgr->display());


    // now get the filemenu and add a source button to it.
    gui::SubMenu& filemenu = m_guiMgr->menu().file_menu();
    
    gui::SubMenu& source_menu = filemenu.subMenu("Set source");
    
    //   ----------------------------------
    //  loop over sources in source_library
    class SetSource : public gui::Command { public:
    SetSource(G4Generator* gg, std::string sn):m_gg(gg),m_name(sn){}
    void execute(){m_gg->setSource(m_name);}
    std::string m_name;
    G4Generator* m_gg;
    };
    std::list<std::string> names = m_fluxSvc->fluxNames();
    for( std::list<std::string>::iterator it = names.begin(); it!=names.end(); ++it){
        source_menu.addButton(*it, new SetSource(this, *it));
    }
    
}
//------------------------------------------------------------------------------
StatusCode G4Generator::execute() 
{
    MsgStream   log( msgSvc(), name() );

    // Here the TDS is prepared to receive hits vectors
    // Check for the MC branch - it will be created if it is not available
    DataObject *mc;
    eventSvc()->retrieveObject("/Event/MC", mc);

    log << MSG::DEBUG << "TDS ready" << endreq;

    // Clean the McParticle hierarchy 
    McParticleManager::getPointer()->clear();

    // following model from previous version, allow property "UIcommands" to generate
    // UI commands here. 
    //
    if( !m_UIcommands.value().empty() ) {
        for( std::vector<std::string>::const_iterator k = m_UIcommands.value().begin(); 
            k!=m_UIcommands.value().end(); ++k){
            G4UImanager::GetUIpointer()->ApplyCommand(*k);
            log << MSG::INFO << "Apply UI command: \"" << (*k) << "\"" <<endreq;
        }
    }  

    //
    // have the flux service create parameters of an incoming particle, 
    // and define it as a MCParticle
    //
    m_flux->generate();

    // these are the particle properties
    std::string name(m_flux->particleName());
    HepVector3D dir(m_flux->launchDir());
    double ke= m_flux->energy() ;
    HepPoint3D p(m_flux->launchPoint());

    /// Starting position, in mm
    p = 10*p;
    /// Energy in MeV
    ke = ke*1000;
    
    PrimaryGeneratorAction* primaryGenerator = 
      (PrimaryGeneratorAction*)m_runManager->GetUserPrimaryGeneratorAction();
    
    // Set the G4 primary generator
    // the position has to be expressed in mm
    // while the energy in MeV
    primaryGenerator->setParticle(name);
    primaryGenerator->setMomentum(dir);
    primaryGenerator->setPosition(p);
    primaryGenerator->setEnergy(ke);  // TODO: this shoule be full energy, but don't know mass yet

    //
    // create entry in McParticleCol for the primary generator.
    //
    G4ParticleDefinition * pdef = primaryGenerator->GetParticleDefinition();
    HepLorentzVector pin= primaryGenerator->GetFourMomentum();

#if 0
    mc::McParticleCol* pcol = new mc::McParticleCol;
    eventSvc()->registerObject("/Event/MC/McParticleCol", pcol);
    mc::McParticle * parent= new mc::McParticle;
    pcol->push_back(parent);

    // This parent particle decay at the start in the first particle, 
    // so initial momentum and final one are the same
    parent->initialize(parent, pdef->GetPDGEncoding(), 
        mc::McParticle::PRIMARY,
        pin);
    parent->finalize(pin, p);
#else

    mc::McParticle * parent= new mc::McParticle;
    // This parent particle decay at the start in the first particle, 
    // so initial momentum and final one are the same
    parent->initialize(parent, pdef->GetPDGEncoding(), 
        mc::McParticle::PRIMARY,
        pin);
    parent->finalize(pin, p);
    
    McParticleManager::getPointer()->addMcParticle(0,parent);
    
#endif    

    // Run geant4
    m_runManager->BeamOn(); 
    
    // Save the McParticle hierarchy in the TDS
    McParticleManager::getPointer()->save();
    
    // set up display of trajectories
    DisplayManager* dm = DisplayManager::instance();
    for( int i = 0; i< m_runManager->getNumberOfTrajectories(); ++i){
        std::auto_ptr<std::vector<Hep3Vector> > points = m_runManager->getTrajectoryPoints(i);
        dm->addTrack(*(points.get()), m_runManager->getTrajectoryCharge(i));
    }

    return StatusCode::SUCCESS;
}

//------------------------------------------------------------------------------
StatusCode G4Generator::finalize() 
{
    MsgStream log(msgSvc(), name());
    log << MSG::INFO << "finalize: " << endreq;

    // delete the runManager of geant4
    delete m_runManager;
    
    return StatusCode::SUCCESS;
}

//------------------------------------------------------------------------------
// called from GUI
void G4Generator::setSource(std::string source_name)
{
    m_source_name=source_name;
    StatusCode sc = m_fluxSvc->source(m_source_name, m_flux);
    if( sc.isFailure() ) {
        gui::GUI::instance()->inform("Could not find the source!");
    }
}



