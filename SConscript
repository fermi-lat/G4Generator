# -*- python -*-
# $Header: /nfs/slac/g/glast/ground/cvs/G4Generator/SConscript,v 1.34 2012/04/25 04:56:38 heather Exp $
# Authors: T. Burnett <tburnett@u.washington.edu>, R.Giannitrapani <riccardo@fisica.uniud.it>, Francesco Longo <Francesco.Longo@ts.infn.it>
# Version: G4Generator-06-06-00

Import('baseEnv')
Import('listFiles')
Import('packages')
progEnv = baseEnv.Clone()
libEnv = baseEnv.Clone()

if 'G4TablesPath' in libEnv:
    # Calculate path relative to GLAST_EXT so it makes sense for all users at run time
    relPath = str(Dir(str(libEnv['GLASTEXTvalue'])).rel_path(Dir(str(libEnv['G4TablesPath']))) )
    relPathQ = '\\"' + relPath + '\\"'
    libEnv.AppendUnique(CPPDEFINES = 'G4TABLESPATH=' + relPathQ)

libEnv.Tool('addLinkDeps', package='G4Generator', toBuild='component')

G4Generator = libEnv.ComponentLibrary('G4Generator',
                                      listFiles(['src/Utilities/*.cxx',
                                                 'src/*.cxx']))


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




