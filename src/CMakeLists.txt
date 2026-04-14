file(GLOB_RECURSE liveTextifyBackend_SOURCES "liveTextify/*.cpp" "liveTextify/*.h")

qt_add_library(liveTextifyBackend STATIC ${liveTextifyBackend_SOURCES} )


# Target specific properties
set_target_properties(liveTextifyBackend PROPERTIES AUTOMOC ON)

target_link_libraries(liveTextifyBackend PUBLIC
        Qt6::Quick Qt6::Multimedia Qt6::Network Qt6::Test Qt6::QuickControls2 Qt6::Sql Qt6::Gui
    spdlog::spdlog
    whisper
    llama
    hnswlib::hnswlib
    nlohmann_json::nlohmann_json
)


target_include_directories(liveTextifyBackend PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}")
message("Project include path is: ${CMAKE_CURRENT_SOURCE_DIR}")
set_target_properties(liveTextifyBackend PROPERTIES  CXX_STANDARD 23
    CXX_STANDARD_REQUIRED ON
    CXX_EXTENSIONS OFF
)



