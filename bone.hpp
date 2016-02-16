#ifndef BONE_HPP
#define BONE_HPP

#include <GL/glew.h>
#include <glfw3.h>

#include <stdio.h>
#include <stdlib.h>
#include <vector>

#include <glm\glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/quaternion.hpp>
#include <glm/gtx/quaternion.hpp>
#include <glm/gtx/euler_angles.hpp>
#include <glm/gtx/norm.hpp>

#include <common/shader.hpp>
#include <common/texture.hpp>
#include <common/controls.hpp>
#include <common/objloader.hpp>

class Bone
{
public:
	Bone *parent;
	std::vector<Bone*> children;
	//list<Bone*> children;

	int boneID;
	glm::mat4 boneModel;
	glm::vec3 pos;
	bool isRoot;

	Bone();
	Bone(int ID, glm::vec3 T, glm::vec3 R, glm::vec3 S);
	~Bone();

	glm::mat4 getBoneModel();
	void update(glm::vec3 translation, glm::vec3 rotation, glm::vec3 scaling);

};

#endif