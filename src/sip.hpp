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

class sip {
public:
	sip() = default;
	~sip();
	void flush();
	void process(const std::string line);
	void process_tcp_1(std::string line);
	void process_tcp_2(std::string line);
	void process_call();
	void process_header(std::string line);
	void process_content(std::string line);
	void process_event(std::string line);
	void finish();
	Call *getCall(std::string id);
private:
	run_stats s;
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
	int content_length = -1;
	bool already_flushed = false;
};

#endif /* SIP_HPP_ */
