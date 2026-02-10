module;

#include <filesystem>
#include <string>
#include <vector>
#include <unordered_set>
#include <algorithm>
#include <cctype>
#include <system_error>

export module loc.Filesystem:DirectoryScanner;

export class DirectoryScanner
{
public:
    DirectoryScanner() = default;

    std::vector<std::filesystem::path> Scan(
        const std::filesystem::path& root,
        const std::vector<std::filesystem::path>& ignore_dir_names = {},
        bool case_insensitive = true,
        bool follow_directory_symlinks = false,
        size_t reserve_result = 0)
    {
        std::vector<std::filesystem::path> result;
        if (reserve_result) result.reserve(reserve_result);

        // Build normalized extension set
        std::unordered_set<std::string> ext_set;
        ext_set.reserve(24);
        for (const auto& e : supported_extensions) {
            auto ne = normalize_ext(e, case_insensitive);
            if (!ne.empty()) ext_set.insert(std::move(ne));
        }
        if (ext_set.empty()) return result; // nothing to match

        // Build ignore dir set (normalized per case setting)
        std::unordered_set<std::string> ignore_set;
        ignore_set.reserve(ignore_dir_names.size() * 2 + 4);
        for (const auto& d : ignore_dir_names) {
            if (d.empty()) continue;
            if (case_insensitive) ignore_set.insert(to_lower_ascii(d.string()));
            else ignore_set.insert(d.string());
        }

        std::error_code ec; // avoid exceptions from filesystem
        std::filesystem::directory_options opts = std::filesystem::directory_options::skip_permission_denied;

        if (follow_directory_symlinks) {
            opts |= std::filesystem::directory_options::follow_directory_symlink;
        }

        std::filesystem::recursive_directory_iterator it(root, opts, ec);
        if (ec) {
            // couldn't start iteration (permission / not found). return empty result.
            return result;
        }
        const std::filesystem::recursive_directory_iterator end_it;

        for (; it != end_it; ++it) {
            // Protect against filesystem errors per-entry
            std::error_code entry_ec;

            const std::filesystem::directory_entry& de = *it;

            // If it's a directory and matches ignore list, skip recursion into it.
            if (de.is_directory(entry_ec)) {
                if (entry_ec) { /* skip problematic entry */ continue; }

                if (!ignore_set.empty()) {
                    // get filename (last component) as string
                    auto filename = de.path().filename().string();
                    if (case_insensitive) filename = to_lower_ascii(filename);
                    if (ignore_set.find(filename) != ignore_set.end()) {
                        it.disable_recursion_pending(); // do not descend into this dir
                        continue;
                    }
                }
                
                // Skip directories starting with '.' (hidden directories)
                auto dirname = de.path().filename().string();
                if (!dirname.empty() && dirname[0] == '.') {
                    it.disable_recursion_pending();
                    continue;
                }
                
                // not a file -> continue
                continue;
            }

            // We're interested only in regular files (skip sockets, device files, etc.)
            if (!de.is_regular_file(entry_ec)) {
                continue;
            }

            // get extension and test
            std::string ext = de.path().extension().string();
            if (case_insensitive) ext = to_lower_ascii(ext);
            if (ext_set.find(ext) != ext_set.end()) {
                // matched; append path (store as std::filesystem::path to avoid forcing string encoding prematurely)
                result.emplace_back(de.path());
            }
        }

        return result;
    }

private:
    // Lowercase ASCII helper (fast; avoids locale overhead)
    inline std::string to_lower_ascii(std::string_view s) {
        std::string out;
        out.reserve(s.size());
        for (unsigned char c : s) out.push_back(static_cast<char>(std::tolower(c)));
        return out;
    }

    // Normalize an extension: ensure it starts with '.' and (optionally) lowercase it.
    inline std::string normalize_ext(std::string_view ext, bool case_insensitive) {
        if (ext.empty()) return std::string{};
        size_t pos = 0;
        // if user passed "txt" convert to ".txt"
        if (ext.front() != '.') {
            std::string tmp;
            tmp.reserve(ext.size() + 1);
            tmp.push_back('.');
            tmp.append(ext);
            if (case_insensitive) return to_lower_ascii(tmp);
            return tmp;
        }
        if (case_insensitive) return to_lower_ascii(ext);
        return std::string(ext);
    }

    static constexpr const char* supported_extensions[] = {
        ".c", ".h",
        ".py", ".pyw",
        ".fs", ".fsx",
        ".cpp", ".hpp", ".cxx",
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
