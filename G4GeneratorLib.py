# $Header: /nfs/slac/g/glast/ground/cvs/GlastRelease-scons/G4Generator/G4GeneratorLib.py,v 1.4 2011/08/17 00:11:53 jrb Exp $
def generate(env, **kw):
    if not kw.get('depsOnly', 0):
        env.Tool('addLibrary', library = ['G4Generator'])
        if env['PLATFORM']=='win32' and env.get('CONTAINERNAME','')=='GlastRelease':
	    env.Tool('findPkgPath', package = 'G4Generator') 
    env.Tool('fluxLib')
    env.Tool('EventLib')
    env.Tool('addLibrary', library = env['geant4Libs'])
    env.Tool('GlastSvcLib')
    env.Tool('geometryLib')
    env.Tool('addLibrary', library = env['clhepLibs'])
def exists(env):
    return 1;
