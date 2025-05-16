set(TARGET_NAME QyhsRuntime)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(json_include ${THIRD_PARTY_DIR}/json11)
set(json_lib_dir ${THIRD_PARTY_DIR}/lib/json11)
add_library(json11 ${json_include}/json11.cpp)
set_target_properties(json11 PROPERTIES FOLDER "ThirdParty/json11")

file(GLOB_RECURSE HEADER_FILES "*.h")
file(GLOB_RECURSE SOURCE_FILES "*.cpp" "*.c")
source_group(TREE "${CMAKE_CURRENT_SOURCE_DIR}" FILES ${HEADER_FILES} ${SOURCE_FILES})

add_library(${TARGET_NAME} ${HEADER_FILES} ${SOURCE_FILES})

set_target_properties(${TARGET_NAME} PROPERTIES CXX_STANDARD 17)
set_target_properties(${TARGET_NAME} PROPERTIES FOLDER "Engine")

target_compile_options(${TARGET_NAME} PUBLIC "$<$<COMPILE_LANG_AND_ID:CXX,MSVC>:/permissive->")
target_compile_options(${TARGET_NAME} PUBLIC "$<$<COMPILE_LANG_AND_ID:CXX,MSVC>:/WX->")

if(MSVC)
add_definitions(-DUNICODE -D_UNICODE)
endif()

#if you want to use volk for development
add_definitions(-DUSE_VOLK)

source_group(TREE "${CMAKE_CURRENT_SOURCE_DIR}" FILES ${HEADER_FILES} ${SOURCE_FILES})

target_link_libraries(${TARGET_NAME} PUBLIC glfw)
target_link_libraries(${TARGET_NAME} PUBLIC spdlog::spdlog)
target_link_libraries(${TARGET_NAME} PRIVATE $<BUILD_INTERFACE:json11>)
target_link_libraries(${TARGET_NAME} PRIVATE tinyobjloader stb)
target_link_libraries(${TARGET_NAME} PUBLIC ${vulkan_lib})

set_target_properties(${TARGET_NAME} PROPERTIES CXX_STANDARD 17)
set_target_properties(${TARGET_NAME} PROPERTIES FOLDER "Engine")

target_include_directories(${TARGET_NAME} PUBLIC ${vulkan_include})
target_include_directories(${TARGET_NAME} PUBLIC ${THIRD_PARTY_DIR}/glm)
target_include_directories(${TARGET_NAME} PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
target_include_directories(${TARGET_NAME} PUBLIC ${json_include})
target_include_directories(${TARGET_NAME} PUBLIC ${THIRD_PARTY_DIR})
target_include_directories(${TARGET_NAME} PUBLIC ${ENGINE_SOURCE_CODE_DIR})
target_include_directories(${TARGET_NAME} PUBLIC ${ENGINE_ROOT_DIR})

set(QYHS_RUNTIME_HEADS ${HEADER_FILES} PARENT_SCOPE)