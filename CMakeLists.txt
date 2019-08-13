cmake_minimum_required(VERSION 3.4)
PROJECT (QTBORDER)

# using Qt meta-system (precompiler)
# Tell CMake to run moc when necessary:
# As moc files are generated in the binary dir, tell CMake
# to always look for includes there:
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTOUIC ON)
#set(CMAKE_AUTORCC ON)
set(CMAKE_INCLUDE_CURRENT_DIR ON)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR})

#set(CMAKE_CXX_FLAGS"${CMAKE_CXX_FLAGS} -g -Wall -Wno-deprecated -frounding-math -DIL_STD")

# compiler setting
if (CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=gnu++11")
else()
    set(CMAKE_CXX_STANDARD 11)
endif()

ADD_DEFINITIONS(-DQT_NO_KEYWORDS)

if (UNIX AND NOT APPLE)

  message(status "Setting G++ flags")
  #set( QT5_PATH_DIR "/home/$ENV{USER}/MyLib/release/qt5-package/5.8/gcc_64")
  set( GRAPHVIZ_PATH_DIR "/home/$ENV{USER}/MyLib/release/graphviz")

  # add the libraries you need below
  find_package(OpenGL REQUIRED)
  find_package(GLUT REQUIRED)
  find_package(CGAL QUIET COMPONENTS Core REQUIRED)

  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -g -Wall -Wno-deprecated -frounding-math -DIL_STD")

elseif(APPLE)

  message(status "Setting CLANG flags")

  # add the libraries you need below
  find_package(GLUT REQUIRED)
  find_package(OpenGL REQUIRED)
  find_package(CGAL QUIET COMPONENTS Core REQUIRED)

  set(CMAKE_CXX_FLAGS"${CMAKE_CXX_FLAGS} -g -Wall -Wno-deprecated -frounding-math -DIL_STD -framework OpenGL -framework GLUT")

endif()

# mandatory for CGAL, or will get a undefined reference to CGAL::assertion_fail
include(${CGAL_USE_FILE})

include_directories(
        /opt/local/include
        /usr/include
        /usr/include/x86_64-linux-gnu
        ${CMAKE_SOURCE_DIR}/include
        ${QT5_PATH_DIR}/include
        ${GRAPHVIZ_PATH_DIR}/include
)
link_directories(
        /opt/local/lib
        /usr/lib/x86_64-linux-gnu/
        ${CMAKE_SOURCE_DIR}/src
        ${QT5_PATH_DIR}/lib
        ${GRAPHVIZ_PATH_DIR}/lib
)


# automatically grab all the source files
#file(GLOB SOURCES src/*.cpp)
#file(GLOB HEADERS include/*.h)
#set(UI_FILES include/ui/Window.ui)
#set(RESOURCES *.qrc)

set(HEADERS
        include/base/LevelHigh.h
        include/base/Common.h
        include/base/CommonClass.h
        include/base/Config.h
        include/base/Coord2.h
        include/base/Polygon2.h
        include/base/Contour2.h
        include/base/Grid2.h
        include/base/QuardTree.h
        include/base/TimeComplexity.h
        include/base/ThreadBase.h
        include/base/ThreadOctilinearBoundary.h
        include/base/ThreadLevelHigh.h
        include/base/ThreadLevelCenter.h
        include/base/ThreadLevelMiddle.h
        include/base/ThreadLevelDetailed.h
        include/base/Color.h
        include/base/Cell.h
        include/base/Road.h
        include/base/Boundary.h
        include/base/Bone.h
        include/base/Pathway.h
        include/base/PathwayData.h
        include/base/RegionData.h
        include/base/Package.h
        include/graph/TreeEdgeProperty.h
        include/graph/TreeGraph.h
        include/graph/TreeGraphProperty.h
        include/graph/TreeVertexProperty.h
        include/graph/ForceEdgeProperty.h
        include/graph/ForceGraph.h
        include/graph/ForceGraphProperty.h
        include/graph/ForceVertexProperty.h
        include/graph/MetaboliteEdgeProperty.h
        include/graph/MetaboliteGraph.h
        include/graph/MetaboliteGraphProperty.h
        include/graph/MetaboliteVertexProperty.h
        include/graph/BoundaryEdgeProperty.h
        include/graph/BoundaryGraph.h
        include/graph/BoundaryGraphProperty.h
        include/graph/BoundaryVertexProperty.h
        include/graph/SkeletonEdgeProperty.h
        include/graph/SkeletonGraph.h
        include/graph/SkeletonGraphProperty.h
        include/graph/SkeletonVertexProperty.h
        include/graph/BaseEdgeProperty.h
        include/graph/BaseGraphProperty.h
        include/graph/BaseVertexProperty.h
        include/graph/DirectedBaseGraph.h
        include/graph/UndirectedBaseGraph.h
        include/graph/UndirectedPropertyGraph.h
        include/optimization/Force.h
        include/optimization/Voronoi.h
        include/optimization/Octilinear.h
        include/optimization/Smooth.h
        include/optimization/Stress.h
        include/optimization/Similarity.h
        include/optimization/Layout.h
        inlcude/steinertree/include.h
        include/ui/GraphicsBallItem.h
        include/ui/GraphicsVertexItem.h
        include/ui/GraphicsEdgeItem.h
        include/ui/GraphicsPolygonItem.h
        include/ui/GraphicsView.h
        include/ui/Window.h
        include/ui/Window.ui
        include/gv/GraphVizAPI.h
        include/boost/create_dual_graph.hpp
    )
set(SOURCES
        src/base/Common.cpp
        src/base/Config.cpp
        src/base/Coord2.cpp
        src/base/Polygon2.cpp
        src/base/Contour2.cpp
        src/base/Grid2.cpp
        src/base/Line2.cpp
        src/base/QuardTree.cpp
        src/base/TimeComplexity.cpp
        src/base/ThreadBase.cpp
        src/base/ThreadOctilinearBoundary.cpp
        src/base/ThreadLevelHigh.cpp
        src/base/ThreadLevelCenter.cpp
        src/base/ThreadLevelMiddle.cpp
        src/base/ThreadLevelDetailed.cpp
        src/base/Color.cpp
        src/base/LevelHigh.cpp
        src/base/Cell.cpp
        src/base/Road.cpp
        src/base/Boundary.cpp
        src/base/Bone.cpp
        src/base/Pathway.cpp
        src/base/PathwayData.cpp
        src/base/RegionData.cpp
        src/base/Package.cpp
        src/graph/TreeEdgeProperty.cpp
        src/graph/TreeGraph.cpp
        src/graph/TreeGraphProperty.cpp
        src/graph/TreeVertexProperty.cpp
        src/graph/ForceEdgeProperty.cpp
        src/graph/ForceGraph.cpp
        src/graph/ForceGraphProperty.cpp
        src/graph/ForceVertexProperty.cpp
        src/graph/MetaboliteEdgeProperty.cpp
        src/graph/MetaboliteGraph.cpp
        src/graph/MetaboliteGraphProperty.cpp
        src/graph/MetaboliteVertexProperty.cpp
        src/graph/BoundaryEdgeProperty.cpp
        src/graph/BoundaryGraph.cpp
        src/graph/BoundaryGraphProperty.cpp
        src/graph/BoundaryVertexProperty.cpp
        src/graph/SkeletonEdgeProperty.cpp
        src/graph/SkeletonGraph.cpp
        src/graph/SkeletonGraphProperty.cpp
        src/graph/SkeletonVertexProperty.cpp
        src/graph/DirectedBaseGraph.cpp
        src/graph/UndirectedBaseGraph.cpp
        src/graph/UndirectedPropertyGraph.cpp
        src/graph/BaseEdgeProperty.cpp
        src/graph/BaseGraphProperty.cpp
        src/graph/BaseVertexProperty.cpp
        src/optimization/Force.cpp
        src/optimization/Voronoi.cpp
        src/optimization/Octilinear.cpp
        src/optimization/Smooth.cpp
        src/optimization/Stress.cpp
        src/optimization/Similarity.cpp
        src/optimization/Layout.cpp
        src/steinertree/dijkstra.cpp
        src/steinertree/spanningtree.cpp
        src/ui/GraphicsBallItem.cpp
        src/ui/GraphicsVertexItem.cpp
        src/ui/GraphicsEdgeItem.cpp
        src/ui/GraphicsPolygonItem.cpp
        src/ui/GraphicsView.cpp
        src/ui/Window.cpp
        src/gv/GraphVizAPI.cpp
        src/main.cpp
    )

set( DEFINEQT5 ON )
find_package(Qt5 COMPONENTS Core Gui Svg Widgets OpenGL REQUIRED)

# using Qt meta-system (precompiler)
QT5_ADD_RESOURCES( RES_SOURCES ${RESOURCES} )
QT5_WRAP_UI( UI_HEADERS ${UI_FILES} )
QT5_WRAP_CPP( MOC_SRCS ${UI_HEADERS})
QT5_WRAP_CPP( MOC_SRCS ${MOC_HEADERS} )
#QT5_WRAP_CPP( MOC_WINDOWUI ${CMAKE_CURRENT_SOURCE_DIR}/include/ui/Window.ui )
QT5_WRAP_CPP( MOC_WINDOW ${CMAKE_CURRENT_SOURCE_DIR}/include/ui/Window.h )
QT5_WRAP_CPP( MOC_GRAPHICSVIEW ${CMAKE_CURRENT_SOURCE_DIR}/include/ui/GraphicsView.h )
#set_property( SOURCE ${CMAKE_CURRENT_SOURCE_DIR}/cmake-build-debug/include/ui/moc_ui_Window.cpp PROPERTY SKIP_AUTOGEN ON )
set_property( SOURCE ${CMAKE_CURRENT_SOURCE_DIR}/cmake-build-debug/include/ui/moc_Window.cpp PROPERTY SKIP_AUTOGEN ON )
set_property( SOURCE ${CMAKE_CURRENT_SOURCE_DIR}/cmake-build-debug/include/ui/moc_GraphicsView.cpp PROPERTY SKIP_AUTOGEN ON )

add_executable(qtborder ${SOURCES} ${MOC_SRCS} ${MOC_WINDOW} ${MOC_GRAPHICSVIEW})
message("CMAKE_CXX_FLAGS" ${CMAKE_CXX_FLAGS})
if (UNIX AND NOT APPLE)
  target_link_libraries (qtborder ${CMAKE_CXX_FLAGS} tinyxml cgraph gvc CGAL gmp boost_thread boost_system boost_graph dl pthread GL GLU glut)
elseif(APPLE)
  target_link_libraries (qtborder ${CMAKE_CXX_FLAGS} tinyxml boost_thread-mt boost_system-mt boost_graph-mt CGAL gmp dl)
endif()

# qt stuff
qt5_use_modules(qtborder Core Gui Svg Widgets OpenGL)
