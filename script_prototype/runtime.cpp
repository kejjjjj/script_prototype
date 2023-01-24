#include "pch.h"

Runtime::Runtime()
{

}
Runtime::~Runtime()
{
	fs::F_CloseFile(f);
}
bool Runtime::InitializeRuntime(const std::string& _path)
{
	std::string path = _path;
	path = fs::GetRootDirectory() + "\\" + path;
	if (!fs::F_OpenFile(f, path, fs::fileopen::FILE_IN)) {
		return false;
	}
	std::cout << std::format("\"{}\" successfully opened!\n", path);

	int a = 1;

	return true;
}
