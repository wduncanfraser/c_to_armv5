cmake_minimum_required(VERSION 3.1)
project(CtoARMCompiler)

FIND_PACKAGE(BISON REQUIRED)
SET(BisonOutput ${CMAKE_SOURCE_DIR}/CtoARMparser.cpp)
IF(BISON_FOUND)
    ADD_CUSTOM_COMMAND(
      OUTPUT ${BisonOutput}
      COMMAND ${BISON_EXECUTABLE}
              --defines=${CMAKE_SOURCE_DIR}/CtoARMtokens.h
              --output=${BisonOutput}
              ${CMAKE_SOURCE_DIR}/CtoARM.ypp
      COMMENT "Generating parser/tokens"
    )
ENDIF()

FIND_PACKAGE(FLEX REQUIRED)
SET(FlexOutput ${CMAKE_SOURCE_DIR}/CtoARMlexer.cpp)
IF(FLEX_FOUND)
    ADD_CUSTOM_COMMAND(
      OUTPUT ${FlexOutput}
      COMMAND ${FLEX_EXECUTABLE}
              --outfile=${FlexOutput}
              ${CMAKE_SOURCE_DIR}/CtoARM.lpp
      COMMENT "Generating lexer"
    )
ENDIF()


set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -std=c++11")
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_SOURCE_DIR}/build/")

set(Source_CtoARM CtoARM.cpp)

add_executable(CtoArm ${BisonOutput} ${FlexOutput} ${Source_CtoARM})