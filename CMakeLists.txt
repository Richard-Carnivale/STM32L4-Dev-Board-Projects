cmake_minimum_required(VERSION 3.22)

# Prevent CMake from executing desktop compiler sanity checks for bare-metal
set(CMAKE_TRY_COMPILE_TARGET_TYPE "STATIC_LIBRARY")

# Deduce project name from directory name matching your reference flow
get_filename_component(PROJECT_NAME_DEDUCED ${CMAKE_CURRENT_SOURCE_DIR} NAME)
project(${PROJECT_NAME_DEDUCED} C CXX ASM)

# Toolchain specifications
set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED ON)
set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Hardware Specific Parameters (STM32L476RGT6U)
set(MCU_MODEL "STM32L476xx")
set(CPU_PARAMETERS
        -mcpu=cortex-m4
        -mthumb
        -mfpu=fpv4-sp-d16
        -mfloat-abi=hard
)

# Custom GCC Linker Script and Vector Table
set(LINKER_SCRIPT "${CMAKE_CURRENT_SOURCE_DIR}/STM32L476RGTx_FLASH.ld")
set(STARTUP_SCRIPT "${CMAKE_CURRENT_SOURCE_DIR}/Drivers/CMSIS/Device/ST/STM32L4xx/Source/Templates/gcc/startup_stm32l476xx.s")

# ==============================================================================
# Source File Discovery
# ==============================================================================

# Core Application Code
file(GLOB CORE_SOURCES "${CMAKE_CURRENT_SOURCE_DIR}/Core/Src/*.c")

# Pull in all ST HAL driver source files
file(GLOB HAL_SOURCES_RAW "${CMAKE_CURRENT_SOURCE_DIR}/Drivers/STM32L4xx_HAL_Driver/Src/*.c")

# EXCLUDE template/example files from the HAL source list to prevent compilation failures
set(HAL_SOURCES ${HAL_SOURCES_RAW})
foreach(TMPL_FILE ${HAL_SOURCES_RAW})
    if(${TMPL_FILE} MATCHES "template")
        list(REMOVE_ITEM HAL_SOURCES ${TMPL_FILE})
    endif()
endforeach()

# FreeRTOS Core & Porting Layers
file(GLOB FREERTOS_SOURCES
        "${CMAKE_CURRENT_SOURCE_DIR}/Middlewares/Third_Party/FreeRTOS/Source/*.c"
        "${CMAKE_CURRENT_SOURCE_DIR}/Middlewares/Third_Party/FreeRTOS/Source/CMSIS_RTOS/cmsis_os.c"
        "${CMAKE_CURRENT_SOURCE_DIR}/Middlewares/Third_Party/FreeRTOS/Source/portable/GCC/ARM_CM4F/port.c"
        "${CMAKE_CURRENT_SOURCE_DIR}/Middlewares/Third_Party/FreeRTOS/Source/portable/MemMang/heap_4.c"
)

# Custom application code tracking
file(GLOB_RECURSE USER_SOURCES
        "${CMAKE_CURRENT_SOURCE_DIR}/user/src/*.c"
        "${CMAKE_CURRENT_SOURCE_DIR}/user/src/*.cpp"
)

# ==============================================================================
# Executable Build Target
# ==============================================================================
set(EXECUTABLE ${PROJECT_NAME})
add_executable(${EXECUTABLE}
        ${CORE_SOURCES}
        ${HAL_SOURCES}
        ${FREERTOS_SOURCES}
        ${USER_SOURCES}
        ${STARTUP_SCRIPT}
)

# Global Macro Definitions
target_compile_definitions(${EXECUTABLE} PRIVATE
        ${MCU_MODEL}
        USE_HAL_DRIVER
)

# ==============================================================================
# Include Directories (Using SYSTEM to silence external warnings)
# ==============================================================================
target_include_directories(${EXECUTABLE} SYSTEM PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/Core/Inc
        ${CMAKE_CURRENT_SOURCE_DIR}/Drivers/STM32L4xx_HAL_Driver/Inc
        ${CMAKE_CURRENT_SOURCE_DIR}/Drivers/STM32L4xx_HAL_Driver/Inc/Legacy
        ${CMAKE_CURRENT_SOURCE_DIR}/Drivers/CMSIS/Device/ST/STM32L4xx/Include
        ${CMAKE_CURRENT_SOURCE_DIR}/Drivers/CMSIS/Include
        ${CMAKE_CURRENT_SOURCE_DIR}/Middlewares/Third_Party/FreeRTOS/Source/include
        ${CMAKE_CURRENT_SOURCE_DIR}/Middlewares/Third_Party/FreeRTOS/Source/CMSIS_RTOS
        ${CMAKE_CURRENT_SOURCE_DIR}/Middlewares/Third_Party/FreeRTOS/Source/portable/GCC/ARM_CM4F
)

target_include_directories(${EXECUTABLE} PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/user/inc
)

# ==============================================================================
# Warnings and Optimizations Flags
# ==============================================================================

# Relaxed warning constraints on generated/middleware components to clean up output
set_source_files_properties(${HAL_SOURCES} ${FREERTOS_SOURCES} PROPERTIES COMPILE_OPTIONS "-w")

# Strict warning checks exclusively applied to your custom logic
set_source_files_properties(${CORE_SOURCES} ${USER_SOURCES} PROPERTIES
        COMPILE_OPTIONS "-Wall;-Wextra;-Wno-unused-parameter"
)

target_compile_options(${EXECUTABLE} PRIVATE
        ${CPU_PARAMETERS}
        $<$<CONFIG:Debug>:-Og -g3 -ggdb>
        $<$<CONFIG:Release>:-O3 -g0>
)

# ==============================================================================
# Linker Settings
# ==============================================================================
target_link_options(${EXECUTABLE} PRIVATE
        -T${LINKER_SCRIPT}
        ${CPU_PARAMETERS}
        -Wl,-Map=${PROJECT_NAME}.map
        --specs=nano.specs
        --specs=nosys.specs
        -Wl,--gc-sections
        -Wl,--start-group
        -lc
        -lm
        -Wl,--end-group
        -Wl,--print-memory-usage
)

# ==============================================================================
# Post-Build Commands: Hex, Binary, and Size Calculations
# ==============================================================================
#add_custom_command(TARGET ${EXECUTABLE} POST_BUILD
#        COMMAND ${CMAKE_SIZE} $<TARGET_FILE:${EXECUTABLE}>
#)

add_custom_command(TARGET ${EXECUTABLE} POST_BUILD
        COMMAND ${CMAKE_OBJCOPY} -O ihex   $<TARGET_FILE:${EXECUTABLE}> ${CMAKE_CURRENT_BINARY_DIR}/${EXECUTABLE}.hex
        COMMAND ${CMAKE_OBJCOPY} -O binary $<TARGET_FILE:${EXECUTABLE}> ${CMAKE_CURRENT_BINARY_DIR}/${EXECUTABLE}.bin
)