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
    gui/DraggableObject.h
    gui/DraggableLineSegment.cpp
    gui/DraggableLineSegment.h
    gui/ObjectDragHandler.cpp
    gui/ObjectDragHandler.h
    gui/InteractionHandler.cpp
    gui/InteractionHandler.h
    gui/DragHandler.cpp
    gui/DragHandler.h
    gui/DragWatcher.cpp
    gui/DragWatcher.h
    gui/ZoomHandler.cpp
    gui/ZoomHandler.h
    gui/InteractiveXSpline.cpp
    gui/InteractiveXSpline.h
)

SET(SOURCES ${SOURCES} ${GUI_SOURCES})

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