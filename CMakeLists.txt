cmake_minimum_required(VERSION 3.16)

project(PennyWise VERSION 0.2 LANGUAGES CXX)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(QT NAMES Qt6 Qt5 REQUIRED COMPONENTS Widgets)
find_package(Qt${QT_VERSION_MAJOR} REQUIRED COMPONENTS Widgets)
find_package(Qt6 REQUIRED COMPONENTS Sql)
find_package(Qt6 REQUIRED COMPONENTS Svg)
find_package(Qt6 REQUIRED COMPONENTS Charts)

set(PROJECT_SOURCES
        main.cpp
)

if(${QT_VERSION_MAJOR} GREATER_EQUAL 6)
    qt_add_executable(PennyWise
        MANUAL_FINALIZATION
        ${PROJECT_SOURCES}
        include/mainWindow.h src/mainWindow.cpp ui/mainWindow.ui
        resources/resources.qrc
        include/status_log.h src/status_log.cpp
        include/database.h src/database.cpp
        include/constants.h
        include/init.h src/init.cpp
        include/welcomescreenDialog.h src/welcomescreenDialog.cpp ui/welcomescreenDialog.ui
        include/addDatabaseDialog.h src/addDatabaseDialog.cpp ui/addDatabaseDialog.ui
        include/statemgr.h src/statemgr.cpp
        include/transactionDialog.h src/transactionDialog.cpp ui/transactionDialog.ui
        include/transaction.h src/transaction.cpp
        include/categoryDialog.h src/categoryDialog.cpp ui/categoryDialog.ui
        include/category.h src/category.cpp
        include/categoryEditDialog.h src/categoryEditDialog.cpp ui/categoryEditDialog.ui
        include/iconPickerDialog.h src/iconPickerDialog.cpp ui/iconPickerDialog.ui
        include/accountDialog.h src/accountDialog.cpp ui/accountDialog.ui
        include/account.h src/account.cpp
        include/chart.h src/chart.cpp
        include/overviewDialog.h src/overviewDialog.cpp ui/overviewDialog.ui
        include/search.h src/search.cpp
        include/helpDialog.h src/helpDialog.cpp ui/helpDialog.ui
        include/exportDialog.h src/exportDialog.cpp ui/exportDialog.ui
        include/export.h src/export.cpp
        include/databaseDialog.h src/databaseDialog.cpp ui/databaseDialog.ui
    )
endif()

target_link_libraries(PennyWise PRIVATE Qt${QT_VERSION_MAJOR}::Widgets)
target_link_libraries(PennyWise PRIVATE Qt6::Sql)
target_link_libraries(PennyWise PRIVATE Qt6::Svg)
target_link_libraries(PennyWise PRIVATE Qt6::Charts)

# Qt for iOS sets MACOSX_BUNDLE_GUI_IDENTIFIER automatically since Qt 6.1.
# If you are developing for iOS or macOS you should consider setting an
# explicit, fixed bundle identifier manually though.
if(${QT_VERSION} VERSION_LESS 6.1.0)
  set(BUNDLE_ID_OPTION MACOSX_BUNDLE_GUI_IDENTIFIER com.example.PennyWise)
endif()
set_target_properties(PennyWise PROPERTIES
    ${BUNDLE_ID_OPTION}
    MACOSX_BUNDLE_BUNDLE_VERSION ${PROJECT_VERSION}
    MACOSX_BUNDLE_SHORT_VERSION_STRING ${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR}
    MACOSX_BUNDLE TRUE
    WIN32_EXECUTABLE TRUE
)

include(GNUInstallDirs)
install(TARGETS PennyWise
    BUNDLE DESTINATION .
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)

if(QT_VERSION_MAJOR EQUAL 6)
    qt_finalize_executable(PennyWise)
endif()

# Datenbankfiles kopieren (fürs Release...)
 set(DB_SOURCE_DIR "${CMAKE_SOURCE_DIR}/db/")
 set(DB_DEST_DIR "${CMAKE_BINARY_DIR}/db/")
 file(MAKE_DIRECTORY ${DB_DEST_DIR})

# Kopiere alle Dateien aus dem db-Ordner im Projektverzeichnis ins Build-Verzeichnis
 file(GLOB DB_FILES "${DB_SOURCE_DIR}/*")
 foreach(file ${DB_FILES})
    file(COPY ${file} DESTINATION ${DB_DEST_DIR})
 endforeach()

 # Ressourcen Kopieren (auch fürs Release...)
 set(RES_SOURCE_DIR "${CMAKE_SOURCE_DIR}/resources/")
 set(RES_DEST_DIR "${CMAKE_BINARY_DIR}/resources/")
 file(MAKE_DIRECTORY ${RES_DEST_DIR})

 # Alles kopieren
 file(GLOB RES_FILES "${RES_SOURCE_DIR}/*")
 foreach(file ${RES_FILES})
   file(COPY ${file} DESTINATION ${RES_DEST_DIR})
endforeach()

set(APP_ICON_RESOURCE_WINDOWS "${CMAKE_CURRENT_SOURCE_DIR}/app.rc")
target_sources(PennyWise PRIVATE ${APP_ICON_RESOURCE_WINDOWS})
