// Include standard headers
#include <stdio.h>
#include <stdlib.h>
#include <vector>
#include <iostream>
// Include GLEW
#include <GL/glew.h>

// Include GLFW
#include <glfw3.h>
GLFWwindow* window;

// Include GLM
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/quaternion.hpp>
#include <glm/gtx/quaternion.hpp>
#include <glm/gtx/euler_angles.hpp>
#include <glm/gtx/norm.hpp>
using namespace glm;

#include <common/shader.hpp>
#include <common/texture.hpp>
#include <common/controls.hpp>
#include <common/objloader.hpp>
#include <common/bone.hpp>
#include <common/skeleton.hpp>


#define NUMBONES 16

glm::mat4 ViewMatrix, ModelMatrix, ProjectionMatrix, TranslationMatrix, RotationMatrix, ScalingMatrix, MVP;
GLuint MatrixID, Texture, TextureID, vertexbuffer, uvbuffer, VertexArrayID, programID;
std::vector<glm::vec3> vertices;

void drawBone(Bone bone, mat4 ProjectionMatrix, mat4 ViewMatrix);
void drawSkeleton(Skeleton skeleton, mat4 ProjectionMatrix, mat4 ViewMatrix);
void checkKeys();

Bone bone[NUMBONES];
Skeleton skeleton = Skeleton(NUMBONES);

//----- Camera Variables --------
glm::vec3 view_angles;
float ViewRotationAngle = 0.2f;
quat ViewOrientation;
glm::mat4 ViewRotationMatrix;
bool camMoved = true;

//------ Transformations -------
float rotAngle = 0.01f;
glm::vec3 left(-0.1, 0, 0);
glm::vec3 right(0.1, 0, 0);
glm::vec3 back(0, 0, -0.1);
glm::vec3 forth(0, 0, 0.1);

int main( void )
{
	// Initialise GLFW
	if( !glfwInit() )
	{
		fprintf( stderr, "Failed to initialize GLFW\n" );
		getchar();
		return -1;
	}

	glfwWindowHint(GLFW_SAMPLES, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE); // To make MacOS happy; should not be needed
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	// Open a window and create its OpenGL context
	window = glfwCreateWindow( 1024, 768, "Tutorial 07 - Model Loading", NULL, NULL);
	if( window == NULL ){
		fprintf( stderr, "Failed to open GLFW window. If you have an Intel GPU, they are not 3.3 compatible. Try the 2.1 version of the tutorials.\n" );
		getchar();
		glfwTerminate();
		return -1;
	}
	glfwMakeContextCurrent(window);

	// Initialize GLEW
	glewExperimental = true; // Needed for core profile
	if (glewInit() != GLEW_OK) {
		fprintf(stderr, "Failed to initialize GLEW\n");
		getchar();
		glfwTerminate();
		return -1;
	}

	// Ensure we can capture the escape key being pressed below
	glfwSetInputMode(window, GLFW_STICKY_KEYS, GL_TRUE);  
    glfwPollEvents();

	glClearColor(0.0f, 0.0f, 0.0f, 0.0f);

	// Enable depth test
	glEnable(GL_DEPTH_TEST);
	// Accept fragment if it closer to the camera than the former one
	glDepthFunc(GL_LESS); 

	// Cull triangles which normal is not towards the camera
	glEnable(GL_CULL_FACE);

	VertexArrayID;
	glGenVertexArrays(1, &VertexArrayID);
	glBindVertexArray(VertexArrayID);

	// Create and compile our GLSL program from the shaders
	programID = LoadShaders( "TransformVertexShader.vertexshader", "TextureFragmentShader.fragmentshader" );

	// Get a handle for our "MVP" uniform
	MatrixID = glGetUniformLocation(programID, "MVP");

	// Load the texture
	Texture = loadDDS("uvmap.DDS");
	//Texture = loadDDS("SopCamel1.DDS");

	// Get a handle for our "myTextureSampler" uniform
	TextureID  = glGetUniformLocation(programID, "myTextureSampler");

	// Read our .obj file
	//std::vector<glm::vec3> vertices;
	std::vector<glm::vec2> uvs;
	std::vector<glm::vec3> normals; // Won't be used at the moment.
	bool res = loadOBJ("cube.obj", vertices, uvs, normals);

	// Load it into a VBO

	vertexbuffer;
	glGenBuffers(1, &vertexbuffer);
	glBindBuffer(GL_ARRAY_BUFFER, vertexbuffer);
	glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(glm::vec3), &vertices[0], GL_STATIC_DRAW);

	uvbuffer;
	glGenBuffers(1, &uvbuffer);
	glBindBuffer(GL_ARRAY_BUFFER, uvbuffer);
	glBufferData(GL_ARRAY_BUFFER, uvs.size() * sizeof(glm::vec2), &uvs[0], GL_STATIC_DRAW);

	// For speed computation
	double lastTime = glfwGetTime();
	double lastFrameTime = lastTime;
	int nbFrames = 0;

	//------------------ Initialize Matrices ----------------
	//--------------- not using controls.cpp ---------------------------
	ProjectionMatrix = glm::perspective(45.0f, 4.0f / 3.0f, 0.1f, 100.0f);
	//ViewMatrix = glm::lookAt(vec3(0,-2,-10), glm::vec3(0, 0, 0), glm::vec3(0, 1, 0));
	//RotationMatrix = glm::mat4(1.0);
	//TranslationMatrix = translate(mat4(), vec3(0.0f, 5.0f,-30.0f));
	//ScalingMatrix = scale(mat4(), vec3(1.0f, 1.0f, 1.0f));
	//ModelMatrix = TranslationMatrix * RotationMatrix * ScalingMatrix;
	//MVP = ProjectionMatrix * ViewMatrix * ModelMatrix;


	//------------------ Create Bones --------------------------
	float x = 0.0f;
	float increment = 2.0f;
	float initialY = 3.0f;
	float y = initialY;
	glm::vec3 fingerScale(0.5, 1.0, 0.5);

	//palm//root
	bone[0] = Bone(0, vec3(0, 0, 0), vec3(2.0, 2.0, 0.5));

	//thumb
	y = 0.0f;
	for (int i = 1; i < 4; i++)
	{
		x = -2.5f;
		bone[i] = Bone(i, vec3(x, y, 0), fingerScale);
		y += increment;
	}
	y = initialY;

	//index
	for (int i = 4; i < 7; i++)
	{
		x = -1.5f;
		bone[i] = Bone(i, vec3(x, y, 0), fingerScale);
		y += increment;
	}
	y = initialY;

	//middle
	for (int i = 7; i < 10; i++)
	{
		x = -0.5f;
		bone[i] = Bone(i, vec3(x, y, 0), fingerScale);
		y += increment;
	}
	y = initialY;

	//ring
	for (int i = 10; i < 13; i++)
	{
		x = 0.5f;
		bone[i] = Bone(i, vec3(x, y, 0), fingerScale);
		y += increment;
	}
	y = initialY;

	//baby
	for (int i = 13; i < 16; i++)
	{
		x = 1.5f;
		bone[i] = Bone(i, vec3(x, y, 0), fingerScale);
		y += increment;
	}

	/*
	for (int i = 0; i < NUMBONES; i++)
	{
		bone[i] = Bone(i, vec3(0, y, 0), false);
		y += 5.0f;
	}
	*/


	//----------------- Make relationships ----------------

	//palm//root
	bone[0].isRoot = true;
	for (int i = 1; i < NUMBONES; i++)
		bone[0].addChild(&bone[i]);

	for (int i = 1; i < (NUMBONES - 2); i+=3)
		bone[i].addParent(&bone[0]);

	//thumb
	for (int i = 3; i > 1; i--)
	{
		bone[1].addChild(&bone[i]);
		bone[i].addParent(&bone[i - 1]);
	}
	bone[2].addChild(&bone[3]);

	//index
	for (int i = 6; i > 4; i--)
	{
		bone[4].addChild(&bone[i]);
		bone[i].addParent(&bone[i - 1]);
	}
	bone[5].addChild(&bone[6]);

	//middle
	for (int i = 9; i > 7; i--)
	{
		bone[7].addChild(&bone[i]);
		bone[i].addParent(&bone[i - 1]);
	}
	bone[8].addChild(&bone[9]);

	//ring
	for (int i = 12; i > 10; i--)
	{
		bone[10].addChild(&bone[i]);
		bone[i].addParent(&bone[i - 1]);
	}
	bone[11].addChild(&bone[12]);

	//baby
	for (int i = 15; i > 13; i--)
	{
		bone[13].addChild(&bone[i]);
		bone[i].addParent(&bone[i - 1]);
	}
	bone[14].addChild(&bone[15]);

	//------------------ Load Skeleton ------------------------
	//skeleton = Skeleton(NUMBONES);

	for (int i = 0; i < NUMBONES; i++)
	{
		skeleton.loadBone(&bone[i]);
	}

	do{

		// Measure speed
		double currentTime = glfwGetTime();
		float deltaTime = (float)(currentTime - lastFrameTime);
		lastFrameTime = currentTime;
		nbFrames++;
		if (currentTime - lastTime >= 1.0){ // If last prinf() was more than 1sec ago
			// printf and reset
			printf("%f ms/frame\n", 1000.0 / double(nbFrames));
			nbFrames = 0;
			lastTime += 1.0;
		}

		// Clear the screen
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		// Use our shader
		glUseProgram(programID);

		/*
		//--------------- to use controls.cpp from plane lab -----------------------
		// Compute the MVP matrix from keyboard and mouse input
		computeMatricesFromInputs();
		glm::mat4 ProjectionMatrix = getProjectionMatrix();
		glm::mat4 ViewMatrix = getViewMatrix();
		glm::mat4 ModelMatrix = glm::mat4(1.0);
		glm::mat4 ModelMatrix = getModelMatrix();
		*/
		//TranslationMatrix = translate(mat4(), vec3(0.0f,10.0f,-30.0f));
		//ModelMatrix = TranslationMatrix * RotationMatrix * ScalingMatrix;
		//MVP = ProjectionMatrix * ViewMatrix * ModelMatrix;


		// Bind our texture in Texture Unit 0
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, Texture);
		// Set our "myTextureSampler" sampler to user Texture Unit 0
		glUniform1i(TextureID, 0);

		// 1rst attribute buffer : vertices
		glEnableVertexAttribArray(0);
		glBindBuffer(GL_ARRAY_BUFFER, vertexbuffer);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);

		// 2nd attribute buffer : UVs
		glEnableVertexAttribArray(1);
		glBindBuffer(GL_ARRAY_BUFFER, uvbuffer);
		glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 0, (void*)0);


		//------------- Update Camera --------------------
		//camMoved = false; // this line disables camera
		if (camMoved)
		{
			ViewMatrix = glm::lookAt(vec3(5, -1, 20), glm::vec3(0, 0, 0), glm::vec3(0, 1, 0));
			glm::quat view_rotation(radians(view_angles));
			ViewOrientation = ViewOrientation * view_rotation;
			ViewRotationMatrix = toMat4(ViewOrientation);
			ViewMatrix = ViewMatrix * ViewRotationMatrix; // switch multiplication to switch to first person
			camMoved = false;
		}

		//------------- Let's Draw! -----------------------
		drawSkeleton(skeleton, ProjectionMatrix, ViewMatrix);

		checkKeys();

		glDisableVertexAttribArray(0);
		glDisableVertexAttribArray(1);
		
		// Swap buffers
		glfwSwapBuffers(window);
		glfwPollEvents();

	} // Check if the ESC key was pressed or the window was closed
	while( glfwGetKey(window, GLFW_KEY_ESCAPE ) != GLFW_PRESS &&
		   glfwWindowShouldClose(window) == 0 );

	// Cleanup VBO and shader
	glDeleteBuffers(1, &vertexbuffer);
	glDeleteBuffers(1, &uvbuffer);
	glDeleteProgram(programID);
	glDeleteTextures(1, &TextureID);
	glDeleteVertexArrays(1, &VertexArrayID);

	// Close OpenGL window and terminate GLFW
	glfwTerminate();

	return 0;
}

/*
void drawBone(Bone bone, mat4 ProjectionMatrix, mat4 ViewMatrix)
{
	ModelMatrix = bone.getBoneModel();
	MVP = ProjectionMatrix * ViewMatrix * ModelMatrix;
	glUniformMatrix4fv(MatrixID, 1, GL_FALSE, &MVP[0][0]);
	glDrawArrays(GL_TRIANGLES, 0, vertices.size());
}
*/

void drawSkeleton(Skeleton skeleton, mat4 ProjectionMatrix, mat4 ViewMatrix)
{
	for (int i = 0; i < skeleton.numBones; i++)
	{
		//drawBone(bone[i], ProjectionMatrix, ViewMatrix);
		ModelMatrix = skeleton.myBone[i]->getBoneModel();
		MVP = ProjectionMatrix * ViewMatrix * ModelMatrix;
		glUniformMatrix4fv(MatrixID, 1, GL_FALSE, &MVP[0][0]);
		glDrawArrays(GL_TRIANGLES, 0, vertices.size());
		
	}
}

void checkKeys()
{
	//------------ palm / root control --------------------
	if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS){
		//bone[1].updateBone(vec3(0, 0, 0), rotAngle);
		skeleton.update(&bone[0], back, 0);
	}
	if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS){
		//bone[0].updateBone(vec3(0, 0, 0), rotAngle);
		skeleton.update(&bone[0], forth, 0);
	}
	if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS){
		//bone[0].updateBone(left, 0);
		skeleton.update(&bone[0], left, 0);
	}
	if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS){
		//bone[0].updateBone(right, 0);
		skeleton.update(&bone[0], right, 0);
	}

	//-------------------- finger controls ---------------------
	//thumb root
	if (glfwGetKey(window, GLFW_KEY_1) == GLFW_PRESS && glfwGetKey(window, GLFW_KEY_N) == GLFW_PRESS)
	{
		skeleton.update(&bone[1], vec3(0, 0, 0), rotAngle);
	}
	if (glfwGetKey(window, GLFW_KEY_1) == GLFW_PRESS && glfwGetKey(window, GLFW_KEY_M) == GLFW_PRESS)
	{
		skeleton.update(&bone[1], vec3(0, 0, 0), -rotAngle);
	}

	//index root
	if (glfwGetKey(window, GLFW_KEY_2) == GLFW_PRESS && glfwGetKey(window, GLFW_KEY_N) == GLFW_PRESS)
	{
		skeleton.update(&bone[4], vec3(0, 0, 0), rotAngle);
	}
	if (glfwGetKey(window, GLFW_KEY_2) == GLFW_PRESS && glfwGetKey(window, GLFW_KEY_M) == GLFW_PRESS)
	{
		skeleton.update(&bone[4], vec3(0, 0, 0), -rotAngle);
	}

	//middle root
	if (glfwGetKey(window, GLFW_KEY_3) == GLFW_PRESS && glfwGetKey(window, GLFW_KEY_N) == GLFW_PRESS)
	{
		skeleton.update(&bone[7], vec3(0, 0, 0), rotAngle);
	}
	if (glfwGetKey(window, GLFW_KEY_3) == GLFW_PRESS && glfwGetKey(window, GLFW_KEY_M) == GLFW_PRESS)
	{
		skeleton.update(&bone[7], vec3(0, 0, 0), -rotAngle);
	}

	//ring root
	if (glfwGetKey(window, GLFW_KEY_4) == GLFW_PRESS && glfwGetKey(window, GLFW_KEY_N) == GLFW_PRESS)
	{
		skeleton.update(&bone[10], vec3(0, 0, 0), rotAngle);
	}
	if (glfwGetKey(window, GLFW_KEY_4) == GLFW_PRESS && glfwGetKey(window, GLFW_KEY_M) == GLFW_PRESS)
	{
		skeleton.update(&bone[10], vec3(0, 0, 0), -rotAngle);
	}

	//baby root
	if (glfwGetKey(window, GLFW_KEY_5) == GLFW_PRESS && glfwGetKey(window, GLFW_KEY_N) == GLFW_PRESS)
	{
		skeleton.update(&bone[13], vec3(0, 0, 0), rotAngle);
	}
	if (glfwGetKey(window, GLFW_KEY_5) == GLFW_PRESS && glfwGetKey(window, GLFW_KEY_M) == GLFW_PRESS)
	{
		skeleton.update(&bone[13], vec3(0, 0, 0), -rotAngle);
	}
	
	//---------- Camera Controls -----------------------
	if (glfwGetKey(window, GLFW_KEY_RIGHT) == GLFW_PRESS){
		view_angles = vec3(0, ViewRotationAngle, 0);
		camMoved = true;
	}
	if (glfwGetKey(window, GLFW_KEY_LEFT) == GLFW_PRESS){
		view_angles = vec3(0, -ViewRotationAngle, 0);
		camMoved = true;
	}
	if (glfwGetKey(window, GLFW_KEY_UP) == GLFW_PRESS){
		view_angles = vec3(-ViewRotationAngle, 0, 0);
		camMoved = true;
	}
	if (glfwGetKey(window, GLFW_KEY_DOWN) == GLFW_PRESS){
		view_angles = vec3(ViewRotationAngle, 0, 0);
		camMoved = true;
	}
}
