#pragma once

#include "Ref.h"
#include <string>
#include <iostream>
using namespace std;
#include "Magick++.h"

class EasyImage : public Ref
{
public:
	bool InitWithFileName(string name);
	const Magick::Image& GetImage(){ return _image; }
	const Magick::Blob& GetBlob(){ return _blob; }
public:
	std::string	_fileName;
	Magick::Image _image;
	Magick::Blob _blob;
};