// $Header: /nfs/slac/g/glast/ground/cvs/G4Generator/src/IntDetectorManager.cxx,v 1.4 2002/03/15 15:26:16 burnett Exp $

#include "IntDetectorManager.h"
#include <iostream>
#include "GlastEvent/MonteCarlo/McIntegratingHit.h"
#include "idents/VolumeIdentifier.h"

#include "CLHEP/Geometry/Transform3D.h"
#include "CLHEP/Vector/ThreeVector.h"

// Geant4 interface
#include "G4Step.hh"
#include "G4VSolid.hh"
#include "G4Box.hh"
#include "G4SDManager.hh"

#include <algorithm>

IntDetectorManager::IntDetectorManager(DetectorConstruction *det,
                                           IDataProviderSvc* esv)
:DetectorManager(det->idMap(), esv,"IntegratingDetectorManager")
{
}

void IntDetectorManager::Initialize(G4HCofThisEvent*HCE)
{
  m_detectorList.clear();
  // At the start of the event we create a new container
  m_intHit = new McIntegratingHitVector;    
}

G4bool IntDetectorManager::ProcessHits(G4Step* aStep,G4TouchableHistory* ROhist)
{
    
    // Energy Deposition & Step Length
    
    G4double edep = aStep->GetTotalEnergyDeposit()/MeV;
    G4double stepl = aStep->GetStepLength()/mm;
    
    if ((edep==0.)) return false;          
    // Physical Volume
    
    G4TouchableHistory* theTouchable
        = (G4TouchableHistory*)(aStep->GetPreStepPoint()->GetTouchable());
    G4VPhysicalVolume* physVol = theTouchable->GetVolume(); 
    G4LogicalVolume* logVol = physVol->GetLogicalVolume();
    G4String material = logVol->GetMaterial()->GetName();
    G4String nameVolume = physVol->GetName();
    
    G4ThreeVector prePos = aStep->GetPreStepPoint()->GetPosition();
    G4ThreeVector postPos = aStep->GetPostStepPoint()->GetPosition();
    
    // determine the ID by studying the history, then call appropriate 
    idents::VolumeIdentifier id = constructId(aStep);

    // We fill an integrating hit
    mc::McIntegratingHit *hit; 
    // If the hit has already been created we use it, otherwise we
    // create a new one
    if( !(hit = m_detectorList[id]))
      {
        // This draw the volume
        makeIntegratingBox( theTouchable );        
        // Filling of the hits container
    	hit = new mc::McIntegratingHit;
        hit->setVolumeID(id);
        m_intHit->push_back(hit);
    	m_detectorList[id] = hit;
      }

#if 1 // this transforms it to local coordinates
    HepTransform3D 
        global(*(theTouchable->GetRotation()), 
        theTouchable->GetTranslation());

    HepTransform3D local = global.inverse();
    prePos = local * (HepVector3D)prePos;
    postPos = local * (HepVector3D)postPos;
#endif

    // fill the energy and position    
    hit->addEnergyItem(edep,0,(prePos+postPos)/2);

    return true;
    
}

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
void IntDetectorManager::EndOfEvent(G4HCofThisEvent* HCE)
{
    // Let's sort the hits
    std::sort(m_intHit->begin(),m_intHit->end(), CompareIntHits());

    // store the hits in the TDS
    m_esv->registerObject("/Event/MC/IntegratingHitsCol", m_intHit);    

    std::cout << "Actual Event done! " << m_intHit->size() 
        << " integrating hits stored in the TDS" << std::endl;

}

