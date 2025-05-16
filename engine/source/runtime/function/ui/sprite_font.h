#pragma once
//#include <string>
#include "function/ui/font.h"
namespace qyhs
{
	class SpriteFont
	{
	public:
		void setShadowColor(Color color) { params.shadowColor = color; }
		void setColorRGBA(uint32_t rgba) { params.color.rgba = rgba; }
		void setText( std::string&& value);
		void setText(const std::string& value);
		std::wstring text;
		void draw(CommandList cmd);
		font::Params params;
		uint32_t getCurrentTextLength()const;
	private:

	};
}