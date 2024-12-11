/* Simulation.h */
#pragma once
#include "Types.h"

#include <imgui.h>

#include <vector>
#include <string>


namespace CSL2
{
	struct TickHistoryNode
	{
		int32_t Ticks;
		std::string ConveyorName;
		int32_t TasksAmount;
	};

	class Simulation
	{
	public:
		Simulation();
		~Simulation();

		void Update();
		void UpdateGUI();

	private:
		/* Logic */
		void StartSimulation();
		void ConveyorSimulation();

		void TryMakeNextTaskActive();
		void PopulateProcessorsWithTasks();
		void SaveProcessorsStatistics();
		void Tick();
		void ReturnTasksToActiveQueue();
		bool CheckNoCollision(int32_t taskStage, std::vector<int32_t> taskScheduling, std::vector<int32_t>* scheduling);


	private:
		/* GUI */
		void DrawTable(const char* tableName, const std::vector<int32_t>& table);
		void DrawResult();
	private:
		int32_t m_TaskCount;
		int32_t m_TasksCompleted;
		int32_t m_ConveyorMode;
		int32_t m_Ticked;

		std::vector<Processor> m_Processors;

		std::vector<int32_t> m_Schedulings[2];

		std::vector<std::unique_ptr<Task>> m_WaitingTaskQueue;
		std::vector<std::unique_ptr<Task>> m_ActiveTaskQueue;

		std::vector<TickData> m_Result;
		std::vector<TickHistoryNode> m_TickResults;

	};
}