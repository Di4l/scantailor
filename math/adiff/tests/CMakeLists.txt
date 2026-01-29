#===============================================================================
# Automatic Differentiation Tests
#===============================================================================
# Unit tests for the automatic differentiation (ADIFF) library.
# These tests validate the correctness of Hessian matrix calculations
# and other automatic differentiation functionality.

#===============================================================================
# Configure Directories
#===============================================================================

INCLUDE_DIRECTORIES(BEFORE ..)

#===============================================================================
# Source Files
#===============================================================================

SET(SOURCES
    ${CMAKE_SOURCE_DIR}/tests/main.cpp
    TestHessians.cpp
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

ADD_EXECUTABLE(adiff_tests ${SOURCES})
TARGET_LINK_LIBRARIES(adiff_tests ${LIBS})
