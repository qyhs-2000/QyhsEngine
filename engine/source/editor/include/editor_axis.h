#pragma once
#include "function/render/render_entity.h"
#include "function/render/render_type.h"

namespace  QYHS
{
    class EditorTranslateAxis:public RenderEntity
    {
    public:
        EditorTranslateAxis();
        RenderMeshData m_mesh_data;
    private:
    };

    class EditorRotateAxis:public RenderEntity
    {
    public:
        EditorRotateAxis();
        RenderMeshData m_mesh_data;
    private:
    };

    class EditorScaleAxis:public RenderEntity
    {
    public:
        EditorScaleAxis();
        RenderMeshData m_mesh_data;
    private:
    };
}
