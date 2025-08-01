#pragma once
#include "function/render/rhi/rhi.h"
namespace qyhs
{
	class Translator
	{
	public:
		enum TRANSLATOR_STATE
		{
			TRANSLATOR_IDLE,
			TRANSLATOR_X,
			TRANSLATOR_Y,
			TRANSLATOR_Z,
			TRANSLATOR_XY,
			TRANSLATOR_XZ,
			TRANSLATOR_YZ,
			TRANSLATOR_XYZ,
		} state = TRANSLATOR_IDLE;
		void Draw(const scene::CameraComponent& camera,CommandList cmd);
		std::vector<scene::PickResult> selected;
		bool isTranslator = true;
		float dist = 0.4f;
		scene::TransformComponent transform;
		float opacity = 1;
	private:
	};
}