// File and Version Information:
// $Header: /nfs/slac/g/glast/ground/cvs/G4Generator/src/DetectorConstruction.cxx,v 1.23 2002/04/18 12:39:07 riccardo Exp $
//
// Description:
//      
//
// Author(s):
//      R.Giannitrapani

#include "DetectorConstruction.h"

#include "G4Material.hh"
#include "G4Box.hh"
#include "G4Tubs.hh"
#include "G4LogicalVolume.hh"
#include "G4ThreeVector.hh"
#include "G4PVPlacement.hh"
#include "globals.hh"
#include "G4SDManager.hh"

#include "PosDetectorManager.h"
#include "IntDetectorManager.h"

#include "G4Geometry.h"
#include "G4Media.h"

#include <iomanip>
#include <cassert>

DetectorConstruction::DetectorConstruction(IGlastDetSvc* gsv,
                                           IDataProviderSvc* esv,
                                           std::string geometry_mode)
                                           :m_gsv(gsv), 
                                           m_geometryMode(geometry_mode)
{
    
  // now create the GlastDetector managers
  m_posDet = new PosDetectorManager(this, esv);
  m_intDet = new IntDetectorManager(this, esv);
}

DetectorConstruction::~DetectorConstruction()

{ 
  delete m_posDet;
  delete m_intDet;
}

G4VPhysicalVolume* DetectorConstruction::Construct()
{  
  G4Geometry* geom = new G4Geometry(m_posDet, m_intDet, &m_idMap, m_geometryMode);
  G4Media* media = new G4Media();
  m_gsv->accept(*media);
  m_gsv->accept(*geom);
  //  std::cout << (*G4Material::GetMaterialTable()) <<std::endl;
  std::cout << "Geometry done with " << geom->getPhysicalNumber() << 
      " physical volumes" << std::endl;
  return geom->getWorld();
}
















