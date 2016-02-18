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
	if (bone->isRoot)
		root = bone;
}

void Skeleton::update(Bone *bone, glm::vec3 translation, float rotation, glm::vec3 axis)
{
	int ID = bone->boneID;
	std::cout << ID << std::endl;
	if (bone->isRoot)
		updateRoot(translation, rotation, axis);
	myBone[ID]->updateBone(translation, rotation, axis);
	// Recursively update children
	if (bone->hasChild())
	{
		update(myBone[ID]->child, translation, rotation, axis);
	}
}

void Skeleton::updateRoot(glm::vec3 translation, float rotation, glm::vec3 axis)
{
	root->updateBone(translation, rotation, axis);
	update(myBone[1], translation, rotation, axis);
	update(myBone[4], translation, rotation, axis);
	update(myBone[7], translation, rotation, axis);
	update(myBone[10], translation, rotation, axis);
	update(myBone[13], translation, rotation, axis);
	return;
}

Skeleton::~Skeleton(){}
