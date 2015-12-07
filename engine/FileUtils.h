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

	bool IsAbsolutePath(const string& strPath) const;
	
	// D:\aaa\bbb\ccc\ddd\abc.txt --> D:/aaa/bbb/ccc/ddd/abc.txt
	string ConvertPathFormatToUnixStyle(const string& path);

	string TCHAR2String(TCHAR* str);
	
	Data GetDataFromFile(const string& filename, bool forString);
	
	std::string FullPathForFilename(const string &filename);

	static string GetExtensionName(const char* fileName);
protected:
	void CheckPath();
public:
	string _resourcePath;
};