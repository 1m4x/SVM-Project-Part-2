#
# CMakeLists.txt
#
# Created by Dmitrii Toksaitov
#

set(SVMASM_TARGET "svmasm")

set(ASM_TARGET "assemblies")
set(ASM_CPU_SAMPLE "change_registers_and_exit")
set(ASM_SCHEDULER_SAMPLE "write_to_register_in_loop")
set(ASM_SOURCES "${ASM_CPU_SAMPLE}.vmasm"
                "${ASM_SCHEDULER_SAMPLE}.vmasm")
set(ASM_TARGETS "${ASM_CPU_SAMPLE}.vmexe"
                "${ASM_SCHEDULER_SAMPLE}.vmexe")

add_custom_command(
    OUTPUT
        "${ASM_CPU_SAMPLE}.vmexe"
    COMMAND
        "${CMAKE_BINARY_DIR}/${SVMASM_TARGET}/${CMAKE_CFG_INTDIR}/${SVMASM_TARGET}"
            "${CMAKE_CURRENT_SOURCE_DIR}/${ASM_CPU_SAMPLE}.vmasm"
            "${ASM_CPU_SAMPLE}.vmexe"
    DEPENDS
        ${SVMASM_TARGET}
        "${ASM_CPU_SAMPLE}.vmasm"
    WORKING_DIRECTORY
        ${CMAKE_CURRENT_BINARY_DIR}
)

add_custom_command(
    OUTPUT
        "${ASM_SCHEDULER_SAMPLE}.vmexe"
    COMMAND
        "${CMAKE_BINARY_DIR}/${SVMASM_TARGET}/${CMAKE_CFG_INTDIR}/${SVMASM_TARGET}"
            "${CMAKE_CURRENT_SOURCE_DIR}/${ASM_SCHEDULER_SAMPLE}.vmasm"
            "${ASM_SCHEDULER_SAMPLE}.vmexe"
    DEPENDS
        ${SVMASM_TARGET}
        "${ASM_SCHEDULER_SAMPLE}.vmasm"
    WORKING_DIRECTORY
        ${CMAKE_CURRENT_BINARY_DIR}
)

add_custom_target(
    ${ASM_TARGET} ALL
    DEPENDS
        ${ASM_TARGETS}
    SOURCES
        ${ASM_SOURCES}
)

source_group("Assembly Sources" FILES ${ASM_SOURCES})
