/*
 * CloneWarrior.h
 *
 *  Created on: 16.08.2008
 *      Author: sa-bu
 */

#ifndef __CLONE_WARRIOR_H__
#define __CLONE_WARRIOR_H__

#include <OgrePlatform.h>
#include <ode/ode.h>

class Warrior
{
public:
	enum BodyPart {
		bpHead,			//голова
		bpRightArm,		//правое предплечье
		bpLeftArm,		//левое предплечье
		bpRightWrist,	//правое.... эээ... забыл :)
		bpLeftWrist,	//
		bpChest,		//туловище
		bpRightThign,	//правое бедро
		bpLeftThign,	//левое бедро
		bpRightCalf,	//правая голень
		bpLeftCalf,		//левая голень
		bpRightFoot,	//правая стопа
		bpLeftFoot,		//левая стопа

		bpNumParts		//кол-во частей
	};

protected:
	Ogre::uint32 mHealth;
	Ogre::uint32 mArmor;

	Ogre::uint32 mScore;
	Ogre::uint32 mDeath;

	dGeomID mGeoms[bpNumParts];

public:
	Warrior();
	~Warrior();
};

#endif
