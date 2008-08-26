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
	struct ClientInfo {
		//Warrior warrior;
		sockaddr_in address;
	};
	
	typedef std::map<Ogre::UTFString, ClientInfo> ClientInfoMap;

	Socket mSocket;
	WorldManagerServer mWorldMgr;

	ClientInfoMap mClients;

	Ogre::String mName;
	Ogre::ushort mMaxPlayers;
	Ogre::ushort mNumPlayers;

public:
	Server(const Ogre::String& name);
	~Server();
	
	void sendServerInfo(sockaddr_in* addr, int len);
	void sendLoadMap(sockaddr_in* addr, int len);
	void sendChatMessageToAll(const Ogre::String& text);

	bool loadMap(const Ogre::String& mapName);

	void checkRecv();
};

#endif
