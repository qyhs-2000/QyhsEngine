cmake_minimum_required(VERSION 3.28.1 FATAL_ERROR)
project(QYHSEngine VERSION 0.1.0)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
#build static library
set(BUILD_SHARED_LIBS OFF)
set(OUTPUT_DIR "${CMAKE_SOURCE_DIR}/BUILD/${CMAKE_SYSTEM_PROCESSOR}/$<CONFIG>/${PROJECT_NAME}/")
include(CMakeDependentOption)


#if(PROJECT_SOURCE_DIR STREQUAL PROJECT_BINARY_DIR)
#  message(
 #   FATAL_ERROR
 #     "In-source builds not allowed. Please make a new directory (called a build directory) and run CMake from there."
 # )
#endif()



set(QYHS_ROOT_DIR "${CMAKE_CURRENT_SOURCE_DIR}")
set(CMAKE_INSTALL_PREFIX "${QYHS_ROOT_DIR}/bin")
set(BINARY_ROOT_DIR ${CMAKE_INSTALL_PREFIX})

#Samples
set(TEMPLATE_WINDOW "Template_Window")
set(TESTS "Tests")
set(EDITOR "Editor")


add_subdirectory(engine)
add_subdirectory(samples/template_windows)
add_subdirectory(samples/tests)
add_subdirectory(editor)

