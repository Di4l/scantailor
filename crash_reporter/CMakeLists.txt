#===============================================================================
# Crash Reporter - Windows Only Crash Handling Utility
#===============================================================================

PROJECT(CrashReporter)

ADD_DEFINITIONS(-D_UNICODE)

INCLUDE_DIRECTORIES(google-breakpad)

#===============================================================================
# HELPER MACRO
#===============================================================================

MACRO(LIST_ITEMS_PREPEND LIST PREFIX)
SET(tmp_list_)
FOREACH(item ${${LIST}})
LIST(APPEND tmp_list_ "${PREFIX}${item}")
ENDFOREACH(item)
SET(${LIST} ${tmp_list_})
ENDMACRO(LIST_ITEMS_PREPEND)

#===============================================================================
# SOURCE FILES - Google Breakpad Components
#===============================================================================

# Exception handler from Google Breakpad
SET(eh_sources exception_handler.cc exception_handler.h)
LIST_ITEMS_PREPEND(eh_sources google-breakpad/client/windows/handler/)

# Crash generation client from Google Breakpad
SET(cg_sources crash_generation_client.cc crash_generation_client.h)
LIST_ITEMS_PREPEND(cg_sources google-breakpad/client/windows/crash_generation/)

# Common utilities from Google Breakpad
SET(common_sources guid_string.cc guid_string.h)
LIST_ITEMS_PREPEND(common_sources google-breakpad/common/windows/)

#===============================================================================
# BUILD TARGET - Crash Handler Library (Windows Only)
#===============================================================================

IF(ENABLE_CRASH_REPORTER)
ADD_LIBRARY(crash_handler ${eh_sources} ${cg_sources} ${common_sources})
ENDIF(ENABLE_CRASH_REPORTER)

#===============================================================================
# SOURCE FILES - Crash Reporter Application
#===============================================================================

SET(
crash_reporter_sources
 qt/main.cpp
 qt/CrashReportDialog.cpp qt/CrashReportDialog.h
 qt/MultipartFormData.cpp qt/MultipartFormData.h
)

SOURCE_GROUP("Sources" FILES ${crash_reporter_sources})

#===============================================================================
# Qt5 AUTOMATIC MOC AND UI FILES
#===============================================================================

SET(CMAKE_AUTOMOC ON)

 FILE(GLOB ui_files "qt/ui/*.ui")
SOURCE_GROUP("UI Files" FILES ${ui_files})
QT5_WRAP_UI(ui_sources ${ui_files})
SET_SOURCE_FILES_PROPERTIES(${ui_sources} PROPERTIES GENERATED TRUE)

#===============================================================================
# SUBDIRECTORIES
#===============================================================================

 ADD_SUBDIRECTORY(qt/ui)
INCLUDE_DIRECTORIES("${CMAKE_CURRENT_BINARY_DIR}")

#===============================================================================
# BUILD TARGET - Crash Reporter Executable (Windows Only)
#===============================================================================

IF(ENABLE_CRASH_REPORTER)
ADD_EXECUTABLE(CrashReporter WIN32 ${crash_reporter_sources} ${ui_sources})
TARGET_LINK_LIBRARIES(
CrashReporter
Qt5::Gui Qt5::Xml Qt5::Network Qt5::Core
${PNG_LIBRARY} ${JPEG_LIBRARY}
winmm imm32 ws2_32
)
ENDIF(ENABLE_CRASH_REPORTER)
