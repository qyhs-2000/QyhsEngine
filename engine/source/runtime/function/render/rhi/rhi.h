#pragma once
namespace QYHS
{
	class CommandList
	{
	public:
		void* internal_state{ nullptr };
		bool isValid() const { return internal_state != nullptr; }
	private:
	};

	struct SwapChianDesc
	{
		int width = 0;
		int height = 0;
	};

	class SwapChain
	{
	public:
		SwapChain() = default;
		SwapChain(SwapChianDesc desc)
			: desc(desc)
		{
		}

		SwapChianDesc desc;
	private:
	};

	class Viewport
	{
	public:
		Viewport() = default;
		Viewport(float x, float y, float width, float height)
			: x(x), y(y), width(width), height(height)
		{
		}

		float x = 0.0f;
		float y = 0.0f;
		float width = 0.0f;
		float height = 0.0f;
	private:
	};

	class RHI
	{
	public:
		virtual ~RHI() = 0;
		virtual void initialize() = 0;
		virtual void initialize2() = 0;
		virtual void prepareContext() = 0;
		virtual CommandList beginCommandList() = 0;
	};

	inline RHI::~RHI() = default;
}