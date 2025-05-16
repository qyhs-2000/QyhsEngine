#include "jobsystem.h"
#include "common_include.h"
#include <cassert>
#include <iostream>
namespace qyhs::jobsystem
{
	void wait(const Context& ctx)
	{
		if (isBusy(ctx))
		{
			PriorityResource& res = internal_state.resources[int(ctx.priority)];

			// Wake any threads that might be sleeping:
			res.wake_condition.notify_all();

			// work() will pick up any jobs that are on stand by and execute them on this thread:
			res.work(res.next_queue.fetch_add(1) % res.threads_num);

			while (isBusy(ctx))
			{
				// If we are here, then there are still remaining jobs that work() couldn't pick up.
				//	In this case those jobs are not standing by on a queue but currently executing
				//	on other threads, so they cannot be picked up by this thread.
				//	Allow to swap out this thread by OS to not spin endlessly for nothing
				std::this_thread::yield();
			}
		}
	}

	bool isBusy(const Context & ctx)
	{
		return atomLoad(&ctx.count) > 0;
	}


	void execute(Context& ctx,const std::function<void(JobArgs)> & task)
	{
		PriorityResource& res = internal_state.resources[int(ctx.priority)];

		atomAdd(&ctx.count, 1);

		Job job;
		job.ctx = &ctx;
		job.task = task;
		job.group_id = 0;
		job.group_job_offset = 0;
		job.group_job_end = 1;

		if (res.threads_num < 1)
		{
			job.execute();
			return;
		}
		res.job_queue_per_thread[res.next_queue.fetch_add(1) % res.threads_num].push_back(job);
		res.wake_condition.notify_one();
	}
	void initialize(uint32_t max_thread_count)
	{
		//has initialized
		if (internal_state.num_cores > 0)
		{
			return;
		}

		internal_state.num_cores = std::thread::hardware_concurrency();
		for (int prio = 0; prio < (int)Priority::Priority_Count; ++prio)
		{
			const Priority& priority = (Priority)prio;
			PriorityResource& res = internal_state.resources[prio];
			switch (priority)
			{
			case Priority::High:
				res.threads_num = internal_state.num_cores - 1;  //-1 for main thread
				break;
			case Priority::Low:
				res.threads_num = internal_state.num_cores - 2; //-1 for main thread,-1 for streaming
				break;
			case Priority::Streaming:
				res.threads_num = 1;
				break;
			default:
				assert(0);
				break;
			}
			res.threads_num = clamp(res.threads_num, 1u, max_thread_count);
			res.job_queue_per_thread.reset(new JobQueue[res.threads_num]);
			res.threads.reserve(res.threads_num);
			
			for (unsigned int thread_id = 0; thread_id < res.threads_num; ++thread_id)
			{
				std::thread& worker = res.threads.emplace_back([thread_id, &res] {
					while (internal_state.alive.load())
					{
						res.work(thread_id);
						std::unique_lock<std::mutex> locker(res.wake_mutex);
						res.wake_condition.wait(locker);
					}
					});
			}
		}

	}
	
	uint32_t dispatchGroupCount(uint32_t job_count, uint32_t group_size)
	{
		return (job_count + group_size - 1) / group_size;
	}

	void dispatch(Context& ctx, uint32_t job_count, uint32_t group_size, const std::function<void(JobArgs)>& task,uint32_t sharedmemory_size)
	{
		if (job_count == 0 || group_size == 0)
		{
			return;
		}
		PriorityResource& resource = internal_state.resources[(int)ctx.priority];
		const uint32_t group_count = dispatchGroupCount(job_count, group_size);
		atomAdd(&ctx.count, group_count);
		Job job;
		job.ctx = &ctx;
		job.task = task;
		job.shared_memory_size = sharedmemory_size;
		for (int group_id = 0; group_id < group_count; ++group_id)
		{
			job.group_id = group_id;
			job.group_job_offset = group_id * group_size;
			job.group_job_end = std::min(job.group_job_offset + group_size, job_count);
			if (resource.threads_num < 1)
			{
				job.execute();
				return;
			}
			else
			{
				resource.job_queue_per_thread[resource.next_queue.fetch_add(1) % resource.threads_num].push_back(job);
			}
		}
		if (resource.threads_num > 1)
		{
			resource.wake_condition.notify_all();
		}
	}

}