// File and Version Information:
// $Header: /nfs/slac/g/glast/ground/cvs/G4Generator/src/G4PropagatorSvc.cxx,v 1.5 2002/05/02 12:22:23 burnett Exp $
//
// Description: Service for particle transport management
//
// Author(s):
//      T.Usher

#include "GlastSvc/Reco/IPropagatorSvc.h"
#include "G4ParticlePropagator.h"

#include "GaudiKernel/Service.h"
#include "GaudiKernel/DataSvc.h"

#include "RunManager.h"
#include "DetectorConstruction.h"
#include "G4VUserDetectorConstruction.hh"

#include <cassert>

/** 
 * @class G4PropagatorSvc
 *
 * @brief A Gaudi Service for interfacing to the G4ParticlePropagator
 *
 * G4PropagatorSvc provides an interface to the world outside the G4Generator
 * package which allows use of the G4ParticlePropagator for transporting tracks
 * and their error matrices throughout the GLAST detector.  The current
 * interface is taken from that used by RecoSvc, the Gismo equivalent 
 *
 * @author Tracy Usher
 *
 */
//class G4PropagatorSvc : virtual public DataSvc, virtual public IPropagatorSvc 
class G4PropagatorSvc : public IPropagatorSvc, public Service
{
 public: 
    
  /// Return a pointer to the G4ParticlePropagator singleton object
  virtual IKalmanParticle* getPropagator() {
    return G4ParticlePropagator::instance();}

  virtual StatusCode initialize();

  virtual StatusCode finalize();
        
  /// queryInterface - for implementing a Service this is necessary
  virtual StatusCode queryInterface(const IID& riid, void** ppvUnknown);

  /// This is needed by propagator for decoding volume identifiers
  static DetectorConstruction* UserDetector;
  static DetectorConstruction::IdMap* idmap;

 protected:

  G4PropagatorSvc(const std::string& name, ISvcLocator* pSvcLocator);

  virtual ~G4PropagatorSvc();
  
 private:
  // Allow SvcFactory to instantiate the service.
  friend class SvcFactory<G4PropagatorSvc>;

  //This is a pointer to the all important volume->idents map
  //obtained from the RunManager singleton
  //DetectorConstruction::IdMap* pIdMap;
};
