#pragma once
#include "MessageHandler.h"
#include <mutex>
#include <thread>
#include <queue>
#include <condition_variable>

class MessageQueue{
public:
	void QueueMessage(Message* message);
	static MessageQueue& Get();
	void WorkerThreadStart(int numThreads);
	void WorkerThreadEnd();
	bool IsEmpty();

	~MessageQueue();
private:
	void ProcessMessage(Message* message);
	void WorkerThread();


	std::queue<Message*> messages;
	std::mutex queueMutex, testingMutex;
	std::condition_variable condition;
	std::vector<std::thread> workerThreads;
	std::atomic<bool> running = false ;



	static MessageQueue* instance;
};