#include "shader_compiler.h"
#include "core/helper.h"
#include <iostream>
#ifdef PLATFORM_DESKTOP_WINDOWS
#define SHADERCOMPILER_ENABLED
#define SHADERCOMPILER_ENABLED_DXCOMPILER
#define SHADERCOMPILER_ENABLED_D3DCOMPILER
#include <wrl/client.h>
#define CComPtr Microsoft::WRL::ComPtr
#endif // _WIN32
#include <cassert>
#include <function/file/archive.h>
namespace qyhs::shadercompiler
{
	void compile(CompilerInput& input, CompilerOutput& output)
	{
		switch (input.format)
		{
		default:
			break;
		case(graphics::ShaderFormat::HLSL6):
		case(graphics::ShaderFormat::SPIRV):
			compileDXCompiler(input, output);
			break;
		}
	}

	Internal_DXCompiler& dx_compiler()
	{
		static Internal_DXCompiler internal_state;
		return internal_state;
	}

	void compileDXCompiler(CompilerInput& input, CompilerOutput& output)
	{
		Internal_DXCompiler& internal_compiler = dx_compiler();
		if (internal_compiler.dxcCreateInstance == nullptr)
		{
			return;
		}
		CComPtr<IDxcUtils> dxcUtils;
		CComPtr<IDxcCompiler3> dxcCompiler;
		HRESULT hr = internal_compiler.dxcCreateInstance(CLSID_DxcUtils, IID_PPV_ARGS(&dxcUtils));
		assert(SUCCEEDED(hr));
		hr = internal_compiler.dxcCreateInstance(CLSID_DxcCompiler, IID_PPV_ARGS(&dxcCompiler));
		assert(SUCCEEDED(hr));

		if (dxcCompiler == nullptr)
		{
			return;
		}

		std::vector<uint8_t> shader_source_data;
		if (!helper::readFile(input.shader_source_filename, shader_source_data))
		{
			return;
		}
		std::vector<std::wstring> args = {
			L"-res-may-alias",
			//L"-flegacy-macro-expansion",
			//L"-no-legacy-cbuf-layout",
			//L"-pack-optimized", // this has problem with tessellation shaders: https://github.com/microsoft/DirectXShaderCompiler/issues/3362
			//L"-all-resources-bound",
			//L"-Gis", // Force IEEE strictness
			//L"-Gec", // Enable backward compatibility mode
			//L"-Ges", // Enable strict mode
			//L"-O0", // Optimization Level 0
			//L"-enable-16bit-types",
			L"-Wno-conversion",
		};

		if (has_flag(input.flags, Flags::DISABLE_OPTIMIZATION))
		{
			args.push_back(L"-Od");
		}

		switch (input.format)
		{
		case graphics::ShaderFormat::HLSL6:
			args.push_back(L"-rootsig-define"); args.push_back(L"WICKED_ENGINE_DEFAULT_ROOTSIGNATURE");
			if (has_flag(input.flags, Flags::STRIP_REFLECTION))
			{
				args.push_back(L"-Qstrip_reflect"); // only valid in HLSL6 compiler
			}
			break;
		case graphics::ShaderFormat::SPIRV:
			args.push_back(L"-spirv");
			args.push_back(L"-fspv-target-env=vulkan1.2");
			//args.push_back(L"-fspv-target-env=vulkan1.3"); // this has some problem with RenderDoc AMD disassembly so it's not enabled for now
			args.push_back(L"-fvk-use-dx-layout");
			args.push_back(L"-fvk-use-dx-position-w");
			//args.push_back(L"-fvk-b-shift"); args.push_back(L"0"); args.push_back(L"0");
			args.push_back(L"-fvk-t-shift"); args.push_back(L"1000"); args.push_back(L"0");
			args.push_back(L"-fvk-u-shift"); args.push_back(L"2000"); args.push_back(L"0");
			args.push_back(L"-fvk-s-shift"); args.push_back(L"3000"); args.push_back(L"0");
			break;
		default:
			assert(0);
			return;
		}

		graphics::ShaderModel minshadermodel = input.minshadermodel;

		if (input.format == graphics::ShaderFormat::HLSL6_XS)
		{
			minshadermodel = graphics::ShaderModel::SM_6_6;
		}

		args.push_back(L"-T");
		switch (input.stage)
		{
		case graphics::ShaderStage::MESH_SHADER:
			switch (minshadermodel)
			{
			default:
				args.push_back(L"ms_6_5");
				break;
			case graphics::ShaderModel::SM_6_6:
				args.push_back(L"ms_6_6");
				break;
			case graphics::ShaderModel::SM_6_7:
				args.push_back(L"ms_6_7");
				break;
			}
			break;
		case graphics::ShaderStage::AMPLIFICATION_SHADER:
			switch (minshadermodel)
			{
			default:
				args.push_back(L"as_6_5");
				break;
			case graphics::ShaderModel::SM_6_6:
				args.push_back(L"as_6_6");
				break;
			case graphics::ShaderModel::SM_6_7:
				args.push_back(L"as_6_7");
				break;
			}
			break;
		case graphics::ShaderStage::VERTEX_SHADER:
			switch (minshadermodel)
			{
			default:
				args.push_back(L"vs_6_0");
				break;
			case graphics::ShaderModel::SM_6_1:
				args.push_back(L"vs_6_1");
				break;
			case graphics::ShaderModel::SM_6_2:
				args.push_back(L"vs_6_2");
				break;
			case graphics::ShaderModel::SM_6_3:
				args.push_back(L"vs_6_3");
				break;
			case graphics::ShaderModel::SM_6_4:
				args.push_back(L"vs_6_4");
				break;
			case graphics::ShaderModel::SM_6_5:
				args.push_back(L"vs_6_5");
				break;
			case graphics::ShaderModel::SM_6_6:
				args.push_back(L"vs_6_6");
				break;
			case graphics::ShaderModel::SM_6_7:
				args.push_back(L"vs_6_7");
				break;
			}
			break;
		case graphics::ShaderStage::HULL_SHADER:
			switch (minshadermodel)
			{
			default:
				args.push_back(L"hs_6_0");
				break;
			case graphics::ShaderModel::SM_6_1:
				args.push_back(L"hs_6_1");
				break;
			case graphics::ShaderModel::SM_6_2:
				args.push_back(L"hs_6_2");
				break;
			case graphics::ShaderModel::SM_6_3:
				args.push_back(L"hs_6_3");
				break;
			case graphics::ShaderModel::SM_6_4:
				args.push_back(L"hs_6_4");
				break;
			case graphics::ShaderModel::SM_6_5:
				args.push_back(L"hs_6_5");
				break;
			case graphics::ShaderModel::SM_6_6:
				args.push_back(L"hs_6_6");
				break;
			case graphics::ShaderModel::SM_6_7:
				args.push_back(L"hs_6_7");
				break;
			}
			break;
		case graphics::ShaderStage::DOMAIN_SHADER:
			switch (minshadermodel)
			{
			default:
				args.push_back(L"ds_6_0");
				break;
			case graphics::ShaderModel::SM_6_1:
				args.push_back(L"ds_6_1");
				break;
			case graphics::ShaderModel::SM_6_2:
				args.push_back(L"ds_6_2");
				break;
			case graphics::ShaderModel::SM_6_3:
				args.push_back(L"ds_6_3");
				break;
			case graphics::ShaderModel::SM_6_4:
				args.push_back(L"ds_6_4");
				break;
			case graphics::ShaderModel::SM_6_5:
				args.push_back(L"ds_6_5");
				break;
			case graphics::ShaderModel::SM_6_6:
				args.push_back(L"ds_6_6");
				break;
			case graphics::ShaderModel::SM_6_7:
				args.push_back(L"ds_6_7");
				break;
			}
			break;
		case graphics::ShaderStage::GEOMETRY_SHADER:
			switch (minshadermodel)
			{
			default:
				args.push_back(L"gs_6_0");
				break;
			case graphics::ShaderModel::SM_6_1:
				args.push_back(L"gs_6_1");
				break;
			case graphics::ShaderModel::SM_6_2:
				args.push_back(L"gs_6_2");
				break;
			case graphics::ShaderModel::SM_6_3:
				args.push_back(L"gs_6_3");
				break;
			case graphics::ShaderModel::SM_6_4:
				args.push_back(L"gs_6_4");
				break;
			case graphics::ShaderModel::SM_6_5:
				args.push_back(L"gs_6_5");
				break;
			case graphics::ShaderModel::SM_6_6:
				args.push_back(L"gs_6_6");
				break;
			case graphics::ShaderModel::SM_6_7:
				args.push_back(L"gs_6_7");
				break;
			}
			break;
		case graphics::ShaderStage::PIXEL_SHADER:
			switch (minshadermodel)
			{
			default:
				args.push_back(L"ps_6_0");
				break;
			case graphics::ShaderModel::SM_6_1:
				args.push_back(L"ps_6_1");
				break;
			case graphics::ShaderModel::SM_6_2:
				args.push_back(L"ps_6_2");
				break;
			case graphics::ShaderModel::SM_6_3:
				args.push_back(L"ps_6_3");
				break;
			case graphics::ShaderModel::SM_6_4:
				args.push_back(L"ps_6_4");
				break;
			case graphics::ShaderModel::SM_6_5:
				args.push_back(L"ps_6_5");
				break;
			case graphics::ShaderModel::SM_6_6:
				args.push_back(L"ps_6_6");
				break;
			case graphics::ShaderModel::SM_6_7:
				args.push_back(L"ps_6_7");
				break;
			}
			break;
		case graphics::ShaderStage::COMPUTE_SHADER:
			switch (minshadermodel)
			{
			default:
				args.push_back(L"cs_6_0");
				break;
			case graphics::ShaderModel::SM_6_1:
				args.push_back(L"cs_6_1");
				break;
			case graphics::ShaderModel::SM_6_2:
				args.push_back(L"cs_6_2");
				break;
			case graphics::ShaderModel::SM_6_3:
				args.push_back(L"cs_6_3");
				break;
			case graphics::ShaderModel::SM_6_4:
				args.push_back(L"cs_6_4");
				break;
			case graphics::ShaderModel::SM_6_5:
				args.push_back(L"cs_6_5");
				break;
			case graphics::ShaderModel::SM_6_6:
				args.push_back(L"cs_6_6");
				break;
			case graphics::ShaderModel::SM_6_7:
				args.push_back(L"cs_6_7");
				break;
			}
			break;
		case graphics::ShaderStage::LIBRARY:
			switch (minshadermodel)
			{
			default:
				args.push_back(L"lib_6_5");
				break;
			case graphics::ShaderModel::SM_6_6:
				args.push_back(L"lib_6_6");
				break;
			case graphics::ShaderModel::SM_6_7:
				args.push_back(L"lib_6_7");
				break;
			}
			break;
		default:
			assert(0);
			return;
		}

		for (auto& x : input.defines)
		{
			args.push_back(L"-D");
			helper::stringConvert(x, args.emplace_back());
		}

		for (auto& x : input.include_directories)
		{
			args.push_back(L"-I");
			helper::stringConvert(x, args.emplace_back());
		}
		// Entry point parameter:
		std::wstring wentry;
		qyhs::helper::stringConvert(input.entrypoint, wentry);
		args.push_back(L"-E");
		args.push_back(wentry.c_str());

		// Add source file name as last parameter. This will be displayed in error messages
		std::wstring wsource;
		helper::stringConvert(helper::getFileNameFromPath(input.shader_source_filename), wsource);
		args.push_back(wsource.c_str());

		DxcBuffer Source;
		Source.Ptr = shader_source_data.data();
		Source.Size = shader_source_data.size();
		Source.Encoding = DXC_CP_ACP;

		struct IncludeHandler :public IDxcIncludeHandler
		{
			const CompilerInput* input = nullptr;
			CompilerOutput* output = nullptr;
			CComPtr<IDxcIncludeHandler> dxcIncludeHandler;
			HRESULT STDMETHODCALLTYPE LoadSource(
				_In_z_ LPCWSTR pFilename,                                 // Candidate filename.
				_COM_Outptr_result_maybenull_ IDxcBlob** ppIncludeSource  // Resultant source object for included file, nullptr if not found.
			) override
			{
				HRESULT hres = dxcIncludeHandler->LoadSource(pFilename, ppIncludeSource);
				if (SUCCEEDED(hres))
				{
					std::string& include_file_name = output->dependencies.emplace_back();
					helper::stringConvert(pFilename, include_file_name);
				}
				return hres;
			}
			HRESULT STDMETHODCALLTYPE QueryInterface(
				/* [in] */ REFIID riid,
				/* [iid_is][out] */ _COM_Outptr_ void __RPC_FAR* __RPC_FAR* ppvObject) override
			{
				return dxcIncludeHandler->QueryInterface(riid, ppvObject);
			}

			ULONG STDMETHODCALLTYPE AddRef(void) override
			{
				return 0;
			}
			ULONG STDMETHODCALLTYPE Release(void) override
			{
				return 0;
			}
		}includehandler;
		includehandler.input = &input;
		includehandler.output = &output;

		hr = dxcUtils->CreateDefaultIncludeHandler(&includehandler.dxcIncludeHandler);
		assert(SUCCEEDED(hr));

		std::vector<const wchar_t*> args_raw;
		args_raw.reserve(args.size());
		for (auto& x : args)
		{
			args_raw.push_back(x.c_str());
		}

		CComPtr<IDxcResult> pResults;
		hr = dxcCompiler->Compile(
			&Source,						// Source buffer.
			args_raw.data(),			// Array of pointers to arguments.
			(uint32_t)args.size(),		// Number of arguments.
			&includehandler,		// User-provided interface to handle #include directives (optional).
			IID_PPV_ARGS(&pResults)	// Compiler output status, buffer, and errors.
		);
		assert(SUCCEEDED(hr));

		CComPtr<IDxcBlobUtf8> pErrors = nullptr;
		hr = pResults->GetOutput(DXC_OUT_ERRORS, IID_PPV_ARGS(&pErrors), nullptr);
		assert(SUCCEEDED(hr));
		if (pErrors != nullptr && pErrors->GetStringLength() != 0)
		{
			output.error_message = pErrors->GetStringPointer();
			std::cout << output.error_message << std::endl;
			assert(0);
		}

		HRESULT hrStatus;
		hr = pResults->GetStatus(&hrStatus);
		assert(SUCCEEDED(hr));
		if (FAILED(hrStatus))
		{
			return;
		}

		CComPtr<IDxcBlob> pShader = nullptr;
		hr = pResults->GetOutput(DXC_OUT_OBJECT, IID_PPV_ARGS(&pShader), nullptr);
		assert(SUCCEEDED(hr));
		if (pShader != nullptr)
		{
			output.dependencies.push_back(input.shader_source_filename);
			output.shader_data = (const uint8_t*)pShader->GetBufferPointer();
			output.shader_size = pShader->GetBufferSize();

			// keep the blob alive == keep shader pointer valid!
			auto internal_state = std::make_shared<CComPtr<IDxcBlob>>();
			*internal_state = pShader;
			output.internal_state = internal_state;
		}

		if (input.format == graphics::ShaderFormat::HLSL6)
		{
			CComPtr<IDxcBlob> pHash = nullptr;
			hr = pResults->GetOutput(DXC_OUT_SHADER_HASH, IID_PPV_ARGS(&pHash), nullptr);
			assert(SUCCEEDED(hr));
			if (pHash != nullptr)
			{
				DxcShaderHash* pHashBuf = (DxcShaderHash*)pHash->GetBufferPointer();
				for (int i = 0; i < _countof(pHashBuf->HashDigest); i++)
				{
					output.shader_hash.push_back(pHashBuf->HashDigest[i]);
				}
			}
		}

	}

	bool isShaderBinaryFileOutDatad(std::string filename)
	{
		if (!helper::existFile(filename))
		{
			return true;
		}

	}

	const char* meta_data_extension = "meta";
	bool saveShaderAndMetaData(const std::string& shader_filename, const qyhs::shadercompiler::CompilerOutput& output)
	{
		qyhs::helper::createDirectory(qyhs::helper::getDirectoryFromPath(shader_filename));
		qyhs::Archive dependency_library(qyhs::helper::replaceExtension(shader_filename, meta_data_extension));
		if (dependency_library.isOpen())
		{
			std::string root_dir = dependency_library.getSourceDir();
			std::vector<std::string> dependencies = output.dependencies;
			for (auto& x : dependencies)
			{
				helper::makeRelativePath(root_dir, x);
			}
			dependency_library << dependencies;
		}
		if (helper::writeFile(shader_filename, output.shader_data, output.shader_size))
		{
			return true;
		}
		return false;
	}
	
	Internal_DXCompiler::Internal_DXCompiler(const std::string& modifier)
	{



		const std::string library = "dxcompiler" + modifier + ".dll";
		HMODULE dxcompiler = LoadLibrary(library.c_str());
		if (dxcompiler != nullptr)
		{
			dxcCreateInstance = (DxcCreateInstanceProc)myGetProcAddress(dxcompiler, "DxcCreateInstance");
			if (dxcCreateInstance != nullptr)
			{
				CComPtr<IDxcCompiler3> dxcCompiler;
				HRESULT hr = dxcCreateInstance(CLSID_DxcCompiler, IID_PPV_ARGS(&dxcCompiler));
				assert(SUCCEEDED(hr));
				CComPtr<IDxcVersionInfo> info;
				hr = dxcCompiler->QueryInterface(IID_PPV_ARGS(&info));
				assert(SUCCEEDED(hr));
				uint32_t minor = 0;
				uint32_t major = 0;
				hr = info->GetVersion(&major, &minor);
				assert(SUCCEEDED(hr));
			}
			else
			{
				assert(0);
			}
		}
	}
}