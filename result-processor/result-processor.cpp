#include <iostream>
#include <iomanip>
#include <fstream>
#include <vector>

using std::string;
using std::ifstream;
using std::ofstream;
using std::ios_base;
using std::setw;
using std::flush;
using std::left;
using std::exception;
using std::cerr;
using std::endl;
using std::cout;
using std::vector;

static const char* result_dir;
static const char* output_dir;

static const char* prefix = "";

//unsigned ceil_div(unsigned a, unsigned b)
//{
//	return (a + b - 1) / b;
//}


auto get_header_and_result_lines(const string& filename)
{
	cout << "Reading file " << filename << flush;
	ifstream in(string(result_dir) + '/' + filename);

	if (!in)
	{
		cout << " Not found" << endl;
		return make_pair(string(), string());
	}
	else
		cout << endl;

	string line;
	while (getline(in, line) && line != "-----BEGIN RESULTS-----") { /* do nothing */ }

	string header;
	getline(in, header);
	string result;
	getline(in, result);

	return make_pair(header, result);
}


class Task
{
public:
	string dataset;
	string filename;
};


void generate_data_file(string output_file_name, const vector<Task>& tasks)
{
	output_file_name = prefix + output_file_name;

	cout << "Writing file " << output_file_name << endl;
	ofstream out;
	out.exceptions(ios_base::failbit | ios_base::badbit);
	out.open(string(output_dir) + '/' + output_file_name);

	string global_header;

	out << "dataset    ";

	for (auto task : tasks)
	{
		string header;
		string result;
		tie(header, result) = get_header_and_result_lines(prefix + task.filename);

		if (result.empty())
			continue;

		if (global_header.empty())
		{
			global_header = header;
			out << header << '\n';
		}

		if (header != global_header)
		{
			cout << "Invalid header\n"
					<< "Expected: " << global_header << '\n'
					<< "Received: " << header << endl;
		}

		out << setw(10) << left << task.dataset << ' ' << result << '\n';
	}
}


void vary_cardinality(const string& output_file_name,
	const string& result_file_prefix,
	const string& result_file_suffix)
{
	generate_data_file(output_file_name,
	{
		{"1.00e3", result_file_prefix + "1e3"    + result_file_suffix},
		{"3.16e3", result_file_prefix + "3.16e3" + result_file_suffix},
		{"1.00e4", result_file_prefix + "1e4"    + result_file_suffix},
		{"3.16e4", result_file_prefix + "3.16e4" + result_file_suffix},
		{"1.00e5", result_file_prefix + "1e5"    + result_file_suffix},
		{"3.16e5", result_file_prefix + "3.16e5" + result_file_suffix},
		{"1.00e6", result_file_prefix + "1e6"    + result_file_suffix},
		{"3.16e6", result_file_prefix + "3.16e6" + result_file_suffix},
		{"1.00e7", result_file_prefix + "1e7"    + result_file_suffix},
		{"3.16e7", result_file_prefix + "3.16e7" + result_file_suffix},
		{"1.00e8", result_file_prefix + "1e8"    + result_file_suffix},
		{"3.16e8", result_file_prefix + "3.16e8" + result_file_suffix},
		{"1.00e9", result_file_prefix + "1e9"    + result_file_suffix},
	});
}


void vary_cardinality_and_operator(const string& output_file_name, const string& result_file_middle)
{
	vary_cardinality("exp-reverse-during-"  + output_file_name, "exp-", "-" + result_file_middle + "-reverse-during.txt");
	vary_cardinality("exp-start-preceding-" + output_file_name, "exp-", "-" + result_file_middle + "-start-preceding.txt");
	vary_cardinality("exp-left-overlap-"    + output_file_name, "exp-", "-" + result_file_middle + "-left-overlap.txt");
}


void vary_rw_dataset(const string& output_file_name,
	const string& result_file_prefix,
	const string& result_file_suffix)
{
	generate_data_file(output_file_name,
	{
		{"flight", result_file_prefix + "flight" + result_file_suffix},
		{"inc",    result_file_prefix + "inc"    + result_file_suffix},
		{"web",    result_file_prefix + "web"    + result_file_suffix},
		{"basf",   result_file_prefix + "basf"   + result_file_suffix},
		{"feed",   result_file_prefix + "big"    + result_file_suffix},
//		{"dasa",   result_file_prefix + "dasa"   + result_file_suffix},
		{"wi",     result_file_prefix + "wi"     + result_file_suffix},
		{"fi",     result_file_prefix + "bi"     + result_file_suffix},
	});
}


void rwSuite()
{
	vary_rw_dataset("rw-reverse-during.txt",  "rw-", "-reverse-during.txt");
	vary_rw_dataset("rw-start-preceding.txt", "rw-", "-start-preceding.txt");
	vary_rw_dataset("rw-left-overlap.txt",    "rw-", "-left-overlap.txt");
}


void expSuite()
{
	vary_cardinality_and_operator("w1e2.txt", "1e2");
	vary_cardinality_and_operator("w1e4.txt", "1e4");
	vary_cardinality_and_operator("w1e6.txt", "1e6");
}



void main__()
{
	output_dir = "../iseql-article/data";

//	result_dir = "results/dacha4";
//	expSuite();
//	return;


	result_dir = "results/dacha1";

	expSuite();
	rwSuite();

	generate_data_file("latency.txt",
	{
		{"flight", "latency-ie-flight.txt"},
		{"inc",    "latency-ie-inc.txt"   },
		{"web",    "latency-ie-web.txt"   },
		{"basf",   "latency-ie-basf.txt"  },
		{"feed",   "latency-ie-big.txt"   },
		{"wi",     "latency-ie-wi.txt"    },
		{"fi",     "latency-ie-bi.txt"    },
	});


	prefix = "counters-";
	rwSuite();
	prefix = "ebi-";
	rwSuite();


}


int main()
{
	try
	{
		main__();
	}
	catch (const exception& e)
	{
		cerr << "Exception: " << e.what() << endl;
		return EXIT_FAILURE;
	}
	catch (...)
	{
		cerr << "Unknown exception occurred" << endl;
		return EXIT_FAILURE;
	}

	return EXIT_SUCCESS;
}
