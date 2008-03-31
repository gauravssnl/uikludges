import os 

EPYDOC="c:\\python25\\scripts\\epydoc"

#: Where are PyS60 source files located
PYS60_ROOT="C:\\Symbian\\9.2\\S60_3rd_FP1\\pys60\\src"
	
#: List of extension modules which are build
EXT_MODULES = ["uikludges", "applicationmanager"]

def templated_builder(target, source, varfile, env):
	""" Generate a file from a template.  
	
	A file is generated based on a template and Python input variables.
	
	@param target: .mmp file name
	@param source: Python file containing template substitutions
	"""
	
	f = open(source, "rt")
	data = f.read()
	f.close()
	
	vars = {}
	execfile(varfile, globals(), vars)
	
	data = data % vars
	
	f = open(target)
	f.write(data)
	f.close()
	
	return None

def build_pys60_extensions(target, source, env):
	ext_path = target
	
	ext_name = target
	
	# File defining extension specific variables
	varfile = os.path.join(ext_path, ext_name + ".extension")
	
	bld_file = ext_path + "/bld.inf"
	mmp_file = ext_path + "/" + ext_name + ".mmp"
	
	templated_builder(bld_file, env["BLD_INF_TEMPLATE"], varfile, env)
	templated_builder(mmp_file, env["MMP_TEMPLATE"], varfile, env)
	env.Program("bldmake bldfiles")
	env.Program("abld build gcce urel")
	
	#: Return list of generated files
	return [ bld_file, 
			 mmp_file,
			 ext_path + "/abld.bat",
			  ]
	
bld = Builder(action = build_pys60_extensions, suffix = '.mmp', src_suffix = '.extension')

env = Environment(
	tools = ['lex'],
	BUILDERS = {'PyS60Extension' : bld},
	MMP_TEMPLATE = "#templates/generic.mmp.in", 
	BLD_INF_TEMPLATE = "#templates/bld.inf"
	)
	
Export(env)
          
for mod in EXT_MODULES:
	SConscript(mod + "/SConscript")
	

	