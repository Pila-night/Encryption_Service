cmake_minimum_required(VERSION 3.16)

project(CryptoClient VERSION 0.1 LANGUAGES CXX)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(QT NAMES Qt6 Qt5 REQUIRED COMPONENTS Widgets Network)
find_package(Qt${QT_VERSION_MAJOR} REQUIRED COMPONENTS Widgets Network)

# ============================================================================
# === GMP И LIBGCRYPT (добавлено) ===
# ============================================================================
# Путь к MSYS2 MinGW64 (где установлены GMP и libgcrypt)
set(MSYS2_MINGW64 "C:/msys64/mingw64")

# Проверка существования путей
if(NOT EXISTS "${MSYS2_MINGW64}/include/gmp.h")
    message(WARNING "GMP не найден в ${MSYS2_MINGW64}. Установите: pacman -S mingw-w64-x86_64-gmp")
endif()
if(NOT EXISTS "${MSYS2_MINGW64}/include/gcrypt.h")
    message(WARNING "libgcrypt не найден в ${MSYS2_MINGW64}. Установите: pacman -S mingw-w64-x86_64-libgcrypt")
endif()
# ============================================================================

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
        ALL.h
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

# ============================================================================
# === GMP И LIBGCRYPT (добавлено) ===
# ============================================================================
# Подключаем заголовки
target_include_directories(CryptoClient PRIVATE
    "${MSYS2_MINGW64}/include"
)

# Подключаем библиотеки
target_link_libraries(CryptoClient PRIVATE
    "${MSYS2_MINGW64}/lib/libgmp.dll.a"
    "${MSYS2_MINGW64}/lib/libgcrypt.dll.a"
    "${MSYS2_MINGW64}/lib/libgpg-error.dll.a"
)

# Для Windows добавляем Winsock (если ещё не добавлен)
if(WIN32)
    target_link_libraries(CryptoClient PRIVATE ws2_32)
endif()

# Автоматическое копирование DLL в папку сборки
if(WIN32)
    add_custom_command(TARGET CryptoClient POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_if_different
            "${MSYS2_MINGW64}/bin/libgmp-10.dll"
            "${MSYS2_MINGW64}/bin/libgcrypt-20.dll"
            "${MSYS2_MINGW64}/bin/libgpg-error-0.dll"
            $<TARGET_FILE_DIR:CryptoClient>
        COMMENT "Copying GMP and libgcrypt DLLs to build directory"
    )
endif()
# ============================================================================

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