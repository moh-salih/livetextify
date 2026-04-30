find_package(Qt6 REQUIRED COMPONENTS Quick Multimedia Network QuickControls2 Sql Gui)

find_package(spdlog REQUIRED)
find_package(hnswlib CONFIG REQUIRED)
find_package(nlohmann_json CONFIG REQUIRED)

file(GLOB_RECURSE liveTextifyBackend_SOURCES "liveTextify/*.cpp" "liveTextify/*.h")

qt_add_library(liveTextifyBackend STATIC ${liveTextifyBackend_SOURCES} )


include(FetchContent)

FetchContent_Declare(
    QtWhisper
    GIT_REPOSITORY https://github.com/moh-salih/QtWhisper.git
    GIT_TAG        dev
    GIT_SHALLOW    TRUE
)

FetchContent_Declare(
    QtLlama
    GIT_REPOSITORY https://github.com/moh-salih/QtLlama.git
    GIT_TAG        dev
    GIT_SHALLOW    TRUE
)

FetchContent_Declare(
    QtAudioCapture
    GIT_REPOSITORY https://github.com/moh-salih/QtAudioCapture.git
    GIT_TAG        dev
    GIT_SHALLOW    TRUE
)

FetchContent_Declare(
    QtRag
    GIT_REPOSITORY https://github.com/moh-salih/QtRag.git
    GIT_TAG        dev
    GIT_SHALLOW    TRUE
)

FetchContent_MakeAvailable(QtWhisper QtLlama QtAudioCapture QtRag)


target_link_libraries(liveTextifyBackend PUBLIC
    Qt6::Quick Qt6::Multimedia Qt6::Network Qt6::QuickControls2 Qt6::Sql Qt6::Gui
    spdlog::spdlog
    QtWhisper::QtWhisper
    QtLlama::QtLlama
    QtAudioCapture::QtAudioCapture
    QtRag::QtRag
    hnswlib::hnswlib
    nlohmann_json::nlohmann_json
)


target_include_directories(liveTextifyBackend PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}")


set_target_properties(liveTextifyBackend 
            PROPERTIES  
    CXX_STANDARD            23
    CXX_STANDARD_REQUIRED   ON
    CXX_EXTENSIONS          OFF
    AUTOMOC                 ON
)



