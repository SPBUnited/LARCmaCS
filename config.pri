CONFIG -= warn_off
CONFIG -= debug_and_release debug_and_release_target
CONFIG += qt thread
CONFIG += console


#win32:!mingw {PROTOC_BIN_DIR=$${VCPKG_DIR}/bin/}
#PROTOC_VER=$$system($${PROTOC_BIN_DIR}protoc --version)
#message("Compiler:$$QMAKE_CXX from $$(PATH) with protoc $$PROTOC_VER")
!contains(QT_ARCH, i386):CONFIG += amd64

mingw {
  amd64 { message(MINGW64) }
  else  { message(MINGW32) }
} else: msvc {
  amd64 { message(MSVC64) }
  else  { message(MSVC32) }
} else: unix {
   message("Building on unix")
} else  {
  error("Unsupported platform. Check config.pri file")
  error($$CONFIG)
}

DEBUG_SUFFIX =
RELEASE_SUFFIX =
win32 {
  DEBUG_SUFFIX = d
  DEBUG_PREFIX = debug/
  !amd64: BIT = 32
  amd64: BIT = 64
}

CONFIG(debug, debug|release) {
  SUFFIX_STR = $${DEBUG_SUFFIX}
  PREFIX_STR = $${DEBUG_PREFIX}
}
else {
  SUFFIX_STR = $${RELEASE_SUFFIX}
  PREFIX_STR = $${RELEASE_PREFIX}
}

DESTDIR = bin$${SUFFIX_STR}

unix {
  DEFINES += UNIX
  DEFINES += LINUX
  DEFINES += _GNU_SOURCE
}

win32:msvc {
  DEFINES += _CRT_SECURE_NO_WARNINGS
  DEFINES += WIN32_LEAN_AND_MEAN
  DEFINES += NOMINMAX

  QMAKE_CXXFLAGS += /wd4250
  QMAKE_CXXFLAGS += /Fd$(IntDir)
  QMAKE_CXXFLAGS += /D__STDC_LIMIT_MACROS

  #enable SSE2
  #QMAKE_CXXFLAGS += /arch:SSE2

  #use multithread (non-debug) dll runtime
  #QMAKE_CXXFLAGS_DEBUG += /MD

  #enable debugging in release build
  #QMAKE_CXXFLAGS_RELEASE += /Zi
  #QMAKE_LFLAGS_RELEASE += /DEBUG
  QMAKE_CXXFLAGS += /MP
} else:mingw {
  QMAKE_CXXFLAGS += -Wno-unused-parameter -Wno-format
  QMAKE_CXXFLAGS += -fsanitize=undefined -fsanitize-undefined-trap-on-error \
		 -fsanitize-address-use-after-scope -ftrapv -fstack-protector-all
  QMAKE_LFLAGS += -fstack-protector-all
}

old_ssl: DEFINES += OLD_SSL_PROTO
