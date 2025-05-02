#include "include/loc.h"

using namespace std;

int main(int argc, char** argv)
{
	// Start time of program
	auto start = chrono::high_resolution_clock::now();

	using namespace boost::program_options;

	// setup program options
	bool is_help{};
	unsigned int default_jobs = std::thread::hardware_concurrency();
	std::string directory{};

	options_description description{ "loc [options] path1 path2 ..." };
	description.add_options()
		("help,h", bool_switch(&is_help), "Display help")
		("jobs,j", value<unsigned int>()->default_value(default_jobs), "Number of threads to use. Default is number of CPU cores available")
		("path", value<std::vector<std::string>>(), "Path to file or directory to search");

	positional_options_description positional;
	positional.add("path", -1);

	command_line_parser parser{ argc, argv };
	parser.options(description);
	parser.positional(positional);

	variables_map vm;
	try
	{
		auto parsed_result = parser.run();
		store(parsed_result, vm);
		notify(vm);
	}
	catch (const std::exception& e)
	{
		cerr << e.what() << "\n";
		return -1;
	}

	if (is_help)
	{
		// show help
		cout << description;
		cout << "\nExamle: loc -j 8 -i src/**/*.cpp src/include/*.h other.cpp\n";
		return 0;
	}

	if (vm["path"].empty()) // No paths provided
	{
		cerr << "Error: At least one path is required\n";
		return -1;
	}

	const auto jobs = vm["jobs"].as<unsigned int>();
	const auto& paths = vm["path"].as<std::vector<std::string>>();

	// set up cout to print commas in large numbers
	cout.imbue(std::locale(std::cout.getloc(), new comma_numpunct()));
	
	// Count the lines of code
	Counter counter(jobs, paths);
	auto lines = counter.Count();

	// Print the lines of code
	cout << "Counted " << lines << " lines of code";

	// print out the total time it took to count the code
	auto end = std::chrono::high_resolution_clock::now();
	chrono::duration<double, std::milli> duration = end - start;
	
	cout << " in " << duration.count() << "ms\n";

	return 0;
}
