#include "Alfred/wait.h"
#include <time.h>
#include <unistd.h>
#include <iostream>
#include <fcntl.h>
#include <string.h>
#include <string>

int CWait::counter = 0;

CWait::CWait()
{
	initialized = false;
    isWaiting = false;
}

void CWait::Begin()
{
	if (!initialized)
	{
        //sem_init(&semObject, 0, 0);
        //sem_unlink("/fredwait");
        id = counter++;
        semObject = sem_open(("/fredwait" + std::to_string(id)).c_str(), O_CREAT, 0666, 0);
	//std::cout << strerror(errno) << "\n";
		initialized = true;
	}
}

void CWait::Set()
{
	if (initialized)
	{
        sem_post(semObject);
	}
}

void CWait::Wait()
{
	if (initialized)
	{
        isWaiting = true;

        sem_wait(semObject);
        //std::cout << strerror(errno) << "\n";
        //sem_destroy(&semObject);
        sem_close(semObject);
        sem_unlink(("/fredwait" + std::to_string(id)).c_str());

        isWaiting = false;
		initialized = false;
	}
}

bool CWait::IsWaiting()
{
    return isWaiting;
}
