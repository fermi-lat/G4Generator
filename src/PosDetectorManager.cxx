// $Header: /nfs/slac/g/glast/ground/cvs/G4Generator/src/PosDetectorManager.cxx,v 1.4 2002/03/12 17:25:20 riccardo Exp $

#include "PosDetectorManager.h"
#include <iostream>
#include "GlastEvent/MonteCarlo/McPositionHit.h"
#include "idents/VolumeIdentifier.h"

// Geant4 interface
#include "G4Step.hh"
#include "G4VSolid.hh"
#include "G4Box.hh"
#include "G4SDManager.hh"

#include <algorithm>



PosDetectorManager::PosDetectorManager(DetectorConstruction *det,
                                           IDataProviderSvc* esv)
:DetectorManager(det->idMap(), esv,"PositionDetectorManager")
{
}

void PosDetectorManager::Initialize(G4HCofThisEvent*HCE)
{
  m_detectorList.clear();
  // At the start of the event we create a new container
  m_posHit = new McPositionHitVector;    
}

G4bool PosDetectorManager::ProcessHits(G4Step* aStep,G4TouchableHistory* ROhist)
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
    
    HepPoint3D prePos = aStep->GetPreStepPoint()->GetPosition();
    HepPoint3D postPos = aStep->GetPostStepPoint()->GetPosition();
    
    // determine the ID by studying the history, then call appropriate 
    idents::VolumeIdentifier id = constructId(aStep);

    // Filling of the hits container
    mc::McPositionHit *hit = new mc::McPositionHit;

#if 0 // this transforms it to local coordinates
    
    HepTransform3D 
        global(*(theTouchable->GetRotation()), 
        theTouchable->GetTranslation());

    HepTransform3D local = global.inverse();
    prePos= local*prePos;
    postPos=local*postPos;
#endif
    hit->init(edep, id, prePos, postPos);
    m_posHit->push_back(hit);

    display(theTouchable, hit);


    return true;
    
}

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
void PosDetectorManager::EndOfEvent(G4HCofThisEvent* HCE)
{
    // Let's sort the hits
    std::sort(m_posHit->begin(),m_posHit->end(), ComparePosHits());
    // store the hits in the TDS
    m_esv->registerObject("/Event/MC/PositionHitsCol", m_posHit);    

    std::cout << "Actual Event done! " << m_posHit->size() 
        << " position hits stored in the TDS" << std::endl;

}

