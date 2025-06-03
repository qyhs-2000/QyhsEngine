#pragma once
#include <stdint.h>
#include "core/math/vector2.h"
#include "core/math/vector3.h"
#include "core/math/vector4.h"
#include "core/math/directx_math.h"
namespace qyhs
{
	class Color
	{
	public:
		uint32_t rgba = 0;
		constexpr Color(uint32_t rgba) :rgba(rgba) {}
		constexpr Color(uint8_t r = 0, uint8_t g = 0, uint8_t b = 0, uint8_t a = 255) : rgba(uint32_t(r) | (uint32_t(g) << 8) | (uint32_t(b) << 16) | (uint32_t(a) << 24)) {}
		static constexpr Color shadow() { return Color(uint32_t(1690874980)); }
		constexpr XMFLOAT4 toFloat4() const
		{
			return XMFLOAT4(
				((rgba >> 0) & 0xFF) / 255.0f,
				((rgba >> 8) & 0xFF) / 255.0f,
				((rgba >> 16) & 0xFF) / 255.0f,
				((rgba >> 24) & 0xFF) / 255.0f
			);
		}
		constexpr operator XMFLOAT4()const { return toFloat4(); }
		constexpr operator uint32_t() const { return rgba; }
		
		constexpr uint8_t getR() const { return (rgba >> 0) & 0xFF; }
		constexpr uint8_t getG() const { return (rgba >> 8) & 0xFF; }
		constexpr uint8_t getB() const { return (rgba >> 16) & 0xFF; }
		constexpr uint8_t getA() const { return (rgba >> 24) & 0xFF; }

		inline void setR(uint8_t value){*this = Color(value, getG(), getB(), getA());}
		inline void setG(uint8_t value){*this = Color(getR(), value, getB(), getA()); }
		inline void setB(uint8_t value){*this = Color(getR(), getG(), value, getA()); }
		inline void setA(uint8_t value){*this = Color(getR(), getG(), getB(), value); }
	private:
	};
}