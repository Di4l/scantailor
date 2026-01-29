#===============================================================================
# Main UI Files
#===============================================================================
# Qt Designer UI files for the main application windows and dialogs.
# These are compiled and wrapped by Qt5 during the build process.

#===============================================================================
# UI Files (Qt Designer)
#===============================================================================

FILE(GLOB UI_FILES "*.ui")
QT5_WRAP_UI(UI_SOURCES ${UI_FILES})

SOURCE_GROUP("UI Files" FILES ${UI_FILES})
SOURCE_GROUP("Generated" FILES ${UI_SOURCES})
