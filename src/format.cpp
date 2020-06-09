#include "format.h"
#include <sstream>

std::string		format::uint2str16(uint16_t i) {
	std::string str;
	std::stringstream ss;
	ss << i;
	ss >> str;
	return str;
}

std::string		format::uint2str32(uint32_t i) {
	std::string str;
	std::stringstream ss;
	ss << i;
	ss >> str;
	return str;
}

std::string		format::uint2str64(uint64_t i) {
	std::string str;
	std::stringstream ss;
	ss << i;
	ss >> str;
	return str;
}

uint16_t		format::str2uint16(const std::string& str) {
	uint16_t val;
	std::stringstream ss;
	ss<<str;
	ss>>val;
	return val;
}

uint32_t		format::str2uint32(const std::string& str) {
	uint32_t val;
	std::stringstream ss;
	ss<<str;
	ss>>val;
	return val;
}

uint64_t		format::str2uint64(const std::string& str) {
	uint64_t val;
	std::stringstream ss;
	ss<<str;
	ss>>val;
	return val;
}

std::string& 	format::trim(std::string &s) 
{
    if( s.empty() ) {
        return s;
    }

    s.erase(0,s.find_first_not_of(" "));
    s.erase(s.find_last_not_of(" ") + 1);
    return s;
}

std::string 	format::uint32ids2str(const std::set<uint32_t>& ids, bool hexStyle/*=false*/)
{
    int idx = 0;
    std::ostringstream oss;
    if (hexStyle)
        oss << std::hex;
    oss << "[";
    for ( std::set<uint32_t>::const_iterator io = ids.begin(); io != ids.end(); ++io )
    {
        if (idx++ > 0)
            oss << ",";
        oss << *io;
    }
    oss << "]";
    return oss.str();
}

std::string format::uint64ids2str(const std::set<uint64_t>& ids, bool hexStyle/*=false*/)
{
    int idx = 0;
    std::ostringstream oss;
    if (hexStyle)
        oss << std::hex;
    oss << "[";
    for ( std::set<uint64_t>::const_iterator io = ids.begin(); io != ids.end(); ++io )
    {
        if (idx++ > 0)
            oss << ",";
        oss << *io;
    }
    oss << "]";
    return oss.str();
}
