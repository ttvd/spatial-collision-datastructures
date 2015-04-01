CMAKE_MINIMUM_REQUIRED(VERSION 2.8.1)
SET(CMAKE_ALLOW_LOOSE_LOOP_CONSTRUCTS true)
CMAKE_POLICY(SET CMP0003 NEW)
CMAKE_POLICY(SET CMP0015 NEW)

PROJECT(spatial-benchmark)

if (${CMAKE_SYSTEM_NAME} MATCHES Windows)
    set (SB_FREEGLUT_DIR "${CMAKE_SOURCE_DIR}/freeglut")
    include_directories (${SB_FREEGLUT_DIR}/include)
    link_directories (${SB_FREEGLUT_DIR}/lib)
endif ()

set (sb_sources
    BruteForce.cpp
    HierarchicalGrid.cpp
    Kdtree.cpp
    LooseOctree.cpp
    Octree.cpp
    SortAndSweep.cpp
    SphereObject.cpp
    UniformGrid.cpp
    Core.cpp
)

set (sb_headers
    Base.h
    BruteForce.h
    HierarchicalGrid.h
    ISpatialCell.h
    ISpatialObject.h
    ISpatialStructure.h
    Kdtree.h
    LooseOctree.h
    Octree.h
    SortAndSweep.h
    SphereObject.h
    UniformGrid.h
    Vector3.h
)

add_executable (spatial-benchmark ${sb_sources} ${sb_headers})

if (${CMAKE_SYSTEM_NAME} MATCHES Darwin)
    target_link_libraries (spatial-benchmark "-framework OpenGL -framework GLUT -framework ApplicationServices")
elseif (${CMAKE_SYSTEM_NAME} MATCHES Linux)
    target_link_libraries (spatial-benchmark glut GLU GL)
elseif (${CMAKE_SYSTEM_NAME} MATCHES Windows)
    target_link_libraries (spatial-benchmark freeglut opengl32)
endif()
