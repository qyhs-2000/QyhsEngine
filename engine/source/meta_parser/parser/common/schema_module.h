#pragma once
#include"common/precompile.h"
#include "language_type/class.h"
class SchemaModule
{
public:
	std::string m_name;
	//all classes that declared in source file
	std::vector<std::shared_ptr<Class>> classes;
private:
};