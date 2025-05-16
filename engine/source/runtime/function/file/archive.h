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
		bool isOpen()const { return data_ptr != nullptr; }
		const uint8_t* data_ptr = nullptr;
		std::vector<uint8_t> DATA;
		size_t data_size = 0;
		uint64_t pos = 0;
		std::string getSourceDir()const { return directory; }
		
		bool read_mode = true;
		std::string directory;

		template<typename T>
		void _write(const T & data)
		{
			size_t _right = pos + sizeof(data);
			if (pos > data_size)
			{
				DATA.resize(_right * 2);
				data_ptr = DATA.data();
				data_size = DATA.size();
			}
			*(T*)(DATA.data() + pos) = data;
			pos = _right;
		}

		// Write operations
		inline Archive& operator<<(bool data)
		{
			_write((uint32_t)(data ? 1 : 0));
			return *this;
		}
		inline Archive& operator<<(char data)
		{
			_write((int8_t)data);
			return *this;
		}
		inline Archive& operator<<(short data)
		{
			_write((int16_t)data);
			return *this;
		}
		inline Archive& operator<<(unsigned char data)
		{
			_write((uint8_t)data);
			return *this;
		}
		inline Archive& operator<<(unsigned short data)
		{
			_write((uint16_t)data);
			return *this;
		}
		inline Archive& operator<<(int data)
		{
			_write((int64_t)data);
			return *this;
		}
		inline Archive& operator<<(unsigned int data)
		{
			_write((uint64_t)data);
			return *this;
		}
		inline Archive& operator<<(long data)
		{
			_write((int64_t)data);
			return *this;
		}
		inline Archive& operator<<(unsigned long data)
		{
			_write((uint64_t)data);
			return *this;
		}
		inline Archive& operator<<(long long data)
		{
			_write((int64_t)data);
			return *this;
		}
		inline Archive& operator<<(unsigned long long data)
		{
			_write((uint64_t)data);
			return *this;
		}
		inline Archive& operator<<(float data)
		{
			_write(data);
			return *this;
		}
		inline Archive& operator<<(double data)
		{
			_write(data);
			return *this;
		}
		inline Archive& operator<<(const XMFLOAT2& data)
		{
			_write(data);
			return *this;
		}
		inline Archive& operator<<(const XMFLOAT3& data)
		{
			_write(data);
			return *this;
		}
		inline Archive& operator<<(const XMFLOAT4& data)
		{
			_write(data);
			return *this;
		}
		inline Archive& operator<<(const XMFLOAT3X3& data)
		{
			_write(data);
			return *this;
		}
		inline Archive& operator<<(const XMFLOAT4X3& data)
		{
			_write(data);
			return *this;
		}
		inline Archive& operator<<(const XMFLOAT4X4& data)
		{
			_write(data);
			return *this;
		}
		inline Archive& operator<<(const XMUINT2& data)
		{
			_write(data);
			return *this;
		}
		inline Archive& operator<<(const XMUINT3& data)
		{
			_write(data);
			return *this;
		}
		inline Archive& operator<<(const XMUINT4& data)
		{
			_write(data);
			return *this;
		}
		inline Archive& operator<<(const qyhs::Color& data)
		{
			_write(data.rgba);
			return *this;
		}
		inline Archive& operator<<(const std::string& data)
		{
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