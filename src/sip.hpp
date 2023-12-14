/*
 * sip.hpp
 *
 *  Created on: Dec 9, 2023
 *      Author: lochnerr
 */

#ifndef SIP_HPP_
#define SIP_HPP_

#include "stats.hpp"

#include <string>
#include <map>			// std::map
#include <vector>		// std::vector

class Call;

enum State {
    Nil = 1,
    TCP,
    Headers,
    Content
};

enum Event {
    TCPInd = 1,
    Space,
    TabData,
    Blank,
	Unexpected
};

// Utility functions.

Event getEvent(std::string line);
std::string get_number(std::string value);
std::string padID(std::string id);

class sip {
public:
	sip() = default;
	~sip();
	void flush();
	void process(const std::string line);
	void process_tcp_1(std::string line);
	void process_call_id();
	void process_headers();
	void process_header(std::string line);
	void process_event(std::string line);
	void finish();
	Call *getCall(std::string id);
	void self_tests();
private:
	run_stats s;
	int block=0;
	int block_seq=0;
	std::map<std::string, Call *> calls;
	Call *current_call = NULL;
	State state = Nil;
	std::string tcp_data1;
	std::string tcp_data2;
	std::string sip_info;
	std::string content;
	std::string id;
	std::string from;
	std::string to;
	std::string cause;
	std::string sip_cause;
	std::string content_length;
	bool headers_processed = false;
	std::vector<std::string> headers;
};

#endif /* SIP_HPP_ */
