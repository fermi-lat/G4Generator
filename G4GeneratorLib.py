# $Header: /nfs/slac/g/glast/ground/cvs/GlastRelease-scons/G4Generator/G4GeneratorLib.py,v 1.3 2009/01/23 00:20:55 ecephas Exp $
def generate(env, **kw):
    if not kw.get('depsOnly', 0):
        env.Tool('addLibrary', library = ['G4Generator'])
    env.Tool('fluxLib')
    env.Tool('EventLib')
    env.Tool('addLibrary', library = env['geant4Libs'])
    env.Tool('GlastSvcLib')
    env.Tool('geometryLib')
    env.Tool('addLibrary', library = env['clhepLibs'])
def exists(env):
    return 1;
