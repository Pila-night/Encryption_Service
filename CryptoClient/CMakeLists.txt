cmake_minimum_required(VERSION 3.16)

project(CryptoClient VERSION 0.1 LANGUAGES CXX)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(QT NAMES Qt6 Qt5 REQUIRED COMPONENTS Widgets Network)
find_package(Qt${QT_VERSION_MAJOR} REQUIRED COMPONENTS Widgets Network)

set(PROJECT_SOURCES
    main.cpp
    mainwindow.cpp
    mainwindow.h
    mainwindow.ui
    Network.h
    Network.cpp
    Controller.h
    Controller.cpp
    Gost3410.h
    Gost3410.cpp
    ProtocolAdapter.h
    ProtocolAdapter.cpp
)

if(${QT_VERSION_MAJOR} GREATER_EQUAL 6)
    qt_add_executable(CryptoClient
        MANUAL_FINALIZATION
        ${PROJECT_SOURCES}
        ProtocolPackets.h
        ProtocolSerializer.h ProtocolSerializer.cpp
        PacketHandler.h PacketHandler.cpp
    )
else()
    if(ANDROID)
        add_library(CryptoClient SHARED
            ${PROJECT_SOURCES}
        )
    else()
        add_executable(CryptoClient
            ${PROJECT_SOURCES}
        )
    endif()
endif()

target_link_libraries(CryptoClient PRIVATE
    Qt${QT_VERSION_MAJOR}::Widgets
    Qt${QT_VERSION_MAJOR}::Network
)

target_compile_options(CryptoClient PRIVATE -Wno-deprecated-declarations)

if(${QT_VERSION} VERSION_LESS 6.1.0)
  set(BUNDLE_ID_OPTION MACOSX_BUNDLE_GUI_IDENTIFIER com.example.CryptoClient)
endif()
set_target_properties(CryptoClient PROPERTIES
    ${BUNDLE_ID_OPTION}
    MACOSX_BUNDLE_BUNDLE_VERSION ${PROJECT_VERSION}
    MACOSX_BUNDLE_SHORT_VERSION_STRING ${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR}
    MACOSX_BUNDLE TRUE
    WIN32_EXECUTABLE TRUE
)

include(GNUInstallDirs)
install(TARGETS CryptoClient
    BUNDLE DESTINATION .
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)

if(QT_VERSION_MAJOR EQUAL 6)
    qt_finalize_executable(CryptoClient)
endif()