import os
import sys
import optparse
import subprocess

parser = optparse.OptionParser()
parser.add_option("--action" , action="store", type="string" , help="What to do" , default="cflags" )
parser.add_option("--build_dir" , action="store", type="string" , help="Project build folder")
parser.add_option("--cflags", action="store", type="string" , help="CFLAGS" , default="02")
parser.add_option("--cxxflags", action="store", type="string" , help="CXXFLAGS" , default="02")
parser.add_option("--export_script_file" , action="store", type="string" , help="")


(options, args) = parser.parse_args()

def dump_arguments() :
	print ("\nProject build folder :[%s]" % (options.build_dir))
	print ("\n               FLAGS :[%s]" % (options.cflags))
	print
	print


if options.action == "cflags" : 
	dump_arguments()
	makefile_list = "%s/makefile_list.txt" % ( options.build_dir )
	cmd = "find %s -iname \"Makefile\" >%s" % (options.build_dir , makefile_list )
	os.system( cmd )
	makefile_list_content = open(makefile_list)
	lines = makefile_list_content.readlines()
	ap = ("\n\nCFLAGS += %s\n\nCXXFLAGS += %s %s\n" % 
		(options.cflags, options.cflags, options.cxxflags,))
	for line in lines :
		makefile = line.strip("\n")
		filein = open(makefile, "rt")
		mk = filein.read()
		filein.close()
		fileout = open(makefile, "wt")
		fileout.write("%s%s" % (mk, ap))
		fileout.close()










def get_brew_output(arguments) :
	cmd = "brew " + arguments 
	stream = os.popen(cmd)
	output = stream.read()
	return output

def get_path_line_from_brew(pkg) :
	output = get_brew_output(" info " + pkg )
	for line in output.split("\n") :
		a = line.find("(")
		if a > 0 :
			b = line[a:].find("files")
			if b > 0 :
				if line[b:].find(")") > 0 :
					return line.split(" ")[0].strip()
	

if options.action == "darwin_path" :
	packages = {
				"OpenSSL" : ["openssl"] , 
				"GMP" :  ["gmp"] , 
				"OMP" :  ["libomp"] ,
				"OpenJDK" : ["openjdk@11" , "openjdk@8"] 
			}
	
	installed_packages = {}
	installed_packages_path = {}
	export_env_script = ""
	pkg_list = get_brew_output("list --full-name")
	
	for pkg in packages.items() :
		package_name = pkg[0]
		package_versions = pkg[1]
		installed_versions = [] 
		for version in package_versions :
			if ( pkg_list.find(version ) > 0):
				installed_versions.append(version) 

		installed_packages[ package_name ] = installed_versions 

	some_not_installed = False
	for pkg in installed_packages.items() :
		package_name = pkg[0]
		installed_versions = pkg[1]
		if ( len(installed_versions) == 0 ) :
			print ( "    %-#10s : not installed" % ( package_name ))
			some_not_installed = True
		else :
			installed_packages_path [ package_name ] = get_path_line_from_brew(installed_versions[0]) 
			print ( "    %-#10s :     installed %-#15s @ %s" % (package_name  , str(installed_versions) , installed_packages_path[package_name] ))
			export_env_script += ( "%s=\"%s\"  " % ( package_name , installed_packages_path[package_name] ) )
	
	export_env_script = "export " + export_env_script + "\n"

	if (some_not_installed ) :
		exit(-1)

	fileout = open( options.export_script_file , "wt")
	fileout.write( export_env_script )
	fileout.close()
 