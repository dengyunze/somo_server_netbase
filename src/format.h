#pragma once

#include <string>
#include <stdint.h>
#include <set>
	
class format {
public:
	static std::string		uint2str16(uint16_t i);
	static std::string		uint2str32(uint32_t i);
	static std::string		uint2str64(uint64_t i);
	static uint16_t			str2uint16(const std::string& str);
	static uint32_t			str2uint32(const std::string& str);
	static uint64_t			str2uint64(const std::string& str);

	static std::string&		trim(std::string &s);	
	static std::string      uint32ids2str(const std::set<uint32_t>& ids, bool hexStyle=false);
    static std::string      uint64ids2str(const std::set<uint64_t>& ids, bool hexStyle=false);
};
