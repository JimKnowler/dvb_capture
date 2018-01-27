#pragma once

#include <fstream>
#include <cinttypes>

namespace util {

	class FileSink {
	public:
		FileSink(const char* filename);
		~FileSink();

		bool write(const uint8_t* buffer, long length);

	private:
		std::ofstream file;
	};

}
