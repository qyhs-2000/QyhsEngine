#include "_generated\reflection\component.reflection.gen.h"
#include "_generated\reflection\vector2.reflection.gen.h"
#include "_generated\reflection\quat.reflection.gen.h"
#include "_generated\reflection\mesh_component.reflection.gen.h"
#include "_generated\reflection\vector3.reflection.gen.h"
#include "_generated\reflection\transform.reflection.gen.h"
#include "_generated\reflection\vector4.reflection.gen.h"
#include "_generated\reflection\motor_component.reflection.gen.h"
#include "_generated\reflection\transform_component.reflection.gen.h"
#include "_generated\reflection\object.reflection.gen.h"
#include "_generated\reflection\mash_data.reflection.gen.h"
#include "_generated\reflection\mesh.reflection.gen.h"
#include "_generated\reflection\material.reflection.gen.h"
#include "_generated\reflection\level_resource.reflection.gen.h"
#include "_generated\reflection\world_resource.reflection.gen.h"
#include "_generated\reflection\camera_config.reflection.gen.h"
#include "_generated\reflection\global_rendering_resource.reflection.gen.h"

namespace QYHS
{
	namespace Reflection
	{
		void TypeMetaRegister::Register()
		{
			TypeWrapperRegister_Component();
			TypeWrapperRegister_Vector2();
			TypeWrapperRegister_Quaternion();
			TypeWrapperRegister_MeshComponent();
			TypeWrapperRegister_Vector3();
			TypeWrapperRegister_Transform();
			TypeWrapperRegister_Vector4();
			TypeWrapperRegister_MotorComponent();
			TypeWrapperRegister_TransformComponent();
			TypeWrapperRegister_ObjectDefinitionResource();
			TypeWrapperRegister_ObjectInstanceResource();
			TypeWrapperRegister_MeshData();
			TypeWrapperRegister_Vertex();
			TypeWrapperRegister_MeshComponentRes();
			TypeWrapperRegister_SubMeshRes();
			TypeWrapperRegister_MaterialRes();
			TypeWrapperRegister_LevelResource();
			TypeWrapperRegister_WorldResource();
			TypeWrapperRegister_CameraConfig();
			TypeWrapperRegister_CameraPose();
			TypeWrapperRegister_GlobalRenderConfig();
			TypeWrapperRegister_SkyBoxSpecularMap();
		}
	}
}

