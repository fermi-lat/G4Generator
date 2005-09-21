/** 
* @file G4Generator_load.cpp
* @brief This is needed for forcing the linker to load all components
* of the library.
*
*  $Header: /nfs/slac/g/glast/ground/cvs/G4Generator/src/Dll/G4Generator_load.cxx,v 1.8 2003/01/23 16:15:05 usher Exp $
*/

#include "GaudiKernel/DeclareFactoryEntries.h"

DECLARE_FACTORY_ENTRIES(G4Generator) 
{
    DECLARE_ALGORITHM( G4Generator);
    DECLARE_SERVICE(   G4GeometrySvc);
    DECLARE_SERVICE(   G4GenErrorSvc);
    DECLARE_TOOL(      G4GeneratorRandom);
} 
