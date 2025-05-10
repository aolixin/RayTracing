#pragma once

//#define DEBUG_MODE
//#define TEST_MODE
//#define RT_MODE

constexpr unsigned int SCR_WIDTH = 1920;
constexpr unsigned int SCR_HEIGHT = 1080;

constexpr unsigned int RENDER_WIDTH = 800;
constexpr unsigned int RENDER_HEIGHT = 600;

extern int mode_idx;

extern int AS_idx;

enum RenderPath
{
	RT,
	Forward,
	DebugIA,

	DebugBVH,
	DebugOctree,
	DebugKdTree,

	TestBVH,
	TestOctree,
	TestKdTree,

	Count
};

extern RenderPath renderPath;

#define RELEASE_PATH (RenderPath::RT)

#define TEST_PATH (RenderPath::TestBVH)

#define DEBUG_PATH (RenderPath::DebugBVH)



//#define USE_BVH
// #define USE_OCTREE
// #define USE_KDTREE



#define DEBUG_BVH
extern int  DEBUG_BVH_START_DEPTH;
extern int  DEBUG_BVH_END_DEPTH;

#define DEBUG_OCTREE
extern int  DEBUG_OCTREE_START_DEPTH;
extern int   DEBUG_OCTREE_END_DEPTH;

#define DEBUG_KDTREE
extern int DEBUG_KDTREE_START_DEPTH;
extern int DEBUG_KDTREE_END_DEPTH;


