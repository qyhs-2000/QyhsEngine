#include "sprite_font.h"
#include "core/helper.h"

namespace qyhs
{

	void SpriteFont::setText(std::string&& value)
	{
		helper::stringConvert(value, text);
	}
	void SpriteFont::setText(const std::string& value)
	{
		helper::stringConvert(value, text);
	}

	void SpriteFont::draw(CommandList cmd)
	{

		font::draw(text.c_str(), getCurrentTextLength(), params, cmd);
	}
	uint32_t SpriteFont::getCurrentTextLength() const
	{
		return text.length();
	}
}
