#===============================================================================
# Page Layout Filter
#===============================================================================
# This filter handles page layout configuration and margin adjustment.
# It provides tools for defining page boundaries and layout parameters.

PROJECT("Page Layout Filter")

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
    CacheDrivenTask.h
    Task.h
    qt/CacheDrivenTask.cpp
    qt/Filter.h
    qt/Utils.cpp
    qt/Utils.h
    qt/ImageView.cpp
    qt/ImageView.h
    qt/Filter.cpp
    qt/OptionsWidget.cpp
    qt/OptionsWidget.h
    qt/Task.cpp
    qt/Params.cpp
    qt/Params.h
    qt/Settings.cpp
    qt/Settings.h
    qt/Thumbnail.cpp
    qt/Thumbnail.h
    qt/ApplyDialog.cpp
    qt/ApplyDialog.h
    qt/Alignment.cpp
    qt/Alignment.h
    qt/OrderByWidthProvider.cpp
    qt/OrderByWidthProvider.h
    qt/OrderByHeightProvider.cpp
    qt/OrderByHeightProvider.h
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

ADD_LIBRARY(page_layout STATIC ${SOURCES} ${UI_SOURCES})

target_include_directories(page_layout PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}
)

#===============================================================================
# Translation Sources
#===============================================================================

TRANSLATION_SOURCES(scantailor ${SOURCES} ${UI_FILES})