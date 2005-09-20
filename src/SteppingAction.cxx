// File and Version Information:
// $Header: /nfs/slac/g/glast/ground/cvs/G4Generator/src/TrackingAction.cxx,v 1.14 2004/12/17 16:44:33 usher Exp $
//
// Description: this method is used to check the state of G4 at every step to avoid infinite event loop
// It checks wheter G4 has gone into a G4StateAbort and throw an exception
//      
// Author(s):
//      F.Longo

#include "SteppingAction.h"
#include "G4StateManager.hh"
#include <stdexcept>
#include <string>
#include <sstream>

SteppingAction::SteppingAction()
{;}

void SteppingAction::UserSteppingAction(const G4Step* aStep)
{
  if(G4StateManager::GetStateManager()->GetCurrentState()==G4State_Abort) 
    {
      // G4 is going into a G4State Abort
      std::stringstream errorStream;
      errorStream << "G4 in G4StateAbort " ;

      // need to set the Base G4State before aborting the Event Loop

      G4StateManager::GetStateManager()->SetNewState(G4State_Idle);

      throw std::domain_error(errorStream.str());
    }
}




