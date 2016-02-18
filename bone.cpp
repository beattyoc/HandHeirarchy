#include "bone.hpp"
#include <iostream>
#include <vector>


Bone::Bone() {
	parent = NULL;
	child = NULL;
	boneID = 0;
	numChildren = 0;
	boneModel = glm::mat4(1.0);
	localTranslation = glm::vec3();
	bool isRoot = false;
	TranslationMatrix = glm::mat4(1.0);
	ScalingMatrix = scale(glm::mat4(1.0), glm::vec3(1.0f, 1.0f, 1.0f));
	RotationMatrix = glm::mat4(1.0);
	//children = std::vector<>;
}

Bone::Bone(int ID, glm::vec3 T, glm::vec3 S)
{
	parent = NULL;
	child = NULL;
	boneID = ID;
	isRoot = false;
	numChildren = 0;
	localTranslation = T;

	//---------- set boneModel ------------------
	ScalingMatrix = scale(glm::mat4(1.0), S);
	TranslationMatrix = translate(glm::mat4(), localTranslation);
	boneModel = TranslationMatrix * RotationMatrix * ScalingMatrix;
	//boneModel = getBoneModel();
	std::cout << "bone[" << boneID << "] created.\n";

}

void Bone::addParent(Bone *bone)
{
	std::cout << "bone[" << boneID << "] has parent bone[" << bone->boneID << "]\n";
	this->parent = bone;
}

void Bone::addChild(Bone *bone)
{
	std::cout << "bone[" << boneID << "] has child bone[" << bone->boneID << "]\n";
	this->child = bone;
	children.push_back(bone);
	numChildren += 1;
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

glm::mat4 Bone::getBoneModel()
{
	return boneModel;
	//return TranslationMatrix * RotationMatrix * ScalingMatrix;
}

void Bone::updateBone(glm::vec3 translation, float rotation)
{
	std::cout << "Updating bone " << boneID << std::endl;
	localTranslation += translation;
	TranslationMatrix = translate(glm::mat4(1.0), localTranslation);
	RotationMatrix = glm::rotate(RotationMatrix, rotation, glm::vec3(1,0,0));
	boneModel = TranslationMatrix * RotationMatrix * ScalingMatrix;
	if (hasParent())
		boneModel = parent->boneModel * boneModel;
}

Bone::~Bone() {}