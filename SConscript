# -*- python -*-
# $Header: /nfs/slac/g/glast/ground/cvs/GlastRelease-scons/G4Generator/SConscript,v 1.5 2009/01/23 00:07:01 ecephas Exp $
# Authors: T. Burnett <tburnett@u.washington.edu>, R.Giannitrapani <riccardo@fisica.uniud.it>, Francesco Longo <Francesco.Longo@ts.infn.it>
# Version: G4Generator-05-22-00
Import('baseEnv')
Import('listFiles')
Import('packages')
progEnv = baseEnv.Clone()
libEnv = baseEnv.Clone()

libEnv.Tool('G4GeneratorLib', depsOnly = 1)
#libEnv.AppendUnique(CPPPATH = ['#G4HadronSim/src/Packaging/include', '#G4HadronSim/src/LHEP/include',
#             '#G4HadronSim/src/LHEP_BERT/include',
#            '#G4HadronSim/src/LHEP_BIC/include',
#            '#G4HadronSim/src/QGSP/include',
#            '#G4HadronSim/src/QGSP_BERT/include',
#            '#G4HadronSim/src/QGSP_BIC/include',
#             '#G4HadronSim/src/QGSC/include',
#             '#G4HadronSim/src/QGSC_LEAD/include',
#             '#G4HadronSim/src/LC/include',
#             '#G4HadronSim/src/SE/include'])

G4Generator = libEnv.SharedLibrary('G4Generator', listFiles(['src/Utilities/*.cxx','src/Dll/*.cxx','src/*.cxx']))


progEnv.Tool('G4GeneratorLib')
test_G4Generator = progEnv.GaudiProgram('test_G4Generator', listFiles(['src/test/*.cxx']), test = 1)

progEnv.Tool('registerTargets', package = 'G4Generator',
             libraryCxts = [[G4Generator, libEnv]],
             testAppCxts = [[test_G4Generator, progEnv]],
             includes = listFiles(['G4Generator/*.h','src/RunManager.h']))




