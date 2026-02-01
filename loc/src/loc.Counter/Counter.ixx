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
#include <map>
#include <iomanip>

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
	* @param directoryPaths A list of directories to scan.
	* @param filePaths A list of files to count in addition to those found in the directory scan.
	* @param includeGenerated If false, common generated directories (obj/out/bin/.git) are excluded.
	* @param ignoreDirs Additional directories to ignore while scanning.
	*
	* This constructor performs a directory scan and builds the internal list of files
	* to be counted according to the provided filters. It does not start the counting
	* threads; call `Count()` to begin processing.
	*/
	Counter(unsigned int jobs, const std::vector<std::filesystem::path>& directoryPaths,
		const std::vector<std::filesystem::path>& filePaths,
		bool includeGenerated, const std::vector<std::filesystem::path>& ignoreDirs)
	{
		this->jobs = jobs;
		this->paths = filePaths;
		expandAllGlobsInPaths(this->paths);

		DirectoryScanner directorScanner{};

		// Create a complete list of directories to ignore
		std::vector<std::filesystem::path> ignore = ignoreDirs;
		if (!includeGenerated)
		{
			std::vector<std::filesystem::path> generatedDirs{ "obj", "out", ".git", "bin" };
			ignore.insert(ignore.end(), generatedDirs.begin(), generatedDirs.end());
		}

		// Get the paths to all the files that match the specified pattern, excluding files in ignored directories
		for (const auto& directoryPath : directoryPaths)
		{
			auto collectedPaths = directorScanner.Scan(directoryPath, ignore);
			paths.insert(paths.end(), collectedPaths.begin(), collectedPaths.end());
		}
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

	void PrintLanguageBreakdown() const
	{
		if (language_line_counts.size() == 0) return;

		// set up cout to print commas in large numbers
		std::cout.imbue(std::locale(std::cout.getloc(), new comma_numpunct()));
		
		std::cout << "+-----------------+----------------------+----------------------+\n";
		std::cout
			<< "| "
			<< std::left
			<< std::setw(15) << "Language" << " | "
			<< std::right << std::setw(20) << "LoC" << " | "
			<< std::right << std::setw(20) << "Files" << " |\n";
		std::cout << "+-----------------+----------------------+----------------------+\n";

		for (const auto& [language, count] : language_line_counts)
		{
			std::string language_name;
			switch (language)
			{
			case FILE_LANGUAGE::C:
				language_name = "C";
				break;
			case FILE_LANGUAGE::CHeader:
				language_name = "C Header";
				break;
			case FILE_LANGUAGE::Cpp:
				language_name = "C++";
				break;
			case FILE_LANGUAGE::CS:
				language_name = "C#";
				break;
			case FILE_LANGUAGE::Go:
				language_name = "Go";
				break;
			case FILE_LANGUAGE::Rust:
				language_name = "Rust";
				break;
			case FILE_LANGUAGE::Python:
				language_name = "Python";
				break;
			case FILE_LANGUAGE::FSharp:
				language_name = "F#";
				break;
			default:
				language_name = "Other";
				break;
			}

			std::ostringstream oss;
			oss.imbue(std::cout.getloc());
			oss << count.first;
			std::string line_col = oss.str() + " lines";

			oss.str("");
			oss.clear();
			oss << count.second;
			std::string file_col = oss.str() + " files";

			std::cout
				<< "| "
				<< std::left
				<< std::setw(15) << language_name << " | "
				<< std::right << std::setw(20) << line_col << " | "
				<< std::right << std::setw(20) << file_col << " |\n";
		}

		std::cout << "+-----------------+----------------------+----------------------+\n";
	}


private:

	enum class FILE_LANGUAGE
	{
		C,
		CHeader,
		Cpp,
		CS,
		Go,
		Rust,
		Python,
		FSharp,
		Other
	};

	unsigned int jobs{};
	std::vector<std::filesystem::path> paths{};
	std::atomic<unsigned long> total_lines{};
	std::atomic<size_t> next_index = 0;

	std::mutex language_line_counts_mutex{};
	std::map<FILE_LANGUAGE, std::pair<unsigned int, unsigned int>> language_line_counts{};

	// struct for printing out large numbers with commas
	struct comma_numpunct : std::numpunct<char>
	{
	protected:
		std::string do_grouping() const override { return "\3"; }
		char do_thousands_sep() const override { return ','; }
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
	unsigned long CountFile(const std::filesystem::path& path)
	{
		// Get the file language
		FILE_LANGUAGE language = GetFileLanguage(path);
		unsigned long lines = 0;

		// use the correct line counting class to count the lines of code in the file
		if (language == FILE_LANGUAGE::Python)
		{
			PyLineCounter counter;
			lines = counter.CountLines(path);
		}
		else if (language == FILE_LANGUAGE::FSharp)
		{
			FSLineCounter counter;
			lines = counter.CountLines(path);
		}
		else
		{
			CLineCounter counter;
			lines = counter.CountLines(path);
		}

		std::scoped_lock lock(language_line_counts_mutex);
		language_line_counts[language].first += lines;
		language_line_counts[language].second++; // File count

		return lines;
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
		std::string extension = path.extension().string();

		if (extension == ".py" || extension == ".pyw")
		{
			return FILE_LANGUAGE::Python;
		}
		else if (extension == ".fs" || extension == ".fsx")
		{
			return FILE_LANGUAGE::FSharp;
		}
		else if (extension == ".c")
		{
			return FILE_LANGUAGE::C;

		}
		else if (extension == ".h")
		{
			return FILE_LANGUAGE::CHeader;
		}
		else if (extension == ".cpp" || extension == ".hpp" || extension == ".cxx" ||
			extension == ".hxx" || extension == ".c++" || extension == ".cc" || extension == ".ixx" || extension == ".cppm")
		{
			return FILE_LANGUAGE::Cpp;
		}
		else if (extension == ".cs")
		{
			return FILE_LANGUAGE::CS;
		}
		else if (extension == ".go")
		{
			return FILE_LANGUAGE::Go;
		}
		else if (extension == ".rs")
		{
			return FILE_LANGUAGE::Rust;
		}
		else
		{
			return FILE_LANGUAGE::Other;
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
