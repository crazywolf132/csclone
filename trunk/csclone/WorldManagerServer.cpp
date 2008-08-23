/*
 * CloneClient.h
 *
 *  Created on: 20.08.2008
 *      Author: sa-bu
 */

#include <OgreStringConverter.h>
#include <MyGUI_XmlDocument.h>
#include "WorldManagerServer.h"

WorldManagerServer::WorldManagerServer()
{
	dInitODE();
	mWorld = dWorldCreate();
	mGlobalSpace = dHashSpaceCreate(0);
	mContactGroup = dJointGroupCreate(0);
	dWorldSetGravity(mWorld, 0, -100, 0);

	mStaticSpace = dSimpleSpaceCreate(mGlobalSpace);
}

WorldManagerServer::~WorldManagerServer()
{
	dSpaceDestroy(mStaticSpace);

	dJointGroupEmpty(mContactGroup);
	dJointGroupDestroy(mContactGroup);
	dWorldDestroy(mWorld);
	dCloseODE();
}

bool WorldManagerServer::loadMap(const Ogre::String& mapName)
{
	WorldManager::loadMap(mapName);
	//cleanup
	for (std::vector<dGeomID>::iterator igeom = mStaticGeometries.begin();
			igeom != mStaticGeometries.end(); igeom++) {
		dGeomDestroy(*igeom);
	}
		//creating new game and loading new map
	Ogre::DataStreamPtr stream = Ogre::ResourceGroupManager::getSingleton().openResource(mapName);
	MyGUI::xml::xmlDocument xml;
	if (!xml.open(stream))
		return false;
	MyGUI::xml::xmlNodePtr root_node = xml.getRoot();
	if (root_node->getName() != "map")
		return false;

	MyGUI::xml::xmlNodeIterator inode = root_node->getNodeIterator();
	while (inode.nextNode()) {
		if (inode->getName() == "geom") {
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
	}

	return true;
}
