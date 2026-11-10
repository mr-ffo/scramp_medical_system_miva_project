cmake_minimum_required(VERSION 3.16)
project(SCAPMS VERSION 1.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Automatically process Qt macros
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

find_package(Qt6 COMPONENTS Widgets REQUIRED)

# We will add all our .cpp files here as we build them
# Add all your .cpp files here so CMake knows to compile them

add_executable(SCAPMS
    src/main.cpp
    src/model/GeneralPatient.cpp
    src/model/EmergencyPatient.cpp
    src/model/FollowUpPatient.cpp
    src/controller/ClinicDatabase.cpp
    src/controller/SearchEngine.cpp
    src/controller/SortEngine.cpp
    src/utils/FileHandler.cpp
    include/gui/MainWindow.h
    src/gui/MainWindow.cpp
    include/gui/PatientPanel.h
    src/gui/PatientPanel.cpp
    include/gui/SearchSortPanel.h
    src/gui/SearchSortPanel.cpp
    include/gui/AppointmentPanel.h
    src/gui/AppointmentPanel.cpp
    include/gui/QueuePanel.h       # <--- ADD THIS
    src/gui/QueuePanel.cpp         # <--- ADD THIS
)

target_link_libraries(SCAPMS PRIVATE Qt6::Widgets)


# THIS LINE FIXES THE INCLUDE ERROR:
target_include_directories(SCAPMS PRIVATE include)
