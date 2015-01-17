#include "utils/FileUtils.h"

#include <Windows.h>
#include <Shlobj.h>

// The root path of resources, the character encoding is UTF-8.
static std::string s_resourcePath = "";

static FileUtils* gs_instance = nullptr;

FileUtils * FileUtils::getInstance()
{
	if (gs_instance == nullptr)
	{
		gs_instance = new FileUtils();
		gs_instance->init();
	}
	return gs_instance;
}

void FileUtils::purge()
{
	SAFE_DELETE(gs_instance);
}

// D:\aaa\bbb\ccc\ddd\abc.txt --> D:/aaa/bbb/ccc/ddd/abc.txt
static inline std::string convertPathFormatToUnixStyle(const std::string& path)
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

static void _checkPath()
{
	if (0 == s_resourcePath.length())
	{
		WCHAR utf16Path[512] = { 0 };
		GetCurrentDirectoryW(sizeof(utf16Path) - 1, utf16Path);

		char utf8Path[512] = { 0 };
		int nNum = WideCharToMultiByte(CP_UTF8, 0, utf16Path, -1, utf8Path, sizeof(utf8Path), nullptr, nullptr);

		s_resourcePath = convertPathFormatToUnixStyle(utf8Path);
		s_resourcePath.append("/");
	}
}

static Data getData(const std::string& filename, bool forString)
{
	if (filename.empty())
	{
		return Data::Null;
	}

	Data ret;
	unsigned char* buffer = nullptr;
	ssize_t size = 0;
	size_t readsize;
	const char* mode = nullptr;
	if (forString)
		mode = "rt";
	else
		mode = "rb";

	do
	{
		// Read the file from hardware
		std::string fullPath = FileUtils::getInstance()->fullPathForFilename(filename);
		FILE *fp = fopen(fullPath.c_str(), mode);
		BREAK_IF(!fp);
		fseek(fp, 0, SEEK_END);
		size = ftell(fp);
		fseek(fp, 0, SEEK_SET);

		if (forString)
		{
			buffer = (unsigned char*)malloc(sizeof(unsigned char) * (size + 1));
			buffer[size] = '\0';
		}
		else
		{
			buffer = (unsigned char*)malloc(sizeof(unsigned char) * size);
		}

		readsize = fread(buffer, sizeof(unsigned char), size, fp);
		fclose(fp);

		if (forString && readsize < size)
		{
			buffer[readsize] = '\0';
		}
	} while (0);

	if (nullptr == buffer || 0 == readsize)
	{
		std::string msg = "Get data from file(";
		msg.append(filename).append(") failed!");
		LOG("%s", msg.c_str());
	}
	else
	{
		ret.fastSet(buffer, readsize);
	}

	return ret;
}

std::string FileUtils::getStringFromFile(const std::string & filename)
{
	Data data = getData(filename, true);
	if (data.isNull())
		return "";

	std::string ret((const char*)data.getBytes());
	return ret;
}

Data FileUtils::getDataFromFile(const std::string & filename)
{
	return getData(filename, false);
}

std::string FileUtils::fullPathForFilename(const std::string & filename)
{
	if (filename.empty())
	{
		return "";
	}

	if (isAbsolutePath(filename))
	{
		return filename;
	}

	// Already Cached ?
	auto cacheIter = _fullPathCache.find(filename);
	if (cacheIter != _fullPathCache.end())
	{
		return cacheIter->second;
	}

	// Get the new file name.
	const std::string newFilename(getNewFilename(filename));

	std::string fullpath;

	for (auto searchIt = _searchPathArray.cbegin(); searchIt != _searchPathArray.cend(); ++searchIt)
	{
		for (auto resolutionIt = _searchResolutionsOrderArray.cbegin(); resolutionIt != _searchResolutionsOrderArray.cend(); ++resolutionIt)
		{
			fullpath = this->getPathForFilename(newFilename, *resolutionIt, *searchIt);

			if (fullpath.length() > 0)
			{
				// Using the filename passed in as key.
				_fullPathCache.insert(std::make_pair(filename, fullpath));
				return fullpath;
			}
		}
	}

	return filename;
}

void FileUtils::setSearchPaths(const std::vector<std::string>& searchPaths)
{
	bool existDefaultRootPath = false;

	_fullPathCache.clear();
	_searchPathArray.clear();
	for (auto iter = searchPaths.cbegin(); iter != searchPaths.cend(); ++iter)
	{
		std::string prefix;
		std::string path;

		if (!isAbsolutePath(*iter))
		{ // Not an absolute path
			prefix = _defaultResRootPath;
		}
		path = prefix + (*iter);
		if (path.length() > 0 && path[path.length() - 1] != '/')
		{
			path += "/";
		}
		if (!existDefaultRootPath && path == _defaultResRootPath)
		{
			existDefaultRootPath = true;
		}
		_searchPathArray.push_back(path);
	}

	if (!existDefaultRootPath)
	{
		_searchPathArray.push_back(_defaultResRootPath);
	}
}

void FileUtils::addSearchPath(const std::string & searchpath, const bool front)
{
	std::string prefix;
	if (!isAbsolutePath(searchpath))
		prefix = _defaultResRootPath;

	std::string path = prefix + searchpath;
	if (path.length() > 0 && path[path.length() - 1] != '/')
	{
		path += "/";
	}
	if (front) {
		_searchPathArray.insert(_searchPathArray.begin(), path);
	}
	else {
		_searchPathArray.push_back(path);
	}
}

const std::vector<std::string>& FileUtils::getSearchPaths() const
{
	return _searchPathArray;
}

std::string FileUtils::getWritablePath()
{
	// Get full path of executable, e.g. c:\Program Files (x86)\My Game Folder\MyGame.exe
	char full_path[512 + 1];
	::GetModuleFileNameA(nullptr, full_path, 512 + 1);

	// Debug app uses executable directory; Non-debug app uses local app data directory
	//#ifndef _DEBUG
	// Get filename of executable only, e.g. MyGame.exe
	char *base_name = strrchr(full_path, '\\');

	if (base_name)
	{
		char app_data_path[512 + 1];

		// Get local app data directory, e.g. C:\Documents and Settings\username\Local Settings\Application Data
		if (SUCCEEDED(SHGetFolderPathA(nullptr, CSIDL_LOCAL_APPDATA, nullptr, SHGFP_TYPE_CURRENT, app_data_path)))
		{
			std::string ret((char*)app_data_path);

			// Adding executable filename, e.g. C:\Documents and Settings\username\Local Settings\Application Data\MyGame.exe
			ret += base_name;

			// Remove ".exe" extension, e.g. C:\Documents and Settings\username\Local Settings\Application Data\MyGame
			ret = ret.substr(0, ret.rfind("."));

			ret += "\\";

			// Create directory
			if (SUCCEEDED(SHCreateDirectoryExA(nullptr, ret.c_str(), nullptr)))
			{
				return convertPathFormatToUnixStyle(ret);
			}
		}
	}
	//#endif // not defined _DEBUG

	// If fetching of local app data directory fails, use the executable one
	std::string ret((char*)full_path);

	// remove xxx.exe
	ret = ret.substr(0, ret.rfind("\\") + 1);

	ret = convertPathFormatToUnixStyle(ret);

	return ret;
}

bool FileUtils::isFileExistInternal(const std::string& strFilePath)
{
	if (0 == strFilePath.length())
	{
		return false;
	}

	std::string strPath = strFilePath;
	if (!isAbsolutePath(strPath))
	{ // Not absolute path, add the default root path at the beginning.
		strPath.insert(0, _defaultResRootPath);
	}

	WCHAR utf16Buf[512] = { 0 };
	MultiByteToWideChar(CP_UTF8, 0, strPath.c_str(), -1, utf16Buf, sizeof(utf16Buf) / sizeof(utf16Buf[0]));

	DWORD attr = GetFileAttributesW(utf16Buf);
	if (attr == INVALID_FILE_ATTRIBUTES || (attr & FILE_ATTRIBUTE_DIRECTORY))
		return false;   //  not a file
	return true;
}
bool FileUtils::isFileExist(const std::string & filename)
{
	if (isAbsolutePath(filename))
	{
		return isFileExistInternal(filename);
	}
	else
	{
		std::string fullpath = searchFullPathForFilename(filename);
		if (fullpath.empty())
			return false;
		else
			return true;
	}
}

bool FileUtils::isAbsolutePath(const std::string & strPath)
{
	if (strPath.length() > 2
		&& ((strPath[0] >= 'a' && strPath[0] <= 'z') || (strPath[0] >= 'A' && strPath[0] <= 'Z'))
		&& strPath[1] == ':')
	{
		return true;
	}
	return false;
}


bool FileUtils::isDirectoryExistInternal(const std::string& dirPath)
{
	unsigned long fAttrib = GetFileAttributesA(dirPath.c_str());
	if (fAttrib != INVALID_FILE_ATTRIBUTES &&
		(fAttrib & FILE_ATTRIBUTE_DIRECTORY))
	{
		return true;
	}
	return false;
}
std::string FileUtils::getFullPathForDirectoryAndFilename(const std::string & directory, const std::string & filename)
{
	// get directory+filename, safely adding '/' as necessary 
	std::string ret = directory;
	if (directory.size() && directory[directory.size() - 1] != '/') {
		ret += '/';
	}
	ret += filename;

	// if the file doesn't exist, return an empty string
	if (!isFileExistInternal(ret)) {
		ret = "";
	}
	return ret;
}
bool FileUtils::isDirectoryExist(const std::string & dirPath)
{
	ASSERT(!dirPath.empty(), "Invalid path");

	if (isAbsolutePath(dirPath))
	{
		return isDirectoryExistInternal(dirPath);
	}

	// Already Cached ?
	auto cacheIter = _fullPathCache.find(dirPath);
	if (cacheIter != _fullPathCache.end())
	{
		return isDirectoryExistInternal(cacheIter->second);
	}

	std::string fullpath;
	for (auto searchIt = _searchPathArray.cbegin(); searchIt != _searchPathArray.cend(); ++searchIt)
	{
		for (auto resolutionIt = _searchResolutionsOrderArray.cbegin(); resolutionIt != _searchResolutionsOrderArray.cend(); ++resolutionIt)
		{
			// searchPath + file_path + resourceDirectory
			fullpath = *searchIt + dirPath + *resolutionIt;
			if (isDirectoryExistInternal(fullpath))
			{
				const_cast<FileUtils*>(this)->_fullPathCache.insert(std::make_pair(dirPath, fullpath));
				return true;
			}
		}
	}

	return false;
}

bool FileUtils::createDirectory(const std::string & path)
{
	if (isDirectoryExist(path))
		return true;

	// Split the path
	size_t start = 0;
	size_t found = path.find_first_of("/\\", start);
	std::string subpath;
	std::vector<std::string> dirs;

	if (found != std::string::npos)
	{
		while (true)
		{
			subpath = path.substr(start, found - start + 1);
			if (!subpath.empty())
				dirs.push_back(subpath);
			start = found + 1;
			found = path.find_first_of("/\\", start);
			if (found == std::string::npos)
			{
				if (start < path.length())
				{
					dirs.push_back(path.substr(start));
				}
				break;
			}
		}
	}
	if ((GetFileAttributesA(path.c_str())) == INVALID_FILE_ATTRIBUTES)
	{
		subpath = "";
		for (int i = 0; i < dirs.size(); ++i)
		{
			subpath += dirs[i];
			if (!isDirectoryExist(subpath))
			{
				BOOL ret = CreateDirectoryA(subpath.c_str(), NULL);
				if (!ret && ERROR_ALREADY_EXISTS != GetLastError())
				{
					return false;
				}
			}
		}
	}
	return true;
}

bool FileUtils::removeDirectory(const std::string & path)
{
	if (path.size() > 0 && path[path.size() - 1] != '/')
	{
		LOG("Fail to remove directory, path must termniate with '/': %s", path.c_str());
		return false;
	}
	std::string command = "cmd /c rd /s /q ";
	// Path may include space.
	command += "\"" + path + "\"";

	if (WinExec(command.c_str(), SW_HIDE) > 31)
		return true;
	else
		return false;
}

bool FileUtils::removeFile(const std::string & path)
{
	std::string command = "cmd /c del /q ";
	std::string win32path = path;
	int len = win32path.length();
	for (int i = 0; i < len; ++i)
	{
		if (win32path[i] == '/')
		{
			win32path[i] = '\\';
		}
	}
	command += win32path;

	if (WinExec(command.c_str(), SW_HIDE) > 31)
		return true;
	else
		return false;
}

long FileUtils::getFileSize(const std::string & filepath)
{
	ASSERT(!filepath.empty(), "Invalid path");

	std::string fullpath = filepath;
	if (!isAbsolutePath(filepath))
	{
		fullpath = searchFullPathForFilename(filepath);
		if (fullpath.empty())
			return 0;
	}

	struct stat info;
	// Get data associated with "crt_stat.c":
	int result = stat(fullpath.c_str(), &info);

	// Check if statistics are valid:
	if (result != 0)
	{
		// Failed
		return -1;
	}
	else
	{
		return (long)(info.st_size);
	}
}

bool FileUtils::init()
{
	_checkPath();
	_defaultResRootPath = s_resourcePath;
	_searchPathArray.push_back(_defaultResRootPath);
	_searchResolutionsOrderArray.push_back("");
	return true;
}

std::string FileUtils::getNewFilename(const std::string & filename)
{
	std::string newFileName;

	// in Lookup Filename dictionary ?
	auto iter = _filenameLookupDict.find(filename);

	if (iter == _filenameLookupDict.end())
	{
		newFileName = filename;
	}
	else
	{
		newFileName = iter->second.asString();
	}
	return newFileName;
}

std::string FileUtils::getPathForFilename(const std::string & filename, const std::string & resolutionDirectory, const std::string & searchPath)
{
	std::string file = filename;
	std::string file_path = "";
	size_t pos = filename.find_last_of("/");
	if (pos != std::string::npos)
	{
		file_path = filename.substr(0, pos + 1);
		file = filename.substr(pos + 1);
	}

	// searchPath + file_path + resourceDirectory
	std::string path = searchPath;
	path += file_path;
	path += resolutionDirectory;

	path = getFullPathForDirectoryAndFilename(path, file);

	//CCLOG("getPathForFilename, fullPath = %s", path.c_str());
	return path;
}

std::string FileUtils::searchFullPathForFilename(const std::string & filename)
{
	if (isAbsolutePath(filename))
	{
		return filename;
	}
	std::string path = const_cast<FileUtils*>(this)->fullPathForFilename(filename);
	if (0 == path.compare(filename))
	{
		return "";
	}
	else
	{
		return path;
	}
}
