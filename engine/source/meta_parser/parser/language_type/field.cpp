#include "field.h"
#include "class.h"
#include "utils.h"
Field::Field(const Cursor& cursor, const Namespace& current_namespace, Class* parent /*= nullptr*/) 
					:TypeInfo(cursor, current_namespace), m_parent(parent), 
					 m_type(Utils::getTypeNameWithoutNamespace(cursor.getType()))
{
	m_name = cursor.getSpelling();
	m_display_name = Utils::getNameWithoutFirstM(m_name);
}

bool Field::shouldCompile()
{
	return isAccessible();
}

bool Field::isAccessible()
{
	return ((m_parent->m_meta_data.hasFlag(NativeProperty::Fields) ||
		m_parent->m_meta_data.hasFlag(NativeProperty::All)) &&
		!m_meta_data.hasFlag(NativeProperty::Disable)) ||
		(m_parent->m_meta_data.hasFlag(NativeProperty::WhiteListFields) &&
			m_meta_data.hasFlag(NativeProperty::Enable));
}
