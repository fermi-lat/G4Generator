// File and Version Information:
// $Header: /nfs/slac/g/glast/ground/cvs/G4Generator/src/PhysicsList.cxx,v 1.5 2002/09/04 15:06:47 burnett Exp $
//
// Description: This class manages the building of particles definitions and
// physics processes setup by creating a set of specialized classes and
// registering them
//      
//
// Author(s):
//      F.Longo

#include "PhysicsList.h"

#include "G4ParticleDefinition.hh"
#include "G4ParticleWithCuts.hh"
#include "G4ProcessManager.hh"
#include "G4ProcessVector.hh"
#include "G4ParticleTable.hh"
#include "G4Material.hh"
#include "G4MaterialTable.hh"
#include "G4ParticleTypes.hh"

#include "GeneralPhysics.h"
#include "EMPhysics.h"
#include "MuonPhysics.h"
#include "HadronPhysics.h"
#include "IonPhysics.h"


PhysicsList::PhysicsList(double cutValue, std::string& physicsChoice):  G4VModularPhysicsList()
{
  // The default cut value for all particles
  defaultCutValue = cutValue;

  // Physics Choice

  m_physicsChoice = physicsChoice;
  
  // General Physics
  RegisterPhysics( new GeneralPhysics("general") );
  
  // EM Physics 

  RegisterPhysics( new EMPhysics("standard EM"));

  // Muon Physics

  RegisterPhysics(  new MuonPhysics("muon"));

  // Full or EM Hadron Physics

  RegisterPhysics(  new HadronPhysics("hadron", m_physicsChoice));
  // RegisterPhysics(  new HadronPhysics("hadron"));
  
  // Full or EM Ion Physics

  RegisterPhysics( new IonPhysics("ion", m_physicsChoice));
  //RegisterPhysics( new IonPhysics("ion"));
  

}

PhysicsList::~PhysicsList()
{;}

void PhysicsList::SetCuts()
{
  SetCutsWithDefault();   
}














