// $Header: /nfs/slac/g/glast/ground/cvs/G4Generator/src/G4Generator.h,v 1.1.1.1 2001/11/19 02:55:42 burnett Exp $


#ifndef G4Generator_h
#define G4Generator_h

#include "GaudiKernel/Algorithm.h"

#include <string>
class IFluxSvc;
class IFlux;
class G4RunManager;
class PrimaryGeneratorAction;

/**
  Geant4 interface
  */

class G4Generator : public Algorithm {
public:
    G4Generator(const std::string& name, ISvcLocator* pSvcLocator); 
    StatusCode initialize();
    StatusCode execute();
    StatusCode finalize();
    
private:
    IFluxSvc* m_fluxSvc;
    IFlux*    m_flux;

    std::string m_source_name;

    /// This is the G4 manager that handles the simulation
    G4RunManager* m_runManager;
    /// This is the primaries generator class 
    PrimaryGeneratorAction* m_primaryGenerator;
};

#endif
