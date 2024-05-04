#include "model.h"
#define TINYGLTF_IMPLEMENTATION
#include "tinygltf/tiny_gltf.h"
namespace QYHS
{

	void GLTFModel::loadImage(tinygltf::Model& model)
	{
		images.resize(model.images.size());
		unsigned int buffer_size;
		unsigned char* buffer;
		bool delete_buffer = false;
		for (size_t i = 0; i < model.images.size(); ++i)
		{
			tinygltf::Image& image = model.images[i];
			if (image.component == 3)
			{
				buffer_size = image.height * image.width * 4;
				buffer = new unsigned char[buffer_size];
				unsigned char* rgba = buffer;
				unsigned char* rgb = &image.image[0];
				for (size_t i = 0; i < image.width * image.height; ++i)
				{
					memcpy(rgba, rgb, sizeof(unsigned char) * 3);
					rgba += 4;
					rgb += 3;
				}
				delete_buffer = true;
			}
			else
			{
				buffer = &image.image[0];
				buffer_size = static_cast<unsigned int>(image.image.size());
			}
			images[i].texture.fromBuffer(buffer, buffer_size, VK_FORMAT_R8G8B8_UNORM, image.width, image.height);

			if (delete_buffer)
			{
				delete []buffer;
			}
		}
	}

	void GLTFModel::loadMaterial(tinygltf::Model& model)
	{
	}

	void GLTFModel::loadTexture(tinygltf::Model& model)
	{
	}

	void GLTFModel::loadResourceFromFile(tinygltf::Model& model)
	{
		loadImage(model);
		loadMaterial(model);
		loadTexture(model);
	}
}
