#include "skeleton.hpp"
#include <iostream>

Skeleton::Skeleton(){}

Skeleton::Skeleton(int nBones)
{
	numBones = nBones;
	std::cout << "Skeleton with " << numBones << " bones made.\n";
}

void Skeleton::loadBone(Bone bone)
{
	int boneID = bone.boneID;
	myBone[boneID] = bone;
	if (bone.isRoot)
		root = bone;
}

Skeleton::~Skeleton(){}
