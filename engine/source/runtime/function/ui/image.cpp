#include "image.h"
#include <iostream>
#include "function/render/rhi/rhi.h"
#include "shader/shader_interop_image.h"
#include "core/math/directx_math.h"
#include "function/render/renderer.h"
namespace qyhs::image
{
	enum DEPTH_TEST_MODE
	{
		DEPTH_TEST_OFF,
		DEPTH_TEST_ON,
		DEPTH_TEST_MODE_COUNT
	};
	enum STRIP_MODE
	{
		STRIP_MODE_ON,
		STRIP_MODE_OFF,
		STRIP_MODE_COUNT
	};
	
	
	static Shader vertex_shader;
	static Shader pixel_shader;
	static RasterizerState rasterizer_state;
	static BlendState blend_states[BLENDMODE_COUNT];
	static DepthStencilState depth_stencil_states[STENCILMODE_COUNT][STENCILREFMODE_COUNT][DEPTH_TEST_MODE_COUNT];
	static PipelineState imagePSO[BLENDMODE_COUNT][STENCILMODE_COUNT][STENCILREFMODE_COUNT][DEPTH_TEST_MODE_COUNT][STRIP_MODE_COUNT];
	static Canvas canvas;
	void setCanvas(const Canvas& current_canvas)
	{
		canvas = current_canvas;
	}
	void setBackground(const Texture& texture)
	{
		background_texture = texture;
	}
    void initialize()
    {
		RHI* rhi = rhi::getRHI();

		BlendState bd;
		bd.render_targets[0].blend_enable = true;
		bd.render_targets[0].src_blend = Blend::SRC_ALPHA;
		bd.render_targets[0].dest_blend = Blend::INV_SRC_ALPHA;
		bd.render_targets[0].blend_op = BlendOp::ADD;
		bd.render_targets[0].src_blend_alpha = Blend::ONE;
		bd.render_targets[0].dest_blend_alpha = Blend::INV_SRC_ALPHA;
		bd.render_targets[0].blend_op_alpha = BlendOp::ADD;
		bd.render_targets[0].render_target_write_mask = ColorWrite::ENABLE_ALL;
		bd.independent_blend_enable = false;
		blend_states[BLENDMODE_ALPHA] = bd;

		bd.render_targets[0].blend_enable = true;
		bd.render_targets[0].src_blend = Blend::ONE;
		bd.render_targets[0].dest_blend = Blend::INV_SRC_ALPHA;
		bd.render_targets[0].blend_op = BlendOp::ADD;
		bd.render_targets[0].src_blend_alpha = Blend::ONE;
		bd.render_targets[0].dest_blend_alpha = Blend::INV_SRC_ALPHA;
		bd.render_targets[0].blend_op_alpha = BlendOp::ADD;
		bd.render_targets[0].render_target_write_mask = ColorWrite::ENABLE_ALL;
		bd.independent_blend_enable = false;
		blend_states[BLENDMODE_PREMULTIPLIED] = bd;

		bd.render_targets[0].blend_enable = false;
		bd.render_targets[0].render_target_write_mask = ColorWrite::ENABLE_ALL;
		bd.independent_blend_enable = false;
		blend_states[BLENDMODE_OPAQUE] = bd;

		bd.render_targets[0].blend_enable = true;
		bd.render_targets[0].src_blend = Blend::SRC_ALPHA;
		bd.render_targets[0].dest_blend = Blend::ONE;
		bd.render_targets[0].blend_op = BlendOp::ADD;
		bd.render_targets[0].src_blend_alpha = Blend::ZERO;
		bd.render_targets[0].dest_blend_alpha = Blend::ONE;
		bd.render_targets[0].blend_op_alpha = BlendOp::ADD;
		bd.render_targets[0].render_target_write_mask = ColorWrite::ENABLE_ALL;
		bd.independent_blend_enable = false;
		blend_states[BLENDMODE_ADDITIVE] = bd;

		bd.render_targets[0].blend_enable = true;
		bd.render_targets[0].src_blend = Blend::ZERO;
		bd.render_targets[0].dest_blend = Blend::SRC_COLOR;
		bd.render_targets[0].blend_op = BlendOp::ADD;
		bd.render_targets[0].src_blend_alpha = Blend::ZERO;
		bd.render_targets[0].dest_blend_alpha = Blend::SRC_ALPHA;
		bd.render_targets[0].blend_op_alpha = BlendOp::ADD;
		bd.render_targets[0].render_target_write_mask = ColorWrite::ENABLE_ALL;
		bd.independent_blend_enable = false;
		blend_states[BLENDMODE_MULTIPLY] = bd;

		SamplerDesc sampler_desc;
		sampler_desc.filter = Filter::MIN_MAG_MIP_LINEAR;
		sampler_desc.address_u = TextureAddressMode::CLAMP;
		sampler_desc.address_v = TextureAddressMode::CLAMP;
		sampler_desc.address_w = TextureAddressMode::CLAMP;
		sampler_desc.mip_lod_bias = 0.0f;
		sampler_desc.max_anisotropy = 0;
		sampler_desc.comparison_func = ComparisonFunc::NEVER;
		sampler_desc.border_color = SamplerBorderColor::TRANSPARENT_BLACK;
		sampler_desc.min_lod = 0;
		sampler_desc.max_lod = std::numeric_limits<float>::max();
		rhi->createSampler(&sampler_desc, &samplers[SAMPLER_LINEAR_CLAMP]);

		loadShaders();
    }

	void loadShaders()
	{
		qyhs::renderer::loadShader(ShaderStage::VERTEX_SHADER, vertex_shader, "imageVS.cso");
		qyhs::renderer::loadShader(ShaderStage::PIXEL_SHADER, pixel_shader, "imagePS.cso");
		
		RHI * rhi = rhi::getRHI();

		PipelineStateDesc desc;
		desc.vertex_shader = &vertex_shader;
		desc.fragment_shader = &pixel_shader;
		desc.rasterizer_state = &rasterizer_state;
		
		for (int j = 0; j < BLENDMODE_COUNT; ++j)
		{
			desc.blend_state = &blend_states[j];
			for (int k = 0; k < STENCILMODE_COUNT; ++k)
			{
				for (int m = 0; m < STENCILREFMODE_COUNT; ++m)
				{
					for (int d = 0; d < DEPTH_TEST_MODE_COUNT; ++d)
					{
						desc.depth_stencil_state = &depth_stencil_states[k][m][d];

						for (int n = 0; n < STRIP_MODE_COUNT; ++n)
						{
							switch (n)
							{
							default:
							case STRIP_ON:
								desc.primitive_topology = PrimitiveTopology::TRIANGLE_STRIP;
								break;
							case STRIP_OFF:
								desc.primitive_topology = PrimitiveTopology::TRIANGLE_LIST;
								break;
							}
							rhi->createPipelineState(&desc, &imagePSO[j][k][m][d][n]);
						}
					}
				}
			}
		}
	}

    void draw(const graphics::Texture* texture, const qyhs::image::Params& params, CommandList cmd)
    {
		auto rhi = rhi::getRHI();
        const Sampler* sampler = &image::samplers[SAMPLER_LINEAR_CLAMP];
		assert(sampler != nullptr);

		if (params.quality == QUALITY_NEAREST)
		{
			if (params.sample_flag == SAMPLEMODE_MIRROR)
			{
				sampler = &image::samplers[SAMPLER_POINT_MIRROR];
			}
			else if (params.sample_flag == SAMPLEMODE_WRAP)
			{
				sampler = &image::samplers[SAMPLER_POINT_WRAP];
			}
			else if (params.sample_flag == SAMPLEMODE_CLAMP)
			{
				sampler = &image::samplers[SAMPLER_POINT_CLAMP];
			}
		}
		else if (params.quality == QUALITY_LINEAR)
		{
			if (params.sample_flag == SAMPLEMODE_MIRROR)
			{
				sampler = &image::samplers[SAMPLER_LINEAR_MIRROR];
			}
			else if (params.sample_flag == SAMPLEMODE_WRAP)
			{
				sampler = &image::samplers[SAMPLER_LINEAR_WRAP];
			}
			else if (params.sample_flag == SAMPLEMODE_CLAMP)
			{
				sampler = &image::samplers[SAMPLER_LINEAR_CLAMP];
			}
		}
		else if (params.quality == QUALITY_ANISOTROPIC)
		{
			if (params.sample_flag == SAMPLEMODE_MIRROR)
			{
				sampler = &image::samplers[SAMPLER_ANISO_MIRROR];
			}
			else if (params.sample_flag == SAMPLEMODE_WRAP)
			{
				sampler = &image::samplers[SAMPLER_ANISO_WRAP];
			}
			else if (params.sample_flag == SAMPLEMODE_CLAMP)
			{
				sampler = &image::samplers[SAMPLER_ANISO_CLAMP];
			}
		}
		
		ImageConstants image = {};
		image.buffer_index = -1;
		image.buffer_offset = 0;
		image.texture_base_index = rhi->getDescriptorIndex(texture, SubresourceType::SRV, params.image_subresource);
		image.texture_mask_index = rhi->getDescriptorIndex(params.mask_map, SubresourceType::SRV, params.mask_subresource);
		if (params.isBackgroundEnabled())
		{
			if (params.background_map != nullptr)
			{
				image.texture_background_index = rhi->getDescriptorIndex(params.background_map, SubresourceType::SRV, params.background_subresource);
			}
			else
			{
				image.texture_background_index = rhi->getDescriptorIndex(&background_texture, SubresourceType::SRV);
			}
		}
		else
		{
			image.texture_background_index = -1;
		}
		image.sampler_index = rhi->getDescriptorIndex(sampler);
		if (image.sampler_index < 0)
			return;
		XMFLOAT4 color = params.color;
		const float darken = 1 - params.fade;
		color.x *= darken;
		color.y *= darken;
		color.z *= darken;
		color.x *= params.intensity;
		color.y *= params.intensity;
		color.z *= params.intensity;
		color.w *= params.opacity;

		XMHALF4 packed_color;
		packed_color.x = XMConvertFloatToHalf(color.x);
		packed_color.y = XMConvertFloatToHalf(color.y);
		packed_color.z = XMConvertFloatToHalf(color.z);
		packed_color.w = XMConvertFloatToHalf(color.w);

		image.packed_color.x = uint(packed_color.v);
		image.packed_color.y = uint(packed_color.v >> 32ull);

		image.saturation = params.saturation;
		image.flags = 0;
		if (params.isFullScreenEnabled())
		{
			image.flags |= IMAGE_FLAG_FULLSCREEN;
		}

		//image.border_soften = params.border_soften;
		//image.mask_alpha_range =  XMConvertFloatToHalf(params.mask_alpha_range_start) | (XMConvertFloatToHalf(params.mask_alpha_range_end) << 16u);

		StripMode strip_mode = STRIP_ON;
		uint32_t index_count = 0;

		if (params.isFullScreenEnabled())
		{
			image.flags |= IMAGE_FLAG_FULLSCREEN;
		}
		if (params.isFullScreenEnabled())
		{
			// full screen triangle, no vertex buffer:
			image.buffer_index = -1;
			image.buffer_offset = 0;
		}
		else
		{
			// vertex buffer:
			XMMATRIX S = XMMatrixScaling(params.scale.x * params.size.x, params.scale.y * params.size.y, 1);
			XMMATRIX M = XMMatrixRotationZ(params.rotation);

			if (params.custom_rotation != nullptr)
			{
				M = M * (*params.custom_rotation);
			}

			M = M * XMMatrixTranslation(params.pos.x, params.pos.y, params.pos.z);
			if (params.custom_projection != nullptr)
			{
				S = XMMatrixScaling(1, -1, 1) * S;
				M = M * (*params.custom_projection);
			}
			else
			{
				assert(canvas.width > 0);
				assert(canvas.height > 0);
				assert(canvas.dpi > 0);
				M = M * canvas.getProjection();
			}

			XMVECTOR V[4];
			float4 corners[4];
			for (int i = 0; i < arraysize(params.corners); ++i)
			{
				V[i] = XMVectorSet(params.corners[i].x - params.pivot.x, params.corners[i].y - params.pivot.y, 0, 1);
				V[i] = XMVector2Transform(V[i], S);
				XMStoreFloat4(corners + i, XMVector2Transform(V[i], M)); // division by w will happen on GPU
			}

			image.b0 = float2(corners[0].x, corners[0].y);
			image.b1 = float2(corners[1].x - corners[0].x, corners[1].y - corners[0].y);
			image.b2 = float2(corners[2].x - corners[0].x, corners[2].y - corners[0].y);
			image.b3 = float2(corners[0].x - corners[1].x - corners[2].x + corners[3].x, corners[0].y - corners[1].y - corners[2].y + corners[3].y);

			if (params.isCornerRoundingEnabled())
			{

			}
			else
			{
				RHI::GPUAllocation mem = rhi->allocateGPU(sizeof(float4) * 4, cmd);
				image.buffer_index = rhi->getDescriptorIndex(&mem.buffer, SubresourceType::SRV);
				image.buffer_offset = (uint)mem.offset;
				std::memcpy(mem.data, corners, sizeof(corners));
			}
		}

		float inv_width = 1.0f;
		float inv_height = 1.0f;
		if (texture != nullptr)
		{
			const TextureDesc& desc = texture->desc;
			inv_width = 1.0f / float(desc.width);
			inv_height = 1.0f / float(desc.height);
		}

		
		image.texMulAdd = XMFLOAT4(1, 1, 0, 0);	// disabled draw rect
		
		image.texMulAdd.z += params.texOffset.x * inv_width;	// texOffset.x: add
		image.texMulAdd.w += params.texOffset.y * inv_height;	// texOffset.y: add

		
		image.texMulAdd2 = XMFLOAT4(1, 1, 0, 0);	// disabled draw rect
		
		image.texMulAdd2.z += params.texOffset2.x * inv_width;	// texOffset.x: add
		image.texMulAdd2.w += params.texOffset2.y * inv_height;	// texOffset.y: add

		rhi->beginEvent("image", cmd);
		uint32_t stencil_ref = params.stencil_ref;
		if (params.stencil_ref_mode == STENCILREFMODE_USER)
		{
			//stencil_ref = qyhs::renderer::CombineStencilrefs(STENCILREF_EMPTY, (uint8_t)stencilRef);
		}
		rhi->bindStencilRef(stencil_ref, cmd);

		rhi->bindPipelineState(&imagePSO[params.blendFlag][params.stencil_mode][params.stencil_ref_mode][params.isDepthTestEnabled()][strip_mode], cmd);

		rhi->bindDynamicConstantBuffer(image, CBSLOT_IMAGE, cmd);

		if (params.isFullScreenEnabled())
		{
			rhi->draw(3, 0, cmd); // full screen triangle
		}
		else
		{
			switch (strip_mode)
			{
			case qyhs::image::STRIP_OFF:
				rhi->drawIndexed(index_count, 0, 0, cmd); // corners rounding with indexed geometry
				break;
			case qyhs::image::STRIP_ON:
			default:
				rhi->draw(4, 0, cmd); // simple quad
				break;
			}
		}

		rhi->endEvent(cmd);
    }

}
