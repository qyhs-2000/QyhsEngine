#pragma once
#include "cursor/cursor.h"

class MetaInfo
{
public:
	MetaInfo(const Cursor& cursor);
	bool hasFlag(const std::string& key) const;
	std::string getProperty(const std::string& key) const;
private:
	typedef std::pair<std::string, std::string> Property;

private:
	std::unordered_map<std::string, std::string> m_properties;
	std::vector<Property> extractProperties(const Cursor& cursor) const;
};