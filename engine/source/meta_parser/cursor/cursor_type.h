#pragma once

class Cursor;
class CursorType
{
public:
	CursorType(const CXType& handle);
	std::string getDisplayName(void)const;
	bool IsConst(void) const;
private:
	CXType m_handle;
};