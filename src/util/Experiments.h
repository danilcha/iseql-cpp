#pragma once

#include <list>
#include <iostream>
#include <string>
#include <map>
#include <memory>
#include <string>
#include <iomanip>
#include <functional>
#include <cstring>
#include <vector>
#include <algorithm>

#include "util/Timer.h"
#include "util/String.h"


class Experiments
{
	struct Experiment
	{
		const std::string name;
		const std::string dependency;
		const std::function<void()> code;
		double seconds;
		bool wasExecuted = false;
		std::vector<unsigned long long> measurements;
		Experiment(const std::string& name, const std::string& dependency)
		:
			name(name), dependency(dependency)
		{
		}

		Experiment(const std::string& name, const std::string& dependency, const std::function<void()>& code)
		:
			name(name), dependency(dependency), code(code)
		{
		}

		bool operator == (const std::string& name)
		{
			return this->name == name;
		}
	};



private:
	std::vector<Experiment> experiments;
	std::vector<std::string> disabledExperiments;

	std::vector<const char*> measurementNames;

public:
	Experiments(const char* disabledExperiments)
	:
		disabledExperiments(split(disabledExperiments, ','))
	{
		experiments.reserve(64);

		std::cout << "Experiment              Seconds Result" << std::endl;
		std::cout << "-------------------- ---------- --------------------" << std::endl;
	}

	Experiments() : Experiments("") { }


	~Experiments()
	{
		print();
	}


	void prepare(const char* name, const char* dependency, const std::function<void()>& code)
	{
		experiments.emplace_back(name, dependency, code);
	}


	size_t divideBy = 0;


	template<typename Function>
	void experiment(const std::string& name, const std::string& dependency, const Function& code)
	{
		experiments.emplace_back(name, dependency);
		auto& experiment = experiments.back();
		executeExperiment(experiment, code);
	}



private:
	template<typename Function>
	void executeExperiment(Experiment& experiment, const Function& code)
	{
		using namespace std;

		experiment.wasExecuted = isMeasurementEnabled(experiment.name);
		if (experiment.wasExecuted)
			resolveDependency(experiment.dependency);

		cout << setw(20) << left << experiment.name << flush;

		int width = 10;
		int precision = 3;

		if (experiment.wasExecuted)
		{
			if (experiment.code)
			{
				Timer timer;
				experiment.code();
				experiment.seconds = timer.stop();
				cout << ' ' << setw(width) << right << fixed << setprecision(precision) << experiment.seconds;
				cout << endl;
			}
			else
			{
				Timer timer;
				const auto result = code();
				experiment.seconds = timer.stop();
				cout << ' ' << setw(width) << right << fixed << setprecision(precision) << experiment.seconds;
				cout << ' ' << left << result << endl;
			}
		}
		else
		{
			cout << ' ' << right << setw(width) << "skipped" << endl;
		}
	}




	void resolveDependency(const std::string& name)
	{
		if (name.empty())
			return;

		auto& experiment = operator[](name);

		if (experiment.wasExecuted)
			return;



		executeExperiment(experiment, [] { return 0; });
	}




public:
	void addExperimentResult(const std::string& name, double seconds)
	{
		experiments.emplace_back(name, "");
		auto& experiment = experiments.back();
		experiment.wasExecuted = true;
		experiment.seconds = seconds;

	}

	Experiment& operator[](const std::string& name)
	{
		return *std::find(experiments.begin(), experiments.end(), name);
	}




	void print()
	{
		using namespace std;

		int width = 13;

		cout << "-----BEGIN RESULTS-----" << endl;
		cout << left;

		for (auto& experiment : experiments)
		{
			cout << setw(width) << experiment.name << ' ';
			if (!experiment.measurements.empty())
			{
				for (const auto& name : measurementNames)
					cout << setw(width) << (std::string(experiment.name) + '-' + name) << ' ';
			}
		}
		cout << endl;

		cout.unsetf(ios::floatfield);
//		cout << defaultfloat;

		for (auto& experiment : experiments)
		{
			if (experiment.wasExecuted)
			{
				cout << setw(width) << setprecision(6) << experiment.seconds << ' ';
				if (!experiment.measurements.empty())
				{
					for (auto value : experiment.measurements)
						cout << setw(width) << setprecision(6) << double(value) / double(divideBy) << ' ';
				}
			}
			else
				cout << setw(width) << left << "nan" << ' ';
		}
		cout << endl;

		cout << "-----END RESULTS-----" << endl;
	}


private:
	bool isMeasurementEnabled(const std::string& name)
	{
		for (const auto& disabledExperiment : disabledExperiments)
		{
			if (disabledExperiment == name)
				return false;
		}
		return true;
	}
};

