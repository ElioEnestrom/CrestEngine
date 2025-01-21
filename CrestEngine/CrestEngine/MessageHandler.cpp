#include "MessageHandler.h"
#include <iostream>

Message::Message(MessageType inType, std::string inMsg) : type(inType), msg(inMsg)
{
	
}


Message::~Message()
{

}

void MessageHandler::ProcessMessage(Message* msg)
{
	switch (msg->type)
	{
	case MessageType::String:
		std::cout << msg->msg << std::endl;
		break;
	case MessageType::Mouse:
		std::cout << "Mouse message: " << msg->msg << std::endl;
		break;
	case MessageType::FloatMessage:
		std::cout << "Float message: " << msg->msg << std::endl;
		break;
	default:
		break;
	}
}