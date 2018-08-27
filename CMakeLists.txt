cmake_minimum_required(VERSION 3.4)
PROJECT (QTBORDER)

# using Qt meta-system (precompiler)
# Tell CMake to run moc when necessary
# As moc files are generated in the binary dir, tell CMake
# to always look for includes there:
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTOUIC ON)
#set(CMAKE_AUTORCC ON)
set(CMAKE_INCLUDE_CURRENT_DIR ON)

if (UNIX AND NOT APPLE)
  message(status "Setting G++ flags")
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -O -Wall -Wno-deprecated -DIL_STD -std=gnu++11 -frounding-math -Wunused-variable")
  include_directories(
      /usr/include
      /usr/include/eigen3
      /usr/X11R6/include
  	  /opt/local/include
      /home/$ENV{USER}/MyLib/release/qt5-package/5.8/gcc_64/include
      ${CMAKE_SOURCE_DIR}/include
      ${CMAKE_SOURCE_DIR}/src
  )
  link_directories(
      /usr/X11R6/lib
      /opt/local/lib
      /usr/lib/x86_64-linux-gnu
      /home/$ENV{USER}/MyLib/release/qt5-package/5.8/gcc_64/lib
  )
  # add the libraries you need below
  set(OPENGL_FLAG GL GLU glut)
  find_package(OpenGL REQUIRED)
  find_package(GLUT REQUIRED)
  find_package(CGAL QUIET COMPONENTS Core REQUIRED)
elseif(APPLE)
  message(status "Setting CLANG flags")
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -g -Wall -Wno-deprecated -DIL_STD -std=c++11")
  include_directories(
      /opt/local/include
      /opt/local/include/Eigen3
	  ${CMAKE_SOURCE_DIR}/include
      ${CMAKE_SOURCE_DIR}/src
  )
  link_directories(
      /opt/local/lib
  )
  # add the libraries you need below
  set( OPENGL_FLAG "-framework OpenGL" "-framework GLUT")
  find_package(GLUT REQUIRED)
  find_package(OpenGL REQUIRED)
endif()

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_INCLUDE_CURRENT_DIR ON)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR})


# automatically grab all the source files
file(GLOB SOURCES src/*.cpp)
file(GLOB HEADERS include/*.h)
file(GLOB UI_FILES include/*.ui)
file(GLOB RESOURCES *.qrc)

set( DEFINEQT5 ON )
set(CMAKE_PREFIX_PATH ${CMAKE_PREFIX_PATH} "/home/yun/MyLib/release/qt5-package/5.8/gcc_64/lib/cmake/Qt5" )
find_package(Qt5 COMPONENTS Core Gui Svg Widgets OpenGL REQUIRED)
# using Qt meta-system (precompiler)
QT5_ADD_RESOURCES( RES_SOURCES ${RESOURCES} )
QT5_WRAP_UI( UI_HEADERS ${UI_FILES} )
QT5_WRAP_CPP( MOC_SRCS ${UI_HEADERS})
QT5_WRAP_CPP( MOC_SRCS ${MOC_HEADERS} )
QT5_WRAP_CPP( MOC_WINDOW ${CMAKE_CURRENT_SOURCE_DIR}/include/Window.h )
QT5_WRAP_CPP( MOC_WINDOW ${CMAKE_CURRENT_SOURCE_DIR}/include/GraphicsView.h )
set_property( SOURCE ${CMAKE_CURRENT_SOURCE_DIR}/cmake-build-debug/moc_ui_Window.cpp PROPERTY SKIP_AUTOGEN ON )
set_property( SOURCE ${CMAKE_CURRENT_SOURCE_DIR}/cmake-build-debug/include/moc_Window.cpp PROPERTY SKIP_AUTOGEN ON )
set_property( SOURCE ${CMAKE_CURRENT_SOURCE_DIR}/cmake-build-debug/include/moc_GraphicsView.cpp PROPERTY SKIP_AUTOGEN ON )

add_executable (qtborder ${SOURCES} ${MOC_SRCS} ${MOC_WINDOW} ${MOC_WIDGET})
if (UNIX AND NOT APPLE)
    target_link_libraries (qtborder pthread boost_thread boost_system boost_graph CGAL gmp mpfr ${OPENGL_FLAG} ${QT_LIBRARIES} pthread dl)
elseif (APPLE)
    target_link_libraries (qtborder pthread CGAL gmp mpfr ${OPENGL_FLAG} ${QT_LIBRARIES})
endif()
# qt stuff
qt5_use_modules(qtborder Core Gui Svg Widgets OpenGL)
