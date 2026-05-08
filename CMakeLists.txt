cmake_minimum_required(VERSION 3.14)
project(inventory-api CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Отключаем все ненужные компоненты
set(USERVER_FEATURE_POSTGRESQL OFF)
set(USERVER_FEATURE_MONGOOSE OFF)
set(USERVER_FEATURE_REDIS OFF)
set(USERVER_FEATURE_CLICKHOUSE OFF)
set(USERVER_FEATURE_GRPC OFF)
set(USERVER_FEATURE_KAFKA OFF)
set(USERVER_FEATURE_RABBITMQ OFF)
set(USERVER_FEATURE_MYSQL OFF)
set(USERVER_FEATURE_SQLITE OFF)
set(USERVER_FEATURE_ODBC OFF)
set(USERVER_FEATURE_PQ OFF)
set(USERVER_FEATURE_MONGO OFF)

find_package(userver REQUIRED COMPONENTS core)

set(SOURCES
    src/main.cpp
    src/storage.cpp
    src/utils/auth_utils.cpp
    src/services/user_service.cpp
    src/services/product_service.cpp
    src/services/receipt_service.cpp
    src/handlers/auth_handler.cpp
    src/handlers/user_handler.cpp
    src/handlers/product_handler.cpp
    src/handlers/receipt_handler.cpp
)

add_executable(inventory-api ${SOURCES})
target_link_libraries(inventory-api userver::core)
target_include_directories(inventory-api PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/src)
target_compile_options(inventory-api PRIVATE -Wall -Wextra -Wno-unused-parameter)