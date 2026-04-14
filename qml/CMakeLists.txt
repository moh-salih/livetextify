qt_add_library(liveTextifyFrontend STATIC)

# Platform detection
if(ANDROID OR IOS)
    set(PLATFORM_DIR "mobile")
    message(STATUS "Building mobile UI")
else()
    set(PLATFORM_DIR "desktop")
    message(STATUS "Building desktop UI")
endif()

# Collect platform-specific QML files
file(GLOB_RECURSE PLATFORM_QML_FILES RELATIVE "${CMAKE_CURRENT_SOURCE_DIR}" "${PLATFORM_DIR}/*.qml")

file(GLOB_RECURSE SINGLETON_QML_FILES RELATIVE "${CMAKE_CURRENT_SOURCE_DIR}" "${PLATFORM_DIR}/singletons/*.qml")

# Set singleton properties
set_source_files_properties(${SINGLETON_QML_FILES} PROPERTIES QT_QML_SINGLETON_TYPE TRUE)

qt_add_qml_module(liveTextifyFrontend
    URI liveTextify
    VERSION 1.0
    QML_FILES
        ${PLATFORM_QML_FILES}
)
