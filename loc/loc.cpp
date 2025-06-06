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
