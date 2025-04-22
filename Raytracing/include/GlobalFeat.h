#pragma once

enum RenderPath
{
    Forward = 0,
    GI = 1,
    DebugIA = 2,
    DebugBVH = 3,
    DebugOctree = 4,
    DebugKdTree = 5,
};

#define RENDER_PATH (RenderPath::DebugOctree)

// #define USE_BVH
#define USE_OCTREE
// #define USE_KDTREE


#define DEBUG_ACCELERATION_STRUCTURE

#ifdef DEBUG_ACCELERATION_STRUCTURE

#ifdef USE_BVH
#define DEBUG_BVH
#define DEBUG_BVH_START_DEPTH 2
#define DEBUG_BVH_END_DEPTH 2
#endif

#ifdef USE_OCTREE
#define DEBUG_OCTREE
#define DEBUG_OCTREE_START_DEPTH 2
#define DEBUG_OCTREE_END_DEPTH 2
#endif

#ifdef USE_KDTREE
#define DEBUG_KDTREE
#define DEBUG_KDTREE_START_DEPTH 4
#define DEBUG_KDTREE_END_DEPTH 4
#endif


#endif
