/*
 * Client.h
 *
 *  Created on: 20.08.2008
 *      Author: sa-bu
 */

#ifndef __CLONE__CLIENT_H__
#define __CLONE__CLIENT_H__

#include "Socket.h"
#include "WorldManagerClient.h"

class Client
{
public:
	struct ServerInfo {
		Ogre::String mapName;
		Ogre::ushort numPlayers;
		Ogre::ushort maxPlayers;
		sockaddr_in address;
	};

	typedef std::map<Ogre::UTFString, ServerInfo> ServerInfoMap;

protected:
	Socket mSocket;
	WorldManagerClient* mWorldMgr;
	ServerInfoMap mServers;
	bool mConnected;
	sockaddr_in mServerAddress;
	Ogre::String mName;

public:
	Client(WorldManagerClient* worldMgr);
	~Client();

	void sendFindServer();
	void sendConnect(struct sockaddr_in* address);

	void connectToServer(const char* host, unsigned short port);
	void connectToServer(const Ogre::UTFString& serverName);

	bool loadMap(const Ogre::String& mapName);

	void recvServerInfo(char* msg, sockaddr_in* addr);

	inline const ServerInfoMap& getServerList() {return mServers;}
	inline bool isConnected() {return mConnected;}

	void checkRecv();
};

#endif
