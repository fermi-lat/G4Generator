#include "DetectorConstruction.h"
#include "SensitiveDetector.h"

#include "G4Material.hh"
#include "G4Box.hh"
#include "G4Tubs.hh"
#include "G4LogicalVolume.hh"
#include "G4ThreeVector.hh"
#include "G4PVPlacement.hh"
#include "globals.hh"
#include "G4SDManager.hh"

#include "detModel/Management/Manager.h"
#include "detModel/Management/XercesBuilder.h"
#include "detModel/Sections/Shape.h"
#include "detModel/Gdd.h"

#include "G4SectionsVisitor.h"
#include "G4MaterialsVisitor.h"

DetectorConstruction::DetectorConstruction()
{
  detModel::Manager* gddManager = detModel::Manager::getPointer();
  
  gddManager->setBuilder(new detModel::XercesBuilder);  
  gddManager->setNameFile("/scratch/users/frailis/GlastPack/xmlUtil/v3r1/xml/flight.xml");

  gddManager->setMode("fastmc");

  gddManager->build(detModel::Manager::all);
  
}

DetectorConstruction::~DetectorConstruction()
{ 
  detModel::Manager* gddManager = detModel::Manager::getPointer();
  gddManager->cleanGdd(); 
}

G4VPhysicalVolume* DetectorConstruction::Construct()
{  
  sDetector = 0;
  // This method build the material -- MUST be implemented with a MaterialVisitor
  DefineMaterial();
  // This method build the geometry with a visitor
  return GeometryConstruct();
}

void DetectorConstruction::DefineMaterial()
{
  //------- MATERIALS --------------------------------------
  
  detModel::Manager* gddManager = detModel::Manager::getPointer();
  
  G4MaterialsVisitor* visitor = new G4MaterialsVisitor;

  gddManager->startVisitor(visitor);

  delete visitor;

}

G4VPhysicalVolume* DetectorConstruction::GeometryConstruct()
{
  unsigned int i;
  G4VPhysicalVolume* res;

  detModel::Manager* gddManager = detModel::Manager::getPointer();
  detModel::Gdd* gdd = gddManager->getGdd();
  
  G4SectionsVisitor* visitor = new G4SectionsVisitor;
  
  G4SDManager* SDman = G4SDManager::GetSDMpointer();

  if(!sDetector)
    {
      sDetector = new SensitiveDetector("SensitiveDetector",this);
      SDman->AddNewDetector( sDetector );
    }

  gddManager->startVisitor(visitor);
  
  for(i=0;i<visitor->g4Logicals.size();i++)
    {
      if(gdd->getShapeByName(visitor->g4Logicals[i]->GetName())!=0)
	if ((gdd->getShapeByName(visitor->g4Logicals[i]->GetName()))->getSensitive())
	  {
	    visitor->g4Logicals[i]->SetSensitiveDetector(sDetector);
	  }
    }//end for
  
  g4ID = visitor->g4Identifiers;

  res = visitor->worldphys;
  
  delete visitor;

  return res;
}


