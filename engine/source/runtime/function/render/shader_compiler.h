#pragma once
#include <string>
#include "function/render/rhi/graphics.h"
#include <Windows.h>
#include "function/platform/platform.h"
#include "core/utils/dxcapi.h"
namespace qyhs::shadercompiler
{
	enum class Flags
	{
		NONE = 0,
		DISABLE_OPTIMIZATION = 1 << 0,
		STRIP_REFLECTION = 1 << 1,
	};

	struct CompilerInput
	{
		graphics::ShaderFormat format = graphics::ShaderFormat::NONE;
		graphics::ShaderStage stage = graphics::ShaderStage::COUNT;
		std::string shader_source_filename;
		Flags flags = Flags::NONE;
		graphics::ShaderModel minshadermodel = graphics::ShaderModel::SM_5_0;
		std::vector<std::string> defines;
		std::vector<std::string> include_directories;
		std::string entrypoint = "main";
	};

	struct Internal_DXCompiler
	{
		DxcCreateInstanceProc dxcCreateInstance = nullptr;
		Internal_DXCompiler(const std::string& modifier = "");
	};

	struct CompilerOutput
	{
		std::vector<std::string> dependencies;
		std::string error_message;
		const uint8_t* shader_data = nullptr;
		size_t shader_size = 0;
		std::shared_ptr<void> internal_state;
		std::vector<uint8_t> shader_hash;

		inline bool isValid() { return internal_state.get() != nullptr; }
	};

	void compile(CompilerInput& input, CompilerOutput& output);
	void compileDXCompiler(CompilerInput & input, CompilerOutput & output);
	bool isShaderBinaryFileOutDatad(std::string filename);
	bool saveShaderAndMetaData(const std::string& shader_filename, const CompilerOutput& output);
}

template<>
struct enable_bitmask_operators<qyhs::shadercompiler::Flags> {
	static const bool enable = true;
};