#pragma once
#include <windows.h>
#include<string>
#include <iostream>
#include <direct.h>
#include  <io.h>
#include <shlobj.h>
#pragma comment(lib, "shell32.lib")



class File {
public:
	bool FileExist(std::string path) {
		if ((access(path.c_str(), 0) != 0) || (access(path.c_str(), 0) != 0)){
			return false;
		}
		return true;
	};


	bool DelFile(std::string path) {
		return !remove(path.c_str());
	}

	std::string GetSystemPath(int path = CSIDL_PERSONAL) {
		CHAR my_documents[MAX_PATH];
		HRESULT result = SHGetFolderPathA(NULL, path, NULL, SHGFP_TYPE_CURRENT, my_documents);
		return my_documents;
	}

};


class str {
public:

	std::wstring Stringtowstring(std::string str)
	{
		std::wstring result;
		int len = MultiByteToWideChar(/*CP_ACP*/CP_UTF8, 0, str.c_str(), str.size(), NULL, 0);
		TCHAR* buffer = new TCHAR[len + 1];
		MultiByteToWideChar(/*CP_ACP*/CP_UTF8, 0, str.c_str(), str.size(), buffer, len);
		buffer[len] = '\0';
		result.append(buffer);
		delete[] buffer;
		return result;
	}

	char* WstrToAstr(WCHAR* wstr)
	{
		unsigned long i = lstrlen(wstr);
		char* astr;
		astr = (char*)malloc(i + 1);
		if (astr == NULL)
			return NULL;
		else
			memset(astr, '\0', i + 1);

		wcstombs(astr, wstr, i + 1);
		return astr;
	}

	std::string printf(std::string text, ...) {
		va_list args;
		va_start(args, text);
		int nParameterLen = _vscprintf(text.c_str(), args) + 1;
		char* buffer = new char[nParameterLen];
		vsnprintf(buffer, nParameterLen, text.c_str(), args);
		va_end(args);

		std::string result(buffer);
		delete[] buffer;

		return result;
	}


};



class BySkydome
{
public:
	File FileSystem() {
		return File_;
	};

	str StrSystem() {
		return Str_;
	};




private:
	File File_;
	str Str_;
};

inline BySkydome SDlib;