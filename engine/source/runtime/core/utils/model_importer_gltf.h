#pragma once
#include <string>
#include <memory>
#include <unordered_map>

namespace qyhs
{
	namespace scene
	{
		class Scene;
	}
	class Level;
	bool import_model_gltf(std::shared_ptr<qyhs::Level> level,std::string file_path);
	bool import_model_gltf(scene::Scene* scene, std::string file_path);
}