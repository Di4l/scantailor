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

set(CMAKE_AUTOUIC_SEARCH_PATHS
    ${CMAKE_CURRENT_SOURCE_DIR}/qt/ui
)

#===============================================================================
# UI Files (Qt Designer)
#===============================================================================

FILE(GLOB UI_FILES "qt/ui/*.ui")
QT5_WRAP_UI(UI_SOURCES ${UI_FILES})
SET_SOURCE_FILES_PROPERTIES(${UI_SOURCES} PROPERTIES GENERATED TRUE)

SOURCE_GROUP("UI Files" FILES ${UI_FILES})
SOURCE_GROUP("Generated" FILES ${UI_SOURCES})

#===============================================================================
# Source Files
#===============================================================================

SET(SOURCES
    Filter.h
    Task.h
    CacheDrivenTask.cpp
    CacheDrivenTask.h

    qt/ImageView.cpp
    qt/ImageView.h
    qt/Thumbnail.cpp
    qt/Thumbnail.h
    qt/Filter.cpp
    qt/OptionsWidget.cpp
    qt/OptionsWidget.h
    qt/Settings.cpp
    qt/Settings.h
    qt/Task.cpp
    qt/Dependencies.cpp
    qt/Dependencies.h
    qt/Params.cpp
    qt/Params.h
    qt/ApplyDialog.cpp
    qt/ApplyDialog.h
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

target_include_directories(deskew PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}
)

#===============================================================================
# Translation Sources
#===============================================================================

TRANSLATION_SOURCES(scantailor ${SOURCES} ${UI_FILES})
