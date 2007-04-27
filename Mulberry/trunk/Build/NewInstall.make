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
	 ln -s ../ACE/ACE_wrappers/ace/libACE.so libACE-5_4_7.so; \
	 ln -s ../ACE/ACE_wrappers/ace/libACE.so.5.4.7 libACE.so.5_4_7; \
	 ln -s ../libjcore/libjcore-2.5.0.a libjcore-2.5.0.a; \
	 ln -s ../libjcore/libjcore-2.5.0.so libjcore-2.5.0.so; \
	 ln -s ../libjcore/libjx-2.5.0.a libjx-2.5.0.a; \
	 ln -s ../libjcore/libjx-2.5.0.so libjx-2.5.0.so)
	touch ${JX_HOME}/ACE/ACE_wrappers/ace/Svc_Conf_l.cpp

mulberryinit:
	(cd ${MULBERRY_HOME}/Sources_Common/Plugins; \
	 ln -s "Address IO" Address_IO; \
	 ln -s "Pswd Change" Pswd_Change)

buildjx:
	(cd ${JX_HOME}; \
	 ./build)

buildmulberry:
	(cd ${MULBERRY_HOME}/Linux_v2; \
	 ./links.sh; \
	 makemake; \
	 make static -k)

buildplugins:
	(cd ${MULBERRY_HOME}/Build; \
	 make plugins)
	(cd ${MULBERRY_HOME}/Plug-ins; \
	 make install)

init: jxsrc librariessrc mulberrysrc jxinit mulberryinit

build: buildjx buildmulberry buildplugins

all: init build
