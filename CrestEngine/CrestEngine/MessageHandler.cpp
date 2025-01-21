#include "MessageHandler.h"
#include <iostream>

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
	case MessageType::FloatMessage:
		std::cout << "Float message: " << message->msg << std::endl;
		break;
	default:
		break;
	}
}