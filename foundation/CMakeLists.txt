#===============================================================================
# Foundation Library - Core Utilities and Data Structures
#===============================================================================

PROJECT("Foundation library")

INCLUDE_DIRECTORIES("${CMAKE_CURRENT_BINARY_DIR}")

#===============================================================================
# SOURCE FILES
#===============================================================================

SET(
sources
# Smart pointer and memory management
NonCopyable.h IntrusivePtr.h RefCountable.h
# Utility containers and helpers
AlignedArray.h
FastQueue.h
ScopedIncDec.h ScopedDecInc.h
Span.h VirtualFunction.h FlagOps.h
# File and property management
Proximity.cpp Proximity.h
Property.h
GridLineTraverser.cpp GridLineTraverser.h
# Data structures
StaticPool.h
DynamicPool.h
NumericTraits.h
VecNT.h VecT.h
MatMNT.h MatT.h
PriorityQueue.h
Grid.h
ValueConv.h
)

SET(
gui_sources
# Qt-dependent utilities
SafeDeletingQObjectPtr.h
AutoRemovingFile.cpp AutoRemovingFile.h
PerformanceTimer.cpp PerformanceTimer.h
PropertyFactory.cpp PropertyFactory.h
PropertySet.cpp PropertySet.h
QtSignalForwarder.cpp QtSignalForwarder.h
)

SET(sources ${sources} gui/${gui_sources})

SOURCE_GROUP("Sources" FILES ${sources})

#===============================================================================
# Qt5 AUTOMATIC MOC
#===============================================================================
SET(CMAKE_AUTOMOC ON)

#===============================================================================
# BUILD TARGET
#===============================================================================

ADD_LIBRARY(foundation STATIC ${sources})
