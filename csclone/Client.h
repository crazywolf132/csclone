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
protected:
	Socket mSocket;
	WorldManagerClient* mWorldMgr;

public:
	Client(WorldManagerClient* worldMgr);
	~Client();

	void sendFindServer();

	bool loadMap(const Ogre::String& mapName);

	void checkRecv();
};

#endif
