#include "stdafx.h"

#include "JobDispatcher.h"

#include <vector>
#include <iostream>
#include <thread>


const int WORKER_THREAD = 8;
const int TEST_OBJECT_COUNT = 10;


class TestObject : public AsyncExecutable
{
public:
	TestObject() : mTestCount(0)
	{}

	void TestFunc0()
	{
		++mTestCount;
		//printf("[%d] TestFunc0 \n", mTestCount);
	}

	void TestFunc1(int b)
	{
		++mTestCount;
		//printf("[%d] TestFunc1 %d\n", mTestCount, b);
	}

	void TestFunc2(double a, int b)
	{
		++mTestCount;
		//printf("[%d] TestFunc2 %f\n", mTestCount, a + b);
	}

	void TestFuncForTimer(int b)
	{
		printf("TestFuncForTimer [%d] \n", b);
	}


	int GetTestCount() { return mTestCount; }
private:

	int mTestCount;

};

TestObject* GTestObject[TEST_OBJECT_COUNT] = { 0, };

void TestWorkerThread(int tid)
{
	LExecuterList = new std::deque<AsyncExecutable*>;
	LTimer = new Timer;

	for (int i = 0; i < 100000; ++i)
	{
		GTestObject[rand() % TEST_OBJECT_COUNT]->DoAsync(&TestObject::TestFunc0);
		GTestObject[rand() % TEST_OBJECT_COUNT]->DoAsync(&TestObject::TestFunc2, double(tid) * 100, i);
		GTestObject[rand() % TEST_OBJECT_COUNT]->DoAsync(&TestObject::TestFunc1, 100);
	}

	GTestObject[tid-1]->DoAsyncAfter(1000, &TestObject::TestFuncForTimer, 337);
	LTimer->DoTimerJob();

	delete LTimer;
	delete LExecuterList;
}



int _tmain(int argc, _TCHAR* argv[])
{

	auto now = Clock::now();

	for (int i = 0; i < TEST_OBJECT_COUNT; ++i)
		GTestObject[i] = new TestObject;

	std::vector<std::thread> threadList;

	for (int i = 0; i <WORKER_THREAD; ++i)
	{
		threadList.push_back(std::thread(TestWorkerThread, i+1));
	}


	for (auto& thread : threadList)
	{
		if (thread.joinable())
			thread.join();
	}

	
	int total = 0;
	for (int i = 0; i < TEST_OBJECT_COUNT; ++i)
		total += GTestObject[i]->GetTestCount();

	printf("TOTAL %d\n", total);


	{
		auto elapsed = Clock::now() - now;
		auto millis = std::chrono::duration_cast<std::chrono::milliseconds>(elapsed).count();

		printf("elapsed: %d\n", millis);
		
	}

	getchar();

	for (int i = 0; i < TEST_OBJECT_COUNT; ++i)
		delete GTestObject[i];


	return 0;
}
