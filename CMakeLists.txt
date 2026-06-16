cmake_minimum_required(VERSION 3.19)
project(Laba4 LANGUAGES CXX)

find_package(Qt6 6.5 REQUIRED COMPONENTS Core Widgets LinguistTools)

qt_standard_project_setup()

qt_add_executable(Laba4
    WIN32 MACOSX_BUNDLE
    main.cpp
    mainwindow.cpp
    mainwindow.h
    mainwindow.ui
    LazySequence.h
    Sequence.h
    LinkedList.h
    LinkedListSequence.h
    ArraySequence.h
    DynamicArray.h
    MapUtils.h
    ZipUnzip.h
    SequenceException.h
    Streams.h
    IStream.h
    StreamExceptions.h
    TaskSyncWorker.cpp
    TaskSyncWorker.h
    tasksortworker.h tasksortworker.cpp
    TimeValue.h
    HeapSorter.h
    BinaryHeap.h
    StreamSynchronizer.h
    WorkingWithSequence.h
    MainMenu.h
    Cardinality.h

)

qt_add_translations(
    TARGETS Laba4
    TS_FILES Laba4_ru_RU.ts
)

target_link_libraries(Laba4
    PRIVATE
        Qt::Core
        Qt::Widgets
)

include(GNUInstallDirs)

install(TARGETS Laba4
    BUNDLE  DESTINATION .
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
)

qt_generate_deploy_app_script(
    TARGET Laba4
    OUTPUT_SCRIPT deploy_script
    NO_UNSUPPORTED_PLATFORM_ERROR
)
install(SCRIPT ${deploy_script})
