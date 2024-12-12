/* Types.h */
#pragma once
#include <memory>
#include <vector>

namespace CSL2
{
	struct TickData
	{
		uint32_t Tick = 0;
		char Procs[3] = { ' ', ' ', ' ' };
	};


	struct Task
	{
		int32_t Stage = 7;
		std::vector<int32_t>* Scheduling = nullptr;

	};
	struct Processor
	{
		std::unique_ptr<Task> OwnedTask;
		bool Tick()
		{
			if (OwnedTask)
			{
				OwnedTask->Stage--;
				if (OwnedTask->Stage == -1)
					return true;
			}
			return false;
		}
	};


}