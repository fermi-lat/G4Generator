#ifndef PhysicsList_h
#define PhysicsList_h 1

#include <string>

#include "G4VModularPhysicsList.hh"
#include "globals.hh"

/** 
 * @class PhysicsList
 *
 * @brief Physics list class for G4Generator
 *
 * This class defines and activates all the physics processes used by
 * G4Generator: in particular it sets
 * 
 * - the particles to be used in the simulation
 * - the range cuts for each particle
 * - the physics processes to be simulated
 *  
 * @author F.Longo 
 *    
 * $Header: /nfs/slac/g/glast/ground/cvs/G4Generator/src/PhysicsList.h,v 1.4 2002/09/04 15:06:47 burnett Exp $
 */
class PhysicsList: public G4VModularPhysicsList
{
 public:
  PhysicsList(double cutValue, std::string& physicsChoice);
  ~PhysicsList();
  
 public:
  
  /// This method set all the physics cuts for the simulation
  virtual void SetCuts();
  

 private:
  
  std::string m_physicsChoice;

};

#endif







