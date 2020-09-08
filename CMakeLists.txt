cmake_minimum_required(VERSION 3.5)

project(ghostly LANGUAGES CXX)

find_package(Qt5 COMPONENTS Quick REQUIRED)

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

add_executable(ghostly
    main.cpp
    player.cpp
    gamemanager.cpp
    networkclient.cpp
    settings.cpp
    map.cpp
    monster.cpp

    resources.qrc
)

target_link_libraries(ghostly PRIVATE Qt5::Quick)
install(TARGETS ghostly DESTINATION bin)
