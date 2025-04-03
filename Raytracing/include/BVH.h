#pragma once
#include <glad/glad.h> // holds all OpenGL type declarations
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include "Triangle.h"
#include <algorithm> 
#include <string>
#include <vector>
using namespace std;
using namespace glm;

#define INF 114514.0

struct BVHNode {
	int left, right; 
	int n, index; 
	vec3 AA, BB;
};

struct BVHNode_encoded {
	vec3 childs;        
	vec3 leafInfo;      
	vec3 AA, BB;
};

class BVH {

public:
	int n = 8;
	vector<Triangle>triangles;
	vector<BVHNode>nodes;

	BVH()
	{
	}
	BVH(vector<Triangle>triangles)
	{
		this->triangles = triangles;
	}


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



    int buildBVHwithSAH( int l, int r) {
		
        if (l > r) return 0;

        nodes.push_back(BVHNode());
        int id = nodes.size() - 1;
        nodes[id].left = nodes[id].right = nodes[id].n = nodes[id].index = 0;
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


        if ((r - l + 1) <= n) {
            nodes[id].n = r - l + 1;
            nodes[id].index = l;
            return id;
        }

        float Cost = INF;
        int Axis = 0;
        int Split = (l + r) / 2;
        for (int axis = 0; axis < 3; axis++) {

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

            std::vector<vec3> leftMax(r - l + 1, vec3(-INF, -INF, -INF));
            std::vector<vec3> leftMin(r - l + 1, vec3(INF, INF, INF));

            for (int i = l; i <= r; i++) {
                Triangle& t = triangles[i];
                int bias = (i == l) ? 0 : 1;  

                leftMax[i - l].x = std::max(leftMax[i - l - bias].x, std::max(t.vertex[0].Position.x, std::max(t.vertex[1].Position.x, t.vertex[2].Position.x)));
                leftMax[i - l].y = std::max(leftMax[i - l - bias].y, std::max(t.vertex[0].Position.y, std::max(t.vertex[1].Position.y, t.vertex[2].Position.y)));
                leftMax[i - l].z = std::max(leftMax[i - l - bias].z, std::max(t.vertex[0].Position.z, std::max(t.vertex[1].Position.z, t.vertex[2].Position.z)));

                leftMin[i - l].x = std::min(leftMax[i - l - bias].x, std::min(t.vertex[0].Position.x, std::min(t.vertex[1].Position.x, t.vertex[2].Position.x)));
                leftMin[i - l].y = std::min(leftMax[i - l - bias].y, std::min(t.vertex[0].Position.y, std::min(t.vertex[1].Position.y, t.vertex[2].Position.y)));
                leftMin[i - l].z = std::min(leftMax[i - l - bias].z, std::min(t.vertex[0].Position.z, std::min(t.vertex[1].Position.z, t.vertex[2].Position.z)));
            }

            std::vector<vec3> rightMax(r - l + 1, vec3(-INF, -INF, -INF));
            std::vector<vec3> rightMin(r - l + 1, vec3(INF, INF, INF));
            for (int i = r; i >= l; i--) {
                Triangle& t = triangles[i];
                int bias = (i == r) ? 0 : 1;  

                rightMax[i - l].x = std::max(leftMax[i - l + bias].x, std::max(t.vertex[0].Position.x, std::max(t.vertex[1].Position.x, t.vertex[2].Position.x)));
                rightMax[i - l].y = std::max(leftMax[i - l + bias].y, std::max(t.vertex[0].Position.y, std::max(t.vertex[1].Position.y, t.vertex[2].Position.y)));
                rightMax[i - l].z = std::max(leftMax[i - l + bias].z, std::max(t.vertex[0].Position.z, std::max(t.vertex[1].Position.z, t.vertex[2].Position.z)));

                rightMin[i - l].x = std::min(leftMax[i - l + bias].x, std::min(t.vertex[0].Position.x, std::min(t.vertex[1].Position.x, t.vertex[2].Position.x)));
                rightMin[i - l].y = std::min(leftMax[i - l + bias].y, std::min(t.vertex[0].Position.y, std::min(t.vertex[1].Position.y, t.vertex[2].Position.y)));
                rightMin[i - l].z = std::min(leftMax[i - l + bias].z, std::min(t.vertex[0].Position.z, std::min(t.vertex[1].Position.z, t.vertex[2].Position.z)));
            }


            float cost = INF;
            int split = l;
            for (int i = l; i <= r - 1; i++) {
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
                if (totalCost < cost) {
                    cost = totalCost;
                    split = i;
                }
            }

            if (cost < Cost) {
                Cost = cost;
                Axis = axis;
                Split = split;
            }
        }

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


        int left = buildBVHwithSAH( l, Split);
        int right = buildBVHwithSAH( Split + 1, r);

        nodes[id].left = left;
        nodes[id].right = right;
        return id;
		
    }
};


