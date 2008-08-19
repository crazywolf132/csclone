/*
 * WorldManager.cpp
 *
 *  Created on: 16.08.2008
 *      Author: sa-bu
 */

#include "WorldManager.h"

template<> WorldManager* Ogre::Singleton<WorldManager>::ms_Singleton = 0;
WorldManager* WorldManager::getSingletonPtr(void)
{
    return ms_Singleton;
}
WorldManager& WorldManager::getSingleton(void)
{
    assert(ms_Singleton);  return (*ms_Singleton);
}
//-----------------------------------------------------------------------
WorldManager::WorldManager(Ogre::SceneManager* sceneMgr)
	: mGameTime(0), mBuyTime(0), mStartTime(0), mBombTime(0),
	mTerrorScore(0), mCTerrorScore(0), mRoundNumber(-1), mGameType(gtNone),
	mSceneMgr(sceneMgr), mWorld(0), mGlobalSpace(0), mStaticSpace(0)
{
	dInitODE();
	mWorld = dWorldCreate();
	mGlobalSpace = dHashSpaceCreate(0);
	mContactGroup = dJointGroupCreate(0);
	dWorldSetGravity(mWorld, 0, -100, 0);

	mStaticSpace = dSimpleSpaceCreate(mGlobalSpace);
}

WorldManager::~WorldManager()
{
	dSpaceDestroy(mStaticSpace);

	dJointGroupEmpty(mContactGroup);
	dJointGroupDestroy(mContactGroup);
	dWorldDestroy(mWorld);
	dCloseODE();
}

bool WorldManager::loadMap(const Ogre::String& mapName)
{
	//очистка
	for (std::vector<Ogre::SceneNode*>::iterator inode = mStaticNodes.begin();
			inode != mStaticNodes.end(); inode++) {
		mSceneMgr->destroySceneNode((*inode)->getName());
		delete (*inode);
	}

	for (std::vector<Ogre::Entity*>::iterator ientity = mStaticEnties.begin();
			ientity != mStaticEnties.end(); ientity++) {
		mSceneMgr->destroyEntity((*ientity));
		delete (*ientity);
	}

	for (std::vector<dGeomID>::iterator igeom = mStaticGeometries.begin();
			igeom != mStaticGeometries.end(); igeom++) {
		dGeomDestroy(*igeom);
	}

	mGameTime = 1800;
	mBuyTime = 60;
	mStartTime = 10;
	mBombTime = 0;
	mTerrorScore = 0;
	mCTerrorScore = 0;
	mRoundNumber = -1;

	//Test map
	Ogre::Entity* entity;
	Ogre::SceneNode* node;
	dGeomID geom;

	//Стены и пол
	entity = mSceneMgr->createEntity("walls", "walls.mesh");
	node = mSceneMgr->getRootSceneNode()->createChildSceneNode();
	node->attachObject(entity);
	geom = dCreatePlane(mStaticSpace, 0, 0, 0, 0);
	mStaticNodes.push_back(node);
	mStaticEnties.push_back(entity);
	mStaticGeometries.push_back(geom);
	//Большой ящик
	entity = mSceneMgr->createEntity("big_box", "big_box.mesh");
	node = mSceneMgr->getRootSceneNode()->createChildSceneNode(Ogre::Vector3(9, 1, 0), Ogre::Quaternion(Ogre::Degree(45), Ogre::Vector3(0, 1, 0)));
	node->attachObject(entity);
	geom = dCreatePlane(mStaticSpace, 0, 0, 0, 0);
	mStaticNodes.push_back(node);
	mStaticEnties.push_back(entity);
	mStaticGeometries.push_back(geom);
	//Первый маленький ящик
	entity = mSceneMgr->createEntity("small_box_01", "small_box.mesh");
	node = mSceneMgr->getRootSceneNode()->createChildSceneNode(Ogre::Vector3(-5.56, 1, -8.66));
	node->attachObject(entity);
	geom = dCreatePlane(mStaticSpace, 0, 0, 0, 0);
	mStaticNodes.push_back(node);
	mStaticEnties.push_back(entity);
	mStaticGeometries.push_back(geom);
	//Второй маленький ящик
	entity = mSceneMgr->createEntity("small_box_02", "small_box.mesh");
	node = mSceneMgr->getRootSceneNode()->createChildSceneNode(Ogre::Vector3(-1.65, 1, 9.42));
	node->attachObject(entity);
	geom = dCreatePlane(mStaticSpace, 0, 0, 0, 0);
	mStaticNodes.push_back(node);
	mStaticEnties.push_back(entity);
	mStaticGeometries.push_back(geom);

	Ogre::Light* light = mSceneMgr->createLight("l");
	light->setPosition(0, 50, 0);

	return true;
}
