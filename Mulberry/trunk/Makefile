DEV_HOME := .
MULBERRY_HOME := ${DEV_HOME}/Mulberry
JX_HOME := ${DEV_HOME}/JX
LIBRARIES_HOME := ${DEV_HOME}/Libraries

SVN_ROOT := http://svn.mulberrymail.com
SVN_MULBERRY := ${SVN_ROOT}/mulberry
SVN_LIBRARIES := ${SVN_ROOT}/repos

jxsrc:
	svn checkout ${SVN_MULBERRY}/JX/trunk JX

librariessrc:
	(mkdir ${LIBRARIES_HOME}; \
	 cd ${LIBRARIES_HOME}; \
	 svn checkout ${SVN_LIBRARIES}/CICalendar/trunk CICalendar; \
	 svn checkout ${SVN_LIBRARIES}/XMLLib/trunk XMLLib)

mulberrysrc:
	svn checkout ${SVN_MULBERRY}/Mulberry/trunk Mulberry

jxinit:
	(cd ${JX_HOME}/include; \
	 ln -s ../libjcore/code jcore; \
	 ln -s ../libjx/code jx; \
	 ln -s ../libjx/image jximage; \
	 ln -s ../libjx/strings jxstrings)
	(cd ${JX_HOME}/lib; \
	 ln -s ../ACE/ACE_wrappers/ace/libACE.a libACE-5_4_7.a; \
	 ln -s ../ACE/ACE_wrappers/ace/libACE.so libACE-5_4_7.so; \
	 ln -s ../ACE/ACE_wrappers/ace/libACE.so.5.4.7 libACE.so.5.4.7; \
	 ln -s ../libjcore/libjcore-2_5_0.a libjcore-2_5_0.a; \
	 ln -s ../libjcore/libjcore-2_5_0.so libjcore-2_5_0.so; \
	 ln -s ../libjx/libjx-2_5_0.a libjx-2_5_.0.a; \
	 ln -s ../libjx/libjx-2_5_0.so libjx-2_5_0.so)
	touch ${JX_HOME}/ACE/ACE_wrappers/ace/Svc_Conf_l.cpp

mulberryinit:
	(cd ${MULBERRY_HOME}/Linux; ./links.sh; makemake)

updatejx:
	(cd ${JX_HOME}; svn update)

buildjx:
	(cd ${JX_HOME}; ./build)

updatemulberry:
	(cd ${MULBERRY_HOME}/Build; make svn)

buildmulberry:
	(cd ${MULBERRY_HOME}/Build; make static)

install:
	(cd ${MULBERRY_HOME}/Build; make install)
	
init: jxsrc librariessrc mulberrysrc jxinit mulberryinit

build: buildjx buildmulberry

update: updatejx updatemulberry

runit: ${MULBERRY_HOME}/Linux/mulberry

run: update build runit

all: init build install
