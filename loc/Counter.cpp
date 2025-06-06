#include "include/Counter.h"
#include "include/CLineCounter.h"
#include <sstream>
#include <filesystem>
#include "include/DirectoryScanner.h"
#include <future>
#include <iostream>
#include "include/PyLineCounter.h"
#include "include/FSLineCounter.h"
#include "include/ExpandGlob.h"

/**
 * Constructor for the Counter class.
 * @param jobs The number of threads to use.
 * @param paths A vector of paths to search for files. Cannot include directories.
 * 
 * This constructor initializes the Counter object and starts the threads for counting. The
 * threads are started after the constructor is finished.
 */
Counter::Counter(unsigned int jobs, const std::vector<std::string>& paths)
{
    this->jobs = jobs;
    this->paths = paths;

    // go through the paths and expand glob patterns
    expandAllGlobsInPaths(this->paths);

    // put the files to be counted in the queue
    for (const auto& path : this->paths)
    {
        std::scoped_lock<std::mutex> lock(file_queue_mutex);
        file_queue.push(path);
    }
}


Counter::Counter(unsigned int jobs, std::string &directoryPath, std::vector<std::string>& extensions)
{
    this->jobs = jobs;

    // Get the paths to all the files that match the specified pattern, excluding files that match any patterns in ignorePatterns
    DirectoryScanner directorScanner{};
    paths = directorScanner.Scan(directoryPath, extensions);

    // put the files to be counted in the queue
    for (const auto& path : this->paths)
    {
        std::scoped_lock<std::mutex> lock(file_queue_mutex);
        file_queue.push(path);
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

    std::vector<std::jthread> threads;

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
bool Counter::IsDirectory(const std::filesystem::path& path) const
{
    // Check if the path is a directory
    return std::filesystem::exists(path) && std::filesystem::is_directory(path);
}

/**
 * Normalize a path by replacing all occurances of the other slash type with the native slash type.
 * @param path The path to normalize.
 * 
 * This function is used to ensure that all paths are represented in the same way, regardless of the
 * OS. It is used to normalize the paths in the ignore list and the paths of the files to be counted.
 */
void Counter::normalizePath(std::string& path) const 
{ 
    std::filesystem::path p {path};
    path = p.make_preferred().string();
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
    else if (language == FILE_LANGUAGE::FSharp)
    {
        FSLineCounter counter;
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
Counter::FILE_LANGUAGE Counter::GetFileLanguage(const std::string& path) const
{
    // if the path ends with .c, .h, .hpp, .cpp, .cxx, .c++, or .cs then return C
    // if the path ends with .py or .pyw then return Python
    // if the path ends with .fs or .fsx then return FSharp

    std::filesystem::path p{ path };
    std::string extension = p.extension().string();

    if (extension == ".py" || extension == ".pyw")
    {
        return FILE_LANGUAGE::Python;
    }
    else if (extension == ".fs" || extension == ".fsx")
    {
        return FILE_LANGUAGE::FSharp;
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
bool Counter::isFileInDirectory(const std::filesystem::path& parentDir, const std::filesystem::path& filePath) const
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
        std::string path{};
        {
            std::scoped_lock<std::mutex> lock(file_queue_mutex);
            
            if (file_queue.size() == 0)
            {
                return;
            }
            path = file_queue.front();
            file_queue.pop();          
        }


        // count the lines of code in the file
        unsigned long lines = CountFile(path);

        // add to total
        total_lines += lines;
    }
}

void Counter::expandAllGlobsInPaths(std::vector<std::string>& paths)
{
    std::vector<std::string> copyVector = paths;
    paths.clear();

    for (auto& path : copyVector)
    {
        // run glob on the path
        ExpandGlob expander{};
        expander.expand_glob(path, paths);
    }
}
