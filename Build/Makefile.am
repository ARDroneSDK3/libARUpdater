###############################################################################
# THE LIBRARIES TO BUILD
###############################################################################
ACLOCAL_AMFLAGS                                             =   -I m4

# The list of header files that belong to the library (to be installed later)
HEADER_FILES                                                =   ../Includes/libARUpdater/ARUPDATER_Manager.h \
                                                                ../Includes/libARUpdater/ARUPDATER_Error.h \
                                                                ../Includes/libARUpdater/ARUpdater.h

# The sources to add to the library and to add to the source distribution
SOURCE_FILES                                                =   $(HEADER_FILES)\
                                                                ../Sources/ARUPDATER_Manager.c \
                                                                ../Sources/ARUPDATER_Manager.h
                                
if HAVE_OBJECTIVE_C

endif

# The library names to build (note we are building static and shared libs)
if DEBUG_MODE
lib_LTLIBRARIES                                             =   libarupdater_dbg.la
libarupdater_dbg_ladir                                 =   $(includedir)/libARUpdater
libarupdater_dbg_la_HEADERS                            =   $(HEADER_FILES)
libarupdater_dbg_la_SOURCES                            =   $(SOURCE_FILES)
else
lib_LTLIBRARIES                                             =   libarupdater.la
libarupdater_ladir                                     =   $(includedir)/libARUpdater
libarupdater_la_HEADERS                                =   $(HEADER_FILES)
libarupdater_la_SOURCES                                =   $(SOURCE_FILES)
endif


check_PROGRAMS                                              =   ../TestBench/Linux/autoTest
# bin_PROGRAMS                                              =   ../TestBench/Linux/autoTest

___TestBench_Linux_autoTest_SOURCES                         =   ../TestBench/Linux/autoTest.c \
                                                                ../TestBench/Linux/test_manager.c

if DEBUG_MODE
___TestBench_Linux_autoTest_LDADD                           =   libarupdater_dbg.la \
                                                                -larsal_dbg         \
                                                                -larutils_dbg       \
                                                                -lcurl
else
___TestBench_Linux_autoTest_LDADD                           =   libarupdater.la     \
                                                                -larsal             \
                                                                -larutils           \
                                                                -lcurl
endif


CLEAN_FILES                                                 =   libarupdater.la       \
                                                                libarupdater_dbg.la

EXTRA_DIST                                                  =   bootstrap

# Ensure the distribution of the doxygen configuration file
# Doxygen support
if HAVE_DOXYGEN
include doxygen.am
EXTRA_DIST                                                  +=  doxygen.ac  \
                                                                doxygen.am  \
                                                                $(DX_CONFIG)
endif

