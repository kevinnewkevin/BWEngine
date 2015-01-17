#pragma once
#include "base/Ref.h"
#include "base/Data.h"
#include "base/Vector.h"
#include "base/Value.h"

class FileUtils : Ref
{
public:
	static FileUtils* getInstance();
	static void purge();

	virtual std::string getStringFromFile(const std::string& filename);
	virtual Data getDataFromFile(const std::string& filename);
	virtual std::string fullPathForFilename(const std::string &filename);
	virtual void setSearchPaths(const std::vector<std::string>& searchPaths);
	void addSearchPath(const std::string & path, const bool front = false);
	virtual const std::vector<std::string>& getSearchPaths() const;
	virtual std::string getWritablePath();
	virtual bool isFileExist(const std::string& filename); 
	virtual bool isFileExistInternal(const std::string& filename);

	virtual bool isAbsolutePath(const std::string& path);
	virtual bool isDirectoryExist(const std::string& dirPath);
	virtual bool createDirectory(const std::string& dirPath);
	virtual bool removeDirectory(const std::string& dirPath);
	virtual bool removeFile(const std::string &filepath);
	virtual long getFileSize(const std::string &filepath);

protected:
	virtual bool init();
	virtual std::string getNewFilename(const std::string &filename);
	virtual std::string getPathForFilename(const std::string& filename, const std::string& resolutionDirectory, const std::string& searchPath);
	virtual std::string searchFullPathForFilename(const std::string& filename);
	virtual bool isDirectoryExistInternal(const std::string& dirPath);
	virtual std::string getFullPathForDirectoryAndFilename(const std::string& directory, const std::string& filename);

protected:
	ValueMap _filenameLookupDict;
	std::vector<std::string> _searchPathArray;
	std::vector<std::string> _searchResolutionsOrderArray;
	std::string _defaultResRootPath;
	std::unordered_map<std::string, std::string> _fullPathCache;
};

