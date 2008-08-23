/*
 * WorldManager.h
 *
 *  Created on: 16.08.2008
 *      Author: sa-bu
 */

#ifndef __CLONE__WORLD_MANAGER_H__
#define __CLONE__WORLD_MANAGER_H__

#include <OgrePrerequisites.h>
#include "CloneWarrior.h"

enum GameType {
	gtNone,
	gtBomb,
	gtHostage,
	gtVip
};

class WorldManager
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

public:
	WorldManager();
	~WorldManager();

	virtual bool loadMap(const Ogre::String& mapName);
	const Ogre::String& getMapName() const;

	//virtual void newRound();
	//virtual void update(Ogre::Real time);
};

#endif
