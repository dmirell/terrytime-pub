cmake_minimum_required(VERSION 3.20)
project(asynchronous)

add_executable(
    ${PROJECT_NAME}
    #
    main.cpp
)

target_link_libraries(
    ${PROJECT_NAME}
    BinAPI::binapilib
    Boost::boost
)