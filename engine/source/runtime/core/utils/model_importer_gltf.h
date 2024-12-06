#pragma once
#include <string>
#include <memory>
#include <unordered_map>
namespace QYHS
{
	
	class Level;
	
	bool import_model_gltf(std::shared_ptr<QYHS::Level> level,std::string file_path);
}