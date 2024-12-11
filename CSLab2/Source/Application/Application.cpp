/* Application.cpp */
#include "Application.h"
#include <print>
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <imgui.h>
#include <backends/imgui_impl_glfw.h>
#include <backends/imgui_impl_opengl3.h>

#include "Simulation.h"


namespace CSL2
{
	static void glfw_error_callback(int erorr, const char* description)
	{
		std::println("[GLFW] Error({0}) : {1}", erorr, description);
	}

	Application::Application()
	{
		int success = glfwInit();
		if (success)
		{
			glfwSetErrorCallback(glfw_error_callback);
		}
		glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
		glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 0);
		glfwWindowHint(GLFW_VISIBLE, GLFW_FALSE);

		m_Window = glfwCreateWindow(800, 800, "CompSystems Lab2", nullptr, nullptr);
		glfwShowWindow(m_Window);
		glfwMakeContextCurrent(m_Window);
		int gladStatus = gladLoadGLLoader((GLADloadproc)glfwGetProcAddress);
		glfwSwapInterval(1);
		glfwSetWindowUserPointer(m_Window, this);
		glfwSetWindowAttrib(m_Window, GLFW_RESIZABLE, true);


		IMGUI_CHECKVERSION();
		ImGui::CreateContext();
		ImGuiIO& io = ImGui::GetIO(); (void)io;
		io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;     // Enable Keyboard Controls	
		io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;

		//io.FontDefault = io.Fonts->AddFontFromFileTTF(("Roboto/Roboto-Regular.ttf").c_str(), 16);

		ImGui::StyleColorsDark();
		ImGui_ImplGlfw_InitForOpenGL(m_Window, true);
		ImGui_ImplOpenGL3_Init("#version 410");
		io.IniFilename = "Config/imgui.ini";

		m_Simualtion = std::make_unique<Simulation>();

	}

	Application::~Application()
	{
		ImGui_ImplOpenGL3_Shutdown();
		ImGui_ImplGlfw_Shutdown();
		ImGui::DestroyContext();
		glfwDestroyWindow(m_Window);
		glfwTerminate();
	}

	void Application::Run()
	{
		ImVec4 clear_color = ImVec4(0.45f, 0.55f, 0.60f, 1.00f);
		while (!glfwWindowShouldClose(m_Window) && m_Running)
		{
			glfwPollEvents();

			m_Simualtion->Update();

			int display_w, display_h;
			glfwGetFramebufferSize(m_Window, &display_w, &display_h);
			glViewport(0, 0, display_w, display_h);
			glClearColor(clear_color.x * clear_color.w, clear_color.y * clear_color.w, clear_color.z * clear_color.w, clear_color.w);
			glClear(GL_COLOR_BUFFER_BIT);

			ImGui_ImplOpenGL3_NewFrame();
			ImGui_ImplGlfw_NewFrame();
			ImGui::NewFrame();

			ImGuiViewport* viewport = ImGui::GetMainViewport();

			// Use ImGuiID for the first argument in DockSpaceOverViewport
			ImGui::DockSpaceOverViewport(viewport->ID, viewport, ImGuiDockNodeFlags_None);


			m_Simualtion->UpdateGUI();

			ImGui::Render();


			ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

			glfwSwapBuffers(m_Window);
		}
	}


}

