env = Environment(LIBPATH = modules, MMP_TEMPLATE="#generic.mmp.in")
Export('env')

#: List of extension modules which are build
modules = ["uikludges", "applicationmanager"]


def build_mmp(target, source, env):
	""" Build MMP file from .mmp.in source. 
	
	An MMP file is generated based on a template and Python input variables.
	
	@param target: .mmp file name
	@param source: Python file containing template substitutions
	"""
	
	template = env["MMP_TEMPLATE"]
	f = open(template, "rt")
	data = f.read()
	f.close()
	
	vars = {}
	execfile(source, globals(), vars)
	
	data = data % vars
	
	f = open(target)
	f.write(data)
	f.close()
	
    return None
          

for mod in modules:
	SConscript(mod + "/SConscript")
	