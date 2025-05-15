#pragma once
#include <functional>
#include <thread>
#include <atomic>
#include <deque>
#include <mutex>
#include "core/common_include.h"
#include <cassert>
namespace qyhs::jobsystem
{
	enum class Priority
	{

		High,
		Low,
		Streaming,
		Priority_Count
	};

	struct Context
	{
		volatile long count = 0;
		Priority priority = Priority::High;
	};

	void wait(const Context& ctx);

	struct JobArgs
	{
		unsigned int group_id;
		unsigned int job_index;
		unsigned int group_index;
		unsigned int is_first_job_in_group;
		unsigned int is_last_job_in_group;
		void* shared_memory;
	};

	struct Job
	{
		Context* ctx;
		std::function<void(JobArgs)> task;
		unsigned int group_id;
		unsigned int group_job_offset;
		unsigned int group_job_end;
		unsigned int shared_memory_size = 0;
		inline void execute()
		{
			JobArgs args;
			args.group_id = group_id;
			if (shared_memory_size > 0)
			{
				args.shared_memory = alloca(shared_memory_size);
			}
			else
			{
				args.shared_memory = nullptr;
			}
			for (unsigned int i = group_job_offset; i < group_job_end; ++i)
			{
				args.job_index = i;
				args.group_index = i - group_job_offset;
				args.is_first_job_in_group = (i == group_job_offset);
				args.is_last_job_in_group = (i == group_job_end - 1);
				task(args);
			}
			atomAdd(&ctx->count, -1);
		}
	};

	struct JobQueue
	{
		std::deque<Job> queue;
		std::mutex locker;

		inline void push_back(const Job& item)
		{
			std::scoped_lock lock(locker);
			queue.push_back(item);
		}
		inline bool pop_front(Job& item)
		{
			std::scoped_lock lock(locker);
			if (queue.empty())
			{
				return false;
			}
			item = std::move(queue.front());
			queue.pop_front();
			return true;
		}
	};

	struct PriorityResource
	{
		std::vector<std::thread> threads;
		std::unique_ptr<JobQueue[]> job_queue_per_thread;
		std::atomic<uint32_t> next_queue{ 0 };
		unsigned int threads_num = 0;
		std::mutex wake_mutex;
		std::condition_variable wake_condition;
		inline void work(uint32_t startingQueue)
		{
			Job job;
			for (uint32_t i = 0; i < threads_num; ++i)
			{
				JobQueue& job_queue = job_queue_per_thread[startingQueue % threads_num];
				while (job_queue.pop_front(job))
				{
					job.execute();
				}
				startingQueue++; // go to next queue
			}
		}
	};
	
	struct InternalState
	{
		PriorityResource resources[int(Priority::Priority_Count)];
		unsigned int num_cores;
		std::atomic_bool alive = { true };
	}static internal_state;

	bool isBusy(const Context & ctx);

	void execute(Context& ctx,const std::function<void(JobArgs)> & task);
	void initialize(uint32_t max_thread_count = ~0u);
	void dispatch(Context& ctx, uint32_t job_count, uint32_t group_size, const std::function<void(JobArgs)>& task, uint32_t sharedmemory_size = 0);
}