#include "FileSystem.h"

#include <fstream>
#include <sstream>

namespace Engine
{
	// Define the static variable
	std::string FileSystem::rootPath = "";

	void FileSystem::SetRootPath(const std::string& path)
	{
		rootPath = path;

		// Format the string to ensure it ends with a slash for easy concatenation
		if (!rootPath.empty() && rootPath.back() != '/' && rootPath.back() != '\\')
		{
			rootPath += '/';
		}
	}

	std::string FileSystem::GetAbsolutePath(const std::string& filepath)
	{
		if (rootPath.empty()) return filepath;

		// Basic check to avoid double-rooting if the user provided an absolute path by mistake
		// (Checks for Windows drive letter 'C:' or Unix absolute root '/')
		if (filepath.length() > 1 && filepath[1] == ':') return filepath;
		if (!filepath.empty() && (filepath[0] == '/' || filepath[0] == '\\')) return filepath;

		return rootPath + filepath;
	}

	bool FileSystem::ReadBinary(const std::string& filepath, std::vector<uint8_t>& outData)
	{
		std::ifstream file(GetAbsolutePath(filepath), std::ios::binary | std::ios::ate);
		if (!file.is_open()) return false;

		size_t fileSize = static_cast<size_t>(file.tellg());
		outData.resize(fileSize);

		file.seekg(0);
		file.read(reinterpret_cast<char*>(outData.data()), fileSize);
		file.close();

		return true;
	}

	bool FileSystem::WriteBinary(const std::string& filepath, const void* data, size_t size)
	{
		std::ofstream file(GetAbsolutePath(filepath), std::ios::binary);
		if (!file.is_open()) return false;

		file.write(reinterpret_cast<const char*>(data), size);
		file.close();

		return true;
	}

	bool FileSystem::ReadText(const std::string& filepath, std::string& outText)
	{
		std::ifstream file(GetAbsolutePath(filepath));
		if (!file.is_open()) return false;

		std::stringstream buffer;
		buffer << file.rdbuf();
		outText = buffer.str();

		file.close();
		return true;
	}

	bool FileSystem::WriteText(const std::string& filepath, const std::string& text)
	{
		std::ofstream file(GetAbsolutePath(filepath));
		if (!file.is_open()) return false;

		file << text;
		file.close();
		return true;
	}
}