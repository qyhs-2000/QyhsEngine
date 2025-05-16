#include "class.h"
#include "utils.h"

BaseClass::BaseClass(Cursor& cursor) :name(Utils::getTypeNameWithoutNamespace(cursor.getType())) {}

Class::Class(const Cursor& cursor, const Namespace& current_namespace) :TypeInfo(cursor, current_namespace),
m_name(cursor.getDisplayName()), m_qualified_name(Utils::getTypeNameWithoutNamespace(cursor.getType()))

{
	m_display_name = Utils::getNameWithoutFirstM(m_qualified_name);
	Utils::replaceAll(m_name, " ", "");
	Utils::replaceAll(m_name, "qyhs::", "");

	for (auto& child : cursor.getChildren())
	{
		switch (child.getKind())
		{
		case CXCursor_CXXBaseSpecifier:
		{
			auto base_class = new BaseClass(child);
			m_base_classes.emplace_back(base_class);
			break;
		}
		case CXCursor_FieldDecl:
		{
			m_fields.emplace_back(new Field(child, current_namespace, this));
			break;
		}
		default:
			break;
		}
	}
}

bool Class::shouldCompile()
{
	return shouldCompileFields();
}

bool Class::shouldCompileFields(void) const
{
	return (m_meta_data.hasFlag(NativeProperty::All) || m_meta_data.hasFlag(NativeProperty::Fields) || m_meta_data.hasFlag(NativeProperty::WhiteListFields));
}

std::string Class::getClassName()
{
	return m_name;
}

bool Class::isAccessible(void) const
{
	return m_enabled;
}


