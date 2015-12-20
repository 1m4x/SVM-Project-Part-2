#
# CMakeLists.txt
#
# Created by Dmitrii Toksaitov
#

set(SVM_TARGET "svm")
set(SVM_INCLUDES "include")
set(SVM_HEADERS "${SVM_INCLUDES}/board.h"
                "${SVM_INCLUDES}/cpu.h"
                "${SVM_INCLUDES}/pic.h"
                "${SVM_INCLUDES}/pit.h"
                "${SVM_INCLUDES}/memory.h"
                "${SVM_INCLUDES}/kernel.h"
                "${SVM_INCLUDES}/process.h")
set(SVM_SOURCES "board.cpp"
                "cpu.cpp"
                "pic.cpp"
                "pit.cpp"
                "memory.cpp"
                "kernel.cpp"
                "process.cpp"
                "svm.cpp")

include_directories(${SVM_INCLUDES})
add_executable(${SVM_TARGET} ${SVM_SOURCES} ${SVM_HEADERS})

if(CMAKE_VERSION VERSION_LESS "3.1")
    if(CMAKE_COMPILER_IS_GNUCXX)
        set(CMAKE_CXX_FLAGS "-std=c++11 ${CMAKE_CXX_FLAGS}")
    endif()
else()
    target_compile_features(
        ${SVM_TARGET}
        PRIVATE
            "cxx_lambdas"
            "cxx_auto_type"
            "cxx_local_type_template_args"
    )
endif()
