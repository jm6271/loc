#include "include/Counter.h"
#include "include/CLineCounter.h"
#include <sstream>
#include <filesystem>
#include "include/DirectoryScanner.h"
#include <future>
#include <iostream>
#include "include/PyLineCounter.h"

/**
 * Constructor for the Counter class.
 * @param jobs The number of threads to use.
 * @param extensions A string of all the file extensions to count, separated by semicolons.
 * @param paths A vector of paths to search for files. If a path is a directory, it is recursed.
 * @param ignores A string of all the paths to ignore, separated by semicolons.
 * 
 * This constructor initializes the Counter object and starts the threads for counting. The
 * threads are started after the constructor is finished.
 */
Counter::Counter(unsigned int jobs, std::string extensions, std::vector<std::string> paths, std::string ignores)
{
    this->jobs = jobs;
    this->paths = paths;

    this->extensions = GetExtensions(extensions);
    this->ignorePaths = GetIgnorePaths(ignores);

    // put the files to be counted in the queue
    for (const auto& path : GetAllFilesWithExtensionsInDirectories())
    {
        file_queue_mutex.lock();
        file_queue.push(path);
        file_queue_mutex.unlock();
    }
}

/**
 * Count the lines of code in all the files in the queue.
 * @return The total number of lines of code.
 * 
 * This function starts the threads for counting and waits for them to finish. It then returns the total number of lines of code.
 */
unsigned long Counter::Count()
{
    // Display the number of files that will be counted
    std::cout << "Counting " << file_queue.size() << " files..." << std::endl;

    std::vector<std::thread> threads;

    // jobs contains the number of threads to start, but we want each thread to count at least 5 files
    if (file_queue.size() < jobs * 5)
    {
        jobs = static_cast<unsigned int>( file_queue.size() / 5);
        if (jobs == 0)
        {
            jobs = 1;
        }
    }

    // Start threads
    for (unsigned int i = 0; i < jobs; ++i) {
        threads.emplace_back(&Counter::CounterWorker, this);
    }

    // Wait for threads to finish
    for (auto& t : threads) {
        if (t.joinable()) {
            t.join();
        }
    }
    
    // return the total
    return total_lines;
}

/**
 * Check if a path is a directory.
 * @param path The path to check.
 * @return true if the path is a directory, false otherwise.
 */
bool Counter::IsDirectory(std::string path)
{
    // Check if the path is a directory
    return std::filesystem::exists(path) && std::filesystem::is_directory(path);
}

/**
 * Split a string of extensions into a vector of strings.
 * @param extensions A string of all the file extensions to count, separated by semicolons.
 * @return A vector of the extensions.
 */
std::vector<std::string> Counter::GetExtensions(std::string extensions)
{
    // Extensions are separated by semicolons
    // Split them into a vector
    std::vector<std::string> extension_vector{};

    std::stringstream ss{ extensions };
    std::string extension{};
    while (std::getline(ss, extension, ';'))
    {
        extension_vector.push_back(extension);
    }

    return extension_vector;
}

/**
 * Split a string of ignore paths into a vector of strings.
 * @param ignores A string of all the paths to ignore, separated by semicolons.
 * @return A vector of the ignore paths.
 */
std::vector<std::string> Counter::GetIgnorePaths(std::string ignores)
{
    std::vector<std::string> ignoresVector{};

    std::stringstream ss{ ignores };
    std::string ignore{};
    while (std::getline(ss, ignore, ';'))
    {
        ignoresVector.push_back(ignore);
    }

    return ignoresVector;
}

/**
 * Check if a path is in the ignore paths list.
 * @param path The path to check.
 * @return true if the path is ignored, false otherwise.
 * 
 * This function normalizes the path and the ignore paths, then checks if the path is equal to or
 * inside any of the ignore paths.
 */
bool Counter::IsIgnored(std::string path)
{
    normalizePath(path);

    for (auto& p : ignorePaths)
    {
        normalizePath(p);
        
        if (p == path)
            return true;
        
        if (isFileInDirectory(std::filesystem::path{ p }, std::filesystem::path{ path }))
            return true;
    }

    return false;
}

/**
 * Normalize a path by replacing all occurances of the other slash type with the native slash type.
 * @param path The path to normalize.
 * 
 * This function is used to ensure that all paths are represented in the same way, regardless of the
 * OS. It is used to normalize the paths in the ignore list and the paths of the files to be counted.
 */
void Counter::normalizePath(std::string& path) 
{ 
#ifdef _WIN32 
    const char slash = '\\';
    const char otherSlash = '/'; 
#else 
    const char slash = '/'; 
    const char otherSlash = '\\'; 
#endif 

    std::replace(path.begin(), path.end(), otherSlash, slash);
}

/**
 * Count the number of lines in a file specified by the given path.
 * Determines the programming language of the file and utilizes
 * the appropriate line counting mechanism for that language.
 * Currently supports counting lines in C/C++ files. 
 * 
 * @param path The file path to be counted.
 * @return The number of lines in the file.
 *         Returns 0 if the file type is not yet supported.
 */
unsigned long Counter::CountFile(std::string path)
{
    // Get the file language
    FILE_LANGUAGE language = GetFileLanguage(path);

    // use the correct line counting class to count the lines of code in the file
    if (language == FILE_LANGUAGE::C)
    {
        CLineCounter counter;
        return counter.CountLines(path);
    }
    else if (language == FILE_LANGUAGE::Python)
    {
        PyLineCounter counter;
        return counter.CountLines(path);
    }
    else
    {
        return 0;
    }
}

/**
 * Determine the programming language of the file specified by the given path.
 * Currently supports C/C++ and Python files.
 * 
 * @param path The file path to be analyzed.
 * @return The type of file language.
 */
Counter::FILE_LANGUAGE Counter::GetFileLanguage(std::string path)
{
    // if the path ends with .c, .h, .hpp, .cpp, .cxx, .c++, or .cs then set return C
    // if the path ends with .py or .pyw then set return Python

    std::filesystem::path p{ path };
    std::string extension = p.extension().string();

    if (p == ".py" || p == ".pyw")
    {
        return FILE_LANGUAGE::Python;
    }
    else // everything else is C
    {
        return FILE_LANGUAGE::C;
    }
}

/**
 * Checks if a file is in a directory or any of its subdirectories.
 * 
 * @param parentDir The parent directory to check.
 * @param filePath The file path to check.
 * @return true if the file is in the directory or any of its subdirectories, false otherwise.
 * 
 * This function first checks if the parent directory exists and is a directory. It then checks if the file
 * exists. If the file exists, it gets the absolute paths of the parent directory and the file, and checks if the
 * file path is within the parent directory by using the lexically_relative() function. If the file path is within
 * the parent directory, it returns true. Otherwise, it returns false.
 */
bool Counter::isFileInDirectory(const std::filesystem::path& parentDir, const std::filesystem::path& filePath) 
{ 
    if (!std::filesystem::exists(parentDir) || !std::filesystem::is_directory(parentDir)) 
    { 
        return false; 
    } 
    
    auto parentAbs = std::filesystem::absolute(parentDir); 
    auto fileAbs = std::filesystem::absolute(filePath); 
    
    // Check if file exists 
    if (!std::filesystem::exists(fileAbs)) 
    { 
        return false; 
    } 
        
    // Check if file is within the parent directory or any of its subdirectories 
    auto relativePath = fileAbs.lexically_relative(parentAbs); 
    
    return !relativePath.empty() && relativePath.string()[0] != '.'; 
}

/**
 * Retrieves all files with specified extensions from the configured directories.
 * 
 * This function iterates through the paths provided to the Counter instance. It checks each path
 * to determine if it is a directory. If a path is a directory and not ignored, it scans the directory
 * for files with the specified extensions using the DirectoryScanner. Files and directories that are
 * in the ignore list are skipped. All valid files found are added to the result list.
 * 
 * @return A vector of strings containing the paths of files that match the specified extensions and
 *         are not ignored.
 */
std::vector<std::string> Counter::GetAllFilesWithExtensionsInDirectories()
{
    std::vector<std::string> files{};

    for(const auto& path : paths)
    {
        if (IsDirectory(path))
        {
            if (IsIgnored(path))
                continue;

            DirectoryScanner scanner;
            for(const auto& file : scanner.Scan(path, extensions))
            {
                if (IsIgnored(file))
                    continue;

                files.push_back(file);
            }
        }
        else
        {
            if (IsIgnored(path))
                continue;

            files.push_back(path);
        }
    }

    return files;
}

/**
 * Thread function for counting lines of code in files.
 * This function continuously takes paths off the file queue and counts the lines of code in each file.
 * The total number of lines is added to the total_lines atomic variable.
 * When the queue is empty, the thread exits.
 */
void Counter::CounterWorker()
{
    while (true)
    {
        // get a path off the queue
        file_queue_mutex.lock();
        if (file_queue.size() == 0)
        {
            file_queue_mutex.unlock();
            return;
        }
        std::string path = file_queue.front();
        file_queue.pop();
        file_queue_mutex.unlock();

        // count the lines of code in the file
        unsigned long lines = CountFile(path);

        // add to total
        total_lines += lines;
    }
}
