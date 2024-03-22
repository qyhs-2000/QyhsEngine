#include "type_info.h"

TypeInfo::TypeInfo(const Cursor& cursor, const Namespace& current_namespace):m_meta_data(cursor),m_root_cursor(cursor),
					m_enabled(m_meta_data.hasFlag(NativeProperty::Enable)),m_namespace(current_namespace)
{
	
}

std::string TypeInfo::getSourceFile(void) const
{
	return m_root_cursor.getSourceFile();
}
