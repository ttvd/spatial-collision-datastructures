CMAKE_MINIMUM_REQUIRED(VERSION 2.8)
SET(CMAKE_ALLOW_LOOSE_LOOP_CONSTRUCTS true)
CMAKE_POLICY(SET CMP0003 NEW)

PROJECT(spatial-benchmark)

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
    target_link_libraries (spatial-benchmark glut32 opengl32)
endif()
