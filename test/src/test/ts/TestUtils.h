#pragma once

#include <vector>
#include <cinttypes>

#define UNUSED(_x) (void) (_x)

std::vector<uint8_t> helperReadFile(const char* filename);

int helperCountTSPacketsInFile(const char* filename, int chunkSize = -1, int numBytes = -1);
