#pragma once
#include "generator.h"
#include "common/precompile.h"
namespace Generator
{
	class ReflectionGenerator :public GeneratorInterface
	{
	public:
		ReflectionGenerator() = delete;
		ReflectionGenerator(std::string source_directory, std::function<std::string(std::string)> get_include_function);
		virtual int generate(std::string path, SchemaModule schema) override;
		virtual void prepareStatus(std::string& path) override;
		virtual void finish() override;
	private:
	};
}