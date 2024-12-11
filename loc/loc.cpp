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

	options_description description{ "loc [options] pattern path1 path2 ..." };
	description.add_options()
		("help,h", bool_switch(&is_help), "Display help")
		("jobs,j", value<unsigned int>()->default_value(default_jobs), "Number of threads to use. Default is number of CPU cores available")
		("ignore,i", value<std::string>(), "Semicolon-separated list of directories and files to ignore")
		("extensions", value<std::string>(), "Semicolon-separated list of extensions to search for")
		("path", value<std::vector<std::string>>(), "Path to file or directory to search");

	positional_options_description positional;
	positional.add("extensions", 1);
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
		cout << "\nExamle: loc -j 8 -i \"program/build\" \".cpp;.h;.hpp\" program tests other.cpp\n";
		return 0;
	}

	if (vm["extensions"].empty()) // No pattern provided
	{
		cerr << "You must provide file extensions\n";
		return -1;
	}

	if (vm["path"].empty()) // No paths provided
	{
		cerr << "You must provide at least one path\n";
		return -1;
	}

	const auto jobs = vm["jobs"].as<unsigned int>();
	const auto& extensions = vm["extensions"].as<std::string>();
	const auto& paths = vm["path"].as<std::vector<std::string>>();
	std::string ignores{};
	if (!vm["ignore"].empty())
	{
		ignores = vm["ignore"].as<std::string>();
	}

	// set up cout to print commas in large numbers
	cout.imbue(std::locale(std::cout.getloc(), new comma_numpunct()));
	
	// Count the lines of code
	Counter counter(jobs, extensions, paths, ignores);
	auto lines = counter.Count();

	// Print the lines of code
	cout << "Counted " << lines << " lines of code";

	// print out the total time it took to count the code
	auto end = std::chrono::high_resolution_clock::now();
	chrono::duration<double, std::milli> duration = end - start;
	
	cout << " in " << duration.count() << "ms\n";

	return 0;
}
