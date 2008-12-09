# $Header: /nfs/slac/g/glast/ground/cvs/GlastRelease-scons/G4Generator/G4GeneratorLib.py,v 1.1 2008/08/15 21:42:34 ecephas Exp $
def generate(env, **kw):
    if not kw.get('depsOnly', 0):
        env.Tool('addLibrary', library = ['G4Generator'])
    env.Tool('fluxLib')
    env.Tool('G4HadronSimLib')
    env.Tool('EventLib')
    env.Tool('addLibrary', library = env['geant4Libs'])
    env.Tool('GlastSvcLib')
    env.Tool('geometryLib')
    env.Tool('GlastMSLib')
    env.Tool('addLibrary', library = env['clhepLibs'])
def exists(env):
    return 1;
