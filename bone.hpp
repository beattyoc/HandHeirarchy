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

	//Bone *children[1];

	int boneID;
	glm::mat4 boneModel;
	glm::vec3 pos;
	//glm::mat4 initalOri;
	//glm::mat4 localTrans;
	//void updateOri();

	Bone();
	~Bone();

	glm::mat4 getBoneModel();
	void setModel(glm::vec3 translation, glm::vec3 rotation, glm::vec3 scaling);
	
	
	/*
	glm::mat4 MVPbone;
	GLuint MatrixIDbone;
	GLuint Texturebone;
	GLuint TextureIDbone;
	GLuint vertexbufferbone;
	GLuint uvbufferbone;
	std::vector<glm::vec3> verticesbone;
	*/
	//void setup(glm::mat4 initalOri, glm::vec3 localTrans);
	//void populate(glm::mat4 MVP, GLuint MatrixID, GLuint Texture, GLuint TextureID, GLuint vertexbuffer, GLuint uvbuffer, std::vector<glm::vec3> vertices);
	//void drawBone();

};

#endif