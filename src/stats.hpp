/*
 * stats.hpp
 *
 *  Created on: Dec 9, 2023
 *      Author: lochnerr
 */

#ifndef STATS_HPP_
#define STATS_HPP_

#include <iostream>		// std::cout
#include <string>		// std::string

using namespace std;

class run_stats {
public:
	run_stats() {};
	~run_stats() {};
	void print();
public:
	int lines=0;
	int tcp1=0;
	int tcp2=0;
	int header=0;
	int content=0;
	int blank=0;
	int other=0;
	int funky=0;
	int funky2=0;
	int calls=0;
	int disconnects=0;
	int no_disco=0;
};

#endif /* STATS_HPP_ */
