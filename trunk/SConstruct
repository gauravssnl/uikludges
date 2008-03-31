
modules = ["uikludges", "applicationmanager"]

env = Environment(LIBPATH = modules, MMP_TEMPLATE="#generic.mmp.in")
Export('env')


def build_mmp(target, source, env):
	""" Build MMP file from .mmp.in source. 
	
	Gets context variables from Python.
	"""
	
	template = env["MMP_TEMPLATE"]
	f = open(template, "rt")
	data = f.read()
	f.close()
	
	vars = {}
	execfile(source, globals(), vars)
	
	data = data % vars
	
	f = open(
	
    return None


for mod in modules:
	SConscript(mod + "/SConscript")
	