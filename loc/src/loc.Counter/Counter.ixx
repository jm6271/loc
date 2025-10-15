module;

#include <vector>
#include <string>
#include <thread>
#include <atomic>
#include <filesystem>
#include <fstream>
#include <sstream>
#include <future>
#include <iostream>
#include <queue>

export module loc.Counter:Counter;

import loc.Filesystem;
import :CLineCounter;
import :PyLineCounter;
import :FSLineCounter;

export class Counter
{
public:

	/**
	* Constructor for the Counter class.
	* @param jobs The number of threads to use.
	* @param paths A vector of paths to search for files. Cannot include directories.
	*
	* This constructor initializes the Counter object and expands any glob patterns
	* found in the provided paths. It does not start counting—call `Count()` to start
	* the work using the configured number of threads.
	*/
	Counter(unsigned int jobs, const std::vector<std::filesystem::path>& paths)
	{
		this->jobs = jobs;
		this->paths = paths;

		// go through the paths and expand glob patterns
		expandAllGlobsInPaths(this->paths);
	}

	/**
	* Alternate constructor that scans a directory for files matching the given
	* extensions and populates the internal path list.
	*
	* @param jobs The number of threads to use.
	* @param directoryPath The root directory to scan for matching files.
	* @param extensions A list of file extensions to include (e.g. {".cpp", ".h"}).
	* @param includeGenerated If false, common generated directories (obj/out/bin/.git) are excluded.
	* @param ignoreDirs Additional directories to ignore while scanning.
	*
	* This constructor performs a directory scan and builds the internal list of files
	* to be counted according to the provided filters. It does not start the counting
	* threads; call `Count()` to begin processing.
	*/
	Counter(unsigned int jobs, const std::filesystem::path& directoryPath, const std::vector<std::string>& extensions,
		bool includeGenerated, const std::vector<std::filesystem::path>& ignoreDirs)
	{
		this->jobs = jobs;

		DirectoryScanner directorScanner{};

		// Create a complete list of directories to ignore
		std::vector<std::filesystem::path> ignore = ignoreDirs;
		if (!includeGenerated)
		{
			auto generatedDirs = directorScanner.FindIgnoredDirectories(directoryPath, { "obj", "out", ".git", "bin" });
			ignore.insert(ignore.end(), generatedDirs.begin(), generatedDirs.end());
		}

		// Get the paths to all the files that match the specified pattern, excluding files in ignored directories
		paths = directorScanner.Scan(directoryPath, extensions, ignore);
	}

	/**
	* Start counting lines of code across the configured files using a thread pool.
	*
	* Ensures a reasonable number of files per thread (at least ~10). Spawns `jobs`
	* threads which each fetch batches of work and count lines using the language
	* specific counters. The total number of counted lines is returned.
	*
	* @return The total number of lines counted across all files.
	*/
	unsigned long Count()
	{
		// Display the number of files that will be counted
		std::cout << "Counting " << paths.size() << " files..." << std::endl;

		std::vector<std::jthread> threads;

		// jobs contains the number of threads to start, but we want each thread to count at least 10 files
		if (paths.size() < size_t(jobs * 10))
		{
			jobs = static_cast<unsigned int>(paths.size() / 10);
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


private:
	unsigned int jobs{};
	std::vector<std::filesystem::path> paths{};
	std::atomic<unsigned long> total_lines{};
	std::atomic<size_t> next_index = 0;

	enum class FILE_LANGUAGE
	{
		C,
		Python,
		FSharp
	};

	/**
	* Check if a path refers to an existing directory on the filesystem.
	*
	* @param path The path to check.
	* @return true if the path exists and is a directory, false otherwise.
	*/
	bool IsDirectory(const std::filesystem::path& path) const
	{
		// Check if the path is a directory
		return std::filesystem::exists(path) && std::filesystem::is_directory(path);
	}

	/**
	* Count the number of lines in a file specified by the given path.
	* Determines the programming language of the file and delegates to the
	* appropriate language-specific line counter.
	*
	* @param path The file path to be counted.
	* @return The number of lines in the file. Returns 0 if the file type is unsupported
	*         or if the language-specific counter fails to read the file.
	*/
	unsigned long CountFile(const std::filesystem::path& path) const
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
	* The decision is made purely on the file extension.
	*
	* @param path The file path to be analyzed.
	* @return The detected file language enum value.
	*/
	FILE_LANGUAGE GetFileLanguage(const std::filesystem::path& path) const
	{
		// if the path ends with .c, .h, .hpp, .cpp, .cxx, .c++, or .cs then return C
		// if the path ends with .py or .pyw then return Python
		// if the path ends with .fs or .fsx then return FSharp

		std::string extension = path.extension().string();

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
	* Worker function executed by each counting thread. Repeatedly fetches a batch
	* of file paths and counts lines for each file until there are no more files.
	*
	* This method is thread-safe because it obtains work via the atomic `next_index`.
	*/
	void CounterWorker()
	{
		std::string path{};

		while (next_index < paths.size())
		{
			// Get 10 paths to process
			std::vector<std::filesystem::path> paths_to_process{};
			GetNextPaths(paths_to_process, 10);

			for (const auto& current_path : paths_to_process)
			{
				// count the lines of code in the file
				unsigned long lines = CountFile(current_path);

				// add to total
				total_lines += lines;
			}
		}
	}

	/**
    * Checks if a file is located inside a directory (including subdirectories).
	*
	* @param parentDir The parent directory to check.
	* @param filePath The file path to check.
	* @return true if the file is in the directory or any of its subdirectories, false otherwise.
	*
	* This function verifies that both the parent directory and the file exist, then
	* compares their absolute paths using `lexically_relative` to determine whether
	* the file is contained within the parent directory tree.
	*/
	bool isFileInDirectory(const std::filesystem::path& parentDir, const std::filesystem::path& filePath) const
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
	* Expand any glob patterns found in the provided paths and populate the
	* internal `paths` vector with the resulting file paths.
	*
	* @param paths_to_expand The input list of paths which may include glob patterns.
	* The function clears the internal `paths` and appends results produced by the
	* `ExpandGlob::expand_glob` helper for each entry.
	*/
	void expandAllGlobsInPaths(const std::vector<std::filesystem::path>& paths_to_expand)
	{
		std::vector<std::filesystem::path> copyVector = paths_to_expand;
		paths.clear();

		for (auto const& path : copyVector)
		{
			// run glob on the path
			ExpandGlob expander{};
			expander.expand_glob(path, paths);
		}
	}

	/**
	* Populates `out_paths` with up to `max_paths` file paths from the internal
	* `paths` vector using an atomic index. This is safe to call concurrently from
	* multiple threads.
	*
	* @param out_paths Reference to a vector that will receive the next paths.
	* @param max_paths Maximum number of paths to fetch in this call.
	*/
	void GetNextPaths(std::vector<std::filesystem::path>& out_paths, int max_paths)
	{
		for (int i = 0; i < max_paths; i++)
		{
			size_t next = next_index.fetch_add(1, std::memory_order_relaxed);
			if (next >= paths.size())
				return;
			out_paths.push_back(paths[next]);
		}
	}
};
