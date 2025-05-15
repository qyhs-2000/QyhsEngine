#include "archive.h"
#include "core/helper.h"
namespace qyhs
{
	Archive::Archive(const std::string& file_name,bool read_mode):read_mode(read_mode)
	{
		if (!file_name.empty())
		{
			directory = helper::getDirectoryFromPath(file_name);
			if (read_mode)
			{

			}
			else
			{
				createEmpty();
			}
		}
	}

	void Archive::createEmpty()
	{
		DATA.resize(128);
		data_ptr = DATA.data();
		data_size = DATA.size();
		pos = 0;
	}


}