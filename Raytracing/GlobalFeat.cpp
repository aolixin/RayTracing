#include "GlobalFeat.h"

RenderPath renderPath = RenderPath::RT;

// 测试指标初始化
float test_build_time = 0.0f;    // 构建时间（毫秒）
size_t test_total_memory = 0;    // 总内存使用（字节）
size_t test_node_memory = 0;     // 节点内存使用（字节）
size_t test_tri_memory = 0;      // 三角形内存使用（字节）
int test_node_count = 0;         // 节点数量
int test_tri_count = 0;          // 三角形数量

//int pathIdx = 0;

int  DEBUG_BVH_START_DEPTH = 0;
int  DEBUG_BVH_END_DEPTH = 0;

int  DEBUG_OCTREE_START_DEPTH = 0;
int   DEBUG_OCTREE_END_DEPTH = 0;

int DEBUG_KDTREE_START_DEPTH = 0;
int DEBUG_KDTREE_END_DEPTH = 0;