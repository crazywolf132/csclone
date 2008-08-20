/*
 * CloneWorldManager.cpp
 *
 *  Created on: 16.08.2008
 *      Author: sa-bu
 */

#include <MyGUI_XmlDocument.h>
#include "CloneWorldManager.h"

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
	mSceneMgr(sceneMgr), mWorld(0), mGlobalSpace(0), mStaticSpace(0),
	mServer(0)
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
	//cleanup
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

	if (mServer) {
		delete mServer;
		mServer = 0;
	}

	//creating new game and loading new map
	mServer = new Server("192.168.0.1", 5000);

	mGameTime = 1800;
	mBuyTime = 60;
	mStartTime = 10;
	mBombTime = 0;
	mTerrorScore = 0;
	mCTerrorScore = 0;
	mRoundNumber = -1;

	static int name_int = 0;

	Ogre::DataStreamPtr stream = Ogre::ResourceGroupManager::getSingleton().openResource(mapName);
	MyGUI::xml::xmlDocument xml;
	if (!xml.open(stream))
		return false;
	MyGUI::xml::xmlNodePtr root_node = xml.getRoot();
	if (root_node->getName() != "map")
		return false;

	MyGUI::xml::xmlNodeIterator inode = root_node->getNodeIterator();
	while (inode.nextNode()) {
		if (inode->getName() == "static") {
			Ogre::String mesh = inode->findAttribute("mesh");
			Ogre::Entity* entity = mSceneMgr->createEntity(mesh+Ogre::StringConverter::toString(name_int), mesh);
			Ogre::SceneNode* node = mSceneMgr->getRootSceneNode()->createChildSceneNode();
			node->attachObject(entity);
			mStaticNodes.push_back(node);
			mStaticEnties.push_back(entity);

			Ogre::String param;
			param = inode->findAttribute("position");
			if (!param.empty())
				node->setPosition(Ogre::StringConverter::parseVector3(param));

			param = inode->findAttribute("rotation");
			if (!param.empty())
				node->setOrientation(Ogre::StringConverter::parseQuaternion(param));
		} else if (inode->getName() == "geom") {
			Ogre::String type = inode->findAttribute("type");
			std::vector<Ogre::String> params = Ogre::StringUtil::split(inode->findAttribute("size"), " ");
			dGeomID geom;

			if (type == "box") {
				geom = dCreateBox(mStaticSpace,
						Ogre::StringConverter::parseReal(params[0]),
						Ogre::StringConverter::parseReal(params[1]),
						Ogre::StringConverter::parseReal(params[2])
					);
			} else if (type == "sphere") {
				geom = dCreateSphere(mStaticSpace,
						Ogre::StringConverter::parseReal(params[0])
					);
			} else if (type == "cylinder") {
				geom = dCreateCylinder(mStaticSpace,
						Ogre::StringConverter::parseReal(params[0]),
						Ogre::StringConverter::parseReal(params[1])
					);
			} else if (type == "capsule") {
				geom = dCreateCapsule(mStaticSpace,
						Ogre::StringConverter::parseReal(params[0]),
						Ogre::StringConverter::parseReal(params[1])
					);
			} else if (type == "plane") {
				geom = dCreatePlane(mStaticSpace,
						Ogre::StringConverter::parseReal(params[0]),
						Ogre::StringConverter::parseReal(params[1]),
						Ogre::StringConverter::parseReal(params[2]),
						Ogre::StringConverter::parseReal(params[3])
					);
			} else {
				return false;
			}

			mStaticGeometries.push_back(geom);
		} else {
			return false;
		}

		name_int++;
	}


	Ogre::Light* light = mSceneMgr->createLight("l");
	light->setPosition(0, 50, 0);

	return true;
}
