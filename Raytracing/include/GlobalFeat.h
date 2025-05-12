#pragma once


constexpr unsigned int SCR_WIDTH = 1920;
constexpr unsigned int SCR_HEIGHT = 1080;

constexpr unsigned int RENDER_WIDTH = 800;
constexpr unsigned int RENDER_HEIGHT = 600;

//extern int mode_idx;
//
//extern int AS_idx;

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


extern int  DEBUG_BVH_START_DEPTH;
extern int  DEBUG_BVH_END_DEPTH;

extern int  DEBUG_OCTREE_START_DEPTH;
extern int   DEBUG_OCTREE_END_DEPTH;

extern int DEBUG_KDTREE_START_DEPTH;
extern int DEBUG_KDTREE_END_DEPTH;

// test metrics
extern float test_build_time;    // 构建时间（毫秒）
extern size_t test_total_memory; // 总内存使用（字节）
extern size_t test_node_memory;  // 节点内存使用（字节）
extern size_t test_tri_memory;   // 三角形内存使用（字节）
extern int test_node_count;      // 节点数量
extern int test_tri_count;       // 三角形数量




