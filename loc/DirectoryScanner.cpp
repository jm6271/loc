#include "include/DirectoryScanner.h"
#include <iostream>

namespace fs = std::filesystem;

/**
* Scans a directory and returns a list of all the paths to the files with the given extensions
* @param directory the directory to scan
* @param extensions the extensions to look for
* @return a list of paths to the files found
*/
std::vector<std::string> DirectoryScanner::Scan(const fs::path& directory, const std::vector<std::string>& extensions,
                                                const std::vector<std::string>& ignoreDirs) const
{
    // Scan a directory and return a list of all the paths to the files with the given extensions
    std::vector<std::string> file_paths; 
    
    if (fs::exists(directory) && fs::is_directory(directory))
    { 
        for (const auto& entry : fs::recursive_directory_iterator(directory))
        { 
            if (fs::is_regular_file(entry.status())) 
            { 
                // Check if the file has one of the extensions
                bool has_extension = false;
                for (const auto& extension : extensions) 
                { 
                    if (entry.path().extension() == extension) 
                    { 
                        has_extension = true;
                        break;
                    } 
                } 
                if (!has_extension) 
                    continue;
                
                try
                {
                    file_paths.push_back(entry.path().string()); 
                }
                catch(const std::system_error& e)
                {
                    // Sometimes throws this error:
                    // No mapping for the Unicode character exists in the target multi-byte code page

                    // Ignore the file and print a warning
                    std::cerr << "Warning: System error thrown while getting a file path. Skipping file.\n";
                    std::cerr << "Error message: " << e.what() << '\n';

                    continue;
                }
            } 
        } 
    }
    else 
    { 
        std::cerr << "Directory does not exist or is not a directory " << directory << std::endl; 
    } 
    
    // Remove files in ignored directories
    RemoveIgnoredFiles(file_paths, ignoreDirs);
    return file_paths;
}

std::vector<std::string> DirectoryScanner::FindIgnoredDirectories(const std::filesystem::path &directory, const std::vector<std::string> &ignorePatterns) const
{
    std::vector<std::string> ignoredDirectories;

    if (fs::exists(directory) && fs::is_directory(directory))
    {
        for (const auto& entry : fs::recursive_directory_iterator(directory))
        {
            if (fs::is_directory(entry.status()))
            {
                // Check if the directory name matches any of the ignore patterns
                for (const auto& pattern : ignorePatterns)
                {
                    if (entry.path().filename().string() == pattern)
                    {
                        try
                        {
                            ignoredDirectories.push_back(entry.path().string());
                        }
                        catch(const std::system_error& e)
                        {
                            // Ignore the directory and print a warning
                            std::cerr << "Warning: System error thrown while getting a directory path. Skipping directory.\n";
                            std::cerr << "Error message: " << e.what() << '\n';
                            continue;
                        }
                        break; // No need to check other patterns for this directory
                    }
                }
            }
        }
    }
    else
    {
        std::cerr << "Directory does not exist or is not a directory '" << directory << '\'' << std::endl;
    }

    return ignoredDirectories;
}

void DirectoryScanner::RemoveIgnoredFiles(std::vector<std::string> &filePaths, const std::vector<std::string> &ignoreDirs) const
{
    filePaths.erase(std::remove_if(filePaths.begin(), filePaths.end(),
                                   [&ignoreDirs](const std::string& filePath)
                                   {
                                       for (const auto& ignoreDir : ignoreDirs)
                                       {
                                           if (filePath.find(ignoreDir) != std::string::npos)
                                           {
                                               return true; // Remove this file
                                           }
                                       }
                                       return false; // Keep this file
                                   }),
                    filePaths.end());
}
