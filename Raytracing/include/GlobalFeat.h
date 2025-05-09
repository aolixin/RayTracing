#pragma once

#define DEBUG_MODE
//#define TEST_MODE
//#define RELEASE_MODE


enum RenderPath
{
	Forward = 0,
	GI = 1,
	DebugIA = 2,

#ifdef DEBUG_MODE
	DebugBVH = 3,
	DebugOctree = 4,
	DebugKdTree = 5,
#endif

#ifdef TEST_MODE
	TestBVH = 6,
	TestOctree = 7,
	TestKdTree = 8,
#endif
};

#define RELEASE_PATH (RenderPath::GI)

#define TEST_PATH (RenderPath::TestBVH)

#define DEBUG_PATH (RenderPath::DebugBVH)


#ifdef TEST_MODE

#define RENDER_PATH TEST_PATH

#elif defined(DEBUG_MODE)

#define RENDER_PATH DEBUG_PATH

#else

#define RENDER_PATH RELEASE_PATH

#endif


#define USE_BVH
// #define USE_OCTREE
// #define USE_KDTREE


#ifdef DEBUG_MODE

#ifdef USE_BVH
#define DEBUG_BVH
#define DEBUG_BVH_START_DEPTH 0
#define DEBUG_BVH_END_DEPTH 0
#endif

#ifdef USE_OCTREE
#define DEBUG_OCTREE
#define DEBUG_OCTREE_START_DEPTH 0
#define DEBUG_OCTREE_END_DEPTH 0
#endif

#ifdef USE_KDTREE
#define DEBUG_KDTREE
#define DEBUG_KDTREE_START_DEPTH 0
#define DEBUG_KDTREE_END_DEPTH 0
#endif


#endif
