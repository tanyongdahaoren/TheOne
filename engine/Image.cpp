#include "Image.h"
#include "FileUtils.h"
#include "Defines.h"

#include "jpeglib.h"

bool Image::initWithImageFile(const std::string& path)
{
	bool ret = false;

	

 	_filePath = FileUtils::getInstance()->fullPathForFilename(path);

 	Data data = FileUtils::getInstance()->getDataFromFile(_filePath, false);

	if (!data.isNull())
	{
		ret = initWithImageData(data.getBytes(), data.getSize());
	}

	return ret;
}

bool Image::initWithImageData(const unsigned char * data, ssize_t dataLen)
{
	unsigned char* unpackedData = const_cast<unsigned char*>(data);
	ssize_t unpackedLen = dataLen;

	_fileType = detectFormat(unpackedData, unpackedLen);

	bool ret = false;

	switch (_fileType)
	{
	case Format::JPG:
		ret = initWithJpgData(unpackedData, unpackedLen);
		break;
	}

	return ret;
}

Image::Format Image::detectFormat(const unsigned char * data, ssize_t dataLen)
{
	if (isJpg(data, dataLen))
	{
		return Format::JPG;
	}
	else
	{
		return Format::UNKNOWN;
	}
}

bool Image::isJpg(const unsigned char * data, ssize_t dataLen)
{
	if (dataLen <= 4)
	{
		return false;
	}

	static const unsigned char JPG_SOI[] = { 0xFF, 0xD8 };

	return memcmp(data, JPG_SOI, 2) == 0;
}

//for jpeg
namespace
{
	struct MyErrorMgr
	{
		struct jpeg_error_mgr pub;	/* "public" fields */
		jmp_buf setjmp_buffer;	/* for return to caller */
	};

	typedef struct MyErrorMgr * MyErrorPtr;

	/*
	* Here's the routine that will replace the standard error_exit method:
	*/

	METHODDEF(void)
		myErrorExit(j_common_ptr cinfo)
	{
		/* cinfo->err really points to a MyErrorMgr struct, so coerce pointer */
		MyErrorPtr myerr = (MyErrorPtr)cinfo->err;

		/* Always display the message. */
		/* We could postpone this until after returning, if we chose. */
		/* internal message function cann't show error message in some platforms, so we rewrite it here.
		* edit it if has version confilict.
		*/
		//(*cinfo->err->output_message) (cinfo);
		char buffer[JMSG_LENGTH_MAX];
		(*cinfo->err->format_message) (cinfo, buffer);
		//printf("jpeg error: %s", buffer);

		/* Return control to the setjmp point */
		longjmp(myerr->setjmp_buffer, 1);
	}
}

bool Image::initWithJpgData(const unsigned char * data, ssize_t dataLen)
{
	struct jpeg_decompress_struct cinfo;
	
	struct MyErrorMgr jerr;

	JSAMPROW row_pointer[1] = { 0 };
	unsigned long location = 0;

	bool ret = false;
	do
	{
		cinfo.err = jpeg_std_error(&jerr.pub);
		jerr.pub.error_exit = myErrorExit;
		if (setjmp(jerr.setjmp_buffer))
		{
			jpeg_destroy_decompress(&cinfo);
			break;
		}

		jpeg_create_decompress(&cinfo);

		jpeg_mem_src(&cinfo, const_cast<unsigned char*>(data), dataLen);

		jpeg_read_header(&cinfo, TRUE);

		if (cinfo.jpeg_color_space == JCS_GRAYSCALE)
		{
			//_renderFormat = Texture2D::PixelFormat::I8;
		}
		else
		{
			cinfo.out_color_space = JCS_RGB;
			//_renderFormat = Texture2D::PixelFormat::RGB888;
		}

		jpeg_start_decompress(&cinfo);

		/* init image info */
		_width = cinfo.output_width;
		_height = cinfo.output_height;

		_dataLen = cinfo.output_width*cinfo.output_height*cinfo.output_components;
		_data = static_cast<unsigned char*>(malloc(_dataLen * sizeof(unsigned char)));
		BREAK_IF(!_data);

		while (cinfo.output_scanline < cinfo.output_height)
		{
			row_pointer[0] = _data + location;
			location += cinfo.output_width*cinfo.output_components;
			jpeg_read_scanlines(&cinfo, row_pointer, 1);
		}

		jpeg_destroy_decompress(&cinfo);

		ret = true;
	} while (0);

	return ret;
}
