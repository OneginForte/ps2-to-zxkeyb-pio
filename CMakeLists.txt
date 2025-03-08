# ====================================================================================
# == DO NOT EDIT THE FOLLOWING LINES for the Raspberry Pi Pico VS Code Extension to work ==
if(WIN32)
    set(USERHOME $ENV{USERPROFILE})
else()
    set(USERHOME $ENV{HOME})
endif()
set(sdkVersion 2.0.0)
set(toolchainVersion 13_2_Rel1)
set(picotoolVersion 2.0.0)
set(picoVscode ${USERHOME}/.pico-sdk/cmake/pico-vscode.cmake)
if (EXISTS ${picoVscode})
    include(${picoVscode})
endif()
# ====================================================================================
cmake_minimum_required(VERSION 3.13)

set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_STANDARD 17)

set(PICO_BOARD waveshare_rp2040_zero CACHE STRING "Board type")

set(MS_RATE_DEFAULT 100 CACHE STRING "Default mouse sample rate")
set(MS_RATE_HOST_CONTROL ON CACHE BOOL "Allow the host to configure the mouse sample rate")

add_compile_definitions(DEBUG=0)

# Pull in Raspberry Pi Pico SDK
include(pico_sdk_import.cmake)

if (NOT PICO_SDK_VERSION_STRING VERSION_LESS "2.1.0")
  message(FATAL_ERROR "Raspberry Pi Pico SDK version 1.5.1 required. Your version is ${PICO_SDK_VERSION_STRING}")
endif()

project(ps2hidtozx C CXX ASM)
SET(BUILD_NAME "${PROJECT_NAME}-${FLASH_SIZE}-${CPU_MHZ}")

# Initialise the Raspberry Pi Pico SDK
pico_sdk_init()

add_executable(${PROJECT_NAME} src/ps2x2pico.c src/usbin.c src/scancodes.c src/scancode_zx.c src/ps2kb.c src/ps2ms.c src/ps2in.c src/ws2812.c)

pico_generate_pio_header(${PROJECT_NAME} ${CMAKE_CURRENT_LIST_DIR}/src/ps2in.pio)
pico_generate_pio_header(${PROJECT_NAME} ${CMAKE_CURRENT_LIST_DIR}/src/ws2812.pio)

# print memory usage, enable all warnings
target_link_options(${PROJECT_NAME} PRIVATE 
        -Xlinker --print-memory-usage --data-sections)
target_compile_options(${PROJECT_NAME} PRIVATE 
        -Wall 
        -Wextra)

target_sources(${PROJECT_NAME} PRIVATE
 # can use 'tinyusb_pico_pio_usb' library later when pico-sdk is updated
 ${PICO_TINYUSB_PATH}/src/portable/raspberrypi/pio_usb/hcd_pio_usb.c
 )
target_compile_definitions(${PROJECT_NAME} PRIVATE PIO_USB_USE_TINYUSB)

### PS/2 input
target_compile_definitions(${PROJECT_NAME} PRIVATE 
KBIN=2 # Keyboard data GPIO2, clock pin is GPIO3
#MSIN=4 # Mouse data GPIO4, clock pin is GPIO5
)
### PIO_USB port definition
target_compile_definitions(${PROJECT_NAME} PRIVATE 
PIOH=0 # PioUSB DP pin GPIO0
)
### LEDPIN
target_compile_definitions(${PROJECT_NAME} PRIVATE 
LEDPIN=16 #zero 16, black 25 
WS2812_PIN=16 # for WS2812 driver 23-black pico 16-zero pico
)
### MT8816 pinout
target_compile_definitions(${PROJECT_NAME} PRIVATE
AX0=4
AX1=5
AX2=6
AX3=7
AY0=8
AY1=9
AY2=10
STBMT=11
CSMT=12
RSTMT=13
DATMT=14
)
### UART pinout for debug
target_compile_definitions(${PROJECT_NAME} PRIVATE
PICO_DEFAULT_UART_TX_PIN=-1
PICO_DEFAULT_UART_RX_PIN=-1

)
add_compile_definitions(PICO_PANIC_FUNCTION=reset)

add_compile_definitions(MS_RATE_DEFAULT=${MS_RATE_DEFAULT})
if (MS_RATE_HOST_CONTROL)
    add_compile_definitions(MS_RATE_HOST_CONTROL)
endif()

pico_set_program_name(${PROJECT_NAME} "ZX keyboard adapter")
pico_set_program_version(${PROJECT_NAME} "0.6")

pico_enable_stdio_uart(${PROJECT_NAME} 0)
pico_enable_stdio_usb(${PROJECT_NAME} 0)

add_subdirectory("./Pico-PIO-USB" pico_pio_usb)

target_include_directories(${PROJECT_NAME} PRIVATE ${CMAKE_CURRENT_LIST_DIR}/src)
target_link_libraries(${PROJECT_NAME} 
        # Standard Pico
        pico_stdlib
        pico_multicore 
        hardware_pio
        # TinyUSB 
        tinyusb_host 
        tinyusb_board 
        pico_pio_usb)

pico_add_extra_outputs(${PROJECT_NAME})
