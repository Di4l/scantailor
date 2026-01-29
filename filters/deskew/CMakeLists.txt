#===============================================================================
# Deskew Filter
#===============================================================================
# This filter handles image deskewing (rotation correction) and provides
# image visualization and parameter adjustment capabilities.

PROJECT("Deskew Filter")

#===============================================================================
# Configure Directories
#===============================================================================

INCLUDE_DIRECTORIES(BEFORE "${CMAKE_CURRENT_BINARY_DIR}")

#===============================================================================
# UI Files (Qt Designer)
#===============================================================================

FILE(GLOB UI_FILES "ui/*.ui")
QT5_WRAP_UI(UI_SOURCES ${UI_FILES})
SET_SOURCE_FILES_PROPERTIES(${UI_SOURCES} PROPERTIES GENERATED TRUE)

SOURCE_GROUP("UI Files" FILES ${UI_FILES})
SOURCE_GROUP("Generated" FILES ${UI_SOURCES})

#===============================================================================
# Source Files
#===============================================================================

SET(SOURCES
    ImageView.cpp
    ImageView.h
    Thumbnail.cpp
    Thumbnail.h
    Filter.cpp
    Filter.h
    OptionsWidget.cpp
    OptionsWidget.h
    Settings.cpp
    Settings.h
    Task.cpp
    Task.h
    CacheDrivenTask.cpp
    CacheDrivenTask.h
    Dependencies.cpp
    Dependencies.h
    Params.cpp
    Params.h
    ApplyDialog.cpp
    ApplyDialog.h
)

SOURCE_GROUP("Sources" FILES ${SOURCES})

#===============================================================================
# Qt5 Meta-Object Compilation
#===============================================================================
# Qt5 uses automatic moc, no need for explicit QT5_WRAP_CPP

SET(CMAKE_AUTOMOC ON)

#===============================================================================
# Build Target
#===============================================================================

ADD_LIBRARY(deskew STATIC ${SOURCES} ${UI_SOURCES})

#===============================================================================
# Translation Sources
#===============================================================================

TRANSLATION_SOURCES(scantailor ${SOURCES} ${UI_FILES})
