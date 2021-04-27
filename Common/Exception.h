#ifndef EXCEPTION_H
#define EXCEPTION_H

#include <exception>


class Not_Found_Ex : public std::exception {

public:

	virtual const char* what() const noexcept override {
		return "Not Found Exception";
	}
};	

class MemoryAlloc_Ex : public std::exception {

public:

	virtual const char* what() const noexcept override {
		return "Memory allocation exception";
	}
};	
#endif
