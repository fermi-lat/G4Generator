//
// ********************************************************************
// * DISCLAIMER                                                       *
// *                                                                  *
// * The following disclaimer summarizes all the specific disclaimers *
// * of contributors to this software. The specific disclaimers,which *
// * govern, are listed with their locations in:                      *
// *   http://cern.ch/geant4/license                                  *
// *                                                                  *
// * Neither the authors of this software system, nor their employing *
// * institutes,nor the agencies providing financial support for this *
// * work  make  any representation or  warranty, express or implied, *
// * regarding  this  software system or assume any liability for its *
// * use.                                                             *
// *                                                                  *
// * This  code  implementation is the  intellectual property  of the *
// * GEANT4 collaboration.                                            *
// * By copying,  distributing  or modifying the Program (or any work *
// * based  on  the Program)  you indicate  your  acceptance of  this *
// * statement, and all its terms.                                    *
// ********************************************************************
//
// $Id: EmStandardPhysics.cc,v 1.1 2011/07/20 12:35:33 flongo Exp $
// GEANT4 tag $Name:  $
//
//---------------------------------------------------------------------------
//
// ClassName:   EmStandardPhysics
//
// Author:      V.Ivanchenko 09.11.2005
//
// Modified:
// 05.12.2005 V.Ivanchenko add controlled verbosity
//
//----------------------------------------------------------------------------
//

#include "EmStandardPhysics.hh"
#include "G4ParticleDefinition.hh"
#include "G4ProcessManager.hh"
#include "G4LossTableManager.hh"

#include "G4ComptonScattering.hh"
#include "G4GammaConversion.hh"
#include "G4PhotoElectricEffect.hh"

#include "MultipleScattering.h"

//#include "G4eMultipleScattering.hh"
#include "G4eIonisation.hh"
#include "G4eBremsstrahlung.hh"
#include "G4eplusAnnihilation.hh"
#include "G4VEnergyLossProcess.hh"

//#include "G4MuMultipleScattering.hh"
#include "G4MuIonisation.hh"
#include "G4MuBremsstrahlung.hh"
#include "G4MuPairProduction.hh"

//#include "G4hMultipleScattering.hh"
#include "G4hIonisation.hh"
#include "G4ionIonisation.hh"

#include "G4Gamma.hh"
#include "G4Electron.hh"
#include "G4Positron.hh"
#include "G4MuonPlus.hh"
#include "G4MuonMinus.hh"
#include "G4PionPlus.hh"
#include "G4PionMinus.hh"
#include "G4KaonPlus.hh"
#include "G4KaonMinus.hh"
#include "G4Proton.hh"
#include "G4AntiProton.hh"
#include "G4Deuteron.hh"
#include "G4Triton.hh"
#include "G4He3.hh"
#include "G4Alpha.hh"
#include "G4GenericIon.hh"

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

EmStandardPhysics::EmStandardPhysics(const G4String& name, G4int ver):
  G4VPhysicsConstructor(name), verbose(ver)
{
  G4LossTableManager::Instance();
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

EmStandardPhysics::~EmStandardPhysics()
{}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void EmStandardPhysics::ConstructParticle()
{
// gamma
  G4Gamma::Gamma();

// leptons
  G4Electron::Electron();
  G4Positron::Positron();
  G4MuonPlus::MuonPlus();
  G4MuonMinus::MuonMinus();

// mesons
  G4PionPlus::PionPlusDefinition();
  G4PionMinus::PionMinusDefinition();
  G4KaonPlus::KaonPlusDefinition();
  G4KaonMinus::KaonMinusDefinition();

// barions
  G4Proton::Proton();
  G4AntiProton::AntiProton();

// ions
  G4Deuteron::Deuteron();
  G4Triton::Triton();
  G4He3::He3();
  G4Alpha::Alpha();
  G4GenericIon::GenericIonDefinition();
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void EmStandardPhysics::ConstructProcess()
{
  // Add standard EM Processes

  theParticleIterator->reset();
  while( (*theParticleIterator)() ){
    G4ParticleDefinition* particle = theParticleIterator->value();
    G4ProcessManager* pmanager = particle->GetProcessManager();
    G4String particleName = particle->GetParticleName();

    if (particleName == "gamma") {

      pmanager->AddDiscreteProcess(new G4PhotoElectricEffect);
      pmanager->AddDiscreteProcess(new G4ComptonScattering);
      pmanager->AddDiscreteProcess(new G4GammaConversion);

    } else if (particleName == "e-") {

      pmanager->AddProcess(new MultipleScattering, -1, 1, 1);
      pmanager->AddProcess(new G4eIonisation,        -1, 2, 2);
      pmanager->AddProcess(new G4eBremsstrahlung(),  -1, 3, 3);

            
    } else if (particleName == "e+") {

      pmanager->AddProcess(new MultipleScattering, -1, 1, 1);
      pmanager->AddProcess(new G4eIonisation,        -1, 2, 2);
      pmanager->AddProcess(new G4eBremsstrahlung,    -1, 3, 3);
      pmanager->AddProcess(new G4eplusAnnihilation,   0,-1, 4);

    } else if (particleName == "mu+")
    {

      pmanager->AddProcess(new MultipleScattering,-1, 1, 1);
      pmanager->AddProcess(new G4MuIonisation,      -1, 2, 2);
      pmanager->AddProcess(new G4MuBremsstrahlung,  -1, 3, 3);
      pmanager->AddProcess(new G4MuPairProduction,  -1, 4, 4);
    }
    else if (particleName == "mu-")
      {
	
	
	pmanager->AddProcess(new MultipleScattering,-1, 1, 1);
	pmanager->AddProcess(new G4MuIonisation,      -1, 2, 2);
	pmanager->AddProcess(new G4MuBremsstrahlung,  -1, 3, 3);
	pmanager->AddProcess(new G4MuPairProduction,  -1, 4, 4);
      }
    else if (particleName == "alpha" ||
	     particleName == "He3" ||
	     particleName == "GenericIon") {
      

      pmanager->AddProcess(new MultipleScattering, -1, 1, 1);
      pmanager->AddProcess(new G4ionIonisation,      -1, 2, 2);
      
    } else if (particleName == "anti_omega-" ||
               particleName == "anti_proton" ||
               particleName == "anti_sigma+" ||
               particleName == "anti_sigma-" ||
               particleName == "anti_xi-" ||
               particleName == "deuteron" ||
               particleName == "kaon+" ||
               particleName == "kaon-" ||
               particleName == "omega-" ||
               particleName == "pi+" ||
               particleName == "pi-" ||
               particleName == "proton" ||
               particleName == "sigma+" ||
               particleName == "sigma-" ||
               particleName == "tau+" ||
               particleName == "tau-" ||
               particleName == "triton" ||
               particleName == "xi-" ) {

      
      pmanager->AddProcess(new MultipleScattering,-1, 1, 1);
      pmanager->AddProcess(new G4hIonisation,       -1, 2, 2);
    }
  }
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......
