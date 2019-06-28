#pragma once
#include <chrono>
#include <ctime>
#include <string>

using namespace std::chrono;
using namespace std;

class ElapsedTimer
{
	// allows for easy reporting of elapsed time
private:
	high_resolution_clock::time_point t1;

public:
	ElapsedTimer();

	long elapsed() {
		duration<double, std::milli> time_span = high_resolution_clock::now() - t1;
		return time_span.count();
	}

	void reset() {
		t1 = high_resolution_clock::now();
	}

	std::string toString() {
		// now resets the timer...
		std::string ret = elapsed() + " ms elapsed";
		reset();
		return ret;
	}

	static void testElapsed(std::string args[])
	{
		ElapsedTimer* t = new ElapsedTimer();
		// cout << ("ms elasped: " + t->elapsed());
	}
};

