#!/usr/bin/python

import os
import sys

newlicense = """/*
    Copyright (c) 2007 Cyrus Daboo. All rights reserved.
    
    Licensed under the Apache License, Version 2.0 (the "License");
    you may not use this file except in compliance with the License.
    You may obtain a copy of the License at
    
        http://www.apache.org/licenses/LICENSE-2.0
    
    Unless required by applicable law or agreed to in writing, software
    distributed under the License is distributed on an "AS IS" BASIS,
    WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
    See the License for the specific language governing permissions and
    limitations under the License.
*/

"""

def process_directory(path):
	for file in os.listdir(path):
		fpath = os.path.join(path, file)
		if os.path.isfile(fpath):
			process_file(fpath)
		elif os.path.isdir(fpath):
			process_directory(fpath)

def process_file(fpath):
	if (fpath.endswith(".cp") or
		fpath.endswith(".cc") or
		fpath.endswith(".cpp") or
		fpath.endswith(".c") or
		fpath.endswith(".h")):
		replace_license(fpath)

def replace_license(fpath):
	print fpath
	fin = open(fpath, "r")
	fout = open("%s.licensed" % fpath, "w")

	first = True
	ignore = False
	reallyignore = False
	ignored = []
	for line in fin:
		if first:
			fout.write(newlicense)
			if line.startswith("/*"):
				ignore = True
		first = False
		if not ignore:
			fout.write(line)
		else:
			ignored.append(line)
		if ignore:
			if line.find("Copyright") != -1:
				reallyignore = True
			if line.find("*/") != -1:
				ignore = False
				if not reallyignore:
					fout.write("".join(ignored))
	
	fin.close()
	fout.close()
	os.rename(fpath, "%s.unlicensed" % fpath)
	os.rename("%s.licensed" % fpath, fpath)

if __name__ == "__main__":
	paths = ("Sources_Common", "Linux_v2",)
	#paths = ("Plug-ins", )

	for path in paths:
		process_directory(path)
			
