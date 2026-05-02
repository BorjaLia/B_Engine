#pragma once

#include <string>
#include <vector>
#include <cstdint>

namespace Engine
{
    /// Provides static utility functions for reading and writing files.
    /// @ingroup Utils
    class FileSystem
    {
    public:
        /// Reads an entire binary file into a byte vector.
        /// @param filepath The path to the file.
        /// @param outData The vector to populate with the file's binary data.
        static bool ReadBinary(const std::string& filepath, std::vector<uint8_t>& outData);

        /// Writes a byte array directly to disk.
        /// @param filepath The path where the file will be saved.
        /// @param data Pointer to the binary data.
        /// @param size The number of bytes to write.
        static bool WriteBinary(const std::string& filepath, const void* data, size_t size);

        /// Reads an entire text file into a string.
        /// @param filepath The path to the file.
        /// @param outText The string to populate with the file's text data.
        static bool ReadText(const std::string& filepath, std::string& outText);

        /// Writes a string directly to disk.
        /// @param filepath The path where the file will be saved.
        /// @param text The text data to write.
        static bool WriteText(const std::string& filepath, const std::string& text);
    };
}