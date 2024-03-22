#include "common/precompile.h"

#include "cursor_type.h"
#include "utils.h"

CursorType::CursorType(const CXType& type)
{
	m_handle = type;
}

std::string CursorType::getDisplayName()const
{
	std::string display_name;
	Utils::toString(clang_getTypeSpelling(m_handle),display_name);
	return display_name;
}
