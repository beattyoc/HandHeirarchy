#include "bone.hpp"

Bone::Bone(){}

glm::mat4 Bone::getBoneModel()
{
	return boneModel;
}

void Bone::setModel(glm::vec3 translation, glm::vec3 rotation, glm::vec3 scaling)
{
	glm::mat4 RotationMatrix = glm::mat4(1.0);

	pos += translation;

	glm::mat4 TranslationMatrix = translate(glm::mat4(), pos);
	glm::mat4 ScalingMatrix = scale(glm::mat4(), glm::vec3(1.0f, 1.0f, 1.0f));

	boneModel = TranslationMatrix * RotationMatrix * ScalingMatrix;
}

Bone::~Bone(){}



/*
void Bone::populate(glm::mat4 MVP, GLuint MatrixID, GLuint Texture, GLuint TextureID, GLuint vertexbuffer, GLuint uvbuffer, std::vector<glm::vec3> vertices)
{
	MVPbone = MVP;
	MatrixIDbone = MatrixID;
	Texturebone = Texture;
	TextureIDbone = TextureID;
	vertexbufferbone = vertexbuffer;
	uvbufferbone = vertexbuffer;
	verticesbone = vertices;
}*/

/*
void Bone::drawBone()
{
	// Send our transformation to the currently bound shader, 
	// in the "MVP" uniform
	glUniformMatrix4fv(MatrixIDbone, 1, GL_FALSE, &MVPbone[0][0]);

	// Bind our texture in Texture Unit 0
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, Texturebone);
	// Set our "myTextureSampler" sampler to user Texture Unit 0
	glUniform1i(TextureIDbone, 0);

	// 1rst attribute buffer : vertices
	glEnableVertexAttribArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, vertexbufferbone);
	glVertexAttribPointer(
		0,                  // attribute
		3,                  // size
		GL_FLOAT,           // type
		GL_FALSE,           // normalized?
		0,                  // stride
		(void*)0            // array buffer offset
		);

	// 2nd attribute buffer : UVs
	glEnableVertexAttribArray(1);
	glBindBuffer(GL_ARRAY_BUFFER, uvbufferbone);
	glVertexAttribPointer(
		1,                                // attribute
		2,                                // size
		GL_FLOAT,                         // type
		GL_FALSE,                         // normalized?
		0,                                // stride
		(void*)0                          // array buffer offset
		);

	// Draw the triangle !
	glDrawArrays(GL_TRIANGLES, 0, verticesbone.size());
	//(GL_TRIANGLES, 0, vertices.size(), 2);

	glDisableVertexAttribArray(0);
	glDisableVertexAttribArray(1);

}*/