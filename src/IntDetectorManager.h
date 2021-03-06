#ifndef INTEGRATINGDETECTORMANAGER_H
#define INTEGRATINGDETECTORMANAGER_H

#include "idents/VolumeIdentifier.h"


#include "Event/MonteCarlo/McIntegratingHit.h"

#include "DetectorManager.h"
#include <map>

class McParticle;
class DetectorConstruction;
class IDataProviderSvc;
class IGlastDetSvc;
/** 
 * @class IntDetectorManager
 *
 * @brief A concrete DetectorManager
 *
 * This class implement the abstract DetectorManager; this is used for all the
 * detector sensitive volume that need to register hits as McIntegratingHits.
 * 
 * @author T.Burnett and R.Giannitrapani
 *    
 * $Header: /nfs/slac/g/glast/ground/cvs/G4Generator/src/IntDetectorManager.h,v 1.9 2003/07/04 22:22:53 burnett Exp $
 */
class IntDetectorManager : public DetectorManager {
public:
    
  /// @param det the DetectorConstruction pointer to retrive the map of volume
  ///        ids for all sensitive detectors 
  /// @param esv the data provider service for TDS access 
  /// @param gsv the interface to the IGlastDetSvc service
  IntDetectorManager( DetectorConstruction* det, IDataProviderSvc* esv, IGlastDetSvc* gsv);
  
  /// Clears things; this implement a pure abstract method in the
  /// hierarchy ancestor of this class (geant4 name convention)
  virtual void Initialize(G4HCofThisEvent*);
  
  /// Called by G4 in each step in a sensitive volume; this implement a pure
  /// abstract method in the hierarchy ancestors of this class (geant4 name
  /// convention)
  virtual G4bool ProcessHits(G4Step*, G4TouchableHistory*);
  
  /// End of event will finish hits retrival; this implement a pure abstract
  /// method in the hierarchy ancestors of this class (geant4 name convention)
  virtual void EndOfEvent(G4HCofThisEvent*);
  
 private:
  /// The collection of McIntegratingHit to be saved in the TDS
  Event::McIntegratingHitVector *m_intHit;  
  
  /// A map of McIntegratingHit indicized by volume id to pile up energy
  /// deposited
  std::map<idents::VolumeIdentifier,Event::McIntegratingHit*> m_detectorList;

  /// Indices of refraction for surfaces
  double m_diodeGlueIndex;
  double m_crystalIndex;

  /// keep the cosine of the critical angle
  double m_cosCritical;

};
#endif
