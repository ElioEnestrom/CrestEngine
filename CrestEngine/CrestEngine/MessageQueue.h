#pragma once
#include "MessageHandler.h"

class MessageQueue{
public:
	void QueueMessage(Message* message);

private:
	void ProcessMessages();
	void ProcessMessage(Message* message);

	std::map<int, Message*> messages;
};