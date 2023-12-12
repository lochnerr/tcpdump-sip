/*
 * stats.cpp
 *
 *  Created on: Dec 9, 2023
 *      Author: lochnerr
 */

#include "stats.hpp"

void run_stats::print() {

	std::cerr << "Calls "				<< calls				<< std::endl;
	std::cerr << "Disconnects " 		<< disconnects			<< std::endl;
	std::cerr << "No Disco " 			<< no_disco				<< std::endl;

	std::cerr << "Read " << lines		<< " lines."			<< std::endl;
	std::cerr << "Read " << tcp1		<< " tcp 1 lines."		<< std::endl;
	std::cerr << "Read " << tcp2		<< " tcp 2 lines."		<< std::endl;
	std::cerr << "Read " << header		<< " header lines."		<< std::endl;
	std::cerr << "Read " << content		<< " content lines."		<< std::endl;
	std::cerr << "Read " << blank		<< " blank lines."		<< std::endl;
	std::cerr << "Read " << other		<< " other lines."		<< std::endl;
	std::cerr << "Read " << funky		<< " funky lines."		<< std::endl;
	std::cerr << "Read " << funky2		<< " funky2 lines."		<< std::endl;
	std::cerr << "Read " << tcp1+tcp2+header+content+blank+other+funky+funky2
			<< " total lines."	<< std::endl;
}
