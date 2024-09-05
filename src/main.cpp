#include <stdio.h>
#include <stdlib.h>
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm.hpp>
#include "ResourceHandlers/ResourceManager.h"
#include "Game.h"
#include "imgui/imgui.h"
#include "imgui/imgui_impl_glfw.h"
#include "imgui/imgui_impl_opengl3.h"

using namespace glm;

void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void key_callback(GLFWwindow* window, int key, int scancode, int action, int mode);
glm::vec2 GetMousePos(GLFWwindow* window);

const unsigned int SCREEN_WIDTH = 1280;
const unsigned int SCREEN_HEIGHT = 720;

bool showdemo = true;

Game* game = new Game(SCREEN_WIDTH, SCREEN_HEIGHT);

GLFWwindow* window;

int main()
{

#pragma region WINDOW SETUP
	glewExperimental = true;
	if (!glfwInit())
	{
		fprintf(stderr, "Failed to initialize GLFW\n");
		return -1;
	}
	glfwWindowHint(GLFW_SAMPLES, 4);//Anti aliasing
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	window = glfwCreateWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "Kitchen Survivors", NULL, NULL);
	if (window == NULL)
	{
		fprintf(stderr, "Failed to create window");
		glfwTerminate();
		return -1;
	}
	glViewport(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT);
	glfwMakeContextCurrent(window);
	glewExperimental = true;
	if (glewInit() != GLEW_OK) 
	{
		fprintf(stderr, "Failed to initialize GLEW");
		return -1;
	}
	glfwSetKeyCallback(window, key_callback);
	glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);


	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

#pragma endregion

	IMGUI_CHECKVERSION();

	ImGui::CreateContext();
	ImGui_ImplGlfw_InitForOpenGL(window, true);

	ImGui::StyleColorsDark();


	ImGui_ImplOpenGL3_Init();

	ImGuiWindowFlags fpsFlag = 0;

	//fpsFlag |= ImGuiWindowFlags_NoMove;
	fpsFlag |= ImGuiWindowFlags_AlwaysAutoResize;
	fpsFlag |= ImGuiWindowFlags_NoCollapse;


	game->Init();

	float deltaTime = 0.0f;
	float lastFrame = 0.0f;
	int frames = 0;
	float t = 0;
	float fps = 60.0f;
	while (!glfwWindowShouldClose(window))
	{
		glfwPollEvents();
		if (glfwGetWindowAttrib(window, GLFW_ICONIFIED) != 0)
		{
			ImGui_ImplGlfw_Sleep(10);
			continue;
		}

		ImGui_ImplOpenGL3_NewFrame();
		ImGui_ImplGlfw_NewFrame();
		ImGui::NewFrame();

		frames++;

		float currentFrame = glfwGetTime();
		deltaTime = currentFrame - lastFrame;
		lastFrame = currentFrame;


		game->MousePos = GetMousePos(window);
		game->ProcessInput(deltaTime);

		game->Update(deltaTime);
		game->Collisions();

		glClearColor(0.0f, 0.0f, 0.6f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT);

		game->Render();
		game->RenderDebug();
		if (t >= 1.0f)
		{
			fps = frames / deltaTime;
			t = 0.0f;
		}
		else
		{
			t += deltaTime;
		}
		float f = 0.0f;
		ImGui::Begin("stats",(bool *)0, fpsFlag);
		ImGui::Text("fps : %.2f",fps);
		ImGui::Text("ms : %.4f", 1000.0f / fps);
		ImGui::SetNextWindowPos(ImVec2(500,100),ImGuiCond_FirstUseEver);
		ImGui::End();
		frames = 0;
		ImGui::Render();

		ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

		glfwSwapBuffers(window);
	}
	
	ResourceManager::Clear();
	ImGui_ImplOpenGL3_Shutdown();
	ImGui_ImplGlfw_Shutdown();
	ImGui::DestroyContext();
	glfwTerminate();

	return 0;
}

void key_callback(GLFWwindow* window, int key, int scancode, int action, int mode)
{
	if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
		glfwSetWindowShouldClose(window, true);
	if (key >= 0 && key < 1024)
	{
		if (action == GLFW_PRESS)
			game->Keys[key] = true;
		else if (action == GLFW_RELEASE)
			game->Keys[key] = false;
	}
}

glm::vec2 GetMousePos(GLFWwindow* window)
{
	double x, y;
	glfwGetCursorPos(window, &x, &y);
	return glm::vec2(x, y);
}

void framebuffer_size_callback(GLFWwindow* window, int width, int heigth)
{
	glViewport(0, 0, width, heigth);
}