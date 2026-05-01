find_package(Qt6 REQUIRED COMPONENTS Quick Qml Multimedia Network QuickControls2 Sql Gui)

find_package(spdlog REQUIRED)
find_package(hnswlib CONFIG REQUIRED)
find_package(nlohmann_json CONFIG REQUIRED)

file(GLOB_RECURSE liveTextifyBackend_SOURCES "liveTextify/*.cpp" "liveTextify/*.h")

qt_add_library(liveTextifyBackend STATIC ${liveTextifyBackend_SOURCES})


target_link_libraries(liveTextifyBackend PUBLIC
    Qt6::Quick Qt6::Multimedia Qt6::Network Qt6::QuickControls2 Qt6::Sql Qt6::Gui Qt6::Qml
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



