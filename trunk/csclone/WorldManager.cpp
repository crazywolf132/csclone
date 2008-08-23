/*
 * WorldManager.cpp
 *
 *  Created on: 16.08.2008
 *      Author: sa-bu
 */

#include "WorldManager.h"

WorldManager::WorldManager()
	: mGameTime(0), mBuyTime(0), mStartTime(0), mBombTime(0),
	mTerrorScore(0), mCTerrorScore(0), mRoundNumber(-1), mGameType(gtNone)
{
}

WorldManager::~WorldManager()
{
}

bool WorldManager::loadMap(const Ogre::String& mapName)
{
	//Socket socket;
	//socket.bind("169.256.101.92", 27015);
	//socket.bind("192.168.1.3", 27015);
	//socket.bind("127.0.0.1", 27015);

	//mServer = new Server("192.168.0.1", 5000);

	mGameTime = 1800;
	mBuyTime = 60;
	mStartTime = 10;
	mBombTime = 0;
	mTerrorScore = 0;
	mCTerrorScore = 0;
	mRoundNumber = -1;
	
	mMapName = mapName;

	return true;
}

const Ogre::String& WorldManager::getMapName() const
{
	return mMapName;
}
