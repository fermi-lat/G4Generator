// File and Version Information:
// $Header: /nfs/slac/g/glast/ground/cvs/G4Generator/src/PhysicsList.cxx,v 1.3 2002/04/18 12:39:07 riccardo Exp $
//
// Description: This class manages the building of the geantino and the generic
// decay process
//
// Author(s):
//      F.Longo

#include "GeneralPhysics.h"

#include "globals.hh"
#include "G4ios.hh"
#include "g4std/iomanip"   

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

void GeneralPhysics::ConstructProcess()
{
  // Purpose and Method: this method is invoked by G4 to build the physics
  //                     processes table

  // Add Decay Process
  theParticleIterator->reset();
  while( (*theParticleIterator)() ){
    G4ParticleDefinition* particle = theParticleIterator->value();
    G4ProcessManager* pmanager = particle->GetProcessManager();
    if (fDecayProcess.IsApplicable(*particle)) { 
      pmanager ->AddProcess(&fDecayProcess);
      // set ordering for PostStepDoIt and AtRestDoIt
      pmanager ->SetProcessOrdering(&fDecayProcess, idxPostStep);
      pmanager ->SetProcessOrdering(&fDecayProcess, idxAtRest);
    }
  }
}





