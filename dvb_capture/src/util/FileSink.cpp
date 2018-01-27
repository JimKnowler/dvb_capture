#include "FileSink.h"

namespace util {

	using std::ios;

	FileSink::FileSink(const char* filename)
	{
		file.open(filename, ios::out | ios::binary);
	}

	FileSink::~FileSink()
	{
		file.close();
	}

	bool FileSink::write(const uint8_t* buffer, long length)
	{
		file.write(reinterpret_cast<const char*>(buffer), length);
		return true;
	}

}
