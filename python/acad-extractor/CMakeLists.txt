cmake_minimum_required(VERSION 3.13)

project(acad-extractor NONE)

set(VARIANTS static shared src)
set(BINDINGS early late)

foreach(VARIANT ${VARIANTS})
    foreach(BINDING ${BINDINGS})
        set(NAME ${PROJECT_NAME}-${BINDING}-binding-${VARIANT})

        add_custom_target(
            ${NAME}
            ALL
            WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
            COMMAND sip-wheel --name ${NAME} --ae-variant ${VARIANT} --ae-binding ${BINDING}
        )
    endforeach()
endforeach()
