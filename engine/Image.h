#pragma once

#include "Ref.h"
#include <string>
using namespace std;

#include "Data.h"

class Image : public Ref
{
public:
	enum class Format
	{
		//! JPEG
		JPG,
		UNKNOWN
	};
public:
	bool Image::initWithImageFile(const std::string& path);

	bool initWithImageData(const unsigned char * data, ssize_t dataLen);
	
protected:
	Format detectFormat(const unsigned char * data, ssize_t dataLen);
	bool isJpg(const unsigned char * data, ssize_t dataLen);

	bool initWithJpgData(const unsigned char *  data, ssize_t dataLen);
public:
	string _filePath;
	Format _fileType;
	int _width;
	int _height;
	unsigned char *_data;
	ssize_t _dataLen;
};