#include "field.h"
#include "class.h"
#include "utils.h"
Field::Field(const Cursor& cursor, const Namespace& current_namespace, Class* parent /*= nullptr*/) 
					:TypeInfo(cursor, current_namespace)
{
	m_is_const = cursor.getType().IsConst();
	m_parent = parent;
	m_name = cursor.getSpelling();
	m_display_name = Utils::getNameWithoutFirstM(m_name);
	m_type = Utils::getTypeNameWithoutNamespace(cursor.getType());
	Utils::replaceAll(m_type, " ", "");
	Utils::replaceAll(m_type, "QYHS::", "");

	auto ret_string = Utils::getStringWithoutQuot(m_meta_data.getProperty("default"));
	m_default = ret_string;
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
