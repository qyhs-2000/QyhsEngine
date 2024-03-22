set(ENGINE_ROOT_DIR ${QYHS_ROOT_DIR}/engine)
set(THIRD_PARTY_DIR ${ENGINE_ROOT_DIR}/3rdparty)
set(ENGINE_SOURCE_CODE_DIR ${ENGINE_ROOT_DIR}/source)
set(ENGINE_ASSET_DIR "/asset")

set(DEPLOY_CONFIG_DIR "configs/deployment")
set(DEVELOP_CONFIG_DIR "configs/development")

#it make compiler compile source file with multiple process and set startup project
if(CMAKE_CXX_COMPILER_ID STREQUAL "MSVC")
	add_compile_options("/MP")
	set_property(DIRECTORY ${CMAKE_SOURCE_DIR} PROPERTY VS_STARTUP_PROJECT QyhsEditor)
endif()

if(WIN32)
	set(vulkan_lib ${THIRD_PARTY_DIR}/VulkanSDK/lib/WIN32/vulkan-1.lib)
	set(glslangValidator_executable ${THIRD_PARTY_DIR}/VulkanSDK/bin/Win32/glslangValidator.exe)
	#it is similar to #define in C++
	add_compile_definitions("QYHS_VK_LAYER_PATH=${THIRD_PARTY_DIR}/VulkanSDK/bin/Win32")
else()
	message(FATAL_ERROR "Unknown Platform")
endif()

set(vulkan_include ${THIRD_PARTY_DIR}/VulkanSDK/include)

set(SHADER_COMPILE_TARGET QyhsShaderCompile)
#add_subdirectory(shader)

add_subdirectory(3rdparty)

add_subdirectory(source/runtime)
add_subdirectory(source/editor)
add_subdirectory(source/meta_parser)

set(CODEGEN_TARGET "QyhsPreCompile")
include(source/precompile/precompile.cmake)
#set QyhsPrecompile under "Engine" folder
set_target_properties("${CODEGEN_TARGET}" PROPERTIES FOLDER "Engine")

add_dependencies(QyhsRuntime "${CODEGEN_TARGET}")
add_dependencies("${CODEGEN_TARGET}" "QyhsParser")




