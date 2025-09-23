#include "include/loc.h"

using namespace std;

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

	// files command
	auto files_command = app.add_subcommand("files", "Count lines of code in one or more files");
	vector<string> input_files{};
	files_command->add_option("paths", input_files, "List of paths to files to count")
		->required()
		->expected(1, -1);

	
	// directory command
	auto directory_command = app.add_subcommand("dir", "Count lines of code in files with provided extensions in a directory");
	string directory_path{};
	vector<string> extensions{};
	bool cpp_extensions = false;
	bool cs_extensions = false;
	bool py_extensions = false;
	bool fs_extensions = false;
	bool c_extensions = false;
	bool include_generated = false;
	vector<string> ignore_dirs{};

	directory_command->add_flag("--cpp", cpp_extensions, "Use C++ extensions (.cpp, .h, .hpp, .cxx, .hxx, .c++, .cc)")
		->capture_default_str()
		->default_val(false);
	directory_command->add_flag("--cs", cs_extensions, "Use C# extensions (.cs)")
		->capture_default_str()
		->default_val(false);
	directory_command->add_flag("--py", py_extensions, "Use Python extensions (.py, .pyw)")
		->capture_default_str()
		->default_val(false);
	directory_command->add_flag("--fs", fs_extensions, "Use F# extensions (.fs, .fsx)")
		->capture_default_str()
		->default_val(false);
	directory_command->add_flag("--c", c_extensions, "Use C extensions (.c, .h)")
		->capture_default_str()
		->default_val(false);

	directory_command->add_option("-e,--extention", extensions, "Extensions of files to count");
	directory_command->add_flag("--include-generated", include_generated, "Include generated files in directories like obj/, out/, .git/, and bin/")
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
		cout << "Counted " << lines << " lines of code";
	}
	else if (*directory_command)
	{
		if (cpp_extensions)
			extensions.insert(extensions.end(), CPP_EXTENSIONS.begin(), CPP_EXTENSIONS.end());
		if (cs_extensions)
			extensions.insert(extensions.end(), CS_EXTENSIONS.begin(), CS_EXTENSIONS.end());
		if (py_extensions)
			extensions.insert(extensions.end(), PY_EXTENSIONS.begin(), PY_EXTENSIONS.end());
		if (fs_extensions)
			extensions.insert(extensions.end(), FS_EXTENSIONS.begin(), FS_EXTENSIONS.begin());
		if (c_extensions)
			extensions.insert(extensions.end(), C_EXTENSIONS.begin(), C_EXTENSIONS.end());

		if (extensions.empty())
		{
			cerr << "Error: No extensions specified\n";
			return -1;
		}

		Counter counter(jobs, directory_path, extensions, include_generated, ignore_dirs);
		auto lines = counter.Count();
		
		cout << "Counted " << lines << " lines of code";
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
