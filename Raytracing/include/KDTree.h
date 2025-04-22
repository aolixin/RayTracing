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

using namespace std;
using namespace glm;

#define INF 114514.0

// Kd树节点结构体
struct KdTreeNode
{
    int left, right;
    int n, index;
    vec3 AA, BB;
    int splitAxis;

    KdTreeNode() : left(0), right(0), n(0), index(0), splitAxis(0)
    {
    }
};

// Kd树类
class KDTree
{
public:
    int maxTrianglesPerNode = 16;
    vector<Triangle> triangles;
    vector<KdTreeNode> nodes;

    KDTree()
    {
    }

    KDTree(vector<Triangle> triangles)
    {
        this->triangles = triangles;
        buildKdTree(0, triangles.size() - 1, 0);
    }

private:
    // 比较函数，根据不同轴比较三角形的质心
    static bool cmp(const Triangle& t1, const Triangle& t2, int axis)
    {
        vec3 c1 = vec3(0.0, 0.0, 0.0), c2 = vec3(0.0, 0.0, 0.0);
        for (int i = 0; i < 3; i++)
        {
            c1 += t1.vertex[i].Position;
            c2 += t2.vertex[i].Position;
        }
        return c1[axis] < c2[axis];
    }

    // 构建Kd树
    int buildKdTree(int l, int r, int depth)
    {
        if (l > r) return 0;

        nodes.push_back(KdTreeNode());
        int id = (int)nodes.size() - 1;

        nodes[id].left = nodes[id].right = nodes[id].n = nodes[id].index = 0;
        nodes[id].AA = vec3(INF, INF, INF);
        nodes[id].BB = vec3(-INF, -INF, -INF);

        // 计算包围盒
        for (int i = l; i <= r; i++)
        {
            float minx = std::min(std::min(triangles[i].vertex[0].Position.x, triangles[i].vertex[1].Position.x),
                                  triangles[i].vertex[2].Position.x);
            float miny = std::min(std::min(triangles[i].vertex[0].Position.y, triangles[i].vertex[1].Position.y),
                                  triangles[i].vertex[2].Position.y);
            float minz = std::min(std::min(triangles[i].vertex[0].Position.z, triangles[i].vertex[1].Position.z),
                                  triangles[i].vertex[2].Position.z);
            nodes[id].AA.x = std::min(nodes[id].AA.x, minx);
            nodes[id].AA.y = std::min(nodes[id].AA.y, miny);
            nodes[id].AA.z = std::min(nodes[id].AA.z, minz);

            float maxx = std::max(std::max(triangles[i].vertex[0].Position.x, triangles[i].vertex[1].Position.x),
                                  triangles[i].vertex[2].Position.x);
            float maxy = std::max(std::max(triangles[i].vertex[0].Position.y, triangles[i].vertex[1].Position.y),
                                  triangles[i].vertex[2].Position.y);
            float maxz = std::max(std::max(triangles[i].vertex[0].Position.z, triangles[i].vertex[1].Position.z),
                                  triangles[i].vertex[2].Position.z);
            nodes[id].BB.x = std::max(nodes[id].BB.x, maxx);
            nodes[id].BB.y = std::max(nodes[id].BB.y, maxy);
            nodes[id].BB.z = std::max(nodes[id].BB.z, maxz);
        }

        if (r - l + 1 <= maxTrianglesPerNode)
        {
            nodes[id].n = r - l + 1;
            nodes[id].index = l;
            return id;
        }

        int axis = depth % 3;
        nodes[id].splitAxis = axis;
        std::sort(triangles.begin() + l, triangles.begin() + r + 1, [axis](const Triangle& t1, const Triangle& t2)
        {
            return cmp(t1, t2, axis);
        });

        int mid = (l + r) / 2;
        int left = buildKdTree(l, mid, depth + 1);
        int right = buildKdTree(mid + 1, r, depth + 1);
        nodes[id].left = left;
        nodes[id].right = right;
        return id;
    }

public:
#ifdef DEBUG_KDTREE
    GLuint DebugVAO, DebugVBO, DebugEBO;
    std::vector<GLuint> DebugIndices;

    // 调试函数，用于绘制Kd树的包围盒
    void BuildDebugKdTree_l(int startDepth, int endDepth)
    {
        std::vector<GLfloat> vertices;
        // 递归函数来遍历Kd树
        std::function<void(int, int)> traverse = [&](int nodeId, int currentDepth)
        {
            if (nodeId >= nodes.size() || currentDepth > endDepth) return;
            if (currentDepth < startDepth)
            {
                traverse(nodes[nodeId].left, currentDepth + 1);
                traverse(nodes[nodeId].right, currentDepth + 1);
                return;
            }
            const KdTreeNode& node = nodes[nodeId];
            vec3 AA = node.AA;
            vec3 BB = node.BB;

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

            traverse(nodes[nodeId].left, currentDepth + 1);
            traverse(nodes[nodeId].right, currentDepth + 1);
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


    // 计算Kd树的内存占用，单位为KB
    float getMemoryUsageInKB() const
    {
        size_t triangleMemory = triangles.size() * sizeof(Triangle);
        size_t nodeMemory = nodes.size() * sizeof(KdTreeNode);
        size_t totalMemory = triangleMemory + nodeMemory;
        return static_cast<float>(totalMemory) / 1024.0;
    }
#endif
};
