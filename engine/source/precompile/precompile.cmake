set(PRECOMPILE_TOOLS_PATH "${CMAKE_CURRENT_SOURCE_DIR}/bin")
set(QYHS_PRECOMPILE_PARAMS_IN_PATH "${CMAKE_CURRENT_SOURCE_DIR}/source/precompile/precompile.json.in")
set(QYHS_PRECOMPILE_PARAMS_PATH "${PRECOMPILE_TOOLS_PATH}/precompile.json")
configure_file(${QYHS_PRECOMPILE_PARAMS_IN_PATH} ${QYHS_PRECOMPILE_PARAMS_PATH})

if(CMAKE_HOST_WIN32)
  set(PRECOMPILE_PRE_EXE)
  set(PRECOMPILE_PARSER ${PRECOMPILE_TOOLS_PATH}/QyhsParser.exe)
  set(sys_include "*")
endif()
set (PARSER_INPUT ${CMAKE_BINARY_DIR}/parser_header.h)

set(PRECOMPILE_TARGET "QyhsPreCompile")
add_custom_target(${PRECOMPILE_TARGET} ALL

# COMMAND # (DEBUG: DON'T USE )
#     this will make configure_file() is called on each compile
#   ${CMAKE_COMMAND} -E touch ${PRECOMPILE_PARAM_IN_PATH}a

# If more than one COMMAND is specified they will be executed in order...
COMMAND
  ${CMAKE_COMMAND} -E echo "************************************************************* "
COMMAND
  ${CMAKE_COMMAND} -E echo "**** [Precompile] BEGIN "
COMMAND
  ${CMAKE_COMMAND} -E echo "${PRECOMPILE_PARSER}"
COMMAND
  ${CMAKE_COMMAND} -E echo "${QYHS_PRECOMPILE_PARAMS_PATH}"
COMMAND
  ${CMAKE_COMMAND} -E echo "${PARSER_INPUT}"
COMMAND
    ${PRECOMPILE_PARSER} "${QYHS_PRECOMPILE_PARAMS_PATH}"  "${PARSER_INPUT}"  "${ENGINE_ROOT_DIR}/source" ${sys_include} "Qyhs" 0
### BUILDING ====================================================================================
COMMAND
    ${CMAKE_COMMAND} -E echo "+++ Precompile finished +++"
)
