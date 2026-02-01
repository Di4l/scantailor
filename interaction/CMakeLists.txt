#===============================================================================
# Interaction Library
#===============================================================================
# This library provides interactive mouse and keyboard handling for UI elements
# such as draggable objects, splines, and zoom operations.

PROJECT(interaction)

#===============================================================================
# Configure Directories
#===============================================================================

INCLUDE_DIRECTORIES("${CMAKE_CURRENT_BINARY_DIR}")

#===============================================================================
# Source Files
#===============================================================================

SET(SOURCES
    DraggablePoint.cpp
    DraggablePoint.h
    InteractionState.cpp
    InteractionState.h
)

SET(GUI_SOURCES
    DraggableObject.h
    DraggableLineSegment.cpp
    DraggableLineSegment.h
    ObjectDragHandler.cpp
    ObjectDragHandler.h
    InteractionHandler.cpp
    InteractionHandler.h
    DragHandler.cpp
    DragHandler.h
    DragWatcher.cpp
    DragWatcher.h
    ZoomHandler.cpp
    ZoomHandler.h
    InteractiveXSpline.cpp
    InteractiveXSpline.h
)

SET(SOURCES ${SOURCES} gui/${GUI_SOURCES})

SOURCE_GROUP(Sources FILES ${SOURCES})

#===============================================================================
# Qt5 Meta-Object Compilation
#===============================================================================
# Qt5 uses automatic moc, no need for explicit QT5_WRAP_CPP

SET(CMAKE_AUTOMOC ON)

#===============================================================================
# Build Target
#===============================================================================

ADD_LIBRARY(interaction STATIC ${SOURCES})

#===============================================================================
# Translation Sources
#===============================================================================

TRANSLATION_SOURCES(scantailor ${SOURCES})