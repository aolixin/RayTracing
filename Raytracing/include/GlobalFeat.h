#pragma once

#define DEBUG_ACCELERATION_STRUCTURE

#define TEST_ACCELERATION_STRUCTURE


enum RenderPath
{
    Forward = 0,
    GI = 1,
    DebugIA = 2,

#ifdef DEBUG_ACCELERATION_STRUCTURE
    DebugBVH = 3,
    DebugOctree = 4,
    DebugKdTree = 5,
#endif

#ifdef TEST_ACCELERATION_STRUCTURE
    TestBVH = 6,
    TestOctree = 7,
    TestKdTree = 8,
#endif
};

#define RELEASE_PATH (RenderPath::Forward)

#define TEST_PATH (RenderPath::TestKdTree)


#ifdef TEST_ACCELERATION_STRUCTURE
#define RENDER_PATH TEST_PATH
#else
#define RENDER_PATH RELEASE_PATH
#endif


// #define USE_BVH
// #define USE_OCTREE
#define USE_KDTREE


#ifdef DEBUG_ACCELERATION_STRUCTURE

#ifdef USE_BVH
#define DEBUG_BVH
#define DEBUG_BVH_START_DEPTH 1
#define DEBUG_BVH_END_DEPTH 1
#endif

#ifdef USE_OCTREE
#define DEBUG_OCTREE
#define DEBUG_OCTREE_START_DEPTH 1
#define DEBUG_OCTREE_END_DEPTH 2
#endif

#ifdef USE_KDTREE
#define DEBUG_KDTREE
#define DEBUG_KDTREE_START_DEPTH 2
#define DEBUG_KDTREE_END_DEPTH 2
#endif


#endif
