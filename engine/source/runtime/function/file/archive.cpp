#include "archive.h"
#include "core/helper.h"
namespace qyhs
{
	Archive::Archive(const std::string& file_name,bool read_mode):read_mode(read_mode),filename(file_name)
	{
		if (!file_name.empty())
		{
			directory = helper::getDirectoryFromPath(file_name);
			if (read_mode)
			{
				if (helper::readFile(file_name, DATA))
				{
					data_ptr = DATA.data();
					data_size = DATA.size();
					setReadModeAndResetPos(true);
				}
			}
			else
			{
				createEmpty();
			}
		}
	}

	void Archive::close()
	{
		if (!read_mode && !filename.empty())
		{
			saveFile(filename);
		}
		DATA.clear();
	}

	bool Archive::saveFile(const std::string& filename)
	{
		return helper::writeFile(filename, data_ptr, pos);
	}

	void Archive::mapVector(const uint8_t*& data, size_t& data_size)
	{
		(*this) >> data_size;
		data = data_ptr + pos;
		pos += data_size;
	}

	void Archive::setReadModeAndResetPos(bool is_read_mode)
	{
		read_mode = is_read_mode;
		pos = 0;
	}

	size_t Archive::writeUnKnownJumpPosition()
	{
		size_t pre_pos = pos;
		_write((uint64_t)pos);
		return pre_pos;
	}

	void Archive::patchUnKnownJumpPosition(size_t offset)
	{
		assert(offset + sizeof(size_t) < data_size);
		assert(!read_mode);
		assert(!DATA.empty());
		*(size_t*)(DATA.data() + offset) = (size_t)(pos);
	}

	void Archive::createEmpty()
	{
		DATA.resize(128);
		data_ptr = DATA.data();
		data_size = DATA.size();
		pos = 0;
	}


}