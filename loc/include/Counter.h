#pragma once

#include <vector>
#include <string>
#include <thread>
#include <atomic>
#include <filesystem>
#include <map>
#include <mutex>

#include "DirectoryScanner.h"
#include "ExpandGlob.h"
#include "LineCounter.h"

class Counter
{
public:

	Counter(unsigned int jobs, const std::vector<std::filesystem::path>& paths);

	Counter(unsigned int jobs, const std::vector<std::filesystem::path>& directoryPaths,
		const std::vector<std::filesystem::path>& filePaths,
		bool includeGenerated, const std::vector<std::filesystem::path>& ignoreDirs);

	unsigned long Count();

	void PrintLanguageBreakdown() const;


private:

	enum class FILE_LANGUAGE
	{
		C,
		CHeader,
		Cpp,
		CS,
		Go,
		Html,
		Java,
		JavaScript,
		TypeScript,
		Kotlin,
		Ruby,
		Rust,
		Shell,
		PowerShell,
		Python,
		FSharp,
		Xaml,
		Xml,
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

	bool IsDirectory(const std::filesystem::path& path) const;
	unsigned long CountFile(const std::filesystem::path& path);
	FILE_LANGUAGE GetFileLanguage(const std::filesystem::path& path) const;
	void CounterWorker();
	bool isFileInDirectory(const std::filesystem::path& parentDir, const std::filesystem::path& filePath) const;
	void expandAllGlobsInPaths(const std::vector<std::filesystem::path>& paths_to_expand);
	void GetNextPaths(std::vector<std::filesystem::path>& out_paths, int max_paths);
};
