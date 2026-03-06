cmake_minimum_required(VERSION 3.16)

project(sync_tcp_boost_server VERSION 0.1 LANGUAGES CXX)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Qt packages
find_package(QT NAMES Qt6 Qt5 REQUIRED COMPONENTS Widgets Network LinguistTools)
find_package(Qt${QT_VERSION_MAJOR} REQUIRED COMPONENTS Widgets Network LinguistTools)

set(TS_FILES
    sync_tcp_boost_server_en_US.ts
)

set(PROJECT_SOURCES
    main.cpp

    LoginDialog.cpp
    LoginDialog.hpp

    MainWindow.cpp
    MainWindow.hpp
    MainWindow.ui

    NetworkManager.cpp
    NetworkManager.hpp

    Protocol.hpp

    ${TS_FILES}
)

if(${QT_VERSION_MAJOR} GREATER_EQUAL 6)

    find_package(Qt6 REQUIRED COMPONENTS Widgets)

    qt_add_executable(sync_tcp_boost_server
        MANUAL_FINALIZATION
        ${PROJECT_SOURCES}
        LoginDialog.hpp LoginDialog.cpp LoginDialog.ui

    )

    qt_create_translation(QM_FILES ${CMAKE_SOURCE_DIR} ${TS_FILES})

else()

    if(ANDROID)

        add_library(sync_tcp_boost_server SHARED
            ${PROJECT_SOURCES}
        )

    else()

        add_executable(sync_tcp_boost_server
            ${PROJECT_SOURCES}
        )

    endif()

    qt5_create_translation(QM_FILES ${CMAKE_SOURCE_DIR} ${TS_FILES})

endif()

# Link Qt libraries
target_link_libraries(sync_tcp_boost_server
    PRIVATE
        Qt${QT_VERSION_MAJOR}::Core
        Qt${QT_VERSION_MAJOR}::Widgets
        Qt${QT_VERSION_MAJOR}::Network
)
target_link_libraries(sync_tcp_boost_server PRIVATE Qt6::Widgets)

# macOS / iOS bundle settings
if(${QT_VERSION} VERSION_LESS 6.1.0)
    set(BUNDLE_ID_OPTION
        MACOSX_BUNDLE_GUI_IDENTIFIER com.example.sync_tcp_boost_server
    )
endif()

set_target_properties(sync_tcp_boost_server PROPERTIES
    ${BUNDLE_ID_OPTION}

    MACOSX_BUNDLE_BUNDLE_VERSION ${PROJECT_VERSION}

    MACOSX_BUNDLE_SHORT_VERSION_STRING
    ${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR}

    MACOSX_BUNDLE TRUE
    WIN32_EXECUTABLE TRUE
)

include(GNUInstallDirs)

install(TARGETS sync_tcp_boost_server
    BUNDLE DESTINATION .
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)

if(QT_VERSION_MAJOR EQUAL 6)
    qt_finalize_executable(sync_tcp_boost_server)
endif()
