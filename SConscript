# -*- python -*-
# $Header: /nfs/slac/g/glast/ground/cvs/GlastRelease-scons/G4Generator/SConscript,v 1.13.2.1 2010/10/08 16:27:50 heather Exp $
# Authors: T. Burnett <tburnett@u.washington.edu>, R.Giannitrapani <riccardo@fisica.uniud.it>, Francesco Longo <Francesco.Longo@ts.infn.it>
# Version: G4Generator-05-26-03-gr01

Import('baseEnv')
Import('listFiles')
Import('packages')
progEnv = baseEnv.Clone()
libEnv = baseEnv.Clone()

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
             jo = listFiles(['src/test/*.txt', 'src/*.txt']),
             xml = ['xml/test_sources.xml'])




