#pragma once

#include "tchar.h"
#include "windows.h"

#include <string>
using namespace std;

#include "Data.h"

class FileUtils
{
public:
	FileUtils();
public:
	static FileUtils* GetInstance();

	static void DestroyInstance();

	bool IsAbsolutePath(const std::string& strPath) const;
	
	// D:\aaa\bbb\ccc\ddd\abc.txt --> D:/aaa/bbb/ccc/ddd/abc.txt
	std::string ConvertPathFormatToUnixStyle(const std::string& path);

	std::string TCHAR2String(TCHAR* str);
	
	Data GetDataFromFile(const std::string& filename, bool forString);
	
	std::string FullPathForFilename(const std::string &filename);
protected:
	void CheckPath();
public:
	string _resourcePath;
};