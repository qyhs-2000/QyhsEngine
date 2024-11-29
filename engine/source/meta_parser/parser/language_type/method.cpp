#include "common/precompile.h"

#include "class.h"
#include "method.h"

Method::Method(const Cursor& cursor, const Namespace& current_namespace, Class* parent) :
    TypeInfo(cursor, current_namespace), m_parent(parent), m_name(cursor.getSpelling())
{}

bool Method::shouldCompile(void) const { return isAccessible(); }

bool Method::isAccessible(void) const
{
    return ((m_parent->m_meta_data.hasFlag(NativeProperty::Methods) ||
        m_parent->m_meta_data.hasFlag(NativeProperty::All)) &&
        !m_meta_data.hasFlag(NativeProperty::Disable)) ||
        (m_parent->m_meta_data.hasFlag(NativeProperty::WhiteListFields) &&
            m_meta_data.hasFlag(NativeProperty::Enable));
}
