#include "MessageQueue.h"
#include <utility>
#include <iostream>

MessageQueue* MessageQueue::instance = nullptr;

MessageQueue& MessageQueue::Get()
{
	if (instance == nullptr)
	{
		instance = new MessageQueue();
	}
	return *instance;
}

void MessageQueue::WorkerThreadStart(int numThreads)
{    
	running = true;

	for (int i = 0; i < numThreads; ++i)
	    workerThreads.emplace_back(&MessageQueue::WorkerThread, this);
}

void MessageQueue::WorkerThreadEnd()
{
	running = false;
	condition.notify_all();

    for (std::thread& thread : workerThreads)
    {
        if (thread.joinable())
            thread.join();
	}

	workerThreads.clear();
}

MessageQueue::~MessageQueue()
{
    WorkerThreadEnd();
}

void MessageQueue::QueueMessage(Message* message)
{
    {
        std::lock_guard<std::mutex> lock(queueMutex);
        messages.push(message);
    }

	condition.notify_one();
}

void MessageQueue::ProcessMessage(Message* message)
{
	MessageHandler::Get().ProcessMessage(message);
}

void MessageQueue::WorkerThread()
{
    while (running)
    {
        Message* message = nullptr;

        {
            std::unique_lock<std::mutex> lock(queueMutex);

            condition.wait(lock, [this] { return !messages.empty() || !running; });

            if (!running && messages.empty())
                break;

            if (!messages.empty())
            {
                message = messages.front();
                messages.pop();
            }
        }

        if (message)
        {
            {
                std::lock_guard<std::mutex> lock(testingMutex);
                std::cout << "Processing message in thread " << std::this_thread::get_id() << std::endl;
            }
            MessageHandler::Get().ProcessMessage(message);
        }
    }
}
bool MessageQueue::IsEmpty()
{
    std::lock_guard<std::mutex> lock(queueMutex);
    return messages.empty();
}