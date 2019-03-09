#ifndef __MY_WAIT
#define __MY_WAIT

#include <semaphore.h>
#include <atomic>

class CWait
{
private:
	std::atomic<bool> initialized;
    std::atomic<bool> isWaiting;
    sem_t *semObject;
    int id;

    static int counter;

public:
	CWait();
	void Begin();
	void Set();
    void Wait();
    bool IsWaiting();
};

#endif
