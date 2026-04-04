
set(libraries glad glfw itugl ${APPLE_LIBRARIES})

file(GLOB_RECURSE target_inc "*.h" )
file(GLOB_RECURSE target_src "*.cpp" )
list(FILTER target_src EXCLUDE REGEX ".*/out/build/.*")
list(FILTER target_src EXCLUDE REGEX ".*/CMakeFiles/.*")
#file(GLOB_RECURSE target_src "${CMAKE_CURRENT_SOURCE_DIR}/*.cpp")
file(GLOB_RECURSE shaders "*.vert" "*.frag" "*.geom" "*.glsl")
source_group("Shaders" FILES ${shaders})


add_executable(${TARGETNAME} ${target_inc} ${target_src} ${shaders} )
#"exercise03 (2)/GearsApplication.cpp" "exercise03 (2)/GearsApplication.h" "exercise03 (2)/main.cpp" "exercise03 (2)/CMakeFiles/3.30.5/CompilerIdC/CMakeCCompilerId.c" "exercise03 (2)/CMakeFiles/3.30.5/CompilerIdCXX/CMakeCXXCompilerId.cpp" "exercise03 (2)/exercise03/GearsApplication.cpp" "exercise03 (2)/exercise03/GearsApplication.h" "exercise03 (2)/exercise03/main.cpp" "exercise03 (2)/exercise03/CMakeFiles/3.30.5/CompilerIdC/CMakeCCompilerId.c" "exercise03 (2)/exercise03/CMakeFiles/3.30.5/CompilerIdCXX/CMakeCXXCompilerId.cpp"
target_link_libraries(${TARGETNAME} ${libraries})
