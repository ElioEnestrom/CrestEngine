#pragma once

#include <string>

namespace MessageHandler {
	void ProcessMessage(Message* message);
}

enum class MessageType {
	String,
	Mouse,
	FloatMessage
};

class Message {
public:
	Message(MessageType inType, std::string inMsg);
	virtual ~Message();

	MessageType type;
	std::string msg;
};