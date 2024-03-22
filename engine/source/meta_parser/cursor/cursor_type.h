#pragma once


class CursorType
{
public:
	CursorType(const CXType& type);
	std::string getDisplayName()const;
private:
	CXType m_handle;
};