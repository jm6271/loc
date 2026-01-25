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

// Extensions for the different programming languages
const std::vector<std::string> C_EXTENSIONS = { ".c", ".h" };
const std::vector<std::string> CPP_EXTENSIONS = { ".cpp", ".h", ".hpp", ".cxx", ".hxx", ".c++", ".cc", ".ixx", ".cppm" };
const std::vector<std::string> PY_EXTENSIONS = { ".py", ".pyw" };
const std::vector<std::string> FS_EXTENSIONS = { ".fs", ".fsx" };
const std::vector<std::string> CS_EXTENSIONS = { ".cs" };


using namespace std;
namespace fs = std::filesystem;

import loc.Counter;

int main(int argc, char** argv)
{
	// Start time of program
	auto start = chrono::high_resolution_clock::now();

	// set up cout to print commas in large numbers
	cout.imbue(std::locale(std::cout.getloc(), new comma_numpunct()));

	CLI::App app{"loc: count the LoC in a file or project"};

	// Top level options

	unsigned jobs = thread::hardware_concurrency();
	app.add_option("-j,--jobs", jobs, "Number of threads to use")
		->capture_default_str()
		->default_val(jobs);

	bool version = false;
	app.add_flag("--version", version, "Print the version number and exit")
		->capture_default_str()
		->default_val(false);

	// files command
	auto files_command = app.add_subcommand("files", "Count lines of code in one or more files");
	vector<fs::path> input_files{};
	files_command->add_option("paths", input_files, "List of paths to files to count")
		->required()
		->expected(1, -1);

	
	// directory command
	auto directory_command = app.add_subcommand("dir", "Count lines of code in files with provided extensions in a directory");
	fs::path directory_path{};
	bool include_generated = false;
	vector<fs::path> ignore_dirs{};

	directory_command->add_flag("--include-hidden", include_generated, "Include generated files in hidden directories like obj/, out/, .git/, and bin/")
		->capture_default_str()
		->default_val(false);
	directory_command->add_option("-i,--ignore", ignore_dirs, "Directories to ignore (relative to the provided directory)");
	directory_command->add_option("directory", directory_path, "Directory to search")
		->required()
		->expected(1);
	
	// Parse the CLI arguments
	CLI11_PARSE(app, argc, argv);

	if (*files_command)
	{
		// Count the lines of code
		Counter counter(jobs, input_files);
		auto lines = counter.Count();

		// Print the lines of code
		cout << std::endl;
		counter.PrintLanguageBreakdown();
		cout << "\nCounted " << lines << " lines of code";
	}
	else if (*directory_command)
	{
		Counter counter(jobs, directory_path, include_generated, ignore_dirs);
		auto lines = counter.Count();
		
		cout << std::endl;
		counter.PrintLanguageBreakdown();
		cout << "\nCounted " << lines << " lines of code";
	}
	else if (version)
	{
		cout << "loc version 1.5.0\n";
		return 0;
	}
	else
	{
		std::cout << app.help() << '\n';
		return 0;
	}

	// print out the total time it took to count the code
	auto end = std::chrono::high_resolution_clock::now();
	chrono::duration<double, std::milli> duration = end - start;
	
	cout << " in " << duration.count() << "ms\n";

	return 0;
}
