#pragma once
#include <filesystem>
#include <string>
#include <string_view>
#include <vector>

class DirectoryScanner
{
public:
    DirectoryScanner() = default;

    std::vector<std::filesystem::path> Scan(
        const std::filesystem::path& root,
        const std::vector<std::filesystem::path>& ignore_dir_names = {},
        bool case_insensitive = true,
        bool follow_directory_symlinks = false,
        size_t reserve_result = 0);

private:
    std::string to_lower_ascii(std::string_view s);
    std::string normalize_ext(std::string_view ext, bool case_insensitive);

    static constexpr const char* supported_extensions[] = {
        ".c", ".h",
        ".py", ".pyw",
        ".fs", ".fsx",
        ".cpp", ".hpp", ".cxx", ".ino",
        ".hxx", ".c++", ".cc", ".ixx", ".cppm",
        ".cs",
        ".rs",
        ".go",
        ".xml", ".xaml", ".html",
        ".java", ".kt", ".kts",
        ".js", ".jsx", ".ts", ".tsx",
        ".rb",
        ".sh", ".zsh",
        ".ps1", ".psd1", "psm1",
    };
};
