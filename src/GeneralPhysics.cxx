// File and Version Information:
// $Header: /nfs/slac/g/glast/ground/cvs/G4Generator/src/GeneralPhysics.cxx,v 1.3 2003/02/19 09:03:20 flongo Exp $
//
// Description: This class manages the building of the geantino and the generic
// decay process
//
// Author(s):
//      F.Longo

#include "GeneralPhysics.h"

#include "globals.hh"
#include "G4ios.hh"
#include <iomanip>   

GeneralPhysics::GeneralPhysics(const G4String& name)
                     :  G4VPhysicsConstructor(name)
{
}

GeneralPhysics::~GeneralPhysics()
{
}

#include "G4ParticleDefinition.hh"
#include "G4ProcessManager.hh"
// Bosons
#include "G4ChargedGeantino.hh"
#include "G4Geantino.hh"

void GeneralPhysics::ConstructParticle()
{


  // pseudo-particles
  G4Geantino::GeantinoDefinition();
  G4ChargedGeantino::ChargedGeantinoDefinition();  
}

#include "G4Decay.hh"

void GeneralPhysics::ConstructProcess()
{
  // Purpose and Method: this method is invoked by G4 to build the physics
  //                     processes table

  // Add Decay Process

  G4Decay* fDecayProcess = new G4Decay();

  theParticleIterator->reset();
  while( (*theParticleIterator)() ){
    G4ParticleDefinition* particle = theParticleIterator->value();
    G4ProcessManager* pmanager = particle->GetProcessManager();
    if (fDecayProcess->IsApplicable(*particle)) { 
      pmanager ->AddProcess(fDecayProcess);
      // set ordering for PostStepDoIt and AtRestDoIt
      pmanager ->SetProcessOrdering(fDecayProcess, idxPostStep);
      pmanager ->SetProcessOrdering(fDecayProcess, idxAtRest);
    }
  }
}





