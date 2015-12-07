#include "FileUtils.h"
#include "Defines.h"
#include <algorithm>

std::string FileUtils::TCHAR2String(TCHAR* str)
{
	int len = WideCharToMultiByte(CP_ACP, 0, str, -1, NULL, 0, NULL, NULL);
	char* chRtn = new char[len * sizeof(char)];
	WideCharToMultiByte(CP_ACP, 0, str, -1, chRtn, len, NULL, NULL);
	string s = chRtn;
	return s;
}

void FileUtils::CheckPath()
{
// 	TCHAR szPath[MAX_PATH];
// 	if (!GetModuleFileName(NULL, szPath, MAX_PATH))
// 	{
// 		printf("GetModuleFileName failed (%d)\n", GetLastError());
// 		return;
// 	}

//	(_tcsrchr(szPath, _T('\\')))[1] = 0;
// 
// 	_resourcePath = TCHAR2String(szPath);

	TCHAR path[MAX_PATH];
	::GetCurrentDirectory(MAX_PATH, path);
	_resourcePath = TCHAR2String(path);
}

static FileUtils* s_instance = nullptr;
FileUtils* FileUtils::GetInstance()
{
	if (!s_instance)
	{
		s_instance = new FileUtils();
	}

	return s_instance;
}

void FileUtils::DestroyInstance()
{
	SAFE_DELETE(s_instance);
}

FileUtils::FileUtils()
{
	CheckPath();
}

string FileUtils::FullPathForFilename(const string &filename)
{
	return _resourcePath + '/' + filename;
}

string FileUtils::GetExtensionName(const char* fileName)
{
	char buff[64];
	strcpy(buff, fileName);
	char* pos = strrchr(buff, '.');
	if (pos)
	{
		string str(pos + 1);
		//1.ת��ΪСд
		//http://blog.csdn.net/infoworld/article/details/29872869
		std::transform(str.begin(), str.end(), str.begin(), ::tolower);
		return str;
	}
	return "";
}

std::string FileUtils::ConvertPathFormatToUnixStyle(const std::string& path)
{
	std::string ret = path;
	int len = ret.length();
	for (int i = 0; i < len; ++i)
	{
		if (ret[i] == '\\')
		{
			ret[i] = '/';
		}
	}
	return ret;
}

bool FileUtils::IsAbsolutePath(const std::string& strPath) const
{
	if ((strPath.length() > 2
		&& ((strPath[0] >= 'a' && strPath[0] <= 'z') || (strPath[0] >= 'A' && strPath[0] <= 'Z'))
		&& strPath[1] == ':') || (strPath[0] == '/' && strPath[1] == '/'))
	{
		return true;
	}
	return false;
}

Data FileUtils::GetDataFromFile(const std::string& fullPath, bool forString)
{
	if (fullPath.empty())
	{
		return Data::Null;
	}

	unsigned char *buffer = nullptr;

	size_t size = 0;
	do
	{
		//std::string fullPath = FileUtils::getInstance()->fullPathForFilename(filename);
		string sss = ConvertPathFormatToUnixStyle(fullPath);

		WCHAR wszBuf[MAX_PATH] = { 0 };
		MultiByteToWideChar(CP_UTF8, 0, sss.c_str(), -1, wszBuf, sizeof(wszBuf) / sizeof(wszBuf[0]));

		HANDLE fileHandle = ::CreateFileW(wszBuf, GENERIC_READ, FILE_SHARE_READ | FILE_SHARE_WRITE, NULL, OPEN_EXISTING, NULL, nullptr);

		size = ::GetFileSize(fileHandle, nullptr);

		if (forString)
		{
			buffer = (unsigned char*)malloc(size + 1);
			buffer[size] = '\0';
		}
		else
		{
			buffer = (unsigned char*)malloc(size);
		}
		DWORD sizeRead = 0;
		BOOL successed = FALSE;
		successed = ::ReadFile(fileHandle, buffer, size, &sizeRead, nullptr);
		::CloseHandle(fileHandle);

		if (!successed)
		{
			// should determine buffer value, or it will cause memory leak
			if (buffer)
			{
				free(buffer);
				buffer = nullptr;
			}
		}
	} while (0);

	Data ret;

	if (buffer == nullptr || size == 0)
	{
		std::string msg = "Get data from file(";
		// Gets error code.
		DWORD errorCode = ::GetLastError();
		char errorCodeBuffer[20] = { 0 };
		_snprintf(errorCodeBuffer, sizeof(errorCodeBuffer), "%d", errorCode);

		//msg = msg + filename + ") failed, error code is " + errorCodeBuffer;

		if (buffer)
			free(buffer);
	}
	else
	{
		ret.FastSet(buffer, size);
	}
	return ret;
}