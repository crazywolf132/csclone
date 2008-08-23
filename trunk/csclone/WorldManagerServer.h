/*
 * WorldManagerServer.h
 *
 *  Created on: 20.08.2008
 *      Author: sa-bu
 */

#ifndef __CLONE__WORLD_MANAGER_SERVER_H__
#define __CLONE__WORLD_MANAGER_SERVER_H__

#include <Ogre.h>
#include <ode/ode.h>
#include "WorldManager.h"

struct Zone {
	Ogre::Vector3 offset;
	Ogre::AxisAlignedBox size;
};

class WorldManagerServer : public WorldManager
{
protected:
	dWorldID mWorld;
	dSpaceID mGlobalSpace;
	dJointGroupID mContactGroup;

	dSpaceID mStaticSpace;
	std::vector<dGeomID> mStaticGeometries;

public:
	WorldManagerServer();
	~WorldManagerServer();

	bool loadMap(const Ogre::String& mapName);
};

#endif
