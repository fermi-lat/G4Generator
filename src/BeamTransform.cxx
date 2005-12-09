/** @file BeamTransform.cxx
    @brief declartion, implementaion of the class BeamTransform

    $Header: /nfs/slac/g/glast/ground/cvs/G4Generator/src/BeamTransform.cxx,v 1.3 2005/12/09 05:35:44 burnett Exp $
*/
// Gaudi system includes
#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/AlgFactory.h"
#include "GaudiKernel/SmartDataPtr.h"
#include "GaudiKernel/Algorithm.h"

// CLHEP for geometry
#include "CLHEP/Vector/Rotation.h"
#include "CLHEP/Vector/ThreeVector.h"


// TDS class declarations: input data, and McParticle tree
#include "Event/MonteCarlo/McParticle.h"
#include "Event/TopLevel/Event.h"
#include "Event/TopLevel/EventModel.h"
#include "Event/TopLevel/MCEvent.h"


#include <cassert>
#include <sstream>

/** @class BeamTransform
@brief alg to transform the beam particle(s)

The requirements are:

- One degree of freedom, the rotation of the scanning table around some z-axis, 
  probably specified as plus and minus degrees,

- Cordinates in the beam frame: The x-coordinate of the point of rotation of the scanning table, and
The x-coordinate of the z=0 plane of the CU, for the table in the unrotated orientation.

These are needed to specify the position and direction of the incoming particles in the CU frame.

For concreteness, and until we know better, default the point of rotation of the table to x = 4280 
(1 meter downstream from the reporting plane), and the z=0 point of the CU to x = 4090 (z position of the center of the CU).

More wisdom from Leon:

the point of rotation won't necessarily be at  z  = 0
and we will have to adjust the distance between the reporting plane (x in the beam frame) and the CU.
I don't have any real  number yet, but I'm assuming that the CU will be somewhat behind the reporting plane... so say we set the x-beam of the z-CU at 4280, one meter downstream of the reporting plane.
the point of rotation, I have no idea, but it should not be at the z=0 of the CU, so we can see if it's handled correctly, so maybe 150 mm upstream of the z=0 point?

so the z=0 plane of the cu on an unrotated table should be at xbeam = 4280, and the rotation point should be at xbeam = 4130.
*/
class BeamTransform : public Algorithm {
public:
    BeamTransform(const std::string& name, ISvcLocator* pSvcLocator);
    /// set parameters and attach to various perhaps useful services.
    StatusCode initialize();
    /// process one event
    StatusCode execute();
    /// clean up
    StatusCode finalize();
    
private: 
    int m_count;
    DoubleProperty  m_transy, m_transz; // move in plane
    DoubleProperty  m_angle; // rotation about instrument y-axis (deg)
    double m_c, m_s;       // cos, sin of rot

    void transform(Event::McParticle& mcp );
    Hep3Vector m_translation;
    HepRotationY m_rot;  // the table rotation
};


static const AlgFactory<BeamTransform>  Factory;
const IAlgFactory& BeamTransformFactory = Factory;

BeamTransform::BeamTransform(const std::string& name, ISvcLocator* pSvcLocator)
:Algorithm(name, pSvcLocator)
,m_count(0)
{
    // declare properties with setProperties calls
    declareProperty("vertical_translation",  m_transy=0);
    declareProperty("horizontal_translation",  m_transz=0);
    declareProperty("table_rotation", m_angle=0);

    
}

StatusCode BeamTransform::initialize(){
    StatusCode  sc = StatusCode::SUCCESS;
    MsgStream log(msgSvc(), name());
    log << MSG::INFO << "initialize" << endreq;
    
    // Use the Job options service to set the Algorithm's parameters
    setProperties();
    
    // define the transformation matrix here.
    m_translation = Hep3Vector(0, m_transy, m_transz);

    // this is the rotation matrix
    m_rot = HepRotationY(m_angle*M_PI/180);

    return sc;
}
void BeamTransform::transform(Event::McParticle& mcp )
{

    // special treatment if it is a mother (code from Leon)
    if(&mcp.mother()!=&mcp) {
        const_cast<Event::McParticle*>(&mcp.mother())->removeDaughter(&mcp);
    }

    // get the initial position and momentum
    HepLorentzVector pbeam = mcp.initialFourMomentum();
    Hep3Vector rbeam = mcp.initialPosition();

    // translate in beam frame
    rbeam += m_translation;

    // convert to unrotated instrument coordinates
    Hep3Vector r(rbeam.z(), rbeam.y(), rbeam.x());
    HepLorentzVector p(-pbeam.z(), pbeam.y(), -pbeam.x(), pbeam.e());

    mcp.initialize(const_cast<Event::McParticle*>( &mcp.mother()), 
        mcp.particleProperty(), mcp.statusFlags(),
        m_rot*p, m_rot*r,
        mcp.getProcess());

}

StatusCode BeamTransform::execute(){
    StatusCode  sc = StatusCode::SUCCESS;
    MsgStream   log( msgSvc(), name() );
    ++m_count;

    // Retrieving pointers from the TDS
    
    SmartDataPtr<Event::EventHeader>   header(eventSvc(),    EventModel::EventHeader);
    SmartDataPtr<Event::MCEvent>     mcheader(eventSvc(),    EventModel::MC::Event);
    SmartDataPtr<Event::McParticleCol> particles(eventSvc(), EventModel::MC::McParticleCol);

    double t = header->time();
    log << MSG::DEBUG << "Event time: " << t << endreq;;
  
    // loop over the monte carlo particle collection
    if (particles) {
        
        Event::McParticleCol::iterator piter;
        
        for (piter = particles->begin(); piter != particles->end(); piter++) {
            Event::McParticle& mcp = **piter;
            
            log << MSG::DEBUG ; log.stream() << mcp; log << endreq;
            
            // translate position
            transform(mcp);

            log << MSG::DEBUG << "after transform: "; log.stream() << mcp; log << endreq;
        }
    }

    return sc;
}

StatusCode BeamTransform::finalize(){
    StatusCode  sc = StatusCode::SUCCESS;
    MsgStream log(msgSvc(), name());
    log << MSG::INFO << "processed " << m_count << " events." << endreq;
    
    return sc;
}

