#include "EasyImage.h"

bool EasyImage::InitWithFileName(string name)
{
	_fileName = name;
	try {
		_image.read(_fileName);
		_image.write(&_blob, "RGBA");
	}
	catch (Magick::Error& Error) {
		std::cout << "Error loading texture '" << _fileName << "': " << Error.what() << std::endl;
		return false;
	}
	return true;
}

