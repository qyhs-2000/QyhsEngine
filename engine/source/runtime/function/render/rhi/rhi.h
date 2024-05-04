#pragma once
namespace QYHS
{
	class RHI
	{
	public:
		virtual ~RHI() = 0;
		virtual void initialize() = 0;
	};

	inline RHI::~RHI() = default;
}