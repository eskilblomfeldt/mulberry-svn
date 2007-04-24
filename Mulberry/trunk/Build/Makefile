# Makefile for Mulberry builds

include make.local
include make.version

DEVHOME=/users/dev
TOP_DIR := ..

APP_DIR := ${TOP_DIR}/Linux_v2
APP_NAME := mulberry

PLUGINS_DIR := ${TOP_DIR}/Plug-ins
PLUGINLIB_DIR := ${PLUGINS_DIR}/PluginLibrary


BITS_DIR := ./Bits
TIMEZONE_DIR := ./Bits/Timezones
RESOURCES_DIR := ../Sources_Common/Resources
LICENSE_FILE := Mulberry_License
CHANGES_FILE := CHANGES
CHANGES_FILE_DEV := CHANGES-DEV
CHANGES_FILE_V3 := CHANGES-v3
CHANGES_FILE_DEV_V3 := CHANGES-DEV-v3
CHANGES_FILE_V4 := CHANGES-v4
CHANGES_FILE_DEV_V4 := CHANGES-DEV-v4

OS_NAME := $(shell echo "`uname -s`")
ifeq (${OS_NAME},SunOS)
PROC_NAME := -$(shell echo "`uname -p`")
REL_NAME := -$(shell echo "`uname -r`")
else
ifeq (${OS_NAME},Linux)
PROC_NAME := -$(shell echo "`uname -m`")
KERNEL := $(shell echo "`uname -r`")
REL_NAME := 
endif
endif

RELEASE_NAME := ${OS_NAME}${PROC_NAME}${REL_NAME}
RELEASE_DIR := ./${RELEASE_NAME}
LIB_DIR := ${RELEASE_DIR}/lib
HIDDEN_DIR := .mulberry
PLUGIN_RELEASE_DIR := ${RELEASE_DIR}/${HIDDEN_DIR}/Plug-ins
RESOURCE_RELEASE_DIR := ${RELEASE_DIR}/${HIDDEN_DIR}/Resources
TIMEZONE_RELEASE_DIR := ${PLUGIN_RELEASE_DIR}/Timezones
LINK_DIR := ./obj
RPM_DIR := ./rpmdir
RPM_SOURCES_DIR := ${RPM_DIR}/SOURCES
RPM_BUILD_DIR := ${RPM_DIR}/BUILD

TAR_FILE := mulberry-${MUL_VERSION}-${ARCHIVE_SUFFIX}.tar
GZIP_FILE := mulberry-${MUL_VERSION}-${ARCHIVE_SUFFIX}.tgz

TAR_ITEMS := ${APP_NAME} ${LICENSE_FILE} ${HIDDEN_DIR}
TAR_ITEMS_LIB := ${APP_NAME} ${LICENSE_FILE} ${HIDDEN_DIR} lib

JXDIR := ${DEVHOME}/JX-1.5.4
ACE_LIB := ${JXDIR}/ACE/ACE_wrappers/ace/libACE.so
ACE_LIB_RENAME := libACE-5_0_2.so
JCORE_LIB := ${JXDIR}/libjcore/libjcore-1_5_4.so
JX_LIB := ${JXDIR}/libjx/libjx-1_5_4.so
STL_LIB := ${DEVHOME}/STLport/lib/libstlport_gcc.so
GCCSTD_LIB := /usr/local/lib/libstdc++.so.2.10.0

# ftp defines
FTP_DIR := /var/ftp/pub/Mulberry/${MUL_STAGE}v${MUL_VERSION}
SSH_HOST := darius.cyrusoft.com
SCP_DEST := daboo@${SSH_HOST}
MKDIR_CMD := ssh -t ${SSH_HOST} 'if [ ! -d ${FTP_DIR} ]; then mkdir ${FTP_DIR}; fi;'
SCP_CMD := scp ${RELEASE_DIR}/${GZIP_FILE} ${SCP_DEST}:${FTP_DIR}
GPG_CMD := ssh -t ${SSH_HOST} 'gpg -b -u mulberry@cyrusoft.com ${FTP_DIR}/${GZIP_FILE};'

# Plugins to build
BUILD_PLUGINS := CRAM-MD5 DIGEST-MD5 GSSAPI Kerberos NTLM \
			CommunicatorAdbkIO Eudora4AdbkIO EudoraAdbkIO NetscapeAdbkIO \
			PineAdbkIO QuickmailAdbkIO vCardAdbkIO \
			ISpell \
			GPG SMIME SSL

# Plugins not built
NO_BUILD_PLUGINS := PSpell PGP QuickmailProAdbkIO Spellswell POPPASSD URLPASSD

STATIC_FLAGS := static=1
NODEBUG_FLAGS := nodebug=1

help:
	@echo "Targets:"
	@echo "cvs      - do cvs update of all mulberry files."
	@echo "           NB does not update new directories."
	@echo "cvsfull  - do full update of all mulberry files."
	@echo "           NB Does update new directories."
	@echo ""
	@echo "static   - build static application package."
	@echo "staticcw - build static application package without app build"
	@echo ""
	@echo "staticnoplugins     - build static application package without plugin build"
	@echo "staticnopluginscw   - build static application package without app/plugin build"
	@echo ""
	@echo "rpm      - build binary rpm package."
	@echo ""
	@echo "upload	- upload to ftp.cyrusoft.com"
	@echo ""

cvs:
	@echo "--> Updating ClientLibs"
	@(cd ../../ClientLibs/Libraries; cvs -q update)
	@echo "--> Updating Sources_Common"
	@(cd ../Sources_Common; cvs -q update)
	@echo "--> Updating Linux_v2"
	@(cd ../Linux_v2; cvs -q update)
	@echo "--> Updating Plug-ins"
	@(cd ../Plug-ins; cvs -q update)
	@echo "--> Updating Bits"
	@(cvs -q update make.version)
	@(cd Bits; cvs -q update)

cvsfull:
	@echo "--> Updating ClientLibs"
	@(cd ../../ClientLibs/Libraries/iCalendar; cvs -q update -d)
	@(cd ../../ClientLibs/Libraries/XMLLib; cvs -q update -d)
	@echo "--> Updating Sources_Common"
	@(cd ../Sources_Common; cvs -q update -d)
	@echo "--> Updating Linux_v2"
	@(cd ../Linux_v2; cvs -q update -d)
	@echo "--> Updating Plug-ins"
	@(cd ../Plug-ins; cvs -q update -d)
	@echo "--> Updating Bits"
	@(cvs -q update make.version)
	@(cd Bits; cvs -q update -d)

static: cleanrelease plugins appstatic archivestatic
staticcw: cleanrelease plugins appstaticcw archivestatic

staticnoplugins: cleanrelease pluginsnobuild appstatic archivestatic
staticnopluginscw: cleanrelease pluginsnobuild appstaticcw archivestatic

dynamic: cleanrelease plugins app lib archivedynamic

cleanall: cleanapp cleanplugins cleanrelease

cleanmost: cleanplugins cleanrelease

cleanapp:
	@echo ""
	@echo "--> Removing current mulberry binary"
	@rm -f ${APP_DIR}/${APP_NAME}

cleanplugins:
	@echo ""
	@echo "--> Removing current plugins"
	@rm -f ${PLUGINS_DIR}/*/*Plugin.so
	@rm -f ${PLUGINLIB_DIR}/*.a

cleanrelease:
	@echo ""
	@echo "--> Removing ${RELEASE_NAME}"
	@rm -rf ${RELEASE_DIR}
	@rm -f ${LINK_DIR}
	@echo "--> Creating ${RELEASE_NAME}"
	@mkdir ${RELEASE_DIR}
	@mkdir ${RELEASE_DIR}/${HIDDEN_DIR}
	@mkdir ${RESOURCE_RELEASE_DIR}
	@ln -s ${RELEASE_DIR} ${LINK_DIR}

pluginlib:
	@echo ""
	@echo "-->Building PluginLibrary"
	@(cd ${PLUGINLIB_DIR}; makemake --bin_dir BIN; ${MAKE})

plugins: pluginlib
	@rm -rf ${PLUGIN_RELEASE_DIR}; mkdir ${PLUGIN_RELEASE_DIR}
	@for i in $(BUILD_PLUGINS); do \
		${MAKE} "PLUGIN=$$i" plugin; \
	done

pluginsnobuild:
	@rm -rf ${PLUGIN_RELEASE_DIR}; mkdir ${PLUGIN_RELEASE_DIR}
	@for i in $(BUILD_PLUGINS); do \
		${MAKE} "PLUGIN=$$i" pluginnobuild; \
	done

plugin:
	@echo ""
	@echo "-->Building ${PLUGIN}"
	@(cd ${PLUGINS_DIR}/${PLUGIN}; makemake --bin_dir BIN; ${MAKE})
	@echo "-->Copying ${PLUGIN}"
	@cp ${PLUGINS_DIR}/${PLUGIN}/*Plugin.so ${PLUGIN_RELEASE_DIR}
	@strip ${PLUGIN_RELEASE_DIR}/*Plugin.so

pluginnobuild:
	@echo ""
	@echo "-->Copying ${PLUGIN}"
	@cp ${PLUGINS_DIR}/${PLUGIN}/*Plugin.so ${PLUGIN_RELEASE_DIR}
	@strip ${PLUGIN_RELEASE_DIR}/*Plugin.so

appstatic:
	@${MAKE} "BUILD_STATIC=1" app

appstaticcw:
	@${MAKE} "BUILD_STATIC=1" appcw

app:
	@echo ""
	@echo "-->Building Mulberry"
	@rm -f ${RELEASE_DIR}/${APP_NAME}
	@(cd ${APP_DIR}; ${MAKE})
	@echo "-->Copying Mulberry"
	@cp ${APP_DIR}/${APP_NAME} ${RELEASE_DIR}
	@strip ${RELEASE_DIR}/${APP_NAME}

appcw:
	@echo ""
	@echo "-->Assuming Mulberry is Already Built"
	@rm -f ${RELEASE_DIR}/${APP_NAME}
	@echo "-->Copying Mulberry"
	@cp ${APP_DIR}/${APP_NAME} ${RELEASE_DIR}
	@strip ${RELEASE_DIR}/${APP_NAME}

bits:
	@echo ""
	@echo "-->Copying Bits to appropriate locations"
	@cp ${BITS_DIR}/${LICENSE_FILE} ${RELEASE_DIR}
	@cp -r ${BITS_DIR}/icons ${RELEASE_DIR}/${HIDDEN_DIR}
	@rm -rf ${RELEASE_DIR}/${HIDDEN_DIR}/icons/CVS
	@cp ${RESOURCES_DIR}/*.xml ${RESOURCE_RELEASE_DIR}
	@cp -r ${TIMEZONE_DIR} ${PLUGIN_RELEASE_DIR}
	@rm -rf ${TIMEZONE_RELEASE_DIR}/CVS
	@rm -rf ${TIMEZONE_RELEASE_DIR}/.AppleDouble
	@cp ${BITS_DIR}/${CHANGES_FILE} ${RELEASE_DIR}/${HIDDEN_DIR}
	@cp ${BITS_DIR}/${CHANGES_FILE_DEV} ${RELEASE_DIR}/${HIDDEN_DIR}
	@cp ${BITS_DIR}/${CHANGES_FILE_V3} ${RELEASE_DIR}/${HIDDEN_DIR}
	@cp ${BITS_DIR}/${CHANGES_FILE_DEV_V3} ${RELEASE_DIR}/${HIDDEN_DIR}
	@cp ${BITS_DIR}/${CHANGES_FILE_V4} ${RELEASE_DIR}/${HIDDEN_DIR}
	@cp ${BITS_DIR}/${CHANGES_FILE_DEV_V4} ${RELEASE_DIR}/${HIDDEN_DIR}

lib:
	@echo ""
	@echo "-->Copying dynamic libraries to appropriate locations"
	@rm -rf ${LIB_DIR}
	@mkdir ${LIB_DIR}
	@cp ${ACE_LIB} ${LIB_DIR}/${ACE_LIB_RENAME}
	@cp ${JCORE_LIB} ${LIB_DIR}/
	@cp ${JX_LIB} ${LIB_DIR}/
	@cp ${STL_LIB} ${LIB_DIR}/
	@cp ${GCCSTD_LIB} ${LIB_DIR}/
	@strip ${LIB_DIR}/*

archivestatic: bits
	@(cd ${RELEASE_DIR}; \
	echo ""; \
	echo "-->Creating tar file"; \
	rm -f ${TAR_FILE} ${GZIP_FILE}; \
	tar cf ${TAR_FILE} ${TAR_ITEMS}; \
	echo "-->Creating gzip file"; \
	gzip -c ${TAR_FILE} > ${GZIP_FILE}; \
	rm -f ${TAR_FILE})

archivedynamic: bits
	@(cd ${RELEASE_DIR}; \
	echo ""; \
	echo "-->Creating tar file"; \
	rm -f ${TAR_FILE} ${GZIP_FILE}; \
	tar cf ${TAR_FILE} ${TAR_ITEMS_LIB}; \
	echo "-->Creating gzip file"; \
	gzip -c ${TAR_FILE} > ${GZIP_FILE}; \
	rm -f ${TAR_FILE})

rpm:
	echo ""
	echo "-->Creating rpm file"
	@rm -rf ${RPM_DIR}
	@mkdir ${RPM_DIR}
	@mkdir ${RPM_SOURCES_DIR}
	@touch ${RPM_SOURCES_DIR}/mulberry.tar
	@mkdir ${RPM_BUILD_DIR}
	@rpm -bb ./mulberry.spec

upload:
	@(${MKDIR_CMD}; \
	${SCP_CMD}; \
	${GPG_CMD})

clean:
	@rm -rf ${RELEASE_DIR}
	@rm -rf ${RPM_DIR}
