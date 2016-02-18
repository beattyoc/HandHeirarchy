#include "skeleton.hpp"
#include <iostream>

Skeleton::Skeleton()
{
	numBones = 0;
	//root = Bone();
}

Skeleton::Skeleton(int nBones)
{
	//root = Bone();
	numBones = nBones;
	std::cout << "Skeleton with " << numBones << " bones made.\n";
}

void Skeleton::loadBone(Bone *bone)
{
	int boneID = bone->boneID;
	myBone[boneID] = bone;
	//allBones.push_back(&bone);
	if (bone->isRoot)
		root = bone;
}

void Skeleton::update(Bone *bone, glm::vec3 translation, float rotation)
{
	int ID = bone->boneID;
	std::cout << ID << std::endl;
	if (bone->isRoot)
		updateRoot(translation, rotation);
	myBone[ID]->updateBone(translation, rotation);
	// Recursively update children
	if (bone->hasChild())
	{
		update(myBone[ID]->child, translation, rotation);
	}
}

void Skeleton::updateRoot(glm::vec3 translation, float rotation)
{
	root->updateBone(translation, rotation);
	update(myBone[1], translation, rotation);
	update(myBone[4], translation, rotation);
	update(myBone[7], translation, rotation);
	update(myBone[10], translation, rotation);
	update(myBone[13], translation, rotation);
	return;
}

Skeleton::~Skeleton(){}
