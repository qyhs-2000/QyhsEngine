#pragma once

#include "function/render/rhi/rhi.h"

#define TINYGLTF_NO_STB_IMAGE_WRITE
#include "tinygltf/tiny_gltf.h"

#include "Texture.h"
namespace QYHS
{
	class  GLTFModel
	{
		struct Image
		{
			Texture2D texture;

		};
	public:


		void loadResourceFromFile(tinygltf::Model& model);
	public:

	private:
		void loadImage(tinygltf::Model& model);
		void loadMaterial(tinygltf::Model& model);
		void loadTexture(tinygltf::Model& model);

	private:
		std::vector<Image> images;

	};
}
