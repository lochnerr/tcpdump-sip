/*
 * sip.cpp
 *
 *  Created on: Dec 9, 2023
 *      Author: lochnerr
 */

#include "sip.hpp"
#include "stats.hpp"

#include <iostream>		// std::cout
#include <locale>		// std::isdigit

// -----------------------------------------------------------------------

enum CState {
	Unknown,
	Waiting_For_ACK,
	Waiting_For_SIP,
	Disconnected
};

class Call {
public:
	Call() = default;
	~Call() = default;
public:
	int index = 0;
	CState state = Unknown;
};
// -----------------------------------------------------------------------

static const std::string STOP;

sip::~sip()
{
	finish();
	process(STOP);
	s.print();
}

Call *sip::getCall(std::string call_id)
{

	//auto it = calls.find(call_id);
	if (calls.find(call_id) == calls.end()) {
		calls[call_id] = new Call();
		s.calls++;
	}
	return calls[call_id];
}

static Event getEvent(std::string line)
{

	if (line.length() > 8 &&
		std::isdigit(line[0]) &&
		std::isdigit(line[1]) &&
		line[2] == ':' &&
		std::isdigit(line[3]) &&
		std::isdigit(line[4]) &&
		line[5] == ':' &&
		std::isdigit(line[6]) &&
		std::isdigit(line[7]))
		return TCPInd;

	if (line.length() > 4 &&
		line[0] == ' ' && line[1] == ' ' &&
		line[2] == ' ' && line[3] == ' ')
		return Space;

	if (line.length() > 1 && line[0] == '\t')
		return TabData;

	if (line.length() == 1 && line[0] == '\t')
		return Blank;

	return Unexpected;
}

void sip::flush()
{

	if (already_flushed)
		return;

	if (!id.empty() && 1 == 2) {
		std::cout
			<< s.lines
			<< "|" << tcp_data1.substr(0,8)
			<< "|ID:" << id
			<< "|" << sip_info
			<< "|" << from
			<< "|" << to
			<< std::endl;
	}

	tcp_data1.clear();
	tcp_data2.clear();
	sip_info.clear();
	content.clear();
	id.clear();
	from.clear();
	to.clear();
	content_length = -1;
	if (block_seq <= 4) {
		;  // TODO This should be counted or something
	}
	block_seq = 1;
	already_flushed = true;
}

void sip::process_tcp_1(std::string line)
{
	flush();
	already_flushed = false;
	s.tcp1++;
	tcp_data1 = line;
}

void sip::process_tcp_2(std::string line)
{
	s.tcp2++;
	tcp_data2 = line;
}

static std::string get_number(std::string value)
{
	auto start = value.find_first_of(':');
	if (start == std::string::npos)
		return "";

	auto end   = value.find_first_of('@');
	if (end == std::string::npos)
		return "";

	// Remove leading + if there is one.
	if (value[start+1] == '+') start++;

	// Strip leading 1, if 1+10.
	if (value[start+1] == '1' && (end-(start+1)) > 10) start++;

	// "sipvicious"<sip:100@1.1.1.1>;tag=6337373836393462313363340131333435343032383532
	// 0123456789 123456789 123456789 123456789 12345
	// start = 16
	// end = 20
	// start  = 17  start + 1
	// length = 4   end - (start + 1)

	return value.substr(start+1,end-(start+1));
}

void sip::process_call()
{
	Call *call = getCall(id);

	auto cmd = sip_info.substr(0,3);

	// BYE can come at any time.
	if (cmd == "BYE") {
		call->state = Waiting_For_SIP;
	}

	if (call->state == Disconnected) {
		std::cerr << "Hmmm. Event after Disconnect: " << s.lines << " : " << sip_info << std::endl;
	}

	if (call->state == Waiting_For_ACK && cmd == "ACK") {
		s.disconnects++;
		call->state = Disconnected;
	}

	if (call->state == Waiting_For_SIP && cmd == "SIP") {
		s.disconnects++;
		call->state = Disconnected;
	}

	if (call->state != Disconnected) {
		if (sip_info.substr(0,7) == "SIP/2.0") {
			// Is this a failure response?
			if (sip_info[8] > '3') {
				call->state = Waiting_For_ACK;
			}
		}
	}
}

void sip::process_header(const std::string line)
{
	s.header++;
	// If this is the third line of a block, it is the SIP request or result value.
	if (block_seq == 3) {
		sip_info = line.substr(1);
		return;
	}

	// If this is a blank line and the content_length has been set to 0, the data is
	// ready to be processed, so go ahead and do it now, rather than waiting for a
	// subsequent call to trigger the processing.  This allows the program to process
	// call events more quickly.
	if (content_length == 0 && getEvent(line) == Blank) {
		flush();
		return;
	}

	// TODO Do the normal header thing...
	// Get the header type.
	auto index = line.find_first_of(':');
	if (index == std::string::npos) {
		s.funky++;
		if (s.funky < 20) {
			std::cout << "funky *** " << s.lines << " = " << line << " ***" << std::endl;
		}
		return;
	}

	// The type starts after the leading tab character and continues to the colon.
	auto type=line.substr(1,index-1);

	// Strip the value. If there is a value, the line will contain the type name, a colon, a
	// space and, at least, one more character.  Hence, line must be longer than the type
	// length plus 3.
	std::string value;
	if (line.length() >= type.length()+3)
		value = line.substr(index+2);

	if (type == "Call-ID") {
		id = value;
		process_call();
	} else if (type == "From") {
		from = get_number(value);
	} else if (type == "To") {
		to = get_number(value);
		if (sip_info.substr(0,6) == "INVITE" && to.substr(0,6) == "641793") {
			std::cerr << "INVITE for " << to << std::endl;
		}
	} else if (type == "Content-Length") {
		if (value.length() > 0 && value[0] == '0') {
			content_length = 0;
		}
	}
}

void sip::process_content(std::string line)
{
	s.content++;
	// Ignore content for now.
	if (line.length() < 2) {
		cout << "Empty content at line " << s.lines << " ???" << std::endl;
		return;
	}
	if (content.length()>0)
		content.append('|',1);

	content.append(line.substr(1));
}

void sip::process_event(std::string line)
{
	// The first record of a tcpdump formatted sip packet always has a numeric
	// character in column 1 (a time field).  The is TCP/IP info.
	// The second record has four spaces preceding additional TCP/IP info.
	// Every record thereafter starts with a tab (\t) character. The first of these
	// are header records followed by a single record with only a tab character.
	// There will be a blank row (single tab character) that separates the header data
	// from the content data, if there is any content.

	// Sample data follows:

	// 10:54:47.353075 IP (tos 0x68, ttl 255, id 2286, offset 0, flags [none], proto UDP (17), length 1152)
	//     resn-1-199-120-105-75.dsl.netins.net.52514 > resn-1-199-120-105-69.dsl.netins.net.sip: [udp sum ok] SIP, length: 1124
	//	INVITE sip:6417939433@199.120.105.69:5060 SIP/2.0
	//	Via: SIP/2.0/UDP  199.120.105.75:5060;branch=z9hG4bK86348EDC1
	//	From: <sip:8454139401@199.120.105.75>;tag=89271C30-175E
	//		* * *
	// 	Content-Type: application/sdp
	//	Content-Length: 277
	//
	//	v=0
	//	o=CiscoSystemsSIP-GW-UserAgent 9708 9726 IN IP4 199.120.105.75
	//	s=SIP Call
	//	c=IN IP4 199.120.105.75
	//	t=0 0
	//	m=audio 19580 RTP/AVP 0 101 19
	//	c=IN IP4 199.120.105.75
	//	a=rtpmap:0 PCMU/8000
	//	a=rtpmap:101 telephone-event/8000
	//	a=fmtp:101 0-16
	//	a=rtpmap:19 CN/8000
	//	a=ptime:20
	// 10:54:47.354750 IP (tos 0x68, ttl 64, id 48108, offset 0, flags [none], proto UDP (17), length 354)

	// The following is a state transition table for processing this data.
	//
	// State				Event			Action			Next State
	//
	// Nil					TCPInd			save_tcp_1		TCP
	//						default			Ignore			Nil (Unchanged)
	//
	// TCP					TCPInd			save_tcp_1		TCP
	// 						Space			save_tcp_2		Headers
	//						default			Ignore			Nil
	//
	// Headers				TCPInd			save_tcp_1		TCP
	//						TabData			process_header	Headers
	// 						TabOnly			Ignore			Content
	//						default			Ignore			Nil
	//
	// Content				TCPInd			save_tcp_1		TCP
	//						TabData			process_content	Content
	//						TabOnly			Ignore			Content
	//						default			Ignore			Nil

	Event event = getEvent(line);
	if (event == Blank) s.blank++;
	s.lines++;

	block_seq++;

	switch(state) {
	case Nil:
		switch(event) {
		case TCPInd:
			process_tcp_1(line);
			state=TCP;
			break;
		default:
			state=Nil;
			break;
		}
		break;
	case TCP:
		switch(event) {
		case TCPInd:
			process_tcp_1(line);
			state=TCP;
			break;
		case Space:
			process_tcp_2(line);
			state=Headers;
			break;
		default:
			state=Nil;
			break;
		}
		break;
	case Headers:
		switch(event) {
		case TCPInd:
			process_tcp_1(line);
			state=TCP;
			break;
		case TabData:
		case Blank:
			process_header(line);
			break;
		default:
			state=Nil;
			break;
		}
		break;
	case Content:
		switch(event) {
		case TCPInd:
			process_tcp_1(line);
			state=TCP;
			break;
		case TabData:
			process_content(line);
			break;
		default:
			state=Nil;
			break;
		}
		break;
	default:
		break;
	}

	return;
}

void sip::finish()
{

	int c=0;
	flush();

	for (auto const& [id, call] : calls)
	{
	    if (call->state != Disconnected) {
	    	// std::cerr << "No Disco: " << id << std::endl;
	    	s.no_disco++;
	    	if (c++ > 50)
	    		break;
	    }
	}
    cerr << "size of calls " << calls.size() << std::endl;
}

void sip::process(const std::string line)
{

	process_event(line);

	return;
}
