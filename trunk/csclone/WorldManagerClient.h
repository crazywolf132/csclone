/*
 * WorldManagerClient.cpp
 *
 *  Created on: 16.08.2008
 *      Author: sa-bu
 */

#ifndef __CLONE__WORLD_MANAGER_CLIENT_H__
#define __CLONE__WORLD_MANAGER_CLIENT_H__

#include <Ogre.h>
#include "WorldManager.h"

class WorldManagerClient : public WorldManager
{
protected:
	//Статическая геометрия
	Ogre::SceneManager* mSceneMgr;
	std::vector<Ogre::SceneNode*> mStaticNodes;
	std::vector<Ogre::Entity*> mStaticEnties;

public:
	WorldManagerClient(Ogre::SceneManager* sceneMgr);
	~WorldManagerClient();

	bool loadMap(const Ogre::String& mapName);
};

#endif
