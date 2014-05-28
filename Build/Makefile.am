###############################################################################
# THE LIBRARIES TO BUILD
###############################################################################
ACLOCAL_AMFLAGS                                             =   -I m4

# The list of header files that belong to the library (to be installed later)
HEADER_FILES                                                =   ../Includes/libARUpdater/ARUPDATER_Manager.h        \
                                                                ../Includes/libARUpdater/ARUPDATER_Downloader.h     \
                                                                ../Includes/libARUpdater/ARUPDATER_Uploader.h       \
                                                                ../Includes/libARUpdater/ARUPDATER_Error.h          \
                                                                ../Includes/libARUpdater/ARUpdater.h

# The sources to add to the library and to add to the source distribution
SOURCE_FILES                                                =   $(HEADER_FILES)\
                                                                ../Sources/ARUPDATER_Manager.c      \
                                                                ../Sources/ARUPDATER_Manager.h      \
                                                                ../Sources/ARUPDATER_Downloader.c   \
                                                                ../Sources/ARUPDATER_Downloader.h   \
                                                                ../Sources/ARUPDATER_Uploader.c     \
                                                                ../Sources/ARUPDATER_Uploader.h     \
                                                                ../Sources/ARUPDATER_Plf.c          \
                                                                ../Sources/ARUPDATER_Plf.h          \
                                                                ../Sources/ARUPDATER_Utils.c        \
                                                                ../Sources/ARUPDATER_Utils.h        \
                                                                ../Sources/ARUPDATER_Error.c
                                
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


check_PROGRAMS                                              =   libarupdater_autoTest
# bin_PROGRAMS                                              =   libarupdater_autoTest

libarupdater_autoTest_SOURCES                               =   ../TestBench/Linux/autoTest.c

if DEBUG_MODE
libarupdater_autoTest_LDADD                                 =   libarupdater_dbg.la \
                                                                -larsal_dbg         \
                                                                -larutils_dbg       \
                                                                -lardatatransfer_dbg\
                                                                -lcurl
else
libarupdater_autoTest_LDADD                                 =   libarupdater.la     \
                                                                -larsal             \
                                                                -larutils           \
                                                                -lardatatransfer    \
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

