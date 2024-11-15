#pragma once
#include "function/framework/component/component.h"
#include "core/math/transform.h"
namespace QYHS
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
        const Vector3 getPosition() { return m_transform.m_position; }
        const Quaternion& getRotation() { return m_transform.m_rotation; }
        void setRotation(Quaternion new_quaternion) { m_transform.m_rotation = new_quaternion; }
        Matrix4x4 getMatrix() { return m_transform.getMatrix(); }
        const Transform& getTransformConst() { return m_transform; }
    protected:
        META(Enable)
            Transform m_transform;
    private:
        bool m_is_dirty{ false };
    };
}