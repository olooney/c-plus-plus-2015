#pragma once
#include <cxxabi.h>
#include <string>

std::string demangle(const std::type_info& t) {
	int status;
	auto mangled_name = t.name();
	char* real_name = abi::__cxa_demangle(mangled_name, 0, 0, &status);
	if (status == 0 ) {
		std::string ret = real_name;
		free(real_name);
		return ret;
	} else if ( status == 1 ) {
		return "abi::__cxa_demangle:notEnoughMemory";
	} else if ( status == 1 ) {
		return std::string("abi::__cxa_demangle:invalidABIname:") + mangled_name;
	} else if ( status == 1 ) {
		return "abi::__cxa_demangle:invalidArguments";
	}
	return "abi::__cxa_demangle:unknownStatusCode";
}

template<typename T>
std::string real_type_name(T& t) { return demangle(typeid(t)); }

