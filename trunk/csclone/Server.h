/*
 * Server.h
 *
 *  Created on: 20.08.2008
 *      Author: sa-bu
 */

#ifndef __CLONE__SERVER_H__
#define __CLONE__SERVER_H__

#include "Socket.h"
#include "WorldManagerServer.h"

class Server
{
protected:
	struct Client {
		Socket socket;
		std::string name;
	};

	std::string mName;

	Socket mSocket;
	WorldManagerServer mWorldManagerServer;

	std::vector<Client*> mClients;

public:
	Server(const Ogre::String& name);
	~Server();
	
	void sendServerInfo(sockaddr_in* addr, int len);
	void sendChatMessageToAll(const Ogre::String& text);

	void checkRecv();
};

#endif
