#include "FileSystem.h"

#include <fstream>
#include <sstream>

namespace Engine
{
    bool FileSystem::ReadBinary(const std::string& filepath, std::vector<uint8_t>& outData)
    {
        // std::ios::ate puts the cursor at the end of the file to quickly determine its size
        std::ifstream file(filepath, std::ios::binary | std::ios::ate);
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
        std::ofstream file(filepath, std::ios::binary);
        if (!file.is_open()) return false;

        file.write(reinterpret_cast<const char*>(data), size);
        file.close();

        return true;
    }

    bool FileSystem::ReadText(const std::string& filepath, std::string& outText)
    {
        std::ifstream file(filepath);
        if (!file.is_open()) return false;

        // Read the entire file at once using a string stream
        std::stringstream buffer;
        buffer << file.rdbuf();
        outText = buffer.str();

        file.close();
        return true;
    }

    bool FileSystem::WriteText(const std::string& filepath, const std::string& text)
    {
        std::ofstream file(filepath);
        if (!file.is_open()) return false;

        file << text;
        file.close();
        return true;
    }
}