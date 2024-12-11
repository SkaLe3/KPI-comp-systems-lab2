/* Application.h */
#pragma once
#include <memory>

struct GLFWwindow;
namespace CSL2
{
	class Simulation;

	class Application
	{
	public:
		Application();
		~Application();
		void Run();

		GLFWwindow* m_Window;

	private:
		bool m_Running = true;
		std::unique_ptr<Simulation> m_Simualtion;

	};
}