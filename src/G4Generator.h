#ifndef G4Generator_h
#define G4Generator_h

#include "GaudiKernel/Algorithm.h"

#include <string>
#include <vector>
class RunManager;
class IParticlePropertySvc;


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
 * $Header: /nfs/slac/g/glast/ground/cvs/G4Generator/src/G4Generator.h,v 1.11 2002/04/20 10:18:33 riccardo Exp $
 */
class G4Generator : public Algorithm {
 public:
  G4Generator(const std::string& name, ISvcLocator* pSvcLocator); 
  StatusCode initialize();
  StatusCode execute();
  StatusCode finalize();
 
  
 private:
  /// a pointer to the service for particle properties
  IParticlePropertySvc* m_ppsvc;


  /// set of UI commands for setup; this is a property of the algorithm and can
  /// be set in the jobOptions file
  StringArrayProperty m_uiCommands;
  
  /// This is the G4 manager that handles the simulation
  RunManager* m_runManager;

  /// internal routine to set up the (optional) gui stuff  
  void setupGui();

  /// the geometry level of details
  std::string m_geometryMode;
};

#endif




