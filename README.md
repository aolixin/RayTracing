# 光追

## 结构

Raytracing
|

+----gl_ref	// glad glfw 源码引用

|

+----include	// 头文件

|	|	assimp	// 模型加载库

|	|	glad		// 管理 OpenGL 的函数指针

|	|	GLFW	// 用于创建窗口、上下文和处理输入事件

|	|	glm		// gl 数学库

|	|	BVH.h

|	|	Camera.h

|	|	Material.h

|	|	Mesh.h

|	|	Model.h

|	|	Renderer.h

|	|	Scene.h

|	|



这周刚离职

看了看, 看了看论文, 看了看 opengl

搭了一下基本的框架

搭了一些 forward 的管线

搭了简单的测试场景

写了一下 BVH，现在有点 bug，可视化还没做出来



下一步

把数据搬到 GPU，写光追的 shader



## 参考
https://github.com/AKGWSB/EzRT?tab=readme-ov-file