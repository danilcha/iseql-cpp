#include <iostream>
#include <iomanip>
#include <fstream>
#include <vector>

static const char* result_dir;
static const char* output_dir;


//unsigned ceil_div(unsigned a, unsigned b)
//{
//	return (a + b - 1) / b;
//}


auto get_header_and_result_lines(const std::string& filename)
{
	std::cout << "Reading file " << filename << std::flush;
	std::ifstream in(std::string(result_dir) + '/' + filename);

	if (!in)
	{
		std::cout << " Not found" << std::endl;
		return std::make_pair(std::string(), std::string());
	}
	else
		std::cout << std::endl;

	std::string line;
	while (std::getline(in, line) && line != "-----BEGIN RESULTS-----") { /* no nothing */ }

	std::string header;
	std::getline(in, header);
	std::string result;
	std::getline(in, result);

	return std::make_pair(header, result);
}


class Task
{
public:
	std::string dataset;
	std::string filename;
};


void generate_data_file(const std::string& output_file_name, const std::vector<Task>& tasks)
{
	std::cout << "Writing file " << output_file_name << std::endl;
	std::ofstream out;
	out.exceptions(std::ios_base::failbit | std::ios_base::badbit);
	out.open(std::string(output_dir) + '/' + output_file_name);

	std::string global_header;

	out << "dataset    ";

	for (auto task : tasks)
	{
		std::string header;
		std::string result;
		std::tie(header, result) = get_header_and_result_lines(task.filename);

		if (result.empty())
			continue;

		if (global_header.empty())
		{
			global_header = header;
			out << header << '\n';
		}

		if (header != global_header)
		{
			std::cerr << "Invalid header\n"
					<< "Expected: " << global_header << "\n"
					<< "Received: " << header << std::endl;
		}

		out << std::setw(10) << std::left << task.dataset << ' ' << result << '\n';
	}
}


void vary_cardinality(const std::string& output_file_name,
	const std::string& result_file_prefix,
	const std::string& result_file_suffix)
{
	generate_data_file(output_file_name,
	{
		{"1.00e3", result_file_prefix + "1e3" + result_file_suffix},
		{"1.00e4", result_file_prefix + "1e4" + result_file_suffix},
		{"1.00e5", result_file_prefix + "1e5" + result_file_suffix},
		{"1.00e6", result_file_prefix + "1e6" + result_file_suffix},
		{"1.00e7", result_file_prefix + "1e7" + result_file_suffix},
		{"1.00e8", result_file_prefix + "1e8" + result_file_suffix},
		{"1.00e9", result_file_prefix + "1e9" + result_file_suffix},
	});
}




void main__()
{
	output_dir = "../iseql-article/data";

	result_dir = "results/cyprus3";

	vary_cardinality("reverse-during-w1e1.txt", "reverse-during-", "-1e1.txt");
	vary_cardinality("reverse-during-w1e2.txt", "reverse-during-", "-1e2.txt");
	vary_cardinality("reverse-during-w1e3.txt", "reverse-during-", "-1e3.txt");
	vary_cardinality("reverse-during-w1e4.txt", "reverse-during-", "-1e4.txt");
	vary_cardinality("reverse-during-w1e5.txt", "reverse-during-", "-1e5.txt");
	vary_cardinality("reverse-during-w1e6.txt", "reverse-during-", "-1e6.txt");

}


int main()
{
	try
	{
		main__();
	}
	catch (const std::exception& e)
	{
		std::cerr << "Exception: " << e.what() << std::endl;
		return EXIT_FAILURE;
	}
	catch (...)
	{
		std::cerr << "Unknown exception occurred" << std::endl;
		return EXIT_FAILURE;
	}

	return EXIT_SUCCESS;
}
