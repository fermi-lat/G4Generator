# -*- python -*-
# $Header: /nfs/slac/g/glast/ground/cvs/GlastRelease-scons/G4Generator/SConscript,v 1.24 2011/07/28 13:46:29 heather Exp $
# Authors: T. Burnett <tburnett@u.washington.edu>, R.Giannitrapani <riccardo@fisica.uniud.it>, Francesco Longo <Francesco.Longo@ts.infn.it>
# Version: G4Generator-06-01-01
Import('baseEnv')
Import('listFiles')
Import('packages')
progEnv = baseEnv.Clone()
libEnv = baseEnv.Clone()

if 'G4TablesPath' in libEnv:
    # Calculate path relative to GLAST_EXT so it makes sense for all users at run time
    relPath = str(Dir(str(libEnv['GLASTEXTvalue'])).rel_path(Dir(str(libEnv['G4TablesPath']))) )
    print "In G4Generator SConscript relPath is ", relPath
    relPathQ = '\\"' + relPath + '\\"'
    print "In G4Generator SConscript relPathQ is ", relPathQ
    libEnv.AppendUnique(CPPDEFINES = 'G4TABLESPATH=' + relPathQ)
else:
    print "In G4Generator SConscript and G4TablesPath is not in libEnv"

libEnv.Tool('addLinkDeps', package='G4Generator', toBuild='component')
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
test_G4Generator = progEnv.GaudiProgram('test_G4Generator',
                                        listFiles(['src/test/*.cxx']),
                                        test = 1, package = 'G4Generator')

progEnv.Tool('registerTargets', package = 'G4Generator',
             libraryCxts = [[G4Generator, libEnv]],
             testAppCxts = [[test_G4Generator, progEnv]],
             includes = listFiles(['G4Generator/*.h','src/RunManager.h']),
             jo = listFiles(['src/test/*.txt', 'src/test/test_sources.xml', 'src/*.txt']),
             xml = ['xml/test_sources.xml'])




