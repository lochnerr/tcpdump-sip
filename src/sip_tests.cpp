/*
 * sip_tests.cpp
 *
 *  Created on: Dec 13, 2023
 *      Author: lochnerr
 */

#include "sip.hpp"

void sip::self_tests()
{
	if (getEvent("17:34:25.794180") != TCPInd)
		throw std::invalid_argument("Failed test 1 getEvent('17:34:25.794180')");
	if (getEvent("    any") != Space)
		throw std::invalid_argument("Failed test 2 getEvent('    any')");
	if (getEvent("\t ") != TabData)
		throw std::invalid_argument("Failed test 3 getEvent('\t ')");
	if (getEvent("\t") != Blank)
		throw std::invalid_argument("Failed test 4 getEvent('\t')");
	if (getEvent("   ") != Unexpected)
		throw std::invalid_argument("Failed test 5 getEvent('   ')");

	if (get_number("\"sipvicious\"<sip:100@1.1.1.1>") != "100")
		throw std::invalid_argument("Failed test 6 get_number('\"sipvicious\"<sip:100@1.1.1.1>')");
	if (get_number("<sip:2144153740@199.120.105.69>") != "2144153740")
		throw std::invalid_argument("Failed test 7 get_number('<sip:2144153740@199.120.105.69>')");
	if (get_number("<sip:+2144153740@199.120.105.69>") != "2144153740")
		throw std::invalid_argument("Failed test 8 get_number('<sip:+2144153740@199.120.105.69>')");
	if (get_number("<sip:+12144153740@199.120.105.69>") != "2144153740")
		throw std::invalid_argument("Failed test 9 get_number('<sip:+12144153740@199.120.105.69>')");

	if (padID("123456789 123456789 123456789 123456789 123456789 12345") !=
			"123456789 123456789 123456789 123456789 123456789 12345     ")
		throw std::invalid_argument("Failed test 10 padID('123456789 123456789 123456789 123456789 123456789 12345')");

}



