#ifndef FILE_IO_H
#define FILE_IO_H

#include <string>

bool compressFile(const std::string& inputFilename, const std::string& outputFilename);
bool decompressFile(const std::string& inputFilename, const std::string& outputFilename);

#endif
