
add_library(lwip INTERFACE)

set(LWIP_DIR ${CMAKE_CURRENT_SOURCE_DIR}/ThirdParty/LwIP)
set(FREERTOS_DIR ${CMAKE_CURRENT_LIST_DIR}/ThirdParty/FreeRTOS)
set(LWIP_INCLUDE_DIRS
    # BSP
    ${CMAKE_SOURCE_DIR}/inc
	${CMAKE_SOURCE_DIR}/port/lwip/include
    # FREERTOS
    ${FREERTOS_DIR}/portable/GCC/ARM_CM4F
    ${FREERTOS_DIR}/portable/MemMang
    ${FREERTOS_DIR}/include
    # LWIP
    ${LWIP_DIR}/src/include
    )

set(LWIP_COMPILER_FLAGS  ${DEFINE_COMPILE_FLAGS})

include(${LWIP_DIR}/src/Filelists.cmake)

target_link_libraries(lwip INTERFACE
    lwipcore
    lwipallapps
)

target_include_directories(lwip INTERFACE
    ${LWIP_INCLUDE_DIRS}
)
