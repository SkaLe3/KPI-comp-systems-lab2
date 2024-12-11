/* Simulation.cpp */
#include "Simulation.h"

#include <imgui.h>
#include <string>
#include <print>
#include <algorithm>
#include <random>


namespace CSL2
{

	Simulation::Simulation()
		: m_TaskCount(100), m_ConveyorMode(0), m_TasksCompleted(0), m_Ticked(0), m_Processors(3)
	{
		m_Schedulings[0] = { 0, 1, 1, 2, 1, 0, 1, 1 };
		m_Schedulings[1] = { 0, 0, 1, 1, 0, 2, 2, 1 };
	}

	Simulation::~Simulation()
	{

	}

	void Simulation::Update()
	{

	}

	void Simulation::UpdateGUI()
	{
		ImGui::PushStyleColor(ImGuiCol_WindowBg, IM_COL32(32, 30, 34, 255));
		ImGui::Begin("Simulation Settings");
		ImGui::SeparatorText("Tables");

		DrawTable("Table 1", m_Schedulings[0]);
		ImGui::Dummy(ImVec2{ 0, 10 });
		DrawTable("Table 2", m_Schedulings[1]);

		ImGui::SeparatorText("Controls");
		ImGui::SetNextItemWidth(100);
		if (ImGui::InputInt("Tasks Amount", &m_TaskCount, 10, 100))
		{
			m_TaskCount = std::clamp(m_TaskCount, 10, 1000);
		}


		ImGui::Text("Conveyor Mode:"); ImGui::SameLine();
		ImGui::RadioButton("Static", &m_ConveyorMode, 0); ImGui::SameLine();
		ImGui::RadioButton("Dynamic", &m_ConveyorMode, 1);

		if (ImGui::Button("Simulate", ImVec2{ 100, 20 }))
		{
			StartSimulation();
		}

		ImGui::End();

		ImGui::PopStyleColor();
		ImGui::PushStyleColor(ImGuiCol_WindowBg, IM_COL32(21, 20, 22, 255));
		ImGui::Begin("Output");

		DrawResult();
		ImGui::End();
		ImGui::PopStyleColor();
	}

	void Simulation::StartSimulation()
	{
		m_Result.clear();
		m_WaitingTaskQueue.clear();
		m_WaitingTaskQueue.clear();
		m_Ticked = 0;
		m_TasksCompleted = 0;

		for (int i = 0; i < m_TaskCount; i++)
			m_WaitingTaskQueue.push_back(std::make_unique<Task>());

		ConveyorSimulation();
		m_TickResults.emplace_back(m_Ticked, m_ConveyorMode ? "Dynamic Conveyor" : "Static Conveyor", m_TasksCompleted);
	}

	void Simulation::ConveyorSimulation()
	{
		TryMakeNextTaskActive();
		PopulateProcessorsWithTasks();

		while (m_TasksCompleted != m_TaskCount)
		{
			SaveProcessorsStatistics();
			Tick();
			ReturnTasksToActiveQueue();
			TryMakeNextTaskActive();
			PopulateProcessorsWithTasks();
		}
	}

	void Simulation::TryMakeNextTaskActive()
	{
		if (m_ConveyorMode == 0)
		{
			if (m_Ticked % 7 == 0)
			{
				if (!m_WaitingTaskQueue.empty())
				{
					m_ActiveTaskQueue.push_back(std::move(m_WaitingTaskQueue[0]));
					m_WaitingTaskQueue.erase(m_WaitingTaskQueue.begin());
					m_ActiveTaskQueue.back()->Scheduling = &m_Schedulings[0];
				}
			}
		}
		else
		{
			static std::random_device rd;
			static std::mt19937 gen(rd());
			static std::uniform_int_distribution<int> dist(0, 1);

			int schedulingIdx = dist(gen);

			std::vector<int32_t>* scheduling = &m_Schedulings[schedulingIdx];

			bool noCollision = true;
			for (const auto& task : m_ActiveTaskQueue)
			{
				noCollision &= CheckNoCollision(task->Stage, *task->Scheduling, scheduling);
			}
			if (!noCollision) return;

			if (!m_WaitingTaskQueue.empty())
			{
				m_ActiveTaskQueue.push_back(std::move(m_WaitingTaskQueue[0]));
				m_WaitingTaskQueue.erase(m_WaitingTaskQueue.begin());
				m_ActiveTaskQueue.back()->Scheduling = scheduling;
			}


		}
	}


	void Simulation::PopulateProcessorsWithTasks()
	{
		for (auto it = m_ActiveTaskQueue.begin(); it != m_ActiveTaskQueue.end(); )
		{
			int32_t stage = (*it)->Stage;
			uint32_t processorID = (*it)->Scheduling->at(stage);
			m_Processors[processorID].OwnedTask = std::move(*it);
			it = m_ActiveTaskQueue.erase(it);
		}
	}

	void Simulation::SaveProcessorsStatistics()
	{
		TickData data;
		data.Tick = m_Ticked;
		int32_t procIdx = 0;

		for (size_t procIdx = 0; procIdx < m_Processors.size(); procIdx++)
		{
			if (m_Processors[procIdx].OwnedTask)
			{
				data.Procs[procIdx] = std::to_string(m_Processors[procIdx].OwnedTask->Stage).front();
			}
			else
				data.Procs[procIdx] = ' ';
		}
		m_Result.push_back(data);
	}

	void Simulation::Tick()
	{
		for (auto& processor : m_Processors)
		{
			if (processor.Tick())
				m_TasksCompleted++;
		}
		m_Ticked++;
	}

	void Simulation::ReturnTasksToActiveQueue()
	{
		for (auto& processor : m_Processors)
		{
			if (processor.OwnedTask)
			{
				auto task = std::move(processor.OwnedTask);
				processor.OwnedTask = nullptr;
				if (task->Stage >= 0)
					m_ActiveTaskQueue.push_back(std::move(task));
			}
		}
	}

	bool Simulation::CheckNoCollision(int32_t taskStage, std::vector<int32_t> taskScheduling, std::vector<int32_t>* scheduling)
	{
		int32_t moveStages = 7 - taskStage;
		taskScheduling.erase(taskScheduling.end() - moveStages, taskScheduling.end());
		for (int i = 0; i < moveStages; i++)
		{
			taskScheduling.insert(taskScheduling.begin(), -1);
		}

		for (size_t i = 0; i < taskScheduling.size(); ++i)
		{
			if (taskScheduling.at(i) == scheduling->at(i))
			{
				return false;
			}
		}
		return true;
	}

	void Simulation::DrawTable(const char* tableName, const std::vector<int32_t>& table)
	{
		ImGui::Text(tableName);

		ImGuiTableFlags flags = ImGuiTableFlags_Borders
			| ImGuiTableFlags_SizingFixedFit | ImGuiTableFlags_NoHostExtendX;
		if (ImGui::BeginTable(tableName, 9, flags))
		{
			ImGui::TableSetupColumn("ID", ImGuiTableColumnFlags_WidthFixed, 16);
			ImGui::TableSetupColumn("0", ImGuiTableColumnFlags_WidthFixed, 16);
			ImGui::TableSetupColumn("1", ImGuiTableColumnFlags_WidthFixed, 16);
			ImGui::TableSetupColumn("2", ImGuiTableColumnFlags_WidthFixed, 16);
			ImGui::TableSetupColumn("3", ImGuiTableColumnFlags_WidthFixed, 16);
			ImGui::TableSetupColumn("4", ImGuiTableColumnFlags_WidthFixed, 16);
			ImGui::TableSetupColumn("5", ImGuiTableColumnFlags_WidthFixed, 16);
			ImGui::TableSetupColumn("6", ImGuiTableColumnFlags_WidthFixed, 16);
			ImGui::TableSetupColumn("7", ImGuiTableColumnFlags_WidthFixed, 16);
			ImGui::TableHeadersRow();


			for (size_t r = 0; r < 3; r++)
			{
				ImGui::TableNextRow();
				ImGui::TableSetColumnIndex(0);
				ImGui::Text(std::to_string(r + 1).c_str());
				for (size_t c = 0; c < table.size(); c++)
				{
					if (ImGui::TableSetColumnIndex(c + 1))
					{
						ImGui::TableSetBgColor(ImGuiTableBgTarget_CellBg, IM_COL32(0, 0, 0, 0));
						if (table[c] == r)
						{
							ImGui::TableSetBgColor(ImGuiTableBgTarget_CellBg, IM_COL32(30, 220, 30, 128));
						}
						ImGui::Text(" ");
					}
				}

			}
			ImGui::EndTable();

		}
	}

	void Simulation::DrawResult()
	{
		if (m_TickResults.empty()) return;

		ImGui::PushStyleColor(ImGuiCol_Border, ImVec4(0.0f, 0.8f, 0.2f, 1.0f));
		ImGui::PushStyleColor(ImGuiCol_FrameBg, ImVec4(0.2f, 0.2f, 0.2f, 1.f));
		ImGui::PushStyleVar(ImGuiStyleVar_FrameRounding, 5.0f);
		ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(10.0f, 5.0f));
		ImGui::PushItemWidth(40);
		for (auto tickResult : m_TickResults)
		{
			ImGui::SeparatorText(tickResult.ConveyorName.c_str());
			ImGui::InputInt("Times Ticked", &tickResult.Ticks, 0, 0, ImGuiInputTextFlags_ReadOnly);
			ImGui::InputInt("Tasks Completed", &tickResult.TasksAmount, 0, 0, ImGuiInputTextFlags_ReadOnly);
		}
		ImGui::PopItemWidth();
		ImGui::PopStyleVar(2);
		ImGui::PopStyleColor(2);
		ImGui::Separator();
		
		if (m_Result.empty()) return;

		ImGuiTableFlags flags = ImGuiTableFlags_Borders | ImGuiTableFlags_RowBg
			| ImGuiTableFlags_SizingFixedFit | ImGuiTableFlags_NoHostExtendX;
		if (ImGui::BeginTable("Result", 4, flags))
		{
			ImGui::TableSetBgColor(ImGuiTableBgTarget_CellBg, IM_COL32(30, 30, 30, 255));
			ImGui::TableSetupColumn("Tick");
			ImGui::TableSetupColumn("P1", ImGuiTableColumnFlags_WidthFixed, 20);
			ImGui::TableSetupColumn("P2", ImGuiTableColumnFlags_WidthFixed, 20);
			ImGui::TableSetupColumn("P3", ImGuiTableColumnFlags_WidthFixed, 20);
			ImGui::TableHeadersRow();

			for (int r = 0; r < m_Result.size(); r++)
			{
				ImGui::TableNextRow();
				ImGui::TableSetColumnIndex(0);
				ImGui::TableSetBgColor(ImGuiTableBgTarget_CellBg, IM_COL32(255, 255, 255, 30));
				ImGui::Text("%d", m_Result[r].Tick);
				for (int c = 0; c < 3; c++)
				{
					ImGui::TableNextColumn();
					ImGui::TableSetBgColor(ImGuiTableBgTarget_CellBg, IM_COL32(0, 0, 0, 0));
					ImGui::Text("%c", m_Result[r].Procs[c]);
				}
			}
			ImGui::EndTable();
		}

	}
}

