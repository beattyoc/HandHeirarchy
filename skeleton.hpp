#ifndef SKELETON_HPP
#define SKELETON_HPP

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
#include <common/bone.hpp>

#define NUMBONES 4

class Skeleton {
public:
	Bone root;
	//int numbones;
	Bone myBone[NUMBONES];
	int numBones;

	Skeleton();
	Skeleton(int nBones);
	~Skeleton();

	void loadBone(Bone bone);
};


#endif