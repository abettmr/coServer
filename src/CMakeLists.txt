set(MODULE_NAME "hola")

file(GLOB srcList "*.cpp" "*.h")
add_executable(${MODULE_NAME} ${srcList})

# message(STATUS "PROJECT_SOURCE_DIR: ${PROJECT_SOURCE_DIR}")
# message(STATUS "PROJECT_BIN_DIR: ${PROJECT_BINARY_DIR}")
# message(STATUS "CCSD: ${CMAKE_CURRENT_SOURCE_DIR}")
# message(STATUS "CCBD: ${CMAKE_CURRENT_BINARY_DIR}")
# message(STATUS "SUBDIR DONE.")
