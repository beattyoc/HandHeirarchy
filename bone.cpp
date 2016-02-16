#include "bone.hpp"
#include <iostream>

Bone::Bone() {}

Bone::Bone(int ID, glm::vec3 T, glm::vec3 R, glm::vec3 S)
{
	boneID = ID;
	isRoot = false;
	pos = T;

	//---------- set boneModel ------------------
	glm::mat4 RotationMatrix = glm::mat4(1.0);
	glm::mat4 TranslationMatrix = translate(glm::mat4(), pos);
	glm::mat4 ScalingMatrix = scale(glm::mat4(), glm::vec3(1.0f, 1.0f, 1.0f));
	boneModel = TranslationMatrix * RotationMatrix * ScalingMatrix;

	std::cout << "bone[" << boneID << "] created.\n";

}

glm::mat4 Bone::getBoneModel()
{
	return boneModel;
}

void Bone::update(glm::vec3 translation, glm::vec3 rotation, glm::vec3 scaling)
{
	glm::mat4 RotationMatrix = glm::mat4(1.0);

	pos += translation;

	glm::mat4 TranslationMatrix = translate(glm::mat4(), pos);
	glm::mat4 ScalingMatrix = scale(glm::mat4(), glm::vec3(1.0f, 1.0f, 1.0f));

	boneModel = TranslationMatrix * RotationMatrix * ScalingMatrix;
}

Bone::~Bone(void) {}