#pragma once
#include "function/framework/component/component.h"
#include "core/math/transform.h"
namespace QYHS
{
    //TODO:if don't add 'class Transform',it will fail in {{class_name}}.reflection.gen.h file, i don't know the reason
    class Transform;
    REFLECTION_TYPE(MotorComponent)
        CLASS(MotorComponent : public Component, WhiteListFields)
    {
        REFLECTION_BODY(MotorComponent)

    public:
        MotorComponent() = default;
        virtual void postLoadResource(std::weak_ptr<GameObject> parent_object) override;
        bool isDirty() { return m_is_dirty; }
        void setDirty(bool is_dirty) { m_is_dirty = is_dirty; }
        Matrix4x4 getMatrix() { return m_transform.getMatrix(); }
    protected:
        META(Enable)
            Transform m_transform;
    private:
        bool m_is_dirty{ false };
    };
}