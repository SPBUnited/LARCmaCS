include(config.pri)
include(personal.pri)
#check personal info

msvc {
  isEmpty(VCPKG_DIR): error("To use MSVC you need to specify the path to VCPKG_DIR!")
}
mingw {
  isEmpty(MSYS_DIR): error("To use MINGW you need to specify the path to MSYS_DIR!")
}

isEmpty(MATLAB_DIR): error("You need to specify the path to MATLAB_DIR!")
include(proto/proto.pri)

SHARED_DIR = $$PWD/macsCommon

include (LARCmaCS/LARCmaCS.pri)


#Need this???
#where to place built objects
OBJECTS_DIR = build/LARCmaCS/obj$${SUFFIX_STR}

#where to place temporary moc sources
MOC_DIR = build/LARCmaCS/moc

#where to place auto-generated UI files
UI_DIR = build/LARCmaCS/ui

#where to place intermediate resource files
RCC_DIR = build/LARCmaCS/resources

defineTest(copyToDestdir) {
	files = $$1

	for(FILE, files) {
		DDIR = $$DESTDIR
		# Replace slashes in paths with backslashes for Windows
		win32:FILE ~= s,/,\\,g
		win32:DDIR ~= s,/,\\,g
		QMAKE_POST_LINK += $$QMAKE_COPY $$quote($$FILE) $$quote($$DDIR) $$escape_expand(\\n\\t)
	}

	export(QMAKE_POST_LINK)
}

INIT_FILES = \
    $$PWD/resources/LARCmaCS.exe.embed.manifest \
    $$PWD/resources/gamepads.txt \
    $$PWD/resources/help.png \
    $$PWD/resources/help.html

copyToDestdir($$INIT_FILES)

MATLAB_BIN = $${MATLAB_DIR}\..\bin\win$$BIT
msvc: OTHER_BIN = '$$[QT_INSTALL_BINS];$${PROTO_DIR}/$${PREFIX_STR}bin/'
mingw: OTHER_BIN = '$${MSYS_DIR}\mingw$${BIT}\bin'
QMAKE_SUBSTITUTES += LARCmaCS.cmd.in
OTHER_FILES += $$QMAKE_SUBSTITUTES
