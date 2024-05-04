#include "cursor.h"
#include "utils.h"

Cursor::Cursor(const CXCursor& cursor) :m_handle(cursor)
{

}

Cursor::List Cursor::getChildren()const
{
	List children;
	auto visitor = [](CXCursor cursor, CXCursor parent, CXClientData data) {
		auto container = static_cast<List*>(data);
		container->emplace_back(cursor);
		if (cursor.kind == CXCursor_LastPreprocessing)
		{
			return CXChildVisit_Break;
		}
		return CXChildVisit_Continue;
	};
	clang_visitChildren(m_handle, visitor, &children);
	return children;
}

std::string Cursor::getSpelling(void) const
{
	std::string spelling;

	Utils::toString(clang_getCursorSpelling(m_handle), spelling);

	return spelling;
}

CursorType Cursor::getType() const
{
	return clang_getCursorType(m_handle);
}

std::string Cursor::getSourceFile(void) const
{
	auto range = clang_Cursor_getSpellingNameRange(m_handle, 0, 0);
	auto start = clang_getRangeStart(range);

	CXFile   file;
	unsigned line, column, offset;

	clang_getFileLocation(start, &file, &line, &column, &offset);
	std::string filename;

	Utils::toString(clang_getFileName(file), filename);

	return filename;
}

std::string Cursor::getDisplayName(void) const
{
	std::string display_name;
	auto name = clang_getCursorDisplayName(m_handle);
	Utils::toString(name, display_name);
	return display_name;
}
