#pragma once
#include "function/framework/component/component.h"
#include "core/math/transform.h"
namespace qyhs
{
    REFLECTION_TYPE(MotorComponentRes)
        CLASS(MotorComponentRes, Fields)
    {
        REFLECTION_BODY(MotorComponentRes)
    public:
        float move_speed{ 100.f };
    private:
    };

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
        virtual void tick(double delta_time) override;
        void calculateTargetMoveDirection(const Quaternion & object_rotation);
        void calculateTargetPosition(Vector3 m_current_position,double delta_time);
        const bool isMoving() { return m_is_moving; }
        const Vector3 getTargetPosition() { return m_target_position; }
    protected:
        META(Enable)
            Transform m_transform;
        META(Enable)
            MotorComponentRes m_motor_res;
        Vector3 m_target_position;
        Vector3 m_diresed_horizontal_direction{ Vector3::ZERO };
        Vector3 m_diresed_vertical_direction{ Vector3::ZERO };
    private:
        bool m_is_dirty{ false };
        bool m_is_moving{ false };
    };
}