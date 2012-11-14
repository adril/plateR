#include "Client.hpp"
#include "FileTools.hpp"
#include "Tools.hpp"

/* SERVER OUTPUT */

void Client::sendPlateInfo(std::string filename)
{
}

void Client::sendLogout()
{
	Message *message = new Message;
	message->encodeHeader(VSP::LOGOUT);
	message->decodeHeader();

	MessageLogOut *customMessage = new MessageLogOut(*message);
	customMessage->encodeBody();
	customMessage->encodeData();
	sendMessage(*customMessage);
}