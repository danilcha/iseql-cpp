cmake_minimum_required(VERSION 2.4)

include_directories(.)


set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS}")

set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -march=native")




add_executable(iseql

		Main.cpp

		model/Interval.h
		model/RelationGenerator.h
		model/Tuple.h
		model/Relation.h
		model/Endpoint.h
		model/Index.h

		algorithms/IEJoin.h
		algorithms/Iterators.h
		algorithms/Joins0.h
		algorithms/Joins1.h
		algorithms/Joins2.h
		algorithms/Joins.h
		algorithms/JoinsInlined.h
		algorithms/LMJoin0.h
		algorithms/LMJoins.h

		util/Arguments.h
		util/Experiments.h
		util/String.h
		util/Timer.h

		MainBefore.h
		MainCommon.h
		MainLatency.h
		MainJoins.h

		containers/GaplessHashMap.h
		containers/GaplessList.h
		containers/Util.h


		util/Aggregates.h)
