#pragma once
#include <string>
#include <vector>

#include "core/math/directx_math.h"
#include "function/render/color.h"
namespace qyhs
{
	class Archive
	{
	public:
		Archive(const std::string& file_name,bool read_mode = true);
		~Archive() { close(); }
		bool isOpen()const { return data_ptr != nullptr; }
		bool isReadMode()const { return read_mode == true; }
		const uint8_t* data_ptr = nullptr;
		void close();
		bool saveFile(const std::string& filename);
		std::vector<uint8_t> DATA;
		size_t data_size = 0;
		uint64_t pos = 0;
		std::string getSourceDir()const { return directory; }
		std::string getSourceFileName()const { return filename; }
		void mapVector(const uint8_t*& data, size_t& data_size);
		void setReadModeAndResetPos(bool is_read_mode);
		size_t writeUnKnownJumpPosition();
		uint64_t getPos()const { return pos; }
		void jump(uint64_t jump_pos)
		{
			assert(jump_pos <= data_size);
			pos = jump_pos;
		}
		void patchUnKnownJumpPosition(size_t offset);
		bool read_mode = true;
		std::string filename;
		std::string directory;

		template<typename T>
		void _write(const T & data)
		{
			size_t _right = pos + sizeof(data);
			if (_right > data_size)
			{
				DATA.resize(_right * 2);
				data_ptr = DATA.data();
				data_size = DATA.size();
			}
			*(T*)(DATA.data() + pos) = data;
			pos = _right;
		}

		template<typename T>
		void _read(T& data)
		{
			assert(read_mode);
			assert(data_ptr != nullptr);
			assert(pos < data_size);
			data = *(const T*)(DATA.data() + pos);
			pos += (size_t)(sizeof(data));
		}

		//read operations
		inline Archive& operator>>(bool & data)
		{
			assert(isReadMode());
			uint32_t tmp;
			_read(tmp);
			data = (tmp == 1);
			return *this;
		}

		inline Archive& operator>>(char& data)
		{
			assert(isReadMode());
			int8_t temp;
			_read(temp);
			data = (char)temp;
			return *this;
		}

		inline Archive& operator>>(std::string& data)
		{
			assert(isReadMode());
			uint64_t len;
			(*this) >> len;
			data.resize(len);
			for (size_t i = 0; i < len; ++i)
			{
				(*this) >> data[i];
			}
			return *this;
		}

		inline Archive& operator>>(unsigned long long& data)
		{
			assert(isReadMode());
			uint64_t tmp;
			_read(tmp);
			data = (unsigned long long)tmp;
			return *this;
		}

		inline Archive& operator>>(unsigned char& data)
		{
			uint8_t temp;
			_read(temp);
			data = (unsigned char)temp;
			return *this;
		}

		inline Archive& operator>>(unsigned int& data)
		{
			assert(isReadMode());
			uint64_t tmp;
			_read(tmp);
			data = (unsigned int)tmp;
			return *this;
		}

		inline Archive& operator>>(XMFLOAT2& data)
		{
			_read(data);
			return *this;
		}

		inline Archive& operator>>(XMFLOAT3& data)
		{
			_read(data);
			return *this;
		}

		inline Archive& operator>>(XMFLOAT4& data)
		{
			_read(data);
			return *this;
		}

		inline Archive& operator>>(XMUINT2& data)
		{
			_read(data);
			return *this;
		}

		inline Archive& operator>>(XMUINT3& data)
		{
			_read(data);
			return *this;
		}

		inline Archive& operator>>(XMUINT4& data)
		{
			_read(data);
			return *this;
		}

		template<typename T>
		inline Archive& operator >> (std::vector<T>& data)
		{
			size_t count;
			(*this) >> count;
			data.resize(count);
			for (int i = 0; i < count; ++i)
			{
				(*this) >> data[i];
			}
			return *this;
		}

		// Write operations
		inline Archive& operator<<(bool data)
		{
			assert(!isReadMode());
			_write((uint32_t)(data ? 1 : 0));
			return *this;
		}
		inline Archive& operator<<(char data)
		{
			assert(!isReadMode());
			_write((int8_t)data);
			return *this;
		}
		inline Archive& operator<<(short data)
		{
			assert(!isReadMode());
			_write((int16_t)data);
			return *this;
		}
		inline Archive& operator<<(unsigned char data)
		{
			assert(!isReadMode());
			_write((uint8_t)data);
			return *this;
		}
		inline Archive& operator<<(unsigned short data)
		{
			assert(!isReadMode());
			_write((uint16_t)data);
			return *this;
		}
		inline Archive& operator<<(int data)
		{
			assert(!isReadMode());
			_write((int64_t)data);
			return *this;
		}
		inline Archive& operator<<(unsigned int data)
		{
			assert(!isReadMode());
			_write((uint64_t)data);
			return *this;
		}
		inline Archive& operator<<(long data)
		{
			assert(!isReadMode());
			_write((int64_t)data);
			return *this;
		}
		inline Archive& operator<<(unsigned long data)
		{
			assert(!isReadMode());
			_write((uint64_t)data);
			return *this;
		}
		inline Archive& operator<<(long long data)
		{
			assert(!isReadMode());
			_write((int64_t)data);
			return *this;
		}
		inline Archive& operator<<(unsigned long long data)
		{
			assert(!isReadMode());
			_write((uint64_t)data);
			return *this;
		}
		inline Archive& operator<<(float data)
		{
			assert(!isReadMode());
			_write(data);
			return *this;
		}
		inline Archive& operator<<(double data)
		{
			assert(!isReadMode());
			_write(data);
			return *this;
		}
		inline Archive& operator<<(const XMFLOAT2& data)
		{
			assert(!isReadMode());
			_write(data);
			return *this;
		}
		inline Archive& operator<<(const XMFLOAT3& data)
		{
			assert(!isReadMode());
			_write(data);
			return *this;
		}
		inline Archive& operator<<(const XMFLOAT4& data)
		{
			assert(!isReadMode());
			_write(data);
			return *this;
		}
		inline Archive& operator<<(const XMFLOAT3X3& data)
		{
			assert(!isReadMode());
			_write(data);
			return *this;
		}
		inline Archive& operator<<(const XMFLOAT4X3& data)
		{
			assert(!isReadMode());
			_write(data);
			return *this;
		}
		inline Archive& operator<<(const XMFLOAT4X4& data)
		{
			assert(!isReadMode());
			_write(data);
			return *this;
		}
		inline Archive& operator<<(const XMUINT2& data)
		{
			assert(!isReadMode());
			_write(data);
			return *this;
		}
		inline Archive& operator<<(const XMUINT3& data)
		{
			assert(!isReadMode());
			_write(data);
			return *this;
		}
		inline Archive& operator<<(const XMUINT4& data)
		{
			assert(!isReadMode());
			_write(data);
			return *this;
		}
		inline Archive& operator<<(const qyhs::Color& data)
		{
			assert(!isReadMode());
			_write(data.rgba);
			return *this;
		}
		inline Archive& operator<<(const std::string& data)
		{
			assert(!isReadMode());
			int a = data.size();
			(*this) << data.length();
			for (const auto& x : data)
			{
				(*this) << x;
			}
			return *this;
		}
		template<typename T>
		inline Archive& operator<<(const std::vector<T>& data)
		{
			assert(!isReadMode());
			// Here we will use the << operator so that non-specified types will have compile error!
			(*this) << data.size();
			for (const T& x : data)
			{
				(*this) << x;
			}
			return *this;
		}
		inline Archive& operator<<(const Archive& other)
		{
			assert(!isReadMode());
			// Here we will use the << operator so that non-specified types will have compile error!
			//	Note: version and thumbnail data is skipped, only data is appended
			const size_t start = sizeof(uint64_t) * 2; // version and thumbnail size
			for (size_t i = start; i < other.pos; ++i)
			{
				(*this) << other.data_ptr[i];
			}
			return *this;
		}

	private:
		void createEmpty();

	};
}