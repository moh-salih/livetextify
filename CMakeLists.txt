cmake_minimum_required(VERSION 3.16)
project("LiveTextify: Voice to Text to Insight" VERSION 1.0.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)


# Build Options
option(BUILD_TESTING "Build Unit Tests" OFF)

# Standard Qt Setup
find_package(Qt6 REQUIRED COMPONENTS Quick Multimedia Network Test QuickControls2 Sql Gui)
find_package(spdlog REQUIRED)
find_package(whisper REQUIRED)
find_package(llama REQUIRED)
find_package(hnswlib CONFIG REQUIRED)
find_package(nlohmann_json CONFIG REQUIRED)


qt_standard_project_setup(REQUIRES 6.8)


# Define the executable first
qt_add_executable(liveTextify src/main.cpp
    .github/workflows/release.yml)

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


# Platform specific properties
set_target_properties(liveTextify PROPERTIES
    MACOSX_BUNDLE_GUI_IDENTIFIER "com.salih.livetextify"
    MACOSX_BUNDLE TRUE
    WIN32_EXECUTABLE TRUE
)


if(ANDROID)
    set_target_properties(liveTextify
        PROPERTIES
        QT_ANDROID_APP_NAME "LiveTextify: Voice to Text to Insight"
        QT_ANDROID_PACKAGE_NAME "com.salih.livetextify"
        QT_ANDROID_PACKAGE_SOURCE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/android
    )

    qt_add_android_permission(liveTextify   NAME android.permission.RECORD_AUDIO)
    # qt_add_android_permission(liveTextify NAME android.permission.READ_EXTERNAL_STORAGE)
    # qt_add_android_permission(liveTextify NAME android.permission.WRITE_EXTERNAL_STORAGE)
    # qt_add_android_permission(liveTextify NAME android.permission.MANAGE_EXTERNAL_STORAGE)
    qt_add_android_permission(liveTextify   NAME android.permission.INTERNET)

    # Openssl support
    include("$ENV{ANDROID_SDK_ROOT}/android_openssl/android_openssl.cmake")
    add_android_openssl_libraries(liveTextify)
endif()





# Installation logic
include(GNUInstallDirs)

install(TARGETS liveTextify
    BUNDLE  DESTINATION .
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)

# Deploy Qt runtime dependencies (Qt 6.5+ built-in)
qt_generate_deploy_qml_app_script(
    TARGET liveTextify
    OUTPUT_SCRIPT deploy_script
    MACOS_BUNDLE_POST_BUILD
    NO_UNSUPPORTED_PLATFORM_ERROR
    DEPLOY_USER_QML_MODULES_ON_UNSUPPORTED_PLATFORM
)
install(SCRIPT ${deploy_script})

# Install whisper model data directory if it ships any (optional)
# install(DIRECTORY ${CMAKE_SOURCE_DIR}/models/ DESTINATION ${CMAKE_INSTALL_DATADIR}/livetextify/models)

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

