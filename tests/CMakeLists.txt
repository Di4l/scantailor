#===============================================================================
# General Tests
#===============================================================================
# Unit tests for general-purpose utilities and algorithms used throughout
# the project, including filename ordering and content span detection.

#===============================================================================
# Configure Directories
#===============================================================================

INCLUDE_DIRECTORIES(BEFORE ..)

#===============================================================================
# Source Files
#===============================================================================

SET(SOURCES
    main.cpp
    TestContentSpanFinder.cpp
    TestSmartFilenameOrdering.cpp
    TestMatrixCalc.cpp
    ../ContentSpanFinder.cpp
    ../ContentSpanFinder.h
    ../SmartFilenameOrdering.cpp
    ../SmartFilenameOrdering.h
)

SOURCE_GROUP("Sources" FILES ${SOURCES})

#===============================================================================
# Link Libraries
#===============================================================================

SET(LIBS
    imageproc
    math
    Qt5::Gui
    Qt5::Core
    ${EXTRA_LIBS}
)

#===============================================================================
# Build Target
#===============================================================================

ADD_EXECUTABLE(tests ${SOURCES})
TARGET_LINK_LIBRARIES(tests ${LIBS})
