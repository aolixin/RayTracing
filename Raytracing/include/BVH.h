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


struct BVHNode
{
	int left, right;
	int n, index;
	vec3 AA, BB;
};

struct BVHNode_encoded
{
	vec3 childs;
	vec3 leafInfo;
	vec3 AA, BB;
};

class BVH
{
public:
	int n = 8;
	vector<Triangle> triangles;
	vector<BVHNode> nodes;

	BVH()
	{
	}

	BVH(vector<Triangle> triangles)
	{
		this->triangles = triangles;
	}


	static bool cmpx(const Triangle& t1, const Triangle& t2)
	{
		vec3 c1 = vec3(0.0, 0.0, 0.0), c2 = vec3(0.0, 0.0, 0.0);
		for (int i = 0; i < 3; i++)
		{
			c1 += t1.vertex[i].Position;
			c2 += t2.vertex[i].Position;
		}
		return c1.x < c2.x;
	}

	static bool cmpy(const Triangle& t1, const Triangle& t2)
	{
		vec3 c1 = vec3(0.0, 0.0, 0.0), c2 = vec3(0.0, 0.0, 0.0);
		for (int i = 0; i < 3; i++)
		{
			c1 += t1.vertex[i].Position;
			c2 += t2.vertex[i].Position;
		}
		return c1.y < c2.y;
	}

	static bool cmpz(const Triangle& t1, const Triangle& t2)
	{
		vec3 c1 = vec3(0.0, 0.0, 0.0), c2 = vec3(0.0, 0.0, 0.0);
		for (int i = 0; i < 3; i++)
		{
			c1 += t1.vertex[i].Position;
			c2 += t2.vertex[i].Position;
		}
		return c1.z < c2.z;
	}


	int BuildBVH(int l, int r)
	{
		if (l > r)return 0;

		nodes.push_back(BVHNode());
		int id = (int)nodes.size() - 1;

		nodes[id].left = nodes[id].right = nodes[id].index = nodes[id].n = 0;
		nodes[id].AA = vec3(1145141919, 1145141919, 1145141919);
		nodes[id].BB = vec3(-1145141919, -1145141919, -1145141919);

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

		if (r - l + 1 <= n)
		{
			nodes[id].n = r - l + 1;
			nodes[id].index = l;
			return id;
		}
		float lenx = nodes[id].BB.x - nodes[id].AA.x;
		float leny = nodes[id].BB.y - nodes[id].AA.y;
		float lenz = nodes[id].BB.z - nodes[id].AA.z;


		if (lenx > leny && lenx > lenz)
		{
			std::sort(triangles.begin() + l, triangles.begin() + r + 1, cmpx);
		}
		if (leny > lenx && leny > lenz)
		{
			std::sort(triangles.begin() + l, triangles.begin() + r + 1, cmpy);
		}
		if (lenz > lenx && lenz > leny)
		{
			std::sort(triangles.begin() + l, triangles.begin() + r + 1, cmpz);
		}


		int mid = (r + l) / 2;
		int left = BuildBVH(l, mid);
		int right = BuildBVH(mid + 1, r);
		nodes[id].left = left;
		nodes[id].right = right;
		return id;
	}


	int BuildBVHWithSAH(int l, int r)
	{
		if (l > r) return 0;

		nodes.push_back(BVHNode());
		int id = nodes.size() - 1;
		nodes[id].left = nodes[id].right = nodes[id].n = nodes[id].index = 0;
		nodes[id].AA = vec3(1145141919, 1145141919, 1145141919);
		nodes[id].BB = vec3(-1145141919, -1145141919, -1145141919);


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


		if ((r - l + 1) <= n)
		{
			nodes[id].n = r - l + 1;
			nodes[id].index = l;
			return id;
		}

		float Cost = INF;
		int Axis = 0;
		int Split = (l + r) / 2;
		for (int axis = 0; axis < 3; axis++)
		{
			if (axis == 0)
				std::sort(&triangles[0] + l, &triangles[0] + r + 1, cmpx);
			if (axis == 1)
				std::sort(&triangles[0] + l, &triangles[0] + r + 1, cmpy);
			if (axis == 2)
				std::sort(&triangles[0] + l, &triangles[0] + r + 1, cmpz);

			std::vector<vec3> leftMax(r - l + 1, vec3(-INF, -INF, -INF));
			std::vector<vec3> leftMin(r - l + 1, vec3(INF, INF, INF));

			for (int i = l; i <= r; i++)
			{
				Triangle& t = triangles[i];
				int bias = (i == l) ? 0 : 1;

				leftMax[i - l].x = std::max(leftMax[i - l - bias].x,
					std::max(t.vertex[0].Position.x,
						std::max(t.vertex[1].Position.x, t.vertex[2].Position.x)));
				leftMax[i - l].y = std::max(leftMax[i - l - bias].y,
					std::max(t.vertex[0].Position.y,
						std::max(t.vertex[1].Position.y, t.vertex[2].Position.y)));
				leftMax[i - l].z = std::max(leftMax[i - l - bias].z,
					std::max(t.vertex[0].Position.z,
						std::max(t.vertex[1].Position.z, t.vertex[2].Position.z)));

				leftMin[i - l].x = std::min(leftMax[i - l - bias].x,
					std::min(t.vertex[0].Position.x,
						std::min(t.vertex[1].Position.x, t.vertex[2].Position.x)));
				leftMin[i - l].y = std::min(leftMax[i - l - bias].y,
					std::min(t.vertex[0].Position.y,
						std::min(t.vertex[1].Position.y, t.vertex[2].Position.y)));
				leftMin[i - l].z = std::min(leftMax[i - l - bias].z,
					std::min(t.vertex[0].Position.z,
						std::min(t.vertex[1].Position.z, t.vertex[2].Position.z)));
			}

			std::vector<vec3> rightMax(r - l + 1, vec3(-INF, -INF, -INF));
			std::vector<vec3> rightMin(r - l + 1, vec3(INF, INF, INF));
			for (int i = r; i >= l; i--)
			{
				Triangle& t = triangles[i];
				int bias = (i == r) ? 0 : 1;

				rightMax[i - l].x = std::max(leftMax[i - l + bias].x,
					std::max(t.vertex[0].Position.x,
						std::max(t.vertex[1].Position.x, t.vertex[2].Position.x)));
				rightMax[i - l].y = std::max(leftMax[i - l + bias].y,
					std::max(t.vertex[0].Position.y,
						std::max(t.vertex[1].Position.y, t.vertex[2].Position.y)));
				rightMax[i - l].z = std::max(leftMax[i - l + bias].z,
					std::max(t.vertex[0].Position.z,
						std::max(t.vertex[1].Position.z, t.vertex[2].Position.z)));

				rightMin[i - l].x = std::min(leftMax[i - l + bias].x,
					std::min(t.vertex[0].Position.x,
						std::min(t.vertex[1].Position.x, t.vertex[2].Position.x)));
				rightMin[i - l].y = std::min(leftMax[i - l + bias].y,
					std::min(t.vertex[0].Position.y,
						std::min(t.vertex[1].Position.y, t.vertex[2].Position.y)));
				rightMin[i - l].z = std::min(leftMax[i - l + bias].z,
					std::min(t.vertex[0].Position.z,
						std::min(t.vertex[1].Position.z, t.vertex[2].Position.z)));
			}


			float cost = INF;
			int split = l;
			for (int i = l; i <= r - 1; i++)
			{
				float lenx, leny, lenz;

				vec3 leftAA = leftMin[i - l];
				vec3 leftBB = leftMax[i - l];
				lenx = leftBB.x - leftAA.x;
				leny = leftBB.y - leftAA.y;
				lenz = leftBB.z - leftAA.z;
				float leftS = 2.0 * ((lenx * leny) + (lenx * lenz) + (leny * lenz));
				float leftCost = leftS * (i - l + 1);


				vec3 rightAA = rightMin[i + 1 - l];
				vec3 rightBB = rightMax[i + 1 - l];
				lenx = rightBB.x - rightAA.x;
				leny = rightBB.y - rightAA.y;
				lenz = rightBB.z - rightAA.z;
				float rightS = 2.0 * ((lenx * leny) + (lenx * lenz) + (leny * lenz));
				float rightCost = rightS * (r - i);


				float totalCost = leftCost + rightCost;
				if (totalCost < cost)
				{
					cost = totalCost;
					split = i;
				}
			}

			if (cost < Cost)
			{
				Cost = cost;
				Axis = axis;
				Split = split;
			}
		}

		if (Axis == 0)
			std::sort(&triangles[0] + l, &triangles[0] + r + 1, cmpx);
		if (Axis == 1)
			std::sort(&triangles[0] + l, &triangles[0] + r + 1, cmpy);
		if (Axis == 2)
			std::sort(&triangles[0] + l, &triangles[0] + r + 1, cmpz);


		int left = BuildBVHWithSAH(l, Split);
		int right = BuildBVHWithSAH(Split + 1, r);

		nodes[id].left = left;
		nodes[id].right = right;
		return id;
	}


	GLuint DebugVAO, DebugVBO, DebugEBO;
	std::vector<GLuint> DebugIndices;

	// Debug
	void BuildDebugBVHTree(int startDepth, int endDepth)
	{
		std::vector<GLfloat> vertices;
		// std::vector<GLuint> indices;
		// 递归函数来遍历 BVH 树
		std::function<void(int, int)> traverse = [&](int nodeId, int currentDepth)
			{
				if (nodeId >= nodes.size() || currentDepth > endDepth) return;
				if (currentDepth < startDepth)
				{
					traverse(nodes[nodeId].left, currentDepth + 1);
					traverse(nodes[nodeId].right, currentDepth + 1);
					return;
				}
				const BVHNode& node = nodes[nodeId];
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

				// 立方体的三角形索引
				GLuint baseIndex = (GLuint)(vertices.size() / 3 - 8);
				DebugIndices.insert(DebugIndices.end(), {
										baseIndex + 0, baseIndex + 1, baseIndex + 2,
										baseIndex + 2, baseIndex + 3, baseIndex + 0,
										baseIndex + 4, baseIndex + 6, baseIndex + 5,
										baseIndex + 4, baseIndex + 7, baseIndex + 6,
										baseIndex + 0, baseIndex + 7, baseIndex + 4,
										baseIndex + 7, baseIndex + 3, baseIndex + 0,
										baseIndex + 1, baseIndex + 5, baseIndex + 6,
										baseIndex + 6, baseIndex + 2, baseIndex + 1,
										baseIndex + 3, baseIndex + 6, baseIndex + 7,
										baseIndex + 2, baseIndex + 6, baseIndex + 3,
										baseIndex + 0, baseIndex + 4, baseIndex + 5,
										baseIndex + 5, baseIndex + 1, baseIndex + 0
					});

				traverse(node.left, currentDepth + 1);
				traverse(node.right, currentDepth + 1);
			};

		// 从根节点开始遍历
		traverse(0, 1);

		glGenVertexArrays(1, &DebugVAO);
		glGenBuffers(1, &DebugVBO);
		glGenBuffers(1, &DebugEBO);

		glBindVertexArray(DebugVAO);

		glBindBuffer(GL_ARRAY_BUFFER, DebugVBO);
		glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(GLfloat), vertices.data(), GL_STATIC_DRAW);

		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, DebugEBO);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, DebugIndices.size() * sizeof(GLuint), DebugIndices.data(),
			GL_STATIC_DRAW);

		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), (GLvoid*)0);
		glEnableVertexAttribArray(0);

		glBindBuffer(GL_ARRAY_BUFFER, 0);
		glBindVertexArray(0);
	}

	void BuildDebugBVHTree_l(int startDepth, int endDepth)
	{
		std::vector<GLfloat> vertices;
		// std::vector<GLuint> indices;
		DebugIndices.clear();
		// 递归函数来遍历 BVH 树
		std::function<void(int, int)> traverse = [&](int nodeId, int currentDepth)
			{
				if (nodeId >= nodes.size() || currentDepth > endDepth) return;
				if (currentDepth < startDepth)
				{
					traverse(nodes[nodeId].left, currentDepth + 1);
					traverse(nodes[nodeId].right, currentDepth + 1);
					return;
				}
				const BVHNode& node = nodes[nodeId];
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

				traverse(node.left, currentDepth + 1);
				traverse(node.right, currentDepth + 1);
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

	// 计算 BVH 树的内存占用，单位为 KB
	float GetMemoryUsageInKB() const {
		size_t triangleMemory = triangles.size() * sizeof(Triangle);
		size_t nodeMemory = nodes.size() * sizeof(BVHNode);
		size_t totalMemory = triangleMemory + nodeMemory;
		return static_cast<float>(totalMemory) / 1024.0;
	}

	float GetNodeMemoryUsageInKB() const {
		size_t nodeMemory = nodes.size() * sizeof(BVHNode);
		return static_cast<float>(nodeMemory) / 1024.0;
	}

	float GetTriangleMemoryUsageInKB() const {
		size_t triangleMemory = triangles.size() * sizeof(Triangle);
		return static_cast<float>(triangleMemory) / 1024.0;
	}

};
