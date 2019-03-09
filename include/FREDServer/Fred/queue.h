#ifndef QUEUE_H
#define QUEUE_H

#include <thread>
#include <list>
#include <utility>
#include <mutex>
#include <atomic>
#include "Fred/fredtopics.h"
#include "Parser/processmessage.h"

using namespace std;

class Fred;

class Queue
{
public:
    Queue(int32_t alfId, Fred* fred);
    ~Queue();

    void newRequest(pair<ProcessMessage *, ChainTopic *> request);

private:
    int32_t id;
    Fred* fred;
    mutex lock;
    thread* queueThread;
    atomic<bool> isFinished, isProcessing;
    list<pair<ProcessMessage *, ChainTopic*> > stack;

    static void clearQueue(Queue* queue);
};

#endif // QUEUE_H
