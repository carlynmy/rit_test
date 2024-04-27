cmake_minimum_required(VERSION 3.5)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

project(RIT_TEST)

set(RIT_TEST_INCLUDE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/src")
set(RIT_TEST_SRC_DIR "${CMAKE_CURRENT_SOURCE_DIR}/src")
set(RIT_TEST_SRC_CLIENT_DIR "${RIT_TEST_SRC_DIR}/client")
set(RIT_TEST_SRC_SERVER_DIR "${RIT_TEST_SRC_DIR}/server")
set(RIT_TEST_SRC_COMMON_DIR "${RIT_TEST_SRC_DIR}/common")

include_directories(
  ${RIT_TEST_INCLUDE_DIR}
)

set(RIT_TEST_COMMON_LIB_SOURCE
  ${RIT_TEST_SRC_COMMON_DIR}/client_socket.h
  ${RIT_TEST_SRC_COMMON_DIR}/client_socket.cc
  ${RIT_TEST_SRC_COMMON_DIR}/message.h
  ${RIT_TEST_SRC_COMMON_DIR}/message.cc
  ${RIT_TEST_SRC_COMMON_DIR}/socket.h
  ${RIT_TEST_SRC_COMMON_DIR}/socket.cc
  ${RIT_TEST_SRC_COMMON_DIR}/socket_address.h
  ${RIT_TEST_SRC_COMMON_DIR}/socket_address.cc
  ${RIT_TEST_SRC_COMMON_DIR}/server_socket.h
  ${RIT_TEST_SRC_COMMON_DIR}/server_socket.cc
)

set(RIT_TEST_SERVER_SOURCE
  ${RIT_TEST_SRC_SERVER_DIR}/main.cc
  ${RIT_TEST_SRC_SERVER_DIR}/server.h
  ${RIT_TEST_SRC_SERVER_DIR}/server.cc
)

set(RIT_TEST_CLIENT_SOURCE
  ${RIT_TEST_SRC_CLIENT_DIR}/main.cc
  ${RIT_TEST_SRC_CLIENT_DIR}/model/model.h
  ${RIT_TEST_SRC_CLIENT_DIR}/model/model.cc
  ${RIT_TEST_SRC_CLIENT_DIR}/controller/controller.h
  ${RIT_TEST_SRC_CLIENT_DIR}/controller/controller.cc
  ${RIT_TEST_SRC_CLIENT_DIR}/view/view.h
  ${RIT_TEST_SRC_CLIENT_DIR}/view/view.cc
)

SET(RIT_TEST_WARNING_FLAGS "-Wall -Werror -Wextra" CACHE STRING "")

SET(RIT_TEST_FLAGS "${RIT_TEST_WARNING_FLAGS}")

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${RIT_TEST_FLAGS}")

add_library(common STATIC ${RIT_TEST_COMMON_LIB_SOURCE})
add_executable(server ${RIT_TEST_SERVER_SOURCE})
add_executable(client ${RIT_TEST_CLIENT_SOURCE})

target_link_libraries( 
  server
  common
)

target_link_libraries( 
  client
  common
)
