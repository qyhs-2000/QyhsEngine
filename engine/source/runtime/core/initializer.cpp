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
	static jobsystem::Context context;
	static std::atomic_bool systems_initialized[INITIALIZED_SYSTEM_COUNT]{};
	bool initializeFinished()
	{
		return initializationStarted.load() && !qyhs::jobsystem::isBusy(context);
	}
	void initializeComponentAsync()
	{
		if (initializeFinished())
		{
			return;
		}
		initializationStarted.store(true);

		qyhs::jobsystem::initialize(~0u);
		qyhs::jobsystem::execute(context, [](qyhs::jobsystem::JobArgs args) {qyhs::image::initialize(), systems_initialized[INITIALIZED_SYSTEM_IMAGE].store(true); });
		qyhs::jobsystem::execute(context, [](qyhs::jobsystem::JobArgs args) {qyhs::font::initialize(), systems_initialized[INITIALIZED_SYSTEM_FONT].store(true); });
		qyhs::jobsystem::execute(context, [](qyhs::jobsystem::JobArgs args) {qyhs::input::initialize(), systems_initialized[INITIALIZED_SYSTEM_INPUT].store(true); });
		qyhs::jobsystem::execute(context, [](qyhs::jobsystem::JobArgs args) {qyhs::renderer::initialize(), systems_initialized[INITIALIZED_SYSTEM_RENDERER].store(true); });
	}
}