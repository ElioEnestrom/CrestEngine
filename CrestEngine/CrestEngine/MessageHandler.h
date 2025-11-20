#pragma once

#include <string>
#include <map>
#include <mutex>

enum class MessageType {
	String,
	Mouse,
	Object,
	FloatMessage
};

class Message {
public:
	Message(MessageType inType, std::string inMsg);
	virtual ~Message();

	MessageType type;
	std::string msg;
};

class MessageHandler {
public:
	void ProcessMessage(Message* message);
	static MessageHandler& Get();

private:
	static MessageHandler* instance;
	//MessageHandler() {};
};
