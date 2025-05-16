#pragma once

#include "function/render/color.h"
#include "core/utils/enum.h"
#include "function/render/rhi/graphics.h"
#include "function/ui/canvas.h"

using namespace qyhs::enums;
using namespace qyhs::graphics;
namespace qyhs
{
	class CommandList;
	namespace image
	{
		// Do not alter order or value because it is bound to lua manually!
		enum STENCILMODE
		{
			STENCILMODE_DISABLED,
			STENCILMODE_EQUAL,
			STENCILMODE_LESS,
			STENCILMODE_LESSEQUAL,
			STENCILMODE_GREATER,
			STENCILMODE_GREATEREQUAL,
			STENCILMODE_NOT,
			STENCILMODE_ALWAYS,
			STENCILMODE_COUNT
		};
		enum STENCILREFMODE
		{
			STENCILREFMODE_ENGINE,
			STENCILREFMODE_USER,
			STENCILREFMODE_ALL,
			STENCILREFMODE_COUNT
		};

		static Sampler samplers[SAMPLER_COUNT];
		static thread_local Texture background_texture;
		void setCanvas(const Canvas& current_canvas);
		void setBackground(const Texture& texture);
		enum  Quality
		{
			QUALITY_NEAREST,
			QUALITY_LINEAR,
			QUALITY_ANISOTROPIC
		};

		enum SampleMode
		{
			SAMPLEMODE_WRAP,
			SAMPLEMODE_MIRROR,
			SAMPLEMODE_CLAMP
		};

		enum StripMode
		{
			STRIP_ON,
			STRIP_OFF
		};

		class Params
		{
		public:
			enum FLAGS
			{
				EMPTY = 0,
				FULLSCREEN = 1 << 0,
				BACKGROUND = 1 << 1,
				CORNER_ROUNDING = 1 << 3,
				DEPTH_TEST = 1 << 4
			};
			uint32_t _flags = EMPTY;
			Vector3 pos{ 0,0,0 };
			Vector2 size{ 0,0 };
			XMFLOAT4 color{ 1,1,1,1 };
			XMFLOAT2 corners[4] = {
				XMFLOAT2(0,0),XMFLOAT2(1,0),
				XMFLOAT2(0,1),XMFLOAT2(1,1)
			};
			XMFLOAT2 texOffset = XMFLOAT2(0, 0);
			XMFLOAT2 texOffset2 = XMFLOAT2(0, 0);
			XMFLOAT2 pivot = XMFLOAT2(0, 0);
			Quality quality = QUALITY_LINEAR;
			SampleMode sample_flag = SAMPLEMODE_CLAMP;
			XMFLOAT2 scale{ 1,1 };
			BLENDMODE blendFlag = BLENDMODE::BLENDMODE_ALPHA;
			STENCILMODE stencil_mode = STENCILMODE::STENCILMODE_DISABLED;
			float rotation = 0;
			const XMMATRIX* custom_rotation = nullptr;
			const XMMATRIX* custom_projection = nullptr;
			const graphics::Texture* mask_map = nullptr;
			const graphics::Texture* background_map = nullptr;
			bool isBackgroundEnabled() const { return _flags & BACKGROUND; }
			bool isFullScreenEnabled()const { return _flags & FULLSCREEN; }
			bool isCornerRoundingEnabled()const { return _flags & CORNER_ROUNDING; }
			bool isDepthTestEnabled()const { return _flags & DEPTH_TEST; }
			void enableFullScreen() { _flags |= FULLSCREEN; }
			int image_subresource = -1;
			int mask_subresource = -1;
			int background_subresource = -1;
			float fade = 0;
			float intensity = 1;
			float opacity = 1;
			float saturation = 1;
			uint32_t stencil_ref = 0;
			StencilRefMode stencil_ref_mode = StencilRefMode::STENCILREFMODE_ALL;
		private:

		};

		void initialize();
		void loadShaders();
		void draw(const graphics::Texture* texture, const qyhs::image::Params& params, CommandList cmd);      //draw image
	}


}
