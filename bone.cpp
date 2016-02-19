#include "bone.hpp"
#include <iostream>
#include <vector>


Bone::Bone() {
	parent = NULL;
	child = NULL;
	boneID = 0;
	boneModel = glm::mat4(1.0);
	isRoot = false;
	TranslationMatrix = glm::mat4(1.0);
	ScalingMatrix = glm::mat4(1.0);
	RotationMatrix = glm::mat4(1.0);
}

Bone::Bone(int ID, glm::vec3 T, glm::vec3 S)
{
	parent = NULL;
	child = NULL;
	boneID = ID;
	isRoot = false;

	// set initial model matrix for bone
	ScalingMatrix = scale(glm::mat4(1.0), S);
	TranslationMatrix = translate(glm::mat4(), T);
	boneModel = TranslationMatrix * RotationMatrix * ScalingMatrix;

}

void Bone::addParent(Bone *bone)
{
	this->parent = bone;
}

void Bone::addChild(Bone *bone)
{
	this->child = bone;
	children.push_back(bone);
}

bool Bone::hasParent()
{
	if (this->parent)
		return true;
	else return false;
}

bool Bone::hasChild()
{
	if (this->child)
		return true;
	else return false;
}

void Bone::updateBone(glm::vec3 translation, float rotation, glm::vec3 axis)
{
	TranslationMatrix = translate(TranslationMatrix, translation);
	RotationMatrix = glm::rotate(RotationMatrix, rotation, axis);
	boneModel = TranslationMatrix * RotationMatrix * ScalingMatrix;
	if (hasParent())
		boneModel = parent->boneModel * boneModel;
}

/*
// if notified that the parent has moved update model matrix
void Bone::parentMoved()
{
	boneModel = parent->boneModel * boneModel;
}
*/

Bone::~Bone() {}