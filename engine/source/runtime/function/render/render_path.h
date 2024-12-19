#pragma once

namespace QYHS
{
	class RenderPath
	{
	public:
		virtual void render();
		virtual void update(float delta_time) {}
	};
}