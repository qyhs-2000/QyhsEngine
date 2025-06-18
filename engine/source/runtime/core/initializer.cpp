#include "initializer.h"
#include <atomic>
#include "function/ui/image.h"
#include "function/ui/font.h"
#include "../samples/tests/triangle.h"
#include "jobsystem.h"
#include "function/render/renderer.h"
#include "function/input/input.h"
namespace qyhs::initializer
{
	static std::atomic_bool initializationStarted{ false };
	static jobsystem::context ctx;
	static std::atomic_bool systems[INITIALIZED_SYSTEM_COUNT]{};

	void InitializeComponentsImmediate()
	{
		if (isInitializeFinished())
			return;
		if (!initializationStarted.load())
		{
			initializeComponentsAsync();
		}
		WaitForInitializationsToFinish();
	}
	void initializeComponentsAsync()
	{
		if (isInitializeFinished())
			return;

		initializationStarted.store(true);

		jobsystem::Initialize();

		jobsystem::Execute(ctx, [](jobsystem::JobArgs args) { font::initialize(); systems[INITIALIZED_SYSTEM_FONT].store(true); });
		jobsystem::Execute(ctx, [](jobsystem::JobArgs args) { image::initialize(); systems[INITIALIZED_SYSTEM_IMAGE].store(true); });
		jobsystem::Execute(ctx, [](jobsystem::JobArgs args) { input::initialize(); systems[INITIALIZED_SYSTEM_INPUT].store(true); });
		jobsystem::Execute(ctx, [](jobsystem::JobArgs args) { renderer::initialize(); systems[INITIALIZED_SYSTEM_RENDERER].store(true); });

		std::thread([] {
			jobsystem::Wait(ctx);
			}).detach();

	}

	bool isInitializeFinished(INITIALIZED_SYSTEM system)
	{
		if (system == INITIALIZED_SYSTEM_COUNT)
		{
			return initializationStarted.load() && !jobsystem::IsBusy(ctx);
		}
		else
		{
			return systems[system].load();
		}
	}

	void WaitForInitializationsToFinish()
	{
		jobsystem::Wait(ctx);
	}
}