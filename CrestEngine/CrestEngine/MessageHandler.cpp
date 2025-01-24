#include "MessageHandler.h"
#include <iostream>
#include <map>

MessageHandler* MessageHandler::instance = nullptr;

MessageHandler& MessageHandler::Get()
{
	if (instance == nullptr)
	{
		instance = new MessageHandler();
	}
	return *instance;
}

Message::Message(MessageType inType, std::string inMsg) : type(inType), msg(inMsg)
{

}

Message::~Message()
{

}

void MessageHandler::ProcessMessage(Message* message)
{
	switch (message->type)
	{
	case MessageType::String:
		std::cout << message->msg << std::endl;
		break;
	case MessageType::Mouse:
		std::cout << "Mouse message: " << message->msg << std::endl;
		break;
	case MessageType::Object:
		std::cout << "Object message: " << message->msg << std::endl;
		break;
	case MessageType::FloatMessage:
		std::cout << "Float message: " << message->msg << std::endl;
		break;
	default:
		break;
	}
}

//int main()
//{
//	MessageHandler::Get().ProcessMessage(new Message(MessageType::String, "Hello, World!"));
//	MessageHandler::Get().ProcessMessage(new Message(MessageType::Mouse, "Mouse message"));
//	MessageHandler::Get().ProcessMessage(new Message(MessageType::FloatMessage, "Float message"));
//}