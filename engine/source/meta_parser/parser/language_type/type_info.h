#pragma once
#include "cursor/cursor.h"
#include "common/namespace.h"
#include "meta/meta_info.h"

class TypeInfo
{
public:
	TypeInfo(const Cursor& cursor, const Namespace& current_namespace);
	std::string getSourceFile(void) const;
	MetaInfo m_meta_data;
protected:
	bool m_enabled;

	Namespace m_namespace;
private:
	Cursor m_root_cursor;
};