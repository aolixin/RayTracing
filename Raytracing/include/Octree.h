#pragma once
#include <glad/glad.h> // holds all OpenGL type declarations
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include "Triangle.h"
#include <algorithm>
#include <string>
#include <vector>
#include "GlobalFeat.h"
#include <functional>
#include <unordered_set>

using namespace std;
using namespace glm;

#define INF 114514.0
#define MIN_NODE_SIZE 0.01 // 新增：节点的最小尺寸限制

// 八叉树节点结构体
struct OctreeNode
{
    vector<int> triangleIndices;
    vec3 center;
    float halfSize;
    int children[8];

    OctreeNode()
    {
        for (int i = 0; i < 8; ++i)
        {
            children[i] = -1;
        }
    }
};

// 八叉树类
class Octree
{
public:
    int maxTrianglesPerNode = 16;
    vector<OctreeNode> nodes;
    vector<Triangle> triangles;

    Octree()
    {
    }

    Octree(vector<Triangle> triangles)
    {
        this->triangles = triangles;
        // 计算初始边界框
        vec3 AA = vec3(INF, INF, INF);
        vec3 BB = vec3(-INF, -INF, -INF);
        for (const auto& triangle : triangles)
        {
            for (int i = 0; i < 3; ++i)
            {
                AA.x = std::min(AA.x, triangle.vertex[i].Position.x);
                AA.y = std::min(AA.y, triangle.vertex[i].Position.y);
                AA.z = std::min(AA.z, triangle.vertex[i].Position.z);
                BB.x = std::max(BB.x, triangle.vertex[i].Position.x);
                BB.y = std::max(BB.y, triangle.vertex[i].Position.y);
                BB.z = std::max(BB.z, triangle.vertex[i].Position.z);
            }
        }
        vec3 center = (AA + BB) / 2.0f;
        float halfSize = std::max(std::max(BB.x - AA.x, BB.y - AA.y), BB.z - AA.z) / 2.0f;

        nodes.push_back(OctreeNode());
        int rootId = (int)nodes.size() - 1;
        nodes[rootId].center = center;
        nodes[rootId].halfSize = halfSize;

        vector<int> triangleIndices;
        for (int i = 0; i < triangles.size(); ++i)
        {
            triangleIndices.push_back(i);
        }
        unordered_set<int> addedTriangles;
        buildOctree(rootId, triangleIndices, addedTriangles);

        cout << "add triangles num = " << addedTriangles.size() << endl;
    }

private:
    // 判断三角形是否与八叉树节点的边界框相交
    bool triangleIntersectsBox(const Triangle& triangle, const vec3& center, float halfSize)
    {
        // 松散八叉树，外层边界半径是内层边界的 1.5 倍
        // float looseHalfSize = halfSize * 1.5f;
        vec3 minBound = center - vec3(halfSize, halfSize, halfSize);
        vec3 maxBound = center + vec3(halfSize, halfSize, halfSize);


        for (int i = 0; i < 3; ++i)
        {
            if (triangle.vertex[i].Position.x >= minBound.x && triangle.vertex[i].Position.x <= maxBound.x &&
                triangle.vertex[i].Position.y >= minBound.y && triangle.vertex[i].Position.y <= maxBound.y &&
                triangle.vertex[i].Position.z >= minBound.z && triangle.vertex[i].Position.z <= maxBound.z)
            {
                return true;
            }
        }
        return false;
    }

    // 构建八叉树
    void buildOctree(int nodeId, const vector<int>& triangleIndices, unordered_set<int>& addedTriangles)
    {
        if (triangleIndices.size() <= maxTrianglesPerNode || nodes[nodeId].halfSize <= MIN_NODE_SIZE)
        {
            for (int index : triangleIndices)
            {
                if (addedTriangles.find(index) == addedTriangles.end())
                {
                    nodes[nodeId].triangleIndices.push_back(index);
                    addedTriangles.insert(index);
                }
            }
            // if (nodes[nodeId].halfSize <= MIN_NODE_SIZE)
            {
                // 重新计算包围盒
                vec3 AA = vec3(INF, INF, INF);
                vec3 BB = vec3(-INF, -INF, -INF);
                for (int index : nodes[nodeId].triangleIndices)
                {
                    const Triangle& triangle = triangles[index];
                    for (int i = 0; i < 3; ++i)
                    {
                        AA.x = std::min(AA.x, triangle.vertex[i].Position.x);
                        AA.y = std::min(AA.y, triangle.vertex[i].Position.y);
                        AA.z = std::min(AA.z, triangle.vertex[i].Position.z);
                        BB.x = std::max(BB.x, triangle.vertex[i].Position.x);
                        BB.y = std::max(BB.y, triangle.vertex[i].Position.y);
                        BB.z = std::max(BB.z, triangle.vertex[i].Position.z);
                    }
                }
                nodes[nodeId].center = (AA + BB) / 2.0f;
                nodes[nodeId].halfSize = std::max(std::max(BB.x - AA.x, BB.y - AA.y), BB.z - AA.z) / 2.0f;
            }
            return;
        }

        float childHalfSize = nodes[nodeId].halfSize / 2.0f;
        vec3 center = nodes[nodeId].center;
        vec3 childCenters[8] = {
            vec3(center.x + childHalfSize, center.y + childHalfSize, center.z + childHalfSize),
            vec3(center.x + childHalfSize, center.y + childHalfSize, center.z - childHalfSize),
            vec3(center.x + childHalfSize, center.y - childHalfSize, center.z + childHalfSize),
            vec3(center.x + childHalfSize, center.y - childHalfSize, center.z - childHalfSize),
            vec3(center.x - childHalfSize, center.y + childHalfSize, center.z + childHalfSize),
            vec3(center.x - childHalfSize, center.y + childHalfSize, center.z - childHalfSize),
            vec3(center.x - childHalfSize, center.y - childHalfSize, center.z + childHalfSize),
            vec3(center.x - childHalfSize, center.y - childHalfSize, center.z - childHalfSize)
        };

        vector<vector<int>> childTriangleIndices(8);
        for (int index : triangleIndices)
        {
            if (addedTriangles.find(index) != addedTriangles.end()) continue;
            const Triangle& triangle = triangles[index];
            for (int i = 0; i < 8; ++i)
            {
                if (triangleIntersectsBox(triangle, childCenters[i], childHalfSize))
                {
                    childTriangleIndices[i].push_back(index);
                }
            }
        }

        float newHalf = nodes[nodeId].halfSize;
        
        for (int i = 0; i < 8; ++i)
        {
            if (!childTriangleIndices[i].empty())
            {
                nodes.push_back(OctreeNode());
                int childId = (int)nodes.size() - 1;
                nodes[childId].center = childCenters[i];
                nodes[childId].halfSize = childHalfSize;
                nodes[nodeId].children[i] = childId;
                buildOctree(childId, childTriangleIndices[i], addedTriangles);
                
                newHalf = std::max(newHalf,nodes[childId].halfSize * 2.0f);
            }
        }

        nodes[nodeId].halfSize = newHalf;

    }

public:
#ifdef DEBUG_OCTREE
public:
    GLuint DebugVAO, DebugVBO, DebugEBO;
    std::vector<GLuint> DebugIndices;
    // 调试函数，用于可视化八叉树的边界框
    void BuildDebugOctree_l(int startDepth, int endDepth)
    {
        std::vector<GLfloat> vertices;
        // 递归函数来遍历八叉树
        std::function<void(int, int)> traverse = [&](int nodeId, int currentDepth)
        {
            if (nodeId >= nodes.size() || currentDepth > endDepth) return;
            if (currentDepth < startDepth)
            {
                for (int i = 0; i < 8; ++i)
                {
                    if (nodes[nodeId].children[i] != -1)
                    {
                        traverse(nodes[nodeId].children[i], currentDepth + 1);
                    }
                }
                return;
            }
            const OctreeNode& node = nodes[nodeId];
            vec3 AA = node.center - vec3(node.halfSize, node.halfSize, node.halfSize);
            vec3 BB = node.center + vec3(node.halfSize, node.halfSize, node.halfSize);

            // 8个顶点
            vertices.insert(vertices.end(), {
                                AA.x, AA.y, AA.z,
                                BB.x, AA.y, AA.z,
                                BB.x, BB.y, AA.z,
                                AA.x, BB.y, AA.z,
                                AA.x, AA.y, BB.z,
                                BB.x, AA.y, BB.z,
                                BB.x, BB.y, BB.z,
                                AA.x, BB.y, BB.z
                            });

            // 线段
            GLuint baseIndex = (GLuint)(vertices.size() / 3 - 8);
            DebugIndices.insert(DebugIndices.end(), {
                                    baseIndex + 0, baseIndex + 1,
                                    baseIndex + 1, baseIndex + 2,
                                    baseIndex + 2, baseIndex + 3,
                                    baseIndex + 3, baseIndex + 0,
                                    baseIndex + 4, baseIndex + 5,
                                    baseIndex + 5, baseIndex + 6,
                                    baseIndex + 6, baseIndex + 7,
                                    baseIndex + 7, baseIndex + 4,
                                    baseIndex + 0, baseIndex + 4,
                                    baseIndex + 1, baseIndex + 5,
                                    baseIndex + 2, baseIndex + 6,
                                    baseIndex + 3, baseIndex + 7
                                });

            for (int i = 0; i < 8; ++i)
            {
                if (nodes[nodeId].children[i] != -1)
                {
                    traverse(nodes[nodeId].children[i], currentDepth + 1);
                }
            }
        };

        // 从根节点开始遍历
        traverse(0, 0);

        glGenVertexArrays(1, &DebugVAO);
        glGenBuffers(1, &DebugVBO);

        glBindVertexArray(DebugVAO);

        glBindBuffer(GL_ARRAY_BUFFER, DebugVBO);
        glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(GLfloat), vertices.data(), GL_STATIC_DRAW);

        glGenBuffers(1, &DebugEBO);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, DebugEBO);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, DebugIndices.size() * sizeof(GLuint), DebugIndices.data(),
                     GL_STATIC_DRAW);

        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), (GLvoid*)0);
        glEnableVertexAttribArray(0);

        glBindBuffer(GL_ARRAY_BUFFER, 0);
        glBindVertexArray(0);
    }


    // 计算八叉树的内存占用，单位为 KB
    float getMemoryUsageInKB() const
    {
        size_t triangleMemory = triangles.size() * sizeof(Triangle);
        size_t nodeMemory = 0;
        for (const auto& node : nodes)
        {
            nodeMemory += sizeof(OctreeNode);
            nodeMemory += node.triangleIndices.size() * sizeof(int);
        }
        size_t totalMemory = triangleMemory + nodeMemory;
        return static_cast<float>(totalMemory) / 1024.0;
    }

#endif
};
