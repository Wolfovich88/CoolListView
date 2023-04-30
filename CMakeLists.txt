cmake_minimum_required(VERSION 3.16)

project(CoolListView VERSION 0.1 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(Qt6 6.4 REQUIRED COMPONENTS Quick Sql)
find_package(PythonInterp)
find_package(Python)

qt_standard_project_setup()

qt_add_executable(appCoolListView
    main.cpp coollistmodel.h coollistmodel.cpp coollistitem.cpp coollistitem.h dataloader.cpp dataloader.h
)

qt_add_qml_module(appCoolListView
    URI CoolListView
    VERSION 1.0
    QML_FILES Main.qml CoolListView.qml Editor.qml
)

set_target_properties(appCoolListView PROPERTIES
    MACOSX_BUNDLE_GUI_IDENTIFIER my.example.com
    MACOSX_BUNDLE_BUNDLE_VERSION ${PROJECT_VERSION}
    MACOSX_BUNDLE_SHORT_VERSION_STRING ${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR}
    MACOSX_BUNDLE TRUE
    WIN32_EXECUTABLE TRUE
)

target_link_libraries(appCoolListView
    PRIVATE Qt6::Quick Qt6::Sql
)

install(TARGETS appCoolListView
    BUNDLE DESTINATION .
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR})

#Generating database
execute_process(COMMAND python ${CMAKE_CURRENT_SOURCE_DIR}/generator.py "${CMAKE_CURRENT_BINARY_DIR}/testdatabase.db")
