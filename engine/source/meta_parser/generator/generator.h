#pragma once
#include "common/precompile.h"
#include "common/schema_module.h"
namespace Generator
{
	class GeneratorInterface
	{
	public:
		GeneratorInterface(std::string		out_path,
			std::string      root_path,
			std::function<std::string(std::string)> get_inlcude_func) :
			m_out_path(out_path), m_root_path(root_path), m_get_include_func(get_inlcude_func) {}
		virtual int  generate(std::string path, SchemaModule schema) = 0;
		virtual void finish() {};
		virtual ~GeneratorInterface() {};
		virtual void prepareStatus(std::string& path);
		virtual void genClassRenderData(std::shared_ptr<Class> class_temp, Mustache::data& class_def);
		virtual void genFieldRenderData(std::shared_ptr<Class> class_temp, Mustache::data& field_defines);
		virtual std::string processFileName(std::string path) = 0;
	protected:
		std::string m_out_path;
		std::string m_root_path;
		std::function<std::string(std::string)> m_get_include_func;
	};

}