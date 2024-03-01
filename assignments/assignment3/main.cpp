#include <stdio.h>
#include <math.h>

#include <ew/external/glad.h>

#include <GLFW/glfw3.h>
#include <imgui.h>
#include <imgui_impl_glfw.h>
#include <imgui_impl_opengl3.h>
#include <samuelbarnett/framebuffer.h>
#include <ew/shader.h>
#include <ew/model.h>
#include <ew/camera.h>
#include <ew/transform.h>
#include <ew/cameraController.h>
#include <ew/texture.h>
#include <ew/mesh.h>
#include <ew/procGen.h>
#include <iostream>

samuelbarnett::Framebuffer shadowbuffer;
samuelbarnett::Gbuffer gbuffer;

// Material setup
struct Material {
	float Ka = 1.0;
	float Kd = 0.5;
	float Ks = 0.5;
	float Shininess = 128;
}material;

// Post process variables
float sharpness = 0; //1.0 / 300.0;

// light dir
glm::vec3 lightDir = glm::vec3(0.0, -1.0, 0.0);

void resetCamera(ew::Camera* camera, ew::CameraController* controller);
void framebufferSizeCallback(GLFWwindow* window, int width, int height);
GLFWwindow* initWindow(const char* title, int width, int height);
void drawUI(ew::Camera* camera, ew::CameraController* cameraController);

//Global state
int screenWidth = 1080;
int screenHeight = 720;
float prevFrameTime;
float deltaTime;


int main() {
	GLFWwindow* window = initWindow("Assignment 3", screenWidth, screenHeight);
	glfwSetFramebufferSizeCallback(window, framebufferSizeCallback);

	// Shader setup
	ew::Shader shader = ew::Shader("assets/lit.vert", "assets/lit.frag");
	ew::Shader shadows = ew::Shader("assets/depthOnly.vert", "assets/depthOnly.frag");
	ew::Shader sharpen = ew::Shader("assets/sharpen.vert", "assets/sharpen.frag");
	ew::Shader gShader = ew::Shader("assets/lit.vert", "assets/geometryPass.frag");

	// Framebuffer setup
	samuelbarnett::Framebuffer framebuffer = samuelbarnett::createFramebuffer(screenWidth, screenHeight, GL_RGB16F);

	// Shadow buffer setup
	shadowbuffer = samuelbarnett::createShadowBuffer(screenWidth, screenHeight);

	// G buffer
	gbuffer = samuelbarnett::createGBuffer(screenWidth, screenHeight);

	// VAO
	unsigned int dummyVAO;
	glCreateVertexArrays(1, &dummyVAO);

	// Model setup
	ew::Model monkeyModel = ew::Model("assets/suzanne.fbx");
	ew::Transform monkeyTransform;

	//ew::Model groundModel = ew::Model("assets/Cube.fbx");
	ew::Mesh groundModel = ew::Mesh(ew::createPlane(10, 10, 5));
	ew::Transform groundTransform;

	groundTransform.position = glm::vec3(0.0f, -3.0f, 0.0f);

	// Texture setup
	GLuint monkeyTexture = ew::loadTexture("assets/brick_color.jpg");

	// Camera setup
	ew::Camera camera;
	camera.position = glm::vec3(0.0f, 0.0f, 5.0f);
	camera.target = glm::vec3(0.0f, 0.0f, 0.0f); //Look at the center of the scene
	camera.aspectRatio = (float)screenWidth / screenHeight;
	camera.fov = 60.0f; //Vertical field of view, in degrees
	ew::CameraController cameraController;

	// Shadow Camera
	ew::Camera shadowCam;
	shadowCam.orthographic = true;
	//shadowCam.position = lightDir;
	shadowCam.target = glm::vec3(0.0f, 0.0f, 0.0f); //Look at the center of the scene
	shadowCam.nearPlane = 0.5;
	shadowCam.farPlane = 20;
	shadowCam.orthoHeight = 5.0;
	shadowCam.aspectRatio = 1.0;
	//shadowCam.fov = 60.0f; //Vertical field of view, in degrees
	

	// OpenGL variables
	glEnable(GL_CULL_FACE);
	glCullFace(GL_BACK); //Back face culling
	glEnable(GL_DEPTH_TEST); //Depth testing

	//shadowCam.target = glm::vec3(0.0f, 0.0f, 0.0f);
	while (!glfwWindowShouldClose(window)) {
		glfwPollEvents();

		float time = (float)glfwGetTime();
		deltaTime = time - prevFrameTime;
		prevFrameTime = time;

		cameraController.move(window, &camera, deltaTime);

		glBindTextureUnit(0, monkeyTexture);
		glBindTextureUnit(1, shadowbuffer.depthBuffer);

		// shadow shader
		// RENDER DEPTH MAP
		glBindFramebuffer(GL_FRAMEBUFFER, shadowbuffer.fbo);
		glViewport(0, 0, shadowbuffer.width, shadowbuffer.height);
		glClear(GL_DEPTH_BUFFER_BIT);

		shadowCam.position = shadowCam.target - lightDir * 5.0f;

		shadows.use();
		shadows.setMat4("_ViewProjection", shadowCam.projectionMatrix() * shadowCam.viewMatrix());
		shadows.setMat4("_Model", monkeyTransform.modelMatrix());
		monkeyModel.draw();

		shadows.setMat4("_Model", groundTransform.modelMatrix());
		groundModel.draw();


		// g buffer
		glBindFramebuffer(GL_FRAMEBUFFER, gbuffer.fbo);
		glViewport(0, 0, gbuffer.width, gbuffer.height);
		glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		gShader.use();
		gShader.setMat4("_ViewProjection", camera.projectionMatrix() * camera.viewMatrix());
		gShader.setMat4("_LightViewProj", shadowCam.projectionMatrix() * shadowCam.viewMatrix());
		gShader.setMat4("_Model", monkeyTransform.modelMatrix());
		monkeyModel.draw();

		gShader.setMat4("_Model", groundTransform.modelMatrix());
		groundModel.draw();


		// RENDER SCENE NORMALLY
		glBindFramebuffer(GL_FRAMEBUFFER, framebuffer.fbo);
		glViewport(0, 0, framebuffer.width, framebuffer.height);
		glClearColor(0.6f,0.8f,0.92f,1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); 
		//glEnable(GL_DEPTH_TEST);

		

		// lit shader-------------------------------------------------
		shader.use();
		//shader.setInt("_MainTex", 0);
		shader.setVec3("_EyePos", camera.position);
		shader.setVec3("_LightDirection",lightDir);
		// material values
		shader.setFloat("_Material.Ka", material.Ka);
		shader.setFloat("_Material.Kd", material.Kd);
		shader.setFloat("_Material.Ks", material.Ks);
		shader.setFloat("_Material.Shininess", material.Shininess);

		glm::mat4 lightViewProj = shadowCam.projectionMatrix() * shadowCam.viewMatrix();
		shader.setMat4("_LightViewProj", lightViewProj);
		shader.setInt("_ShadowMap", 1);
		shader.setFloat("_MinBias", 0.005);
		shader.setFloat("_MaxBias", 0.015);

		// Rotate model around Y axis
		monkeyTransform.rotation = glm::rotate(monkeyTransform.rotation, deltaTime, glm::vec3(0.0, 1.0, 0.0));
		//transform.modelMatrix() combines translation, rotation, and scale into a 4x4 model matrix
		shader.setMat4("_ViewProjection", camera.projectionMatrix() * camera.viewMatrix());
		shader.setMat4("_Model", monkeyTransform.modelMatrix());
		monkeyModel.draw(); //Draws monkey model using current shader

		shader.setMat4("_Model", groundTransform.modelMatrix());
		groundModel.draw();


		// switch buffers
		
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
		glBindTextureUnit(0, framebuffer.colorBuffer);
		//glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		glViewport(0, 0, framebuffer.width, framebuffer.height);

		// sharpen shader------------------------------------------------
		sharpen.use();
		sharpen.setInt("_ColorBuffer", 0);
		sharpen.setFloat("_Sharpness", sharpness);
		
		//glDisable(GL_DEPTH_TEST);
		

		glBindVertexArray(dummyVAO);
		glDrawArrays(GL_TRIANGLES, 0, 6);

		drawUI(&camera, &cameraController);

		glfwSwapBuffers(window);
	}
	printf("Shutting down...");
}

void resetCamera(ew::Camera* camera, ew::CameraController* controller)
{
	camera->position = glm::vec3(0, 0, 5.0f);
	camera->target = glm::vec3(0);
	controller->yaw = controller->pitch = 0;
}

void drawUI(ew::Camera* camera, ew::CameraController* cameraController) {
	ImGui_ImplGlfw_NewFrame();
	ImGui_ImplOpenGL3_NewFrame();
	ImGui::NewFrame();

	ImGui::Begin("Settings");
	if (ImGui::Button("Reset Camera"))
	{
		resetCamera(camera, cameraController);
	}

	if (ImGui::CollapsingHeader("Material"))
	{
		ImGui::SliderFloat("AmbientK", &material.Ka, 0.0f, 1.0f);
		ImGui::SliderFloat("DiffuseK", &material.Kd, 0.0f, 1.0f);
		ImGui::SliderFloat("SpecularK", &material.Ks, 0.0f, 1.0f);
		ImGui::SliderFloat("Shininess", &material.Shininess, 0.0f, 1.0f);

	}

	if (ImGui::CollapsingHeader("Light Direction"))
	{
		ImGui::SliderFloat("Light X", &lightDir.x, -2.0f, 2.0f);
		ImGui::SliderFloat("Light Y", &lightDir.y, -2.0f, 2.0f);
		ImGui::SliderFloat("Light Z", &lightDir.z, -2.0f, 2.0f);

	}

	if (ImGui::CollapsingHeader("Sharpen"))
	{
		ImGui::SliderFloat("Sharpness", &sharpness, 0.0f, 1.0f);
	}

	ImGui::End();
	
	ImGui::Begin("ShadowMap");
	ImGui::BeginChild("ShadowMap");
	ImVec2 windowSize = ImGui::GetWindowSize();
	ImGui::Image((ImTextureID)shadowbuffer.depthBuffer, windowSize, ImVec2(0, 1), ImVec2(1, 0));
	ImGui::EndChild();
	ImGui::End();


	ImGui::Begin("GBuffers");
	ImVec2 texSize = ImVec2(gbuffer.width / 4, gbuffer.height / 4);
	for (size_t i = 0; i < 3; i++)
	{
		ImGui::Image((ImTextureID)gbuffer.colorBuffers[i], texSize, ImVec2(0, 1), ImVec2(1, 0));
	}

	ImGui::End();
	

	ImGui::Render();
	ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
}

void framebufferSizeCallback(GLFWwindow* window, int width, int height)
{
	glViewport(0, 0, width, height);
	screenWidth = width;
	screenHeight = height;
}

/// <summary>
/// Initializes GLFW, GLAD, and IMGUI
/// </summary>
/// <param name="title">Window title</param>
/// <param name="width">Window width</param>
/// <param name="height">Window height</param>
/// <returns>Returns window handle on success or null on fail</returns>
GLFWwindow* initWindow(const char* title, int width, int height) {
	printf("Initializing...");
	if (!glfwInit()) {
		printf("GLFW failed to init!");
		return nullptr;
	}

	GLFWwindow* window = glfwCreateWindow(width, height, title, NULL, NULL);
	if (window == NULL) {
		printf("GLFW failed to create window");
		return nullptr;
	}
	glfwMakeContextCurrent(window);

	if (!gladLoadGL(glfwGetProcAddress)) {
		printf("GLAD Failed to load GL headers");
		return nullptr;
	}

	//Initialize ImGUI
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGui_ImplGlfw_InitForOpenGL(window, true);
	ImGui_ImplOpenGL3_Init();

	return window;
}

