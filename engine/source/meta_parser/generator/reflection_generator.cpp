#include "reflection_generator.h"

Generator::ReflectionGenerator::ReflectionGenerator(std::string source_directory, std::function<std::string(std::string)> get_include_function):
	Generator::GeneratorInterface(source_directory+"_generated/reflection",source_directory,get_include_function )
{

}

int Generator::ReflectionGenerator::generate(std::string path, SchemaModule schema)
{
	return 0;
}

void Generator::ReflectionGenerator::prepareStatus(std::string& path)
{

}

void Generator::ReflectionGenerator::finish()
{

}
