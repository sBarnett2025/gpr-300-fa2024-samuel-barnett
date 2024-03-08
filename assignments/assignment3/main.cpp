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
samuelbarnett::Framebuffer lightOrbfb;

// Material setup
struct Material {
	float Ka = 1.0;
	float Kd = 0.5;
	float Ks = 0.5;
	float Shininess = 128;
}material;

// point light struct
struct PointLight
{
	glm::vec3 pos;
	float radius;
	glm::vec3 color;
};
const int MAX_POINT_LIGHTS = 64;
PointLight pointLights[MAX_POINT_LIGHTS];


// Post process variables
float sharpness = 0; //1.0 / 300.0;

// light dir
glm::vec3 lightDir = glm::vec3(0.0, -1.0, 0.0);

// point light radius
float lightRadius = 8;

void resetCamera(ew::Camera* camera, ew::CameraController* controller);
void framebufferSizeCallback(GLFWwindow* window, int width, int height);
GLFWwindow* initWindow(const char* title, int width, int height);
void drawUI(ew::Camera* camera, ew::CameraController* cameraController);

//Global state
int screenWidth = 1920;
int screenHeight = 1080;
float prevFrameTime;
float deltaTime;


int main() {
	GLFWwindow* window = initWindow("Assignment 3", screenWidth, screenHeight);
	glfwSetFramebufferSizeCallback(window, framebufferSizeCallback);

	// Shader setup
	ew::Shader deferredShader = ew::Shader("assets/deferredLit.vert", "assets/deferredLit.frag");
	ew::Shader shadows = ew::Shader("assets/depthOnly.vert", "assets/depthOnly.frag");
	ew::Shader sharpen = ew::Shader("assets/sharpen.vert", "assets/sharpen.frag");
	ew::Shader gShader = ew::Shader("assets/lit.vert", "assets/geometryPass.frag");
	ew::Shader lightOrbShader = ew::Shader("assets/lightOrb.vert","assets/lightOrb.frag");

	// Framebuffer setup
	samuelbarnett::Framebuffer framebuffer = samuelbarnett::createFramebuffer(screenWidth, screenHeight, GL_RGB16F);

	// Shadow buffer setup
	shadowbuffer = samuelbarnett::createShadowBuffer(screenWidth, screenHeight);

	// G buffer
	gbuffer = samuelbarnett::createGBuffer(screenWidth, screenHeight);

	// light orbs
	lightOrbfb = samuelbarnett::createFramebuffer(screenWidth, screenHeight, GL_RGB16F);

	// VAO
	unsigned int dummyVAO;
	glCreateVertexArrays(1, &dummyVAO);

	unsigned int VAO2;
	glCreateVertexArrays(1, &VAO2);

	unsigned int VAO3;
	glCreateVertexArrays(1, &VAO3);

	// Model setup
	ew::Model monkeyModel = ew::Model("assets/suzanne.fbx");
	ew::Transform monkeyTransform;

	//ew::Model groundModel = ew::Model("assets/Cube.fbx");
	ew::Mesh groundModel = ew::Mesh(ew::createPlane(40, 40, 5));
	ew::Transform groundTransform;

	groundTransform.position = glm::vec3(18.0f, -3.0f, 18.0f);

	// sphere
	ew::Mesh sphereMesh = ew::Mesh(ew::createSphere(1.0f, 8));

	// Texture setup
	GLuint monkeyTexture = ew::loadTexture("assets/brick_color.jpg");

	// Camera setup
	ew::Camera camera;
	camera.position = glm::vec3(0.0f, 0.0f, 5.0f);
	camera.target = glm::vec3(0.0f, 0.0f, 0.0f); //Look at the center of the scene
	camera.aspectRatio = (float)screenWidth / screenHeight;
	camera.fov = 60.0f; //Vertical field of view, in degrees
	ew::CameraController cameraController;
	cameraController.moveSpeed = 10;

	// Shadow Camera
	ew::Camera shadowCam;
	shadowCam.orthographic = true;
	//shadowCam.position = lightDir;
	shadowCam.target = glm::vec3(0.0f, 0.0f, 0.0f); //Look at the center of the scene
	shadowCam.nearPlane = 0.5;
	shadowCam.farPlane = 30;
	shadowCam.orthoHeight = 80.0;
	shadowCam.aspectRatio = 1.0;
	//shadowCam.fov = 60.0f; //Vertical field of view, in degrees

	// create lights
	int c = 0;
	for (int z = 0; z < 8; z++)
	{
		for (int x = 0; x < 8; x++)
		{
			pointLights[c].pos = glm::vec3(x * 5, 2.0f, z * 5);
			c++;
		}
	}
	for (int i = 0; i < MAX_POINT_LIGHTS; i++)
	{
		pointLights[i].radius = 8;
		pointLights[i].color = glm::vec3(rand() % 2, rand() % 2, rand() % 2);
	}

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
		monkeyTransform.position = glm::vec3(0, 0, 0);
		for (int z = 0; z < 8; z++)
		{
			for (int x = 0; x < 8; x++)
			{
				monkeyTransform.position = glm::vec3(x * 5, 0, z * 5);
				shadows.setMat4("_Model", monkeyTransform.modelMatrix());
				monkeyModel.draw();
			}
		}

		shadows.setMat4("_Model", groundTransform.modelMatrix());
		groundModel.draw();


		// GEOMETRY PASS
		glBindFramebuffer(GL_FRAMEBUFFER, gbuffer.fbo);
		glViewport(0, 0, gbuffer.width, gbuffer.height);
		glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		glEnable(GL_DEPTH_TEST);

		gShader.use();
		gShader.setMat4("_ViewProjection", camera.projectionMatrix() * camera.viewMatrix());
		gShader.setMat4("_LightViewProj", shadowCam.projectionMatrix() * shadowCam.viewMatrix());

		gShader.setMat4("_Model", groundTransform.modelMatrix());
		groundModel.draw();

		monkeyTransform.position = glm::vec3(0, 0, 0);
		for (int z = 0; z < 8; z++)
		{
			for (int x = 0; x < 8; x++)
			{
				monkeyTransform.position = glm::vec3(x*5, 0, z*5);
				gShader.setMat4("_Model", monkeyTransform.modelMatrix());
				monkeyModel.draw();
			}
		}

		// LIGHTING PASS
		glBindFramebuffer(GL_FRAMEBUFFER, framebuffer.fbo);
		glViewport(0, 0, framebuffer.width, framebuffer.height);
		glClearColor(0.6f,0.8f,0.92f,1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); 

		deferredShader.use();

		glBindTextureUnit(0, gbuffer.colorBuffers[0]);
		glBindTextureUnit(1, gbuffer.colorBuffers[1]);
		glBindTextureUnit(2, gbuffer.colorBuffers[2]);
		glBindTextureUnit(3, shadowbuffer.depthBuffer);

		//shader.setInt("_MainTex", 0);
		deferredShader.setVec3("_EyePos", camera.position);
		deferredShader.setVec3("_LightDirection",lightDir);
		// material values
		deferredShader.setFloat("_Material.Ka", material.Ka);
		deferredShader.setFloat("_Material.Kd", material.Kd);
		deferredShader.setFloat("_Material.Ks", material.Ks);
		deferredShader.setFloat("_Material.Shininess", material.Shininess);

		glm::mat4 lightViewProj = shadowCam.projectionMatrix() * shadowCam.viewMatrix();
		deferredShader.setInt("_ShadowMap", 3);
		deferredShader.setMat4("_LightViewProj", lightViewProj);
		deferredShader.setFloat("_MinBias", 0.005);
		deferredShader.setFloat("_MaxBias", 0.015);

		// point lights 
		for (int i = 0; i < MAX_POINT_LIGHTS; i++)
		{
			std::string prefix = "_PointLights[" + std::to_string(i) + "].";
			deferredShader.setVec3(prefix + "position", pointLights[i].pos);
			//defferedShader.setFloat(prefix + "radius", pointLights[i].radius);
			deferredShader.setFloat(prefix + "radius", lightRadius);
			deferredShader.setVec3(prefix + "color", pointLights[i].color);
		}

		// Rotate model around Y axis
		monkeyTransform.rotation = glm::rotate(monkeyTransform.rotation, deltaTime, glm::vec3(0.0, 1.0, 0.0));
		//transform.modelMatrix() combines translation, rotation, and scale into a 4x4 model matrix
		deferredShader.setMat4("_ViewProjection", camera.projectionMatrix() * camera.viewMatrix());

		glBindVertexArray(VAO2);
		glDrawArrays(GL_TRIANGLES, 0, 6);

		/*
		defferedShader.setMat4("_Model", monkeyTransform.modelMatrix());
		monkeyModel.draw(); //Draws monkey model using current shader
		defferedShader.setMat4("_Model", groundTransform.modelMatrix());
		groundModel.draw();
		*/

		// LIGHT ORBS PASS
		//glBindFramebuffer(GL_READ_BUFFER, gbuffer.fbo);
		glBindFramebuffer(GL_DRAW_FRAMEBUFFER, lightOrbfb.fbo);
		//glBlitFramebuffer(0,0, screenWidth, screenHeight, 0,0, screenWidth, screenHeight, GL_DEPTH_BUFFER_BIT, GL_NEAREST);
		//glBindFramebuffer(GL_FRAMEBUFFER, framebuffer.fbo);
		glViewport(0, 0, framebuffer.width, framebuffer.height);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		// draw all light orbs
		lightOrbShader.use();
		lightOrbShader.setMat4("_ViewProjection", camera.projectionMatrix() * camera.viewMatrix());

		for (int i = 0; i < MAX_POINT_LIGHTS; i++)
		{
			glm::mat4 m = glm::mat4(1.0f);
			m = glm::translate(m, pointLights[i].pos);
			m = glm::scale(m, glm::vec3(0.5f));

			lightOrbShader.setMat4("_Model", m);
			lightOrbShader.setVec3("_Color", pointLights[i].color);
			sphereMesh.draw();
		}

		//glBindVertexArray(VAO3);
		//glDrawArrays(GL_TRIANGLES, 0, 6);

		// SHARPEN
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
		ImGui::SliderFloat("Light X", &lightDir.x, -1.0f, 1.0f);
		ImGui::SliderFloat("Light Y", &lightDir.y, -1.0f, 1.0f);
		ImGui::SliderFloat("Light Z", &lightDir.z, -1.0f, 1.0f);

	}

	if (ImGui::CollapsingHeader("Point Light Radius"))
	{
		ImGui::SliderFloat("Light Radius", &lightRadius, 0.0f, 20.0f);
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

	ImGui::Begin("Light Orbs");
	ImGui::Image((ImTextureID)lightOrbfb.colorBuffer, texSize, ImVec2(0, 1), ImVec2(1, 0));
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

