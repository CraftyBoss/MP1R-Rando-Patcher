cmake_minimum_required(VERSION 3.21)
project(subsdk CXX C ASM)

## Error if not using switch toolchain file
if (NOT SWITCH)
  message(FATAL_ERROR "Not targeting switch, make sure to specify -DCMAKE_TOOLCHAIN_FILE=cmake/toolchain.cmake")
endif ()

set(CMAKE_C_STANDARD 17)
set(CMAKE_CXX_STANDARD 23)

if (LOGGER_IP)
  add_compile_definitions(LOGGER_IP="${LOGGER_IP}")
endif ()

## Title Id
set(TITLE_ID 010012101468C000)
set(CONFIG_TITLE_ID 0x${TITLE_ID})
add_compile_definitions(EXL_PROGRAM_ID=${CONFIG_TITLE_ID} IMGUI_USER_CONFIG="imgui_backend/nvn_imgui_config.h")
configure_file(${CMAKE_CURRENT_SOURCE_DIR}/module/subsdk9.json.template ${CMAKE_CURRENT_BINARY_DIR}/subsdk9.json)

## Project sources
include_directories(${PROJECT_SOURCE_DIR}/src/lib)

include_directories(${PROJECT_SOURCE_DIR}/libs/game)
include_directories(${PROJECT_SOURCE_DIR}/libs/cJSON)
include_directories(${PROJECT_SOURCE_DIR}/libs/NintendoSDK)
include_directories(${PROJECT_SOURCE_DIR}/libs/NintendoSDK/nn)
include_directories(${PROJECT_SOURCE_DIR}/libs/NintendoSDK/nvn)
include_directories(${PROJECT_SOURCE_DIR}/libs/NintendoSDK/nvnTool)

include_directories(${PROJECT_SOURCE_DIR}/src/program)

include_directories(${PROJECT_SOURCE_DIR}/src)
include_directories(${PROJECT_SOURCE_DIR}/imgui)

file(GLOB_RECURSE SOURCES_ASM ${PROJECT_SOURCE_DIR}/src/*.s)
file(GLOB_RECURSE SOURCES_H ${PROJECT_SOURCE_DIR}/src/*.h ${PROJECT_SOURCE_DIR}/src/*.hpp)
file(GLOB_RECURSE SOURCES_CXX ${PROJECT_SOURCE_DIR}/src/*.cpp ${PROJECT_SOURCE_DIR}/src/*.c ${PROJECT_SOURCE_DIR}/libs/cJSON/*.c)
file(GLOB IMGUI_SOURCES ${PROJECT_SOURCE_DIR}/imgui/*.cpp)
file(GLOB_RECURSE LOCAL_LIBRARIES ${PROJECT_SOURCE_DIR}/libs/*.a)

## Include nx tools
include(${CMAKE_SOURCE_DIR}/cmake/SwitchTools.cmake)

## subsdk9
set(CMAKE_EXECUTABLE_SUFFIX ".elf")
#set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS}")
#set_source_files_properties(${SOURCES_CXX} PROPERTIES LANGUAGE CXX)
add_executable(subsdk9 ${SOURCES_ASM} ${SOURCES_C} ${SOURCES_H} ${SOURCES_CXX} ${IMGUI_SOURCES})
add_compile_definitions(EXL_LOAD_KIND=Module EXL_LOAD_KIND_ENUM=EXL_LOAD_KIND_MODULE)
target_link_libraries(subsdk9 ${LOCAL_LIBRARIES})

# generate header file for imgui shader binary
target_embed_binaries(subsdk9 "${PROJECT_SOURCE_DIR}/romfs/ShaderData/imgui.bin")

## Prepare subsdk ld
set(SUBSDK_LD_PATH ${CMAKE_CURRENT_SOURCE_DIR}/module/subsdk9.ld)
execute_process(COMMAND uname OUTPUT_VARIABLE uname)
if (uname MATCHES "^MINGW")
  string(REGEX REPLACE "^/([a-zA-Z])/" "\\1:/" SUBSDK_LD_PATH "${SUBSDK_LD_PATH}")
endif ()
configure_file(${CMAKE_CURRENT_SOURCE_DIR}/module/subsdk9.specs.template ${CMAKE_CURRENT_BINARY_DIR}/subsdk9.specs)

## Build nso & generate npdm
add_nso_target_subsdk(subsdk9)
set_target_properties(subsdk9 PROPERTIES CONFIG_JSON "${CMAKE_CURRENT_BINARY_DIR}/subsdk9.json")
__generate_npdm(subsdk9)
set_target_properties(subsdk9 PROPERTIES LINK_FLAGS "-specs ${CMAKE_BINARY_DIR}/subsdk9.specs -g3 ${ARCH} -nostartfiles")

add_custom_target(subsdk9_meta DEPENDS create_npdm subsdk9_nso)
if (FTP_IP)
    ## Upload to target switch
    if (FTP_USERNAME AND FTP_PASSWORD) # login using a username and password
        add_custom_command(TARGET subsdk9_meta POST_BUILD COMMAND ncftpput -u ${FTP_USERNAME} -p ${FTP_PASSWORD} -P 5000 ${FTP_IP} /atmosphere/contents/${TITLE_ID}/exefs ${CMAKE_BINARY_DIR}/main.npdm ${CMAKE_BINARY_DIR}/subsdk9)
    else ()
        add_custom_command(TARGET subsdk9_meta POST_BUILD COMMAND ncftpput -u crafty -p boss -P 5000 ${FTP_IP} /atmosphere/contents/${TITLE_ID}/exefs ${CMAKE_BINARY_DIR}/main.npdm ${CMAKE_BINARY_DIR}/subsdk9)
    endif ()
endif ()

if (RYU_PATH)
    add_custom_command(TARGET subsdk9_meta POST_BUILD COMMAND mkdir -p ${RYU_PATH}/mods/contents/${TITLE_ID}/mods/exefs/)
    add_custom_command(TARGET subsdk9_meta POST_BUILD COMMAND cp ${CMAKE_BINARY_DIR}/main.npdm ${RYU_PATH}/mods/contents/${TITLE_ID}/mods/exefs/main.npdm)
    add_custom_command(TARGET subsdk9_meta POST_BUILD COMMAND cp ${CMAKE_BINARY_DIR}/subsdk9 ${RYU_PATH}/mods/contents/${TITLE_ID}/mods/exefs/subsdk9)
endif ()

# Copy to deploy folder (for release builds)
add_custom_command(TARGET subsdk9_meta POST_BUILD COMMAND mkdir -p ${PROJECT_SOURCE_DIR}/deploy/atmosphere/contents/${TITLE_ID}/exefs)
add_custom_command(TARGET subsdk9_meta POST_BUILD COMMAND cp ${CMAKE_BINARY_DIR}/main.npdm ${PROJECT_SOURCE_DIR}/deploy/atmosphere/contents/${TITLE_ID}/exefs/main.npdm)
add_custom_command(TARGET subsdk9_meta POST_BUILD COMMAND cp ${CMAKE_BINARY_DIR}/subsdk9 ${PROJECT_SOURCE_DIR}/deploy/atmosphere/contents/${TITLE_ID}/exefs/subsdk9)