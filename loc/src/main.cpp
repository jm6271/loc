#include <iostream>
#include <thread>
#include <string>
#include <vector>
#include <chrono>
#include <locale>
#include <filesystem>

#include <CLI/CLI.hpp>


// struct for printing out large numbers with commas
struct comma_numpunct : std::numpunct<char>
{
protected:
	std::string do_grouping() const override { return "\3"; }
};


using namespace std;
namespace fs = std::filesystem;

import loc.Counter;

int main(int argc, char** argv)
{
	// Start time of program
	auto start = chrono::high_resolution_clock::now();

	// set up cout to print commas in large numbers
	cout.imbue(std::locale(std::cout.getloc(), new comma_numpunct()));

	CLI::App app{ "loc: count the LoC in a file or project" };

	// Top level options

	unsigned jobs = thread::hardware_concurrency();
	app.add_option("-j,--jobs", jobs, "Number of threads to use")
		->capture_default_str()
		->default_val(jobs);

	bool version = false;
	app.add_flag("-v,--version", version, "Print the version number and exit")
		->capture_default_str()
		->default_val(false);

	bool include_generated = false;
	app.add_flag("--include-hidden", include_generated, "Include hidden files and files in build directories")
		->capture_default_str()
		->default_val(false);

	vector<fs::path> ignore_dirs{};
	app.add_option("-i,--ignore", ignore_dirs, "Directories to ignore");

	vector<fs::path> paths{};
	app.add_option("paths", paths, "Files and Directories to count")
		->check(CLI::ExistingPath)
		->expected(0, -1);

	// Parse the CLI arguments
	CLI11_PARSE(app, argc, argv);

	if (version)
	{
		cout << "loc version 1.6.1\n";
		return 0;
	}
	else if (paths.empty())
	{
		std::cout << app.help() << '\n';
		return 0;
	}

	// Separate files and directories
	vector<fs::path> input_files{};
	vector<fs::path> directory_paths{};
	for (const auto& path : paths)
	{
		if (fs::is_directory(path))
		{
			directory_paths.push_back(path);
		}
		else if (fs::is_regular_file(path))
		{
			input_files.push_back(path);
		}
	}

	Counter counter(jobs, directory_paths, input_files, include_generated, ignore_dirs);
	auto lines = counter.Count();

	// Print the lines of code
	cout << std::endl;
	counter.PrintLanguageBreakdown();
	cout << "\nCounted " << lines << " lines of code";


	// print out the total time it took to count the code
	auto end = std::chrono::high_resolution_clock::now();
	chrono::duration<double, std::milli> duration = end - start;

	cout << " in " << duration.count() << "ms\n";

	return 0;
}
