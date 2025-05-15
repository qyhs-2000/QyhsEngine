#pragma once
#include "function/render/rhi/rhi.h"
namespace qyhs::font
{
	static constexpr int FONTSIZE_DEFAULT = 16;
	struct Cursor
	{
		XMFLOAT2 position = {};
		XMFLOAT2 size = {};
	};
	enum Alignment
	{
		FONT_ALIGN_LEFT,		// left alignment (horizontal)
		FONT_ALIGN_CENTER,	// center alignment (horizontal or vertical)
		FONT_ALIGN_RIGHT,		// right alignment (horizontal)
		FONT_ALIGN_TOP,		// top alignment (vertical)
		FONT_ALIGN_BOTTOM		// bottom alignment (vertical)
	};
	struct Params
	{
		enum FLAGS
		{
			EMPTY = 0,
			SDF_RENDERING = 1 << 0,
			OUTPUT_COLOR_SPACE_HDR10_ST2084 = 1 << 1,
			OUTPUT_COLOR_SPACE_LINEAR = 1 << 2,
			DEPTH_TEST = 1 << 3,
			FLIP_HORIZONTAL = 1 << 4,
			FLIP_VERTICAL = 1 << 5,
		};
		Cursor cursor = {};
		int size = FONTSIZE_DEFAULT;
		float spacingX = 0, spacingY = 0;
		float h_wrap = -1;
		int style = 0;
		Color color;
		float scaling = 1; // this will apply upscaling to the text while keeping the same resolution (size) of the font
		float rotation = 0; // rotation around alignment anchor (in radians)
		union
		{
			XMFLOAT3 position = {};
			struct
			{
				float posX;
				float posY;
			};
		};
		Color shadowColor;
		float shadow_intensity = 1.0f; // shadow color multiplier
		float shadow_bolden = 0.1f; // value in [0,1] range (requires SDF rendering to be enabled)
		float shadow_offset_x = 0; // offset for shadow under the text in logical canvas coordinates
		float shadow_offset_y = 0; // offset for shadow under the text in logical canvas coordinates
		float shadow_softness = 0.5f; // value in [0,1] range (requires SDF rendering to be enabled)
		float softness = 0; // value in [0,1] range (requires SDF rendering to be enabled)
		float bolden = 0; // value in [0,1] range (requires SDF rendering to be enabled)
		Alignment h_align = Alignment::FONT_ALIGN_LEFT;
		Alignment v_align = Alignment::FONT_ALIGN_TOP;
		float intensity = 1.0f;
		uint32_t _flags = SDF_RENDERING;
		const XMMATRIX* custom_projection = nullptr;
		const XMMATRIX* custom_rotation = nullptr;
		constexpr bool isSDFRenderingEnabled() const { return _flags & SDF_RENDERING; }
		constexpr bool isFlippedHorizontally() const { return _flags & FLIP_HORIZONTAL; }
		constexpr bool isFlippedVertically() const { return _flags & FLIP_VERTICAL; }
		constexpr bool isDepthTestEnabled() const { return _flags & DEPTH_TEST; }
		
	};


	struct ParseStatus
	{
		Cursor cursor;
		uint32_t quad_count = 0;
		size_t last_word_begin = 0;
		bool start_new_word = false;
	};
	void draw(const wchar_t* text, uint32_t length, Params params, CommandList cmd);
	void initialize();
	int addFontStyle(const std::string& font_name, const uint8_t* data, size_t size, bool copy_data = false);
	void loadShaders();
	void updateAtlas(float upscaling);
	void setCanvas(const Canvas& current_canvas);
}