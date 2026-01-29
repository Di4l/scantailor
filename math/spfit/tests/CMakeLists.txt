#===============================================================================
# Spline Fitting Tests
#===============================================================================
# Unit tests for the spline fitting (SPFIT) library.
# These tests validate the correctness of spline approximation algorithms.

#===============================================================================
# Configure Directories
#===============================================================================

INCLUDE_DIRECTORIES(BEFORE ..)

#===============================================================================
# Source Files
#===============================================================================

SET(SOURCES
    ${CMAKE_SOURCE_DIR}/tests/main.cpp
    TestSqDistApproximant.cpp
)

SOURCE_GROUP("Sources" FILES ${SOURCES})

#===============================================================================
# Link Libraries
#===============================================================================

SET(LIBS
    math
    Qt5::Core
    ${EXTRA_LIBS}
)

#===============================================================================
# Build Target
#===============================================================================

ADD_EXECUTABLE(spfit_tests ${SOURCES})
TARGET_LINK_LIBRARIES(spfit_tests ${LIBS})
