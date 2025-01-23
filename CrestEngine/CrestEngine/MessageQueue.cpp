#include "MessageQueue.h"
#include <utility>

void MessageQueue::QueueMessage(Message* message)
{
	static int messageId = 0;
	messages[messageId++] = message;


	ProcessMessages();
}

void MessageQueue::ProcessMessages()
{
	for (auto& message : messages)
	{
		ProcessMessage(message.second);
	}
	messages.clear();
}

void MessageQueue::ProcessMessage(Message* message)
{
	MessageHandler::Get().ProcessMessage(message);
}
