#pragma once


#include "Renderer.h"

#include <chrono>
#include <stack>


bool Renderer::already_init = false;
std::shared_ptr<Renderer> Renderer::renderer = nullptr;
GLFWwindow* Renderer::window = nullptr;

std::shared_ptr<Renderer> Renderer::GetRenderer(RenderPath path, GLFWwindow* window)
{
	if (renderer == nullptr)
	{
		renderer = std::make_shared<Renderer>();
		//renderer->renderPath = path;
		renderer->window = window;
		renderer->InitRenderer();
	}
	return renderer;
}

void Renderer::InitRenderer()
{
	if (already_init)return;

	if (renderPath != RenderPath::RT)
	{
		RegisterCallback();
	}

	// init shader
	RTShader = Shader("Resources/shaders/screen.vert", "Resources/shaders/RT.frag");
	screenShader = Shader("Resources/shaders/screen.vert", "Resources/shaders/framebuffersToscreen.frag");
	postShader = Shader("Resources/shaders/screen.vert", "Resources/shaders/post.frag");
	skyboxShader = Shader("Resources/shaders/skybox.vert", "Resources/shaders/skybox.frag");
	unlitShader = Shader("Resources/shaders/phong.vert", "Resources/shaders/unlit.frag");


	debug_ia_shader = Shader("Resources/shaders/screen.vert", "Resources/shaders/debug_ia.frag");
	testShader = Shader("Resources/shaders/screen.vert", "Resources/shaders/unlit.frag");

	frameBuffer0 = GetFrameBuffer(RENDER_WIDTH, RENDER_HEIGHT, frameTextures0, 1, 0);

	frameBuffer1 = GetFrameBuffer(RENDER_WIDTH, RENDER_HEIGHT, frameTextures1, 1, 0);
}

void Renderer::ResetRender(/*std::shared_ptr<Camera> newCamera, std::shared_ptr<Scene> newScene*/) {
	lastX = RENDER_WIDTH / 2.0f;
	lastY = RENDER_HEIGHT / 2.0f;

	firstMouse = true;
	frameCount = 0;
	debugDepth = 0;
}

void Renderer::DestroyRenderer()
{
}

void Renderer::SetupScene(std::shared_ptr<Scene> scene)
{
	this->scene = scene;
	{
		this->scene->SetupGIScene();
	}
}

void Renderer::Draw(GLuint targetFrameBuffer)
{
	glEnable(GL_DEPTH_TEST);
	RenderContext context;
	context.projection = Perspective();
	context.view = View();
	context.viewPos = camera->Position;

	if (renderPath == RenderPath::Forward)
	{
		glEnable(GL_DEPTH_TEST);
		glBindFramebuffer(GL_FRAMEBUFFER, targetFrameBuffer);
		glViewport(0, 0, RENDER_WIDTH, RENDER_HEIGHT);
		// glEnable(GL_CULL_FACE);
		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

		glClearColor(0.05f, 0.05f, 0.05f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		for (auto& render_node : scene->render_nodes)
		{
			render_node.Draw(context);
		}
		DrawSkybox();
	}
	else if (renderPath == RenderPath::RT)
	{
		// pass1
		glViewport(0, 0, RENDER_WIDTH, RENDER_HEIGHT);

		glBindFramebuffer(GL_FRAMEBUFFER, frameBuffer0);
		// glEnable(GL_DEPTH_TEST);
		glDisable(GL_DEPTH_TEST);
		glClearColor(0.05f, 0.05f, 0.05f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		RTShader.use();
		RTShader.setInt("frameCounter", frameCount++);
		RTShader.setInt("width", RENDER_WIDTH);
		RTShader.setInt("height", RENDER_HEIGHT);
		RTShader.setVec3("cameraPos", camera->Position);
		RTShader.setMat4("cameraRotate", inverse(CameraRotate()));

		RTShader.setTextureBuffer("triangles", scene->trianglesTextureBuffer, 2);
		RTShader.setInt("nTriangles", scene->nTriangles);

		RTShader.setTextureBuffer("nodes", scene->nodesTextureBuffer, 3);
		RTShader.setInt("nNodes", scene->nNodes);

		RTShader.setTextureBuffer("materials", scene->materialsTextureBuffer, 4);
		RTShader.setInt("nMaterials", scene->nMaterials);

		RTShader.setTexture("lastFrame", frameTextures1[0], 5);

		RTShader.setTexture("hdrMap", scene->hdrMap, 6);
		RTShader.setTexture("hdrCache", scene->hdrCache, 7);
		RTShader.setInt("hdrResolution", scene->hdrWidth);

		DrawQuad(RTShader);


		// pass2
		glBindFramebuffer(GL_FRAMEBUFFER, frameBuffer1);
		glDisable(GL_DEPTH_TEST);
		glClearColor(0.05f, 0.05f, 0.05f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT);
		screenShader.use();
		screenShader.setTexture("screenTexture", frameTextures0[0], 6);
		DrawQuad(screenShader);


		// pass3
		glBindFramebuffer(GL_FRAMEBUFFER, targetFrameBuffer);
		glDisable(GL_DEPTH_TEST);
		glClearColor(0.05f, 0.05f, 0.05f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT);

		postShader.use();
		postShader.setTexture("screenTexture", frameTextures1[0], 6);
		DrawQuad(postShader);
	}
	else if (renderPath == RenderPath::DebugIA)
	{
		glBindFramebuffer(GL_FRAMEBUFFER, targetFrameBuffer);
		glDisable(GL_DEPTH_TEST);
		glClearColor(0.05f, 0.05f, 0.05f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT);

		debug_ia_shader.use();

		debug_ia_shader.setInt("frameCounter", frameCount++);
		debug_ia_shader.setInt("width", RENDER_WIDTH);
		debug_ia_shader.setInt("height", RENDER_HEIGHT);

		debug_ia_shader.setTexture("hdrMap", scene->hdrMap, 2);
		debug_ia_shader.setTexture("hdrCache", scene->hdrCache, 3);
		debug_ia_shader.setInt("hdrResolution", scene->hdrWidth);

		DrawQuad(postShader);
	}
	else if (renderPath == RenderPath::DebugBVH)
	{
		glEnable(GL_DEPTH_TEST);
		glBindFramebuffer(GL_FRAMEBUFFER, targetFrameBuffer);
		glClearColor(0.05f, 0.05f, 0.05f, 1.0f);
		glViewport(0, 0, RENDER_WIDTH, RENDER_HEIGHT);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
		
		// draw meshs

		for (auto& render_node : scene->render_nodes)
		{
			render_node.Draw(context);
		}

		// draw bvh
		unlitShader.use();
		unlitShader.setMat4("projection", context.projection);
		unlitShader.setMat4("view", context.view);
		unlitShader.setMat4("model", glm::mat4(1.0f));
		unlitShader.setVec3("objectColor", vec3(1.0f, 0.0f, 0.0f));

		glLineWidth(2.0f);
		glColor3f(1.0f, 0.0f, 0.0f);
		glBindVertexArray(scene->myBVH.DebugVAO);

		frameCount++;
		if (frameCount / 400 > 0)
		{
			frameCount = 0;
			debugDepth += 1;
			scene->myBVH.BuildDebugBVHTree_l(debugDepth, debugDepth);
		}

		glDrawElements(GL_LINES, scene->myBVH.DebugIndices.size(), GL_UNSIGNED_INT, 0);
		glBindVertexArray(0);


		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

	}
	else if (renderPath == RenderPath::DebugOctree)
	{

		glBindFramebuffer(GL_FRAMEBUFFER, targetFrameBuffer);
		glViewport(0, 0, RENDER_WIDTH, RENDER_HEIGHT);
		// glEnable(GL_CULL_FACE);

		glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

		// draw meshs
		glClearColor(0.05f, 0.05f, 0.05f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		for (auto& render_node : scene->render_nodes)
		{
			render_node.Draw(context);
		}

		// draw bvh
		unlitShader.use();
		unlitShader.setMat4("projection", context.projection);
		unlitShader.setMat4("view", context.view);
		unlitShader.setMat4("model", glm::mat4(1.0f));
		unlitShader.setVec3("objectColor", vec3(1.0f, 0.0f, 0.0f));

		// glBindVertexArray(scene->myBVH.DebugVAO);
		// glDrawElements(GL_TRIANGLES, scene->myBVH.DebugIndices.size(), GL_UNSIGNED_INT, 0);
		// glBindVertexArray(0);

		glLineWidth(2.0f);
		glColor3f(1.0f, 0.0f, 0.0f);
		glBindVertexArray(scene->myOctree.DebugVAO);

		frameCount++;
		if (frameCount / 400 > 0)
		{
			frameCount = 0;
			debugDepth += 1;
			scene->myOctree.BuildDebugOctree_l(debugDepth, debugDepth);
		}

		glDrawElements(GL_LINES, scene->myOctree.DebugIndices.size(), GL_UNSIGNED_INT, 0);
		glBindVertexArray(0);

		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

	}
	else if (renderPath == RenderPath::DebugKdTree)
	{
		glBindFramebuffer(GL_FRAMEBUFFER, targetFrameBuffer);
		glViewport(0, 0, RENDER_WIDTH, RENDER_HEIGHT);
		// glEnable(GL_CULL_FACE);

		glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

		// draw meshs
		glClearColor(0.05f, 0.05f, 0.05f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		for (auto& render_node : scene->render_nodes)
		{
			render_node.Draw(context);
		}

		// draw kd
		unlitShader.use();
		unlitShader.setMat4("projection", context.projection);
		unlitShader.setMat4("view", context.view);
		unlitShader.setMat4("model", glm::mat4(1.0f));
		unlitShader.setVec3("objectColor", vec3(1.0f, 0.0f, 0.0f));

		// glBindVertexArray(scene->myBVH.DebugVAO);
		// glDrawElements(GL_TRIANGLES, scene->myBVH.DebugIndices.size(), GL_UNSIGNED_INT, 0);
		// glBindVertexArray(0);

		glLineWidth(2.0f);
		glColor3f(1.0f, 0.0f, 0.0f);
		glBindVertexArray(scene->myKdTree.DebugVAO);

		frameCount++;
		if (frameCount / 400 > 0)
		{
			frameCount = 0;
			debugDepth += 1;
			scene->myKdTree.BuildDebugKdTree_l(debugDepth, debugDepth);
		}

		glDrawElements(GL_LINES, scene->myKdTree.DebugIndices.size(), GL_UNSIGNED_INT, 0);
		glBindVertexArray(0);

		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

	}

}

void Renderer::FrameBufferToScreen(GLuint frameBufferTexture) {
	glViewport(0, 0, RENDER_WIDTH, RENDER_HEIGHT);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	glDisable(GL_DEPTH_TEST);
	glClearColor(1.0f, 0.05f, 0.05f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT);
	screenShader.use();
	screenShader.setTexture("screenTexture", frameBufferTexture, 6);
	DrawQuad(screenShader);
}

glm::mat4 Renderer::Perspective()
{
	if (!camera)return glm::mat4(1.0f);
	return glm::perspective(glm::radians(camera->Zoom), (float)RENDER_WIDTH / (float)RENDER_HEIGHT, 0.1f, 100.0f);
}

glm::mat4 Renderer::View()
{
	return camera->GetViewMatrix();
}

glm::mat4 Renderer::CameraRotate()
{
	return camera->GetRotateMatrix();
}

int Renderer::RendererClose()
{
	return glfwWindowShouldClose(window);
}


void Renderer::SwapBuffers()
{
	glfwSwapBuffers(window);
}

void Renderer::PollEvents()
{
	glfwPollEvents();
}

void Renderer::Terminate()
{
	glfwTerminate();
}


void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
	glViewport(0, 0, width, height);
}

void mouse_callback(GLFWwindow* window, double xposIn, double yposIn)
{
	float xpos = static_cast<float>(xposIn);
	float ypos = static_cast<float>(yposIn);

	if (Renderer::GetRenderer()->firstMouse)
	{
		Renderer::GetRenderer()->lastX = xpos;
		Renderer::GetRenderer()->lastY = ypos;
		Renderer::GetRenderer()->firstMouse = false;
	}

	float xoffset = xpos - Renderer::GetRenderer()->lastX;
	float yoffset = Renderer::GetRenderer()->lastY - ypos;

	Renderer::GetRenderer()->lastX = xpos;
	Renderer::GetRenderer()->lastY = ypos;

	Renderer::GetRenderer()->camera->ProcessMouseMovement(xoffset, yoffset);
}

void scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{
	Renderer::GetRenderer()->camera->ProcessMouseScroll(static_cast<float>(yoffset));
}

void Renderer::RegisterCallback()
{
	glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
	glfwSetCursorPosCallback(window, mouse_callback);
	glfwSetScrollCallback(window, scroll_callback);
}

void Renderer::processInput(float deltaTime)
{
	if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
		glfwSetWindowShouldClose(window, true);

	if (renderPath == RenderPath::RT)
	{
		return;
	}

	if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
		camera->ProcessKeyboard(FORWARD, deltaTime);
	if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
		camera->ProcessKeyboard(BACKWARD, deltaTime);
	if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
		camera->ProcessKeyboard(LEFT, deltaTime);
	if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
		camera->ProcessKeyboard(RIGHT, deltaTime);
}

void Renderer::DrawSkybox()
{
	glDepthFunc(GL_LEQUAL);
	skyboxShader.use();
	glm::mat4 passToProjection = glm::perspective(glm::radians(camera->Zoom), (float)RENDER_WIDTH / (float)RENDER_HEIGHT,
		0.1f, 100.0f);
	glm::mat4 passToView = camera->GetViewMatrix();

	skyboxShader.setMat4("projection", passToProjection);
	skyboxShader.setMat4("view", passToView);

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_CUBE_MAP, scene->envCubeMap);
	skyboxShader.setInt("environmentMap", 0);


	glViewport(0, 0, RENDER_WIDTH, RENDER_HEIGHT);

	DrawCube(skyboxShader);

	glDepthFunc(GL_LESS);
}



struct Renderer::Ray
{
	vec3 startPoint;
	vec3 direction;
};

struct Renderer::HitResult
{
	bool isHit;
	bool isInside;
	float distance;
	vec3 hitPoint;
	vec3 normal;
	vec3 viewDir;
};


BVHNode Renderer::GetBVHNode(int i)
{
	return scene->myBVH.nodes[i];
}


Renderer::HitResult Renderer::HitTriangle(Triangle triangle, Ray ray)
{
	HitResult res;
	res.distance = INF;
	res.isHit = false;
	res.isInside = false;

	vec3 p1 = triangle.vertex[0].Position;
	vec3 p2 = triangle.vertex[1].Position;
	vec3 p3 = triangle.vertex[2].Position;

	vec3 S = ray.startPoint;
	vec3 d = ray.direction;
	vec3 N = normalize(cross(p2 - p1, p3 - p1));

	if (dot(N, d) > 0.0f)
	{
		N = -N;
		res.isInside = true;
	}

	if (abs(dot(N, d)) < 0.00001f) return res;

	float t = (dot(N, p1) - dot(S, N)) / dot(d, N);
	if (t < 0.0005f) return res;

	vec3 P = S + d * t;


	vec3 c1 = cross(p2 - p1, P - p1);
	vec3 c2 = cross(p3 - p2, P - p2);
	vec3 c3 = cross(p1 - p3, P - p3);
	bool r1 = (dot(c1, N) > 0 && dot(c2, N) > 0 && dot(c3, N) > 0);
	bool r2 = (dot(c1, N) < 0 && dot(c2, N) < 0 && dot(c3, N) < 0);


	if (r1 || r2)
	{
		res.isHit = true;
		res.hitPoint = P;
		res.distance = t;
		res.normal = N;
		res.viewDir = d;

		float alpha = (-(P.x - p2.x) * (p3.y - p2.y) + (P.y - p2.y) * (p3.x - p2.x)) / (-(p1.x - p2.x - 0.00005) * (p3.y
			- p2.y + 0.00005) + (p1.y - p2.y + 0.00005) * (p3.x - p2.x + 0.00005));
		float beta = (-(P.x - p3.x) * (p1.y - p3.y) + (P.y - p3.y) * (p1.x - p3.x)) / (-(p2.x - p3.x - 0.00005) * (p1.y
			- p3.y + 0.00005) + (p2.y - p3.y + 0.00005) * (p1.x - p3.x + 0.00005));
		float gama = 1.0 - alpha - beta;
		vec3 Nsmooth = alpha * triangle.vertex[0].Normal + beta * triangle.vertex[1].Normal + gama * triangle.vertex[2].
			Normal;
		Nsmooth = normalize(Nsmooth);
		res.normal = (res.isInside) ? (-Nsmooth) : (Nsmooth);
	}

	return res;
}

Triangle Renderer::GetTriangle(int i)
{
	if (renderPath <= RenderPath::DebugBVH || renderPath == RenderPath::TestBVH)
		return scene->myBVH.triangles[i];
	else if (renderPath == RenderPath::DebugOctree || renderPath == RenderPath::TestOctree)
		return scene->myOctree.triangles[i];
	else if (renderPath == RenderPath::DebugKdTree || renderPath == RenderPath::TestKdTree)
		return scene->myKdTree.triangles[i];
}

Renderer::HitResult Renderer::HitArray(Ray ray, int l, int r)
{
	HitResult res;
	res.isHit = false;
	res.distance = INF;
	for (int i = l; i <= r; i++)
	{
		Triangle triangle = GetTriangle(i);
		HitResult r = HitTriangle(triangle, ray);
		if (r.isHit && r.distance < res.distance)
		{
			res = r;
		}
	}
	return res;
}

float Renderer::HitAABB(Ray r, vec3 AA, vec3 BB)
{
	vec3 invdir = vec3(1.0 / r.direction.x, 1.0 / r.direction.y, 1.0 / r.direction.z);

	vec3 f = (BB - r.startPoint) * invdir;
	vec3 n = (AA - r.startPoint) * invdir;

	vec3 tmax = max(f, n);
	vec3 tmin = min(f, n);

	float t1 = std::min(tmax.x, std::min(tmax.y, tmax.z));
	float t0 = std::max(tmin.x, std::max(tmin.y, tmin.z));

	return (t1 >= t0) ? ((t0 > 0.0) ? (t0) : (t1)) : (-1.0);
}

Renderer::HitResult Renderer::HitBVH(Ray ray)
{
	HitResult res;
	res.isHit = false;
	res.distance = INF;

	int stack[256];
	int sp = 0;

	stack[sp++] = 0;
	while (sp > 0)
	{
		int top = stack[--sp];
		BVHNode node = GetBVHNode(top);


		if (node.n > 0)
		{
			int L = node.index;
			int R = node.index + node.n - 1;
			HitResult r = HitArray(ray, L, R);
			if (r.isHit && r.distance < res.distance) res = r;
			continue;
		}


		float d1 = INF;
		float d2 = INF;
		if (node.left > 0)
		{
			BVHNode leftNode = GetBVHNode(node.left);
			d1 = HitAABB(ray, leftNode.AA, leftNode.BB);
		}
		if (node.right > 0)
		{
			BVHNode rightNode = GetBVHNode(node.right);
			d2 = HitAABB(ray, rightNode.AA, rightNode.BB);
		}


		if (d1 > 0 && d2 > 0)
		{
			if (d1 < d2)
			{
				stack[sp++] = node.right;
				stack[sp++] = node.left;
			}
			else
			{
				stack[sp++] = node.left;
				stack[sp++] = node.right;
			}
		}
		else if (d1 > 0)
		{
			stack[sp++] = node.left;
		}
		else if (d2 > 0)
		{
			stack[sp++] = node.right;
		}
	}

	return res;
}

Renderer::HitResult Renderer::HitOctree(Ray ray)
{
	HitResult res;
	res.isHit = false;
	res.distance = INF;

	stack<int> nodeStack;
	nodeStack.push(0); // 从根节点开始

	while (!nodeStack.empty())
	{
		int nodeId = nodeStack.top();
		nodeStack.pop();

		const OctreeNode& node = scene->myOctree.nodes[nodeId];

		// 检查射线是否与当前节点的包围盒相交
		float boxHitDistance = HitAABB(ray, node.center - vec3(node.halfSize), node.center + vec3(node.halfSize));
		if (boxHitDistance < 0) continue; // 不相交，跳过该节点

		// 如果是叶子节点，检查射线与节点中的三角形相交
		if (node.triangleIndices.size() > 0)
		{
			for (int triangleIndex : node.triangleIndices)
			{
				Triangle triangle = GetTriangle(triangleIndex);
				HitResult triangleHit = HitTriangle(triangle, ray);
				if (triangleHit.isHit && triangleHit.distance < res.distance)
				{
					res = triangleHit;
				}
			}
		}
		else
		{
			// 非叶子节点，将子节点压入栈中
			for (int i = 0; i < 8; ++i)
			{
				if (node.children[i] != -1)
				{
					nodeStack.push(node.children[i]);
				}
			}
		}
	}
	return res;
}

Renderer::HitResult Renderer::HitKDTree(Ray ray)
{
	HitResult res;
	res.isHit = false;
	res.distance = INF;

	std::stack<int> nodeStack;
	nodeStack.push(0); // 从根节点开始

	while (!nodeStack.empty())
	{
		int nodeId = nodeStack.top();
		nodeStack.pop();

		const KdTreeNode& node = scene->myKdTree.nodes[nodeId];

		// 检查射线是否与当前节点的包围盒相交
		float boxHitDistance = HitAABB(ray, node.AA, node.BB);
		if (boxHitDistance < 0) continue; // 不相交，跳过该节点

		// 如果是叶子节点，检查射线与节点中的三角形相交
		if (node.n > 0)
		{
			int L = node.index;
			int R = node.index + node.n - 1;
			HitResult r = HitArray(ray, L, R);
			if (r.isHit && r.distance < res.distance)
			{
				res = r;
			}
		}
		else
		{
			// 非叶子节点，将子节点压入栈中
			if (node.left > 0)
			{
				nodeStack.push(node.left);
			}
			if (node.right > 0)
			{
				nodeStack.push(node.right);
			}
		}
	}
	return res;
}


void Renderer::TestDraw(GLuint targetFrameBuffer)
{
	glViewport(0, 0, RENDER_WIDTH, RENDER_HEIGHT);
	RenderContext context;
	context.projection = Perspective();
	context.view = View();
	context.viewPos = camera->Position;

	if (renderPath == RenderPath::TestBVH)
	{
		float pixelWidth = 2.0f / SCR_WIDTH;
		float pixelHeight = 2.0f / SCR_HEIGHT;

		auto allStart = std::chrono::high_resolution_clock::now();

		// 预先计算，不要每次循环 inverse
		mat4 cameraRotate = inverse(CameraRotate());

		std::vector<vec2> hitPoints;
		hitPoints.reserve(SCR_WIDTH * SCR_HEIGHT); // 提前分配内存，防止频繁realloc

		for (int x = 0; x < SCR_WIDTH; ++x)
		{
			for (int y = 0; y < SCR_HEIGHT; ++y)
			{
				// 直接算出当前像素在NDC（Normalized Device Coordinates）中的位置
				float ndcX = -1.0f + (x + 0.5f) * pixelWidth; // 注意加0.5f，采样中心
				float ndcY = -1.0f + (y + 0.5f) * pixelHeight;

				vec4 dir = vec4(ndcX, ndcY, -1.5f, 0.0f); // 方向向量w=0更合理
				dir = cameraRotate * dir;

				Ray ray;
				ray.startPoint = camera->Position;
				ray.direction = normalize(vec3(dir)); // 归一化方向

				// 只总计时，不要每个像素单独计时
				HitResult r = HitBVH(ray);

				if (r.isHit)
				{
					hitPoints.emplace_back(ndcX, ndcY);
				}
			}
		}

		auto allEnd = std::chrono::high_resolution_clock::now();
		auto allDuration = std::chrono::duration_cast<std::chrono::milliseconds>(allEnd - allStart).count();
		std::cout << "All rays tracing time = " << allDuration << " ms" << std::endl;


		// Render hit points to the framebuffer

		glGenVertexArrays(1, &TestVAO);
		glGenBuffers(1, &TestVBO);
		// 绑定 VAO
		glBindVertexArray(TestVAO);
		// 绑定 VBO，上传数据
		glBindBuffer(GL_ARRAY_BUFFER, TestVBO);
		glBufferData(GL_ARRAY_BUFFER, hitPoints.size() * sizeof(vec2), hitPoints.data(), GL_STATIC_DRAW);
		// 设置顶点属性
		glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, sizeof(vec2), (void*)0);
		glEnableVertexAttribArray(0);
		// 绑定完成，解绑 VAO（可以选择解绑，保险起见）
		glBindVertexArray(0);


		// 使用 unlitShader
		testShader.use();
		testShader.setMat4("projection", context.projection);
		testShader.setMat4("view", context.view);
		testShader.setMat4("model", glm::mat4(1.0f));
		testShader.setVec3("objectColor", vec3(1.0f, 0.0f, 0.0f)); // 红色

		glBindFramebuffer(GL_FRAMEBUFFER, frameBuffer0);
		glClearColor(0.0f, 1.0f, 0.0f, 1.0f); // 绿色背景
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		// 绘制点
		glBindVertexArray(TestVAO);
		glPointSize(3.0f); // 设置点大小
		glDrawArrays(GL_POINTS, 0, hitPoints.size());
		glBindVertexArray(0);

		// 解绑 framebuffer
		glBindFramebuffer(GL_FRAMEBUFFER, 0);

	}
	else if (renderPath == RenderPath::TestOctree)
	{
		float pixelWidth = 2.0f / SCR_WIDTH;
		float pixelHeight = 2.0f / SCR_HEIGHT;

		auto allStart = std::chrono::high_resolution_clock::now();

		// 预先计算，不要每次循环 inverse
		mat4 cameraRotate = inverse(CameraRotate());

		std::vector<vec2> hitPoints;
		hitPoints.reserve(SCR_WIDTH * SCR_HEIGHT); // 提前分配内存，防止频繁realloc

		for (int x = 0; x < SCR_WIDTH; ++x)
		{
			for (int y = 0; y < SCR_HEIGHT; ++y)
			{
				// 直接算出当前像素在NDC（Normalized Device Coordinates）中的位置
				float ndcX = -1.0f + (x + 0.5f) * pixelWidth; // 注意加0.5f，采样中心
				float ndcY = -1.0f + (y + 0.5f) * pixelHeight;

				vec4 dir = vec4(ndcX, ndcY, -1.5f, 0.0f); // 方向向量w=0更合理
				dir = cameraRotate * dir;

				Ray ray;
				ray.startPoint = camera->Position;
				ray.direction = normalize(vec3(dir)); // 归一化方向

				// 只总计时，不要每个像素单独计时
				HitResult r = HitOctree(ray);

				if (r.isHit)
				{
					hitPoints.emplace_back(ndcX, ndcY);
				}
			}
		}

		auto allEnd = std::chrono::high_resolution_clock::now();
		auto allDuration = std::chrono::duration_cast<std::chrono::milliseconds>(allEnd - allStart).count();
		std::cout << "All rays tracing time = " << allDuration << " ms" << std::endl;


		// Render hit points to the framebuffer

		glGenVertexArrays(1, &TestVAO);
		glGenBuffers(1, &TestVBO);
		// 绑定 VAO
		glBindVertexArray(TestVAO);
		// 绑定 VBO，上传数据
		glBindBuffer(GL_ARRAY_BUFFER, TestVBO);
		glBufferData(GL_ARRAY_BUFFER, hitPoints.size() * sizeof(vec2), hitPoints.data(), GL_STATIC_DRAW);
		// 设置顶点属性
		glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, sizeof(vec2), (void*)0);
		glEnableVertexAttribArray(0);
		// 绑定完成，解绑 VAO（可以选择解绑，保险起见）
		glBindVertexArray(0);


		// 使用 unlitShader
		testShader.use();
		testShader.setMat4("projection", context.projection);
		testShader.setMat4("view", context.view);
		testShader.setMat4("model", glm::mat4(1.0f));
		testShader.setVec3("objectColor", vec3(1.0f, 0.0f, 0.0f)); // 红色

		glBindFramebuffer(GL_FRAMEBUFFER, frameBuffer0);
		glClearColor(0.0f, 1.0f, 0.0f, 1.0f); // 绿色背景
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		// 绘制点
		glBindVertexArray(TestVAO);
		glPointSize(3.0f); // 设置点大小
		glDrawArrays(GL_POINTS, 0, hitPoints.size());
		glBindVertexArray(0);

		// 解绑 framebuffer
		glBindFramebuffer(GL_FRAMEBUFFER, 0);

	}
	else if (renderPath == RenderPath::TestKdTree)
	{
		float pixelWidth = 2.0f / SCR_WIDTH;
		float pixelHeight = 2.0f / SCR_HEIGHT;

		auto allStart = std::chrono::high_resolution_clock::now();

		// 预先计算，不要每次循环 inverse
		mat4 cameraRotate = inverse(CameraRotate());

		std::vector<vec2> hitPoints;
		hitPoints.reserve(SCR_WIDTH * SCR_HEIGHT); // 提前分配内存，防止频繁realloc

		for (int x = 0; x < SCR_WIDTH; ++x)
		{
			for (int y = 0; y < SCR_HEIGHT; ++y)
			{
				// 直接算出当前像素在NDC（Normalized Device Coordinates）中的位置
				float ndcX = -1.0f + (x + 0.5f) * pixelWidth; // 注意加0.5f，采样中心
				float ndcY = -1.0f + (y + 0.5f) * pixelHeight;

				vec4 dir = vec4(ndcX, ndcY, -1.5f, 0.0f); // 方向向量w=0更合理
				dir = cameraRotate * dir;

				Ray ray;
				ray.startPoint = camera->Position;
				ray.direction = normalize(vec3(dir)); // 归一化方向

				// 只总计时，不要每个像素单独计时
				HitResult r = HitKDTree(ray);

				if (r.isHit)
				{
					hitPoints.emplace_back(ndcX, ndcY);
				}
			}
		}

		auto allEnd = std::chrono::high_resolution_clock::now();
		auto allDuration = std::chrono::duration_cast<std::chrono::milliseconds>(allEnd - allStart).count();
		std::cout << "All rays tracing time = " << allDuration << " ms" << std::endl;


		// Render hit points to the framebuffer

		glGenVertexArrays(1, &TestVAO);
		glGenBuffers(1, &TestVBO);
		// 绑定 VAO
		glBindVertexArray(TestVAO);
		// 绑定 VBO，上传数据
		glBindBuffer(GL_ARRAY_BUFFER, TestVBO);
		glBufferData(GL_ARRAY_BUFFER, hitPoints.size() * sizeof(vec2), hitPoints.data(), GL_STATIC_DRAW);
		// 设置顶点属性
		glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, sizeof(vec2), (void*)0);
		glEnableVertexAttribArray(0);
		// 绑定完成，解绑 VAO（可以选择解绑，保险起见）
		glBindVertexArray(0);


		// 使用 unlitShader
		testShader.use();
		testShader.setMat4("projection", context.projection);
		testShader.setMat4("view", context.view);
		testShader.setMat4("model", glm::mat4(1.0f));
		testShader.setVec3("objectColor", vec3(1.0f, 0.0f, 0.0f)); // 红色

		glBindFramebuffer(GL_FRAMEBUFFER, frameBuffer0);
		glClearColor(0.0f, 1.0f, 0.0f, 1.0f); // 绿色背景
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		// 绘制点
		glBindVertexArray(TestVAO);
		glPointSize(3.0f); // 设置点大小
		glDrawArrays(GL_POINTS, 0, hitPoints.size());
		glBindVertexArray(0);

		// 解绑 framebuffer
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
	}
}


void Renderer::DrawFramwBuffer(GLuint targetFrameBuffer)
{
	glViewport(0, 0, RENDER_WIDTH, RENDER_HEIGHT);
	glBindFramebuffer(GL_FRAMEBUFFER, targetFrameBuffer);
	glDisable(GL_DEPTH_TEST);
	glClearColor(1.0f, 0.05f, 0.05f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT);
	screenShader.use();
	screenShader.setTexture("screenTexture", frameTextures0[0], 6);
	DrawQuad(screenShader);
}