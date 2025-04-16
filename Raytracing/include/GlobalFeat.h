#pragma once

enum RenderPath
{
    Forward = 0,
    GI = 1,
    DebugIA = 2,
    DebugBVH = 3,
    Count
};

#define DEBUG_BVH
#define DEBUG_BVH_START_DEPTH 2
#define DEBUG_BVH_END_DEPTH 2


#define RENDER_PATH (RenderPath::DebugBVH)