#pragma once

namespace qyhs::initializer
{
	enum INITIALIZED_SYSTEM
	{
		INITIALIZED_SYSTEM_IMAGE = 0,
		INITIALIZED_SYSTEM_FONT,

		INITIALIZED_SYSTEM_INPUT,
		INITIALIZED_SYSTEM_RENDERER,
		INITIALIZED_SYSTEM_COUNT
	};
	bool isInitializeFinished(INITIALIZED_SYSTEM system = INITIALIZED_SYSTEM_COUNT);
	void initializeComponentsAsync();
	void WaitForInitializationsToFinish();
}