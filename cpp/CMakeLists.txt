cmake_minimum_required(VERSION 3.13)

project(acad-extractor CXX)
set(CMAKE_CXX_STANDARD 11)

if(WIN32 AND (CMAKE_GENERATOR MATCHES "Ninja"))
    # Language pack for en must be installed in VS
    execute_process(COMMAND set VSLANG=1033)
    execute_process(COMMAND setx VSLANG 1033)
    set(CMAKE_CL_SHOWINCLUDES_PREFIX "Note: including file:")
endif()

set(SRC core.cpp utils.cpp)

set(BINDINGS early late)

foreach(BINDING ${BINDINGS})
    set(PREFIX ${PROJECT_NAME}-${BINDING}-binding)

    # static

    add_library(${PREFIX}-static STATIC ${SRC})
    if(BINDING MATCHES "early")
        target_compile_definitions(${PREFIX}-static PRIVATE AE_EARLY_BINDING)
    endif()

    add_executable(${PREFIX}-test-static test.cpp utils.cpp)
    target_link_libraries(${PREFIX}-test-static ${PREFIX}-static)

    # shared

    add_library(${PREFIX}-shared SHARED ${SRC})
    target_compile_definitions(${PREFIX}-shared PRIVATE AE_EXPORT)
    if(BINDING MATCHES "early")
        target_compile_definitions(${PREFIX}-shared PRIVATE AE_EARLY_BINDING)
    endif()

    add_executable(${PREFIX}-test-shared test.cpp utils.cpp)
    target_compile_definitions(${PREFIX}-test-shared PRIVATE AE_IMPORT)
    target_link_libraries(${PREFIX}-test-shared ${PREFIX}-shared)

    # src

    add_executable(${PREFIX}-test test.cpp ${SRC})
    if(BINDING MATCHES "early")
        target_compile_definitions(${PREFIX}-test PRIVATE AE_EARLY_BINDING)
    endif()
endforeach()
