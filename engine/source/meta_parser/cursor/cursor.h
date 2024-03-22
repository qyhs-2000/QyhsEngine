#pragma once
#include "common/precompile.h"
#include "cursor_type.h"
//CXCursor can navigate to the variable or function in the analysis of source code that has been parsed by clang
class Cursor
{
public:
	typedef std::vector<Cursor> List;
	Cursor() = default;
	Cursor(const CXCursor& cursor) ;
	List getChildren()const;
	CursorType getType()const;
	CXCursorKind getKind() { return m_handle.kind; }
	std::string getSpelling(void) const;
	std::string getSourceFile(void)const;
	bool isDefinition() { return clang_isCursorDefinition(m_handle); }
	std::string getDisplayName(void) const;
private:
	CXCursor m_handle;
};