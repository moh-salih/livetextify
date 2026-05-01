cmake_minimum_required(VERSION 3.21)
project("LiveTextify: Voice to Text to Insight" VERSION 1.0.0 LANGUAGES CXX)


# Build Options
option(BUILD_TESTING "Build Unit Tests" OFF)

# Standard Qt Setup
find_package(Qt6 REQUIRED COMPONENTS Core)
qt_standard_project_setup(REQUIRES 6.8)


# Define the executable first
qt_add_executable(liveTextify src/main.cpp)

# Add subdirectories
add_subdirectory(src)
add_subdirectory(qml)


# App Resources - Recursively collect all assets from the resources directory
file(GLOB_RECURSE APP_RESOURCES CONFIGURE_DEPENDS "resources/*")
qt_add_resources(liveTextify PREFIX "/" BASE "resources" FILES ${APP_RESOURCES})



if(BUILD_TESTING)
    enable_testing()
    add_subdirectory(tests)
endif()

# Link dependencies of the application
target_link_libraries(liveTextify PRIVATE liveTextifyBackend liveTextifyFrontendplugin)
qt_import_qml_plugins(liveTextify)

# ==============================================================================
# PLATFORM-SPECIFIC CONFIGURATION
# ==============================================================================

# --- Base properties (all platforms) ---
set(liveTextify_PROPERTIES
    WIN32_EXECUTABLE                    TRUE
    AUTOMOC                             ON
    CXX_STANDARD                        23
    CXX_STANDARD_REQUIRED               ON
)

# --- macOS Desktop ---
if(APPLE AND NOT IOS)
    list(APPEND liveTextify_PROPERTIES
        MACOSX_BUNDLE                       TRUE
        MACOSX_BUNDLE_BUNDLE_NAME           "LiveTextify"
        MACOSX_BUNDLE_GUI_IDENTIFIER        "com.salih.livetextify"
        MACOSX_BUNDLE_BUNDLE_VERSION        "${PROJECT_VERSION}"
        MACOSX_BUNDLE_SHORT_VERSION_STRING  "${PROJECT_VERSION}"
        MACOSX_BUNDLE_INFO_PLIST            "${CMAKE_CURRENT_SOURCE_DIR}/platform/macos/Info.plist.in"
    )

    if(NOT DEFINED CMAKE_OSX_DEPLOYMENT_TARGET)
        set(CMAKE_OSX_DEPLOYMENT_TARGET "13.0" CACHE STRING "Minimum macOS version")
    endif()

    # Optional: Add code signing entitlements if file exists
    if(EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/platform/macos/entitlements.plist")
        list(APPEND liveTextify_PROPERTIES
            XCODE_ATTRIBUTE_CODE_SIGN_ENTITLEMENTS "${CMAKE_CURRENT_SOURCE_DIR}/platform/macos/entitlements.plist"
        )
    endif()
endif()

# --- iOS ---
if(IOS)
    list(APPEND liveTextify_PROPERTIES
        MACOSX_BUNDLE                       TRUE
        MACOSX_BUNDLE_BUNDLE_NAME           "LiveTextify"
        MACOSX_BUNDLE_GUI_IDENTIFIER        "com.salih.livetextify"
        MACOSX_BUNDLE_BUNDLE_VERSION        "${PROJECT_VERSION}"
        MACOSX_BUNDLE_SHORT_VERSION_STRING  "${PROJECT_VERSION}"
        MACOSX_BUNDLE_INFO_PLIST            "${CMAKE_CURRENT_SOURCE_DIR}/platform/macos/Info.plist.in"
    )

    if(NOT DEFINED CMAKE_OSX_DEPLOYMENT_TARGET)
        set(CMAKE_OSX_DEPLOYMENT_TARGET "15.0" CACHE STRING "Minimum iOS version")
    endif()
endif()

# --- Android ---
if(ANDROID)
    list(APPEND liveTextify_PROPERTIES
        QT_ANDROID_APP_NAME             "LiveTextify: Voice to Text to Insight"
        QT_ANDROID_PACKAGE_NAME         "com.salih.livetextify"
        QT_ANDROID_PACKAGE_SOURCE_DIR   "${CMAKE_CURRENT_SOURCE_DIR}/platform/android"
    )

    # Add permissions
    qt_add_android_permission(liveTextify NAME android.permission.RECORD_AUDIO)
    qt_add_android_permission(liveTextify NAME android.permission.INTERNET)

    # OpenSSL support
    include("$ENV{ANDROID_SDK_ROOT}/android_openssl/android_openssl.cmake")
    add_android_openssl_libraries(liveTextify)
endif()

# --- Apply all properties at once ---
set_target_properties(liveTextify PROPERTIES ${liveTextify_PROPERTIES})







# Installation logic
include(GNUInstallDirs)

install(TARGETS liveTextify
    BUNDLE  DESTINATION .
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)

# Deploy Qt runtime dependencies
qt_generate_deploy_qml_app_script(
    TARGET liveTextify
    OUTPUT_SCRIPT deploy_script
    MACOS_BUNDLE_POST_BUILD
    NO_UNSUPPORTED_PLATFORM_ERROR
    DEPLOY_USER_QML_MODULES_ON_UNSUPPORTED_PLATFORM
)
install(SCRIPT ${deploy_script})

# Install desktop entry and icon for Linux
if(UNIX AND NOT APPLE AND NOT ANDROID)
    install(FILES
        ${CMAKE_SOURCE_DIR}/packaging/linux/liveTextify.desktop
        DESTINATION ${CMAKE_INSTALL_DATADIR}/applications
    )
    install(FILES
        ${CMAKE_SOURCE_DIR}/packaging/linux/liveTextify.png
        DESTINATION ${CMAKE_INSTALL_DATADIR}/icons/hicolor/256x256/apps
    )
endif()

