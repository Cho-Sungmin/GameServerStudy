#ifndef _TIMER_H_
#define _TIMER_H_

#include <signal.h>
#include <exception>


class Timer{
public:
	virtual
		void awake() = 0;
	virtual
		void asleep() = 0;
	virtual
		void start() = 0;
	virtual
		void stop() = 0;
	
	
};

class Awake_Ex : public std::exception {
public:
	virtual const char* what() const noexcept override {
		return "awake() Exception!";
	}
};
class Asleep_Ex : public std::exception {
public:
	virtual const char* what() const noexcept override {
		return "asleep() Exception!";
	}
};
class Start_Ex : public std::exception {
public:
	virtual const char* what() const noexcept override {
		return "start() Exception!";
	}
};
class Stop_Ex : public std::exception {
public:
	virtual const char* what() const noexcept override {
		return "stop() Exception!";
	}
};
class Handler_Ex : public std::exception {
public:
	virtual const char* what() const noexcept override {
		return "handler() Exception!";
	}
};
class Sig_Ex : public std::exception {
public:
	virtual const char* what() const noexcept override {
		return "sigaction() Exception!";
	}
};

#endif
