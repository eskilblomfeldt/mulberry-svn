createdirs:
	mkdir /users/dev
	mkdir /users/dev/mulberry4
	mkdir /users/dev/ClientLibs

initcvs:
	export CVSROOT=:ext:daboo@cvs.mulberrymail.com:/users/dev/CVSClient

frameworksrc:
	cd /users/dev
	cvs checkout STLport
	cvs checkout ACE_wrappers_v2
	cvs checkout JX-1.5.4

mulberrysrc:
	cd /users/dev/mulberry
	cvs checkout Build
	cvs checkout -r Mulberry_v4 Plug-ins
	cvs checkout -r Mulberry_v4 Sources_Common
	cvs checkout -r Mulberry_v4 Linux_v2
	cd /users/dev/ClientLibs
	cvs checkout Libraries/iCalendar
	cvs checkout Libraries/XMLLib

jxinit:
	cd /users/dev/JX-1.5.4/ACE
	ln -s ../../ACE_wrappers_v2 ACE_wrappers
	cd ../lib
	ln -s ../ACE_wrappers/ace/libACE.a libACE-5_0_2.a
	ln -s ../ACE_wrappers/ace/libACE.so libACE-5_0_2.so
	ln -s ../libjcore/libjcore-1.5.4.a libjcore-1.5.4.a
	ln -s ../libjcore/libjcore-1.5.4.so libjcore-1.5.4.so
	ln -s ../libjcore/libjx-1.5.4.a libjx-1.5.4.a
	ln -s ../libjcore/libjx-1.5.4.so libjx-1.5.4.so
	cd ../include
	ln -s ../libjcore/code jcore
	ln -s ../libjx/code jx

mulberryinit:
	cd /users/dev/mulberry/Build
	touch make.local
	cd /users/dev/mulberry/Sources_Common/Plugins
	ln -s "Address IO" Address_IO
	ln -s "Pswd Change" Pswd_Change

buildstl:
	cd /users/dev/STLport/src
	make -f xxx release_static release_dynamic
	cd ../lib
	ln -s libstlport_gcc.so.4.6 libstlport_gcc.so

buildjx:
	cd /users/dev/JX-1.5.4
	./build

buildmulberry:
	cd /users/dev/mulberry4/Linux_v2
	./links.sh
	makemake
	make -k

buildplugins:
	cd ../Build
	make plugins
	make splugins
	cd ../Plug-ins
	source installall

init: createdirs initcvs frameworksrc mulberrysrc jxinit mulberry init

build: buildstl buildjx buildmulberry buildplugins

all: init build
