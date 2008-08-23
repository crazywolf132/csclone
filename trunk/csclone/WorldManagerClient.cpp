/*
 * CloneClient.h
 *
 *  Created on: 20.08.2008
 *      Author: sa-bu
 */

#include <MyGUI_XmlDocument.h>
#include "WorldManagerClient.h"

WorldManagerClient::WorldManagerClient(Ogre::SceneManager* sceneMgr)
	: mSceneMgr(sceneMgr)
{
}

WorldManagerClient::~WorldManagerClient()
{
}

bool WorldManagerClient::loadMap(const Ogre::String& mapName)
{
	WorldManager::loadMap(mapName);

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
	//creating new game and loading new map
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
		} else {
			return false;
		}

		name_int++;
	}


	Ogre::Light* light = mSceneMgr->createLight("l");
	light->setPosition(0, 50, 0);

	return true;
}
