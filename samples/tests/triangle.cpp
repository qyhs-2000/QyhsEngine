
#include "triangle.h"
#include "function/render/renderer.h"
namespace qyhs::triangle
{
	graphics::Shader triangle_vs;
	graphics::Shader triangle_ps;
	static RasterizerState rasterizer_state;
	static BlendState blend_state;
	static DepthStencilState depth_stencil_state;
	static PipelineState pso;
	void loadShaders()
	{
		renderer::loadShader(ShaderStage::VERTEX_SHADER,triangle_vs, "triangleVS.cso");
		renderer::loadShader(ShaderStage::PIXEL_SHADER,triangle_ps, "trianglePS.cso");
		
		RHI* rhi = rhi::getRHI();
		PipelineStateDesc desc;
		desc.vertex_shader = &triangle_vs;
		desc.fragment_shader = &triangle_ps;
		desc.blend_state = &blend_state;
		desc.depth_stencil_state = &depth_stencil_state;
		desc.rasterizer_state = &rasterizer_state;
		desc.primitive_topology = PrimitiveTopology::TRIANGLE_LIST;
		rhi->createPipelineState(&desc, &pso);
	}

	void initialize()
	{
		RasterizerState rs;
		rs.fill_mode = FillMode::SOLID;
		rs.cull_mode = CullMode::NONE;
		rs.front_counter_clockwise = true;
		rs.depth_bias = 0;
		rs.depth_bias_clamp = 0;
		rs.slope_scaled_depth_bias = 0;
		rs.depth_clip_enable = false;
		rs.multisample_enable = false;
		rs.antialiased_line_enable = false;
		rasterizer_state = rs;

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
		blend_state = bd;

		DepthStencilState dsd;
		dsd.depth_enable = true;
		dsd.depth_write_mask = DepthWriteMask::ZERO;
		dsd.depth_func = ComparisonFunc::GREATER_EQUAL;
		depth_stencil_state = dsd;

		loadShaders();
	}

	void draw(CommandList cmd)
	{
		RHI* rhi = rhi::getRHI();
		rhi->bindPipelineState(&pso, cmd);
		rhi->draw(3, 0, cmd);
	}

}
