//============================================================================
// Name        : tcpdump-sip.cpp
// Author      : 
// Version     :
// Copyright   : Your copyright notice
// Description : Hello World in C++, Ansi-style
//============================================================================

#include "stats.hpp"
#include "sip.hpp"
#include <fstream>		// ifstream
#include <unistd.h>		// sleep


sip processor;

int main() {
	std::ifstream input("/home/lochnerr/reasnor-sip1.txt");
	std::string line;

	processor.self_tests();

	// Read and process the data.
	// See: https://unix.stackexchange.com/questions/25372/turn-off-buffering-in-pipe
	while (std::getline(input, line))
	{
		try {
			processor.process(line);
		} catch(const std::exception& e) {
			std::cerr << "Caught exception: " << e.what() << "!" << std::endl;
		} catch(...) {
			std::cerr << "Caught catch-all exception!" << std::endl;
		}
	}

	return 0;
}
