cmake_minimum_required(VERSION 3.10)
PROJECT (HARMONY)
set(Target_Name harmony)

#----------------------------------------------------------------------
# cmake setting
#----------------------------------------------------------------------
# set output folder
# set(CMAKE_INCLUDE_CURRENT_DIR ON)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin")
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin")
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin")
set(CMAKE_BUILD_TYPE RELEASE)

#----------------------------------------------------------------------
# compiler setting
#----------------------------------------------------------------------
set(CMAKE_C_COMPILER /usr/bin)
if( APPLE )
  set( CMAKE_CXX_STANDARD 11)
  set( CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -framework Accelerate -DEIGEN_USE_BLAS -ltinyxml -lcgraph -lgvc -lboost_thread-mt -lboost_system-mt -lboost_graph-mt -lcgal -lgmp -lmpfr -ldl -Wa" )
elseif( UNIX )
  message( "Ubuntu" )
  set( CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=gnu++11 -DEIGEN_USE_ACML -ltinyxml -lcgraph -lgvc -lboost_thread -lboost_system -lboost_graph -lCGAL -lgmp -lmpfr -ldl -lpthread" )
endif()
#set( CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fno-inline -g -Wall -Wno-deprecated -DIL_STD -O3 -DNDEBUG" )
set( CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fno-inline -DIL_STD -O3 -DNDEBUG -frounding-math" )
#set( CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fno-inline -g -Wall -Wno-deprecated -DIL_STD -O3 -DNDEBUG -openmp -omp" )
#set( CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fno-inline -g -Wall -Wno-deprecated -DIL_STD" )

#----------------------------------------------------------------------
# sources
#----------------------------------------------------------------------
file( GLOB_RECURSE SOURCES
        ../third_party/steinertree/*.h
        ../third_party/steinertree/*.cpp
        src/*.h
        src/*.cpp
        resources/*.qrc
        )
add_executable(${Target_Name} ${SOURCES})


#----------------------------------------------------------------------
# path setting
#----------------------------------------------------------------------
if( APPLE )

  #set(THIRD_PARTY_INCLUDE ${THIRD_PARTY_INCLUDE} ${CMAKE_SOURCE_DIR}/third_party/mylib/include/pcl-1.9/ )
  #set(THIRD_PARTY_LIB ${THIRD_PARTY_LIB} ${CMAKE_SOURCE_DIR}/third_party/mylib/lib/ )

  target_include_directories(${Target_Name}
          PUBLIC
          /opt/local/include/
          ${CMAKE_SOURCE_DIR}/src/
          #${CMAKE_SOURCE_DIR}/third_party/libSBGN2/src/
          #${CMAKE_SOURCE_DIR}/third_party/libSBGN2/build/src/
          ${THIRD_PARTY_INCLUDE}
          )

  target_link_directories(${Target_Name}
          PUBLIC
          /opt/local/lib/
          ${CMAKE_SOURCE_DIR}/src/
          ${THIRD_PARTY_LIB}
          )

elseif( UNIX )

  #set(THIRD_PARTY_INCLUDE ${THIRD_PARTY_INCLUDE} ${CMAKE_SOURCE_DIR}/third_party/mylib/include/pcl-1.9/ )
  #set(THIRD_PARTY_LIB ${THIRD_PARTY_LIB} ${CMAKE_SOURCE_DIR}/third_party/mylib/lib/ )
  set( QT5_PATH_DIR "/home/$ENV{USER}/MyLib/release/qt5-package/5.8/gcc_64")
  #set( QT5_PATH_DIR "/home/$ENV{USER}/MyLib/release/qt5-package/5.8/gcc_64")
  set( GRAPHVIZ_PATH_DIR "/home/$ENV{USER}/MyLib/release/graphviz")

  target_include_directories(${Target_Name}
          PUBLIC
          /opt/local/include/
          /usr/include/
          /usr/include/x86_64-linux-gnu/
          ${CMAKE_SOURCE_DIR}/src/
          #${QT5_PATH_DIR}/include/
          ${GRAPHVIZ_PATH_DIR}/include/
          ${THIRD_PARTY_INCLUDE}
          )

  target_link_directories( ${Target_Name}
          PUBLIC
          /opt/local/lib/
          /usr/lib/
          /usr/lib/x86_64-linux-gnu/
          ${CMAKE_SOURCE_DIR}/src/
          ${THIRD_PARTY_LIB}
          #${QT5_PATH_DIR}/lib/
          ${GRAPHVIZ_PATH_DIR}/lib/
          )
elseif( WIN32 )
endif()

# strip the trailing space
string(STRIP ${CMAKE_CXX_FLAGS} CMAKE_CXX_FLAGS)
SET_PROPERTY(TARGET ${Target_Name} PROPERTY DEBUG_POSTFIX ${CMAKE_DEBUG_POSTFIX})

if( APPLE )
  target_link_libraries(${Target_Name} PUBLIC ${CMAKE_CXX_FLAGS} ${OPENGL_LIBRARIES} ${GLUT_LIBRARIES}
          )
  set_target_properties(${Target_Name} PROPERTIES
          OUTPUT_NAME ${Target_Name}
          )
elseif( UNIX )
  target_link_libraries(${Target_Name} PUBLIC ${CMAKE_CXX_FLAGS} ${OPENGL_LIBRARIES} ${GLUT_LIBRARIES} )
  set_target_properties(${Target_Name} PROPERTIES
          OUTPUT_NAME ${Target_Name}
          )
elseif( WIN32 )
endif()

#----------------------------------------------------------------------
# QT
#----------------------------------------------------------------------
message(STATUS "-- Linking qt")
set(QT_REQUIRED_COMPONENTS Core Gui Svg Widgets OpenGL)
find_package(Qt5 COMPONENTS ${QT_REQUIRED_COMPONENTS} REQUIRED)
ADD_DEFINITIONS(-DQT_NO_KEYWORDS)

# Add components to the target
foreach(component ${QT_REQUIRED_COMPONENTS})
  target_link_libraries(${Target_Name}
          PUBLIC
          Qt5::${component}
          )
endforeach()

# Enable automoc/uic/rcc
set_target_properties(${Target_Name}
        PROPERTIES
        AUTOMOC ON
        AUTOUIC ON
        AUTORCC ON
        )

#----------------------------------------------------------------------
# EIGEN
#----------------------------------------------------------------------
message(STATUS "-- Linking eigen")

find_package(Eigen3 3.3 REQUIRED)
target_link_libraries(${Target_Name}
        PUBLIC
        Eigen3::Eigen
        )

#----------------------------------------------------------------------
# Boost
#----------------------------------------------------------------------
message(STATUS "-- Linking boost")

set(Boost_USE_STATIC_LIBS OFF)
set(Boost_USE_MULTITHREADED ON)
set(Boost_USE_STATIC_RUNTIME OFF)
find_package(Boost)

if(Boost_FOUND)
  include_directories(${Boost_INCLUDE_DIRS})
  target_link_libraries(${Target_Name} ${Boost_LIBRARIES})
endif()

#----------------------------------------------------------------------
# CGAL
#----------------------------------------------------------------------
message(STATUS "-- Linking CGAL")

find_package(CGAL QUIET COMPONENTS Core REQUIRED)

if(CGAL_FOUND)
  include_directories(${CGAL_USE_FILE})
  target_link_libraries(${Target_Name} ${CGAL_LIBRARIES})
endif()


#----------------------------------------------------------------------
# COPY CONFIG AND DATA FOLDER
#----------------------------------------------------------------------

# Create directories
execute_process( COMMAND ${CMAKE_COMMAND} -E make_directory ${directory} "${CMAKE_BINARY_DIR}/bin/third_party/" )
add_custom_command(TARGET ${Target_Name}
        POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E make_directory ${directory} "$<TARGET_FILE_DIR:${Target_Name}>/config/"
        COMMAND ${CMAKE_COMMAND} -E make_directory ${directory} "$<TARGET_FILE_DIR:${Target_Name}>/data/"
        COMMAND ${CMAKE_COMMAND} -E make_directory ${directory} "$<TARGET_FILE_DIR:${Target_Name}>/svg/"
        COMMAND ${CMAKE_COMMAND} -E make_directory ${directory} "${CMAKE_BINARY_DIR}/bin/third_party/micans"
        )

execute_process( COMMAND ${CMAKE_COMMAND} -E create_symlink "${PROJECT_SOURCE_DIR}/config/"  ${CMAKE_BINARY_DIR}/bin/config )
execute_process( COMMAND ${CMAKE_COMMAND} -E create_symlink "${PROJECT_SOURCE_DIR}/data/"  ${CMAKE_BINARY_DIR}/bin/data )
execute_process( COMMAND ${CMAKE_COMMAND} -E create_symlink "${PROJECT_SOURCE_DIR}/svg/"  ${CMAKE_BINARY_DIR}/bin/svg )
execute_process( COMMAND ${CMAKE_COMMAND} -E create_symlink "${PROJECT_SOURCE_DIR}/../third_party/micans"  ${CMAKE_BINARY_DIR}/bin/third_party/micans )