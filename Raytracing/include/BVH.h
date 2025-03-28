#pragma once
#include <glad/glad.h> // holds all OpenGL type declarations
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include "Triangle.h"
#include <algorithm> // 包含头文件以使用sort函数
#include <string>
#include <vector>
using namespace std;
using namespace glm;

#define INF 114514.0

struct BVHNode {

	int left, right; //左右子树索引
	int n, index; // 一个node包含的三角形个数, node的索引
	vec3 AA, BB;
};

struct BVHNode_encoded {
	vec3 childs;        // (left, right, 保留)
	vec3 leafInfo;      // (n, index, 保留)
	vec3 AA, BB;
};

class BVH {

public:
	int n = 8;//
	vector<Triangle>triangles;
	vector<BVHNode>nodes;

	BVH()
	{
	}
	BVH(vector<Triangle>triangles)
	{
		this->triangles = triangles;
	}

	// 按照三角形中心排序 -- 比较函数
	bool cmpx(const Triangle& t1, const Triangle& t2) {
		vec3 c1, c2;
		for (int i = 0; i < 3; i++)
		{
			c1 += t1.vertex[i].Position;
			c2 += t1.vertex[i].Position;
		}
		return c1.x < c2.x;
	}
	bool cmpy(const Triangle& t1, const Triangle& t2) {
		vec3 c1, c2;
		for (int i = 0; i < 3; i++)
		{
			c1 += t1.vertex[i].Position;
			c2 += t1.vertex[i].Position;
		}
		return c1.y < c2.y;
	}
	bool cmpz(const Triangle& t1, const Triangle& t2) {
		vec3 c1, c2;
		for (int i = 0; i < 3; i++)
		{
			c1 += t1.vertex[i].Position;
			c2 += t1.vertex[i].Position;
		}
		return c1.z < c2.z;
	}


	int buildBVH(int l, int r) {
		if (l > r)return 0;

		nodes.push_back(BVHNode());
		int id = (int)nodes.size() - 1;

		nodes[id].left = nodes[id].right = nodes[id].index = nodes[id].n = 0;
		nodes[id].AA = vec3(1145141919, 1145141919, 1145141919);
		nodes[id].BB = vec3(-1145141919, -1145141919, -1145141919);

		for (int i = l; i <= r; i++)
		{
			float minx = std::min(std::min(triangles[i].vertex[0].Position.x, triangles[i].vertex[1].Position.x), triangles[i].vertex[2].Position.x);
			float miny = std::min(std::min(triangles[i].vertex[0].Position.y, triangles[i].vertex[1].Position.y), triangles[i].vertex[2].Position.y);
			float minz = std::min(std::min(triangles[i].vertex[0].Position.z, triangles[i].vertex[1].Position.z), triangles[i].vertex[2].Position.z);
			nodes[id].AA.x = std::min(nodes[id].AA.x, minx);
			nodes[id].AA.y = std::min(nodes[id].AA.y, miny);
			nodes[id].AA.z = std::min(nodes[id].AA.z, minz);

			float maxx = std::max(std::max(triangles[i].vertex[0].Position.x, triangles[i].vertex[1].Position.x), triangles[i].vertex[2].Position.x);
			float maxy = std::max(std::max(triangles[i].vertex[0].Position.y, triangles[i].vertex[1].Position.y), triangles[i].vertex[2].Position.y);
			float maxz = std::max(std::max(triangles[i].vertex[0].Position.z, triangles[i].vertex[1].Position.z), triangles[i].vertex[2].Position.z);
			nodes[id].BB.x = std::max(nodes[id].BB.x, maxx);
			nodes[id].BB.y = std::max(nodes[id].BB.y, maxy);
			nodes[id].BB.z = std::max(nodes[id].BB.z, maxz);
		}

		if (r - l + 1 <= n)
		{
			nodes[id].n = r - l + 1;
			nodes[id].index = l;
			return id;
		}
		float lenx = nodes[id].BB.x - nodes[id].AA.x;
		float leny = nodes[id].BB.y - nodes[id].AA.y;
		float lenz = nodes[id].BB.z - nodes[id].AA.z;

		//x最大,按x划分
		if (lenx > leny && lenx > lenz)
		{
			std::sort(triangles.begin() + l, triangles.begin() + r + 1, [](const Triangle& t1, const Triangle& t2) {
				vec3 c1, c2;
				for (int i = 0; i < 3; i++)
				{
					c1 += t1.vertex[i].Position;
					c2 += t1.vertex[i].Position;
				}
				return c1.x < c2.x;
				});
		}
		if (leny > lenx && leny > lenz)
		{
			std::sort(triangles.begin() + l, triangles.begin() + r + 1, [](const Triangle& t1, const Triangle& t2) {
				vec3 c1, c2;
				for (int i = 0; i < 3; i++)
				{
					c1 += t1.vertex[i].Position;
					c2 += t1.vertex[i].Position;
				}
				return c1.y < c2.y;
				});
		}
		if (lenz > lenx && lenz > leny)
		{
			std::sort(triangles.begin() + l, triangles.begin() + r + 1, [](const Triangle& t1, const Triangle& t2) {
				vec3 c1, c2;
				for (int i = 0; i < 3; i++)
				{
					c1 += t1.vertex[i].Position;
					c2 += t1.vertex[i].Position;
				}
				return c1.z < c2.z;
				});
		}


		int mid = (r + l) / 2;
		int left = buildBVH(l, mid);
		int right = buildBVH(mid+1, r);
		nodes[id].left = left;
		nodes[id].right = right;
		return id;
	}


    // SAH 优化构建 BVH
    int buildBVHwithSAH( int l, int r) {
		
        if (l > r) return 0;

        nodes.push_back(BVHNode());
        int id = nodes.size() - 1;
        nodes[id].left = nodes[id].right = nodes[id].n = nodes[id].index = 0;
        nodes[id].AA = vec3(1145141919, 1145141919, 1145141919);
        nodes[id].BB = vec3(-1145141919, -1145141919, -1145141919);

        // 计算 AABB
		for (int i = l; i <= r; i++)
		{
			float minx = std::min(std::min(triangles[i].vertex[0].Position.x, triangles[i].vertex[1].Position.x), triangles[i].vertex[2].Position.x);
			float miny = std::min(std::min(triangles[i].vertex[0].Position.y, triangles[i].vertex[1].Position.y), triangles[i].vertex[2].Position.y);
			float minz = std::min(std::min(triangles[i].vertex[0].Position.z, triangles[i].vertex[1].Position.z), triangles[i].vertex[2].Position.z);
			nodes[id].AA.x = std::min(nodes[id].AA.x, minx);
			nodes[id].AA.y = std::min(nodes[id].AA.y, miny);
			nodes[id].AA.z = std::min(nodes[id].AA.z, minz);

			float maxx = std::max(std::max(triangles[i].vertex[0].Position.x, triangles[i].vertex[1].Position.x), triangles[i].vertex[2].Position.x);
			float maxy = std::max(std::max(triangles[i].vertex[0].Position.y, triangles[i].vertex[1].Position.y), triangles[i].vertex[2].Position.y);
			float maxz = std::max(std::max(triangles[i].vertex[0].Position.z, triangles[i].vertex[1].Position.z), triangles[i].vertex[2].Position.z);
			nodes[id].BB.x = std::max(nodes[id].BB.x, maxx);
			nodes[id].BB.y = std::max(nodes[id].BB.y, maxy);
			nodes[id].BB.z = std::max(nodes[id].BB.z, maxz);
		}

        // 不多于 n 个三角形 返回叶子节点
        if ((r - l + 1) <= n) {
            nodes[id].n = r - l + 1;
            nodes[id].index = l;
            return id;
        }
        // 否则递归建树
        float Cost = INF;
        int Axis = 0;
        int Split = (l + r) / 2;
        for (int axis = 0; axis < 3; axis++) {
            // 分别按 x，y，z 轴排序
            if (axis == 0) std::sort(&triangles[0] + l, &triangles[0] + r + 1, [](const Triangle& t1, const Triangle& t2) {
				vec3 c1, c2;
				for (int i = 0; i < 3; i++)
				{
					c1 += t1.vertex[i].Position;
					c2 += t1.vertex[i].Position;
				}
				return c1.x < c2.x;
				});
            if (axis == 1) std::sort(&triangles[0] + l, &triangles[0] + r + 1, [](const Triangle& t1, const Triangle& t2) {
				vec3 c1, c2;
				for (int i = 0; i < 3; i++)
				{
					c1 += t1.vertex[i].Position;
					c2 += t1.vertex[i].Position;
				}
				return c1.y < c2.y;
				});
            if (axis == 2) std::sort(&triangles[0] + l, &triangles[0] + r + 1, [](const Triangle& t1, const Triangle& t2) {
				vec3 c1, c2;
				for (int i = 0; i < 3; i++)
				{
					c1 += t1.vertex[i].Position;
					c2 += t1.vertex[i].Position;
				}
				return c1.z < c2.z;
				});
            // leftMax[i]: [l, i] 中最大的 xyz 值
            // leftMin[i]: [l, i] 中最小的 xyz 值
            std::vector<vec3> leftMax(r - l + 1, vec3(-INF, -INF, -INF));
            std::vector<vec3> leftMin(r - l + 1, vec3(INF, INF, INF));
            // 计算前缀 注意 i-l 以对齐到下标 0
            for (int i = l; i <= r; i++) {
                Triangle& t = triangles[i];
                int bias = (i == l) ? 0 : 1;  // 第一个元素特殊处理

                leftMax[i - l].x = std::max(leftMax[i - l - bias].x, std::max(t.vertex[0].Position.x, std::max(t.vertex[1].Position.x, t.vertex[2].Position.x)));
                leftMax[i - l].y = std::max(leftMax[i - l - bias].y, std::max(t.vertex[0].Position.y, std::max(t.vertex[1].Position.y, t.vertex[2].Position.y)));
                leftMax[i - l].z = std::max(leftMax[i - l - bias].z, std::max(t.vertex[0].Position.z, std::max(t.vertex[1].Position.z, t.vertex[2].Position.z)));

                leftMin[i - l].x = std::min(leftMax[i - l - bias].x, std::min(t.vertex[0].Position.x, std::min(t.vertex[1].Position.x, t.vertex[2].Position.x)));
                leftMin[i - l].y = std::min(leftMax[i - l - bias].y, std::min(t.vertex[0].Position.y, std::min(t.vertex[1].Position.y, t.vertex[2].Position.y)));
                leftMin[i - l].z = std::min(leftMax[i - l - bias].z, std::min(t.vertex[0].Position.z, std::min(t.vertex[1].Position.z, t.vertex[2].Position.z)));
            }
            // rightMax[i]: [i, r] 中最大的 xyz 值
            // rightMin[i]: [i, r] 中最小的 xyz 值
            std::vector<vec3> rightMax(r - l + 1, vec3(-INF, -INF, -INF));
            std::vector<vec3> rightMin(r - l + 1, vec3(INF, INF, INF));
            // 计算后缀 注意 i-l 以对齐到下标 0
            for (int i = r; i >= l; i--) {
                Triangle& t = triangles[i];
                int bias = (i == r) ? 0 : 1;  // 第一个元素特殊处理

                rightMax[i - l].x = std::max(leftMax[i - l + bias].x, std::max(t.vertex[0].Position.x, std::max(t.vertex[1].Position.x, t.vertex[2].Position.x)));
                rightMax[i - l].y = std::max(leftMax[i - l + bias].y, std::max(t.vertex[0].Position.y, std::max(t.vertex[1].Position.y, t.vertex[2].Position.y)));
                rightMax[i - l].z = std::max(leftMax[i - l + bias].z, std::max(t.vertex[0].Position.z, std::max(t.vertex[1].Position.z, t.vertex[2].Position.z)));

                rightMin[i - l].x = std::min(leftMax[i - l + bias].x, std::min(t.vertex[0].Position.x, std::min(t.vertex[1].Position.x, t.vertex[2].Position.x)));
                rightMin[i - l].y = std::min(leftMax[i - l + bias].y, std::min(t.vertex[0].Position.y, std::min(t.vertex[1].Position.y, t.vertex[2].Position.y)));
                rightMin[i - l].z = std::min(leftMax[i - l + bias].z, std::min(t.vertex[0].Position.z, std::min(t.vertex[1].Position.z, t.vertex[2].Position.z)));
            }

            // 遍历寻找分割
            float cost = INF;
            int split = l;
            for (int i = l; i <= r - 1; i++) {
                float lenx, leny, lenz;
                // 左侧 [l, i]
                vec3 leftAA = leftMin[i - l];
                vec3 leftBB = leftMax[i - l];
                lenx = leftBB.x - leftAA.x;
                leny = leftBB.y - leftAA.y;
                lenz = leftBB.z - leftAA.z;
                float leftS = 2.0 * ((lenx * leny) + (lenx * lenz) + (leny * lenz));
                float leftCost = leftS * (i - l + 1);

                // 右侧 [i+1, r]
                vec3 rightAA = rightMin[i + 1 - l];
                vec3 rightBB = rightMax[i + 1 - l];
                lenx = rightBB.x - rightAA.x;
                leny = rightBB.y - rightAA.y;
                lenz = rightBB.z - rightAA.z;
                float rightS = 2.0 * ((lenx * leny) + (lenx * lenz) + (leny * lenz));
                float rightCost = rightS * (r - i);

                // 记录每个分割的最小答案
                float totalCost = leftCost + rightCost;
                if (totalCost < cost) {
                    cost = totalCost;
                    split = i;
                }
            }
            // 记录每个轴的最佳答案
            if (cost < Cost) {
                Cost = cost;
                Axis = axis;
                Split = split;
            }
        }
        // 按最佳轴分割
        if (Axis == 0) std::sort(&triangles[0] + l, &triangles[0] + r + 1, [](const Triangle& t1, const Triangle& t2) {
			vec3 c1, c2;
			for (int i = 0; i < 3; i++)
			{
				c1 += t1.vertex[i].Position;
				c2 += t1.vertex[i].Position;
			}
			return c1.x < c2.x;
			});
        if (Axis == 1) std::sort(&triangles[0] + l, &triangles[0] + r + 1, [](const Triangle& t1, const Triangle& t2) {
			vec3 c1, c2;
			for (int i = 0; i < 3; i++)
			{
				c1 += t1.vertex[i].Position;
				c2 += t1.vertex[i].Position;
			}
			return c1.y < c2.y;
			});
        if (Axis == 2) std::sort(&triangles[0] + l, &triangles[0] + r + 1, [](const Triangle& t1, const Triangle& t2) {
			vec3 c1, c2;
			for (int i = 0; i < 3; i++)
			{
				c1 += t1.vertex[i].Position;
				c2 += t1.vertex[i].Position;
			}
			return c1.z < c2.z;
			});

        // 递归
        int left = buildBVHwithSAH( l, Split);
        int right = buildBVHwithSAH( Split + 1, r);

        nodes[id].left = left;
        nodes[id].right = right;
        return id;
		
    }
};


