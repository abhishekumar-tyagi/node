#***************************************************************************
# Project        ___       __ _ _ __ ___  ___ 
#               / __|____ / _` | '__/ _ \/ __|
#              | (_|_____| (_| | | |  __/\__ \
#               \___|     \__,_|_|  \___||___/
#
exec_prefix=@CMAKE_INSTALL_FULL_BINDIR@
libdir=@CMAKE_INSTALL_FULL_LIBDIR@
includedir=@CMAKE_INSTALL_FULL_INCLUDEDIR@

Name: c-ares
URL: https://c-ares.org/
Description: asynchronous DNS lookup library
Version: @CARES_VERSION@
Requires: 
Requires.private: 
Cflags: -I${includedir} @CPPFLAG_CARES_STATICLIB@
Libs: -L${libdir} -lcares
Libs.private: @CARES_PRIVATE_LIBS@
