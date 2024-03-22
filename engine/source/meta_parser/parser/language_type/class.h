#pragma once
#include "type_info.h"
#include "cursor/cursor.h"
#include "common/namespace.h"
#include "field.h"

/*基类，如果A类是B的父类，那么指向A的Cursor就是CXCursor_CXXBaseSpecifier*/
struct BaseClass
{
	std::string name;
	BaseClass(Cursor& cursor);

};

class Class:public TypeInfo
{
public:
	Class(const Cursor& cursor, const Namespace& current_namespace);
	bool shouldCompile();
	bool shouldCompileFields(void) const;
	std::string m_display_name;
	std::string m_name;
	std::string getClassName();
	template<typename T>
	using SharePtrVector = std::vector<std::shared_ptr<T>>;

	SharePtrVector<BaseClass> m_base_classes;
	SharePtrVector<Field>	  m_fields;
};