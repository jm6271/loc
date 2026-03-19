#include "Counter.h"

#include <fstream>
#include <sstream>
#include <future>
#include <iostream>
#include <queue>
#include <iomanip>

Counter::Counter(unsigned int jobs, const std::vector<std::filesystem::path>& paths)
{
	this->jobs = jobs;
	this->paths = paths;

	// go through the paths and expand glob patterns
	expandAllGlobsInPaths(this->paths);
}

Counter::Counter(unsigned int jobs, const std::vector<std::filesystem::path>& directoryPaths,
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

unsigned long Counter::Count()
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

void Counter::PrintLanguageBreakdown() const
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
		case FILE_LANGUAGE::Shell:
			language_name = "Shell";
			break;
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
		case FILE_LANGUAGE::Html:
			language_name = "HTML";
			break;
		case FILE_LANGUAGE::Java:
			language_name = "Java";
			break;
		case FILE_LANGUAGE::JavaScript:
			language_name = "JavaScript";
			break;
		case FILE_LANGUAGE::Kotlin:
			language_name = "Kotlin";
			break;
		case FILE_LANGUAGE::Ruby:
			language_name = "Ruby";
			break;
		case FILE_LANGUAGE::Rust:
			language_name = "Rust";
			break;
		case FILE_LANGUAGE::TypeScript:
			language_name = "TypeScript";
			break;
		case FILE_LANGUAGE::PowerShell:
			language_name = "PowerShell";
			break;
		case FILE_LANGUAGE::Python:
			language_name = "Python";
			break;
		case FILE_LANGUAGE::FSharp:
			language_name = "F#";
			break;
		case FILE_LANGUAGE::Xaml:
			language_name = "XAML";
			break;
		case FILE_LANGUAGE::Xml:
			language_name = "XML";
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

bool Counter::IsDirectory(const std::filesystem::path& path) const
{
	// Check if the path is a directory
	return std::filesystem::exists(path) && std::filesystem::is_directory(path);
}

unsigned long Counter::CountFile(const std::filesystem::path& path)
{
	// Get the file language
	FILE_LANGUAGE language = GetFileLanguage(path);
	unsigned long lines = 0;

	// use the correct line counting class to count the lines of code in the file
	if (language == FILE_LANGUAGE::Python || language == FILE_LANGUAGE::Shell)
	{
		LineCounter counter;
		lines = counter.CountLines(path, "#", "", "");
	}
	else if (language == FILE_LANGUAGE::PowerShell)
	{
		LineCounter counter;
		lines = counter.CountLines(path, "#", "<#", "#>");
	}
	else if (language == FILE_LANGUAGE::Ruby)
	{
		LineCounter counter;
		lines = counter.CountLines(path, "#", "=begin", "=end");
	}
	else if (language == FILE_LANGUAGE::FSharp)
	{
		LineCounter counter;
		lines = counter.CountLines(path, "//", "(*", "*)");
	}
	else if (language == FILE_LANGUAGE::Html || language == FILE_LANGUAGE::Xaml || language == FILE_LANGUAGE::Xml)
	{
		LineCounter counter;
		lines = counter.CountLines(path, "", "<!--", "-->");
	}
	else
	{
		LineCounter counter;
		lines = counter.CountLines(path, "//", "/*", "*/");
	}

	std::scoped_lock lock(language_line_counts_mutex);
	language_line_counts[language].first += lines;
	language_line_counts[language].second++; // File count

	return lines;
}

Counter::FILE_LANGUAGE Counter::GetFileLanguage(const std::filesystem::path& path) const
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
	else if (extension == ".cpp" || extension == ".hpp" || extension == ".cxx" || extension == ".ino" ||
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
	else if (extension == ".java")
	{
		return FILE_LANGUAGE::Java;
	}
	else if (extension == ".js" || extension == ".jsx")
	{
		return FILE_LANGUAGE::JavaScript;
	}
	else if (extension == ".kt" || extension == ".kts")
	{
		return FILE_LANGUAGE::Kotlin;
	}
	else if (extension == ".ps1" || extension == ".psd1" || extension == ".psm1")
	{
		return FILE_LANGUAGE::PowerShell;
	}
	else if (extension == ".rb")
	{
		return FILE_LANGUAGE::Ruby;
	}
	else if (extension == ".rs")
	{
		return FILE_LANGUAGE::Rust;
	}
	else if (extension == ".sh" || extension == ".zsh")
	{
		return FILE_LANGUAGE::Shell;
	}
	else if (extension == ".ts" || extension == ".tsx")
	{
		return FILE_LANGUAGE::TypeScript;
	}
	else if (extension == ".xml")
	{
		return FILE_LANGUAGE::Xml;
	}
	else if (extension == ".html")
	{
		return FILE_LANGUAGE::Html;
	}
	else if (extension == ".xaml")
	{
		return FILE_LANGUAGE::Xaml;
	}
	else
	{
		return FILE_LANGUAGE::Other;
	}
}

void Counter::CounterWorker()
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

void Counter::expandAllGlobsInPaths(const std::vector<std::filesystem::path>& paths_to_expand)
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

void Counter::GetNextPaths(std::vector<std::filesystem::path>& out_paths, int max_paths)
{
	for (int i = 0; i < max_paths; i++)
	{
		size_t next = next_index.fetch_add(1, std::memory_order_relaxed);
		if (next >= paths.size())
			return;
		out_paths.push_back(paths[next]);
	}
}
