#ifndef G4Generator_h
#define G4Generator_h

#include "GaudiKernel/Algorithm.h"

#include <string>
#include <vector>
class IFluxSvc;
class IFlux;
class IParticlePropertySvc;
class RunManager;

/** 
 * @class G4Generator
 *
 * @brief Gaudi algorithm for GLAST simulation
 *
 * This is the main Gaudi algorithm to use the Geant4 toolkit as a Montecarlo
 * propagator
 *  
 * @author T.Burnett and R.Giannitrapani
 *    
 * $Header: /nfs/slac/g/glast/ground/cvs/G4Generator/src/G4Generator.h,v 1.9 2002/04/19 03:56:30 burnett Exp $
 */
class G4Generator : public Algorithm {
 public:
  G4Generator(const std::string& name, ISvcLocator* pSvcLocator); 
  StatusCode initialize();
  StatusCode execute();
  StatusCode finalize();
 
  
 private:
  /// a pointer to the flux service main classes
  IFluxSvc* m_fluxSvc;
  IFlux*    m_flux;

  /// a pointer to the service for particle properties
  IParticlePropertySvc* m_ppsvc;

  /// source name to get from the Flux service; this is a property of the
  /// algorithm and can be setted in the jobOptions file
  std::string m_source_name;

  /// set of UI commands for setup; this is a property of the algorithm and can
  /// be setted in the jobOptions file
  StringArrayProperty m_uiCommands;
  
  /// This is the G4 manager that handles the simulation
  RunManager* m_runManager;

  /// internal routine to set up the (optional) gui stuff  
  void setupGui();

  /// the geometry level of details
  std::string m_geometryMode;
};

#endif




