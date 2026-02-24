cmake_minimum_required(VERSION 3.10)
project(fraud_engine)

set(CMAKE_CXX_STANDARD 17)

include_directories(
    include
    external/crow/include
)

add_executable(fraud_engine
    src/main.cpp
    src/server.cpp
    src/database.cpp
    src/fraud_engine.cpp
    src/auth.cpp
    src/jwt.cpp
)

find_package(OpenSSL REQUIRED)

target_link_libraries(fraud_engine
    PRIVATE
    sqlite3
    OpenSSL::Crypto
    ws2_32
    mswsock
    iphlpapi
)