#pragma once
#include "function/framework/component/component.h"
#include "function/framework/component/hierarchy/hierarchy_component.h"
#include "function/framework/component/transform/transform_component.h"
#include "function/framework/component/mesh/mesh_component.h"
#include "core/math/transform.h"
namespace qyhs
{
    //TODO:if don't add 'class Transform',it will fail in {{class_name}}.reflection.gen.h file, i don't know the reason
    class Transform;
    REFLECTION_TYPE(TransformComponent)
        CLASS(TransformComponent : public Component, WhiteListFields)
    {
        REFLECTION_BODY(TransformComponent)
    public:
        TransformComponent() = default;
        virtual void postLoadResource(std::weak_ptr<GameObject> parent_object) override;
        bool isDirty() { return m_is_dirty; }
        void setDirty(bool is_dirty) { m_is_dirty = is_dirty; }
        void setPosition(Vector3 position);
        void setScale(Vector3 scale);
        const Vector3 getPosition() { return m_transform.m_position; }
        const Quaternion& getRotation() { return m_transform.m_rotation; }
        void setRotation(Quaternion new_quaternion) { m_transform.m_rotation = new_quaternion; }
        void matrixTransform(const Matrix4x4& matrix);
        void updateWorldMatrix();
        void updateWorldMatrix_Parented(const TransformComponent & parent_transform_comp);
        void applyTransform();
        void updateTransToWorld();
        Matrix4x4 getMatrix() { return m_transform.getMatrix(); }
        const Transform& getTransformConst() { return m_transform; }
        Matrix4x4 world_matrix;
    protected:
        META(Enable)
            Transform m_transform;            //local transform
		Vector3 scale{ 1,1,1 };               //world scale
		Vector3 translation{ 0.0,0.0,0.0 };   //world translation
    private:
        bool m_is_dirty{ true };
    };
}