/*
 * CloneWorldManager.h
 *
 *  Created on: 16.08.2008
 *      Author: sa-bu
 */

#ifndef __WORLD_MANAGER_H__
#define __WORLD_MANAGER_H__

#include <Ogre.h>
#include <ode/ode.h>
#include "CloneWarrior.h"
#include "CloneServer.h"
#include "CloneClient.h"

enum GameType {
	gtNone,
	gtBomb,
	gtHostage,
	gtVip
};

struct Zone {
	Ogre::Vector3 offset;
	Ogre::AxisAlignedBox size;
};

class WorldManager : public Ogre::Singleton<WorldManager>
{
private:
	//Таймеры
	Ogre::Real mGameTime;	//Время на раунд
	Ogre::Real mBuyTime;	//Время в течение которого можно покупать оружие
	Ogre::Real mStartTime;	//Время с начала
	Ogre::Real mBombTime;	//Завод бомбы
	//Бойцы
	std::map<Ogre::String, Warrior*> mWarriors;
	//Счётчики
	int mTerrorScore;
	int mCTerrorScore;
	int mRoundNumber;
	//Информация о карте
	Ogre::String mMapName;
	GameType mGameType;
	//Статическая геометрия
	Ogre::SceneManager* mSceneMgr;
	dWorldID mWorld;
	dSpaceID mGlobalSpace;
	dJointGroupID mContactGroup;
	std::vector<Ogre::SceneNode*> mStaticNodes;
	std::vector<Ogre::Entity*> mStaticEnties;
	dSpaceID mStaticSpace;
	std::vector<dGeomID> mStaticGeometries;

	Server* mServer;

public:
	WorldManager(Ogre::SceneManager* sceneMgr);
	~WorldManager();

	bool loadMap(const Ogre::String& mapName);
	void newRound();
	void update(Ogre::Real time);

    static WorldManager& getSingleton(void);
    static WorldManager* getSingletonPtr(void);
};

#endif
