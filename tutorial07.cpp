// Include standard headers
#include <stdio.h>
#include <stdlib.h>
#include <vector>
#include <iostream>
#include <Windows.h>
#include <cmath>
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
#include <glm/gtx/vector_angle.hpp>
using namespace glm;

#include <common/shader.hpp>
#include <common/texture.hpp>
#include <common/controls.hpp>
#include <common/objloader.hpp>
#include <common/bone.hpp>
#include <common/skeleton.hpp>

#define NUMBONES 6

glm::mat4 ViewMatrix, ModelMatrix, ProjectionMatrix, TranslationMatrix, RotationMatrix, ScalingMatrix, MVP, targetTranslationMatrix, targetModelMatrix;
glm::mat4 cubeTranslationMatrix, cubeModelMatrix, mvpCube, ballModelMatrix;
glm::mat4 gongModelMatrix, ccModelMatrix, grassModelMatrix;
GLuint MatrixID, Texture, TextureID, vertexbuffer, uvbuffer, VertexArrayID, programID;
GLuint vb_cube, ub_cube, matrixID_cube, texture_cube, textureID_cube, vertexArrayID_cube;
GLuint texture_gong, textureID_gong, texture_cc, textureID_cc, texture_grass, textureID_grass;
std::vector<glm::vec3> vertices, normals, vertices_cube, normals_cube;
std::vector<glm::vec2> uvs, uvs_cube;
bool updated = true;

void drawBone(Bone bone, mat4 ProjectionMatrix, mat4 ViewMatrix);
void drawSkeleton(Skeleton skeleton, mat4 ProjectionMatrix, mat4 ViewMatrix);
void checkKeys(mat4 ProjectionMatrix, mat4 ViewMatrix, float deltaTime);
void CCD();
void drawTarget(mat4 ProjectionMatrix, mat4 ViewMatrix);
void drawCube(mat4 ProjectionMatrix, mat4 ViewMatrix);
void drawBall(mat4 ProjectionMatrix, mat4 ViewMatrix, vec3 ballPos, vec3 ballScale);
void drawGong(mat4 ProjectionMatrix, mat4 ViewMatrix, vec3 gongPos, vec3 gongScale);
void drawCC(mat4 ProjectionMatrix, mat4 ViewMatrix, vec3 ccPos, vec3 ccScale);
void drawGrass(mat4 ProjectionMatrix, mat4 ViewMatrix, vec3 grassPos, vec3 grassScale);
vec3 bezierCurve(float t, vec3 p0, vec3 p1, vec3 p2, vec3 p3, vec3 p4, vec3 p5);

Bone bone[NUMBONES];
Skeleton skeleton = Skeleton(NUMBONES);

Bone cubeBone;
Skeleton cubeSkeleton = Skeleton(1);

//----- Camera Variables --------
glm::vec3 view_angles;
float ViewRotationAngle = 0.5f;
quat ViewOrientation;
glm::mat4 ViewRotationMatrix;
bool camMoved = true;
glm::vec3 center(0.0f, 10.0f, -8.0f);
glm::vec3 eye(-10, 5, -30);

// ----- Animation Variables -------
bool bounce = false;
bool thrown = false;

float gravityWeight = -1;
float timeWeight = 1;

vec3 velocity0_b(0.0f, 0.0f, 0.0f);
vec3 velocity_b(0.0f, 0.0f, 0.0f);
vec3 velocity0_t(0.0f, 2.0f, 1.0f);
vec3 velocity_t(0.0f, 0.0f, 0.0f);

vec3 lastVelocity(0.0f, 0.0f, 0.0f);
float Vyo = 10;
float spd = 10;
float Zspd = 25;
float timePassed = 0;
bool squash = false;
bool stretch = false;
bool goingUp = false;
float gravity = 9.81;
float gravityUp = 9.81;
float gravityDown = 9.81;
bool timeToZero = false;

bool revert = false;
bool hitting = true;

bool simulateForce = false;
bool ballIsZero = false;

//------ Gong -------
vec3 gongPos(0.0f, 30.0f, 15.0f);
vec3 gongScale(3.0f, 3.0f, 0.5f);
bool shrinkGong = false, exaggerateGong = false;

//-------- Candy Cane --------
vec3 ccPos(0.0f, 0.0f, 15.5f);
vec3 ccScale(0.25f, 18.0f, 0.25f);

//---------- Grass ------------
vec3 grassPos(0.0f, 0.0f, 0.0f);
vec3 grassScale(20.0f, 0.25f, 20.0f);

//------ Transformations -------
float rotAngle = 0.005f;
glm::vec3 nullVec(0, 0, 0);
glm::vec3 left(-0.05, 0, 0);
glm::vec3 right(0.05, 0, 0);
glm::vec3 up(0, 0.05, 0);
glm::vec3 down(0, -0.05, 0);
glm::vec3 back(0, 0, -0.05);
glm::vec3 forth(0, 0, 0.05);
glm::vec3 axisX(1, 0, 0);
glm::vec3 axisY(0, 1, 0);
glm::vec3 axisZ(0, 0, 1);

// --------- Target info ---------
glm::vec3 targetPos(0.01f, 5.0f, -8.0f);
glm::vec3 targetScale(0.1f, 0.1f, 0.1f);

glm::vec3 cubePos(4.0f, 0.0f, 0.0f);

// --------- ball info ------------
//glm::vec3 ballPos(0.0f, 5.0f, -8.0f);
glm::vec3 ballPos(0.0f, 0.1f, 15.0f);
glm::vec3 ballScale(1.0f, 1.0f, 1.0f);

//------- Bezier Curve Points ------
vec3 p0(10, 10, 0);
vec3 p1(0, 7, -10);
vec3 p2(-12, 6, -5);
vec3 p3(-8, 5, 5);
vec3 p4(8, 5, 5);
vec3 p5(8, 8, -2);
bool followCurve = false;
bool curveOne = true;

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


	//VertexArrayID;
	glGenVertexArrays(1, &VertexArrayID);
	glBindVertexArray(VertexArrayID);

	// Create and compile our GLSL program from the shaders
	programID = LoadShaders( "TransformVertexShader.vertexshader", "TextureFragmentShader.fragmentshader" );

	// Get a handle for our "MVP" uniform
	MatrixID = glGetUniformLocation(programID, "MVP");

	// Load the texture
	Texture = loadDDS("snakeSkin.DDS");

	// Get a handle for our "myTextureSampler" uniform
	TextureID  = glGetUniformLocation(programID, "myTextureSampler");

	// Read our .obj file
	
	bool res = loadOBJ("sphere.obj", vertices, uvs, normals);
	// Load it into a VBO

	// ------------------- spehere ----------------------------
	//vertexbuffer;
	glGenBuffers(1, &vertexbuffer);
	glBindBuffer(GL_ARRAY_BUFFER, vertexbuffer);
	glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(glm::vec3), &vertices[0], GL_STATIC_DRAW);

	//uvbuffer;
	glGenBuffers(1, &uvbuffer);
	glBindBuffer(GL_ARRAY_BUFFER, uvbuffer);
	glBufferData(GL_ARRAY_BUFFER, uvs.size() * sizeof(glm::vec2), &uvs[0], GL_STATIC_DRAW);



	// ----------------- cube ------------------------
	glGenVertexArrays(1, &vertexArrayID_cube);
	glBindVertexArray(vertexArrayID_cube);
	matrixID_cube = glGetUniformLocation(programID, "mvpCube");
	// Load the texture
	//texture_cube = loadDDS("uvmap.DDS");
	texture_cube = loadDDS("basketball.DDS");
	// Get a handle for our "myTextureSampler" uniform
	textureID_cube = glGetUniformLocation(programID, "myTextureSampler_cube");
	bool sq = loadOBJ("cube.obj", vertices_cube, uvs_cube, normals_cube);
	//vertexbuffer;
	glGenBuffers(1, &vb_cube);
	glBindBuffer(GL_ARRAY_BUFFER, vb_cube);
	glBufferData(GL_ARRAY_BUFFER, vertices_cube.size() * sizeof(glm::vec3), &vertices_cube[0], GL_STATIC_DRAW);
	//uvbuffer;
	glGenBuffers(1, &ub_cube);
	glBindBuffer(GL_ARRAY_BUFFER, ub_cube);
	glBufferData(GL_ARRAY_BUFFER, uvs_cube.size() * sizeof(glm::vec2), &uvs_cube[0], GL_STATIC_DRAW);
	
	//-------------- Gong --------------
	texture_gong = loadDDS("bronze.dds");
	textureID_cube = glGetUniformLocation(programID, "myTextureSampler_gong");

	//------------- Candy Cane -----------
	texture_cc = loadDDS("candycane.dds");
	textureID_cc = glGetUniformLocation(programID, "myTextureSamepler_cc");

	//------------- Grass ---------------
	texture_grass = loadDDS("grass.dds");
	textureID_grass = glGetUniformLocation(programID, "myTextureSampler_grass");


	// For speed computation
	double lastTime = glfwGetTime();
	double lastFrameTime = lastTime;
	int nbFrames = 0;



	//------------------ Initialize Matrices ----------------
	//ProjectionMatrix = glm::perspective(45.0f, 4.0f / 3.0f, 0.1f, 100.0f);
	ProjectionMatrix = glm::perspective(45.0f, 4.0f / 3.0f, 0.1f, 1000.0f);

	targetTranslationMatrix = translate(mat4(), targetPos);
	targetModelMatrix = targetTranslationMatrix * mat4(1.0) * scale(mat4(), targetScale);


	//------------------ Create Bones --------------------------
	vec3 scale = vec3(1, 1, 1);
	
	int Y = 0;
	for (int i = 0; i < NUMBONES - 1; i++)
	{
		bone[i] = Bone(i, vec3(0, Y, 0), scale, vec3(0, -Y, 0));
		Y += 2;
	}
	bone[NUMBONES - 1] = Bone(NUMBONES - 1, vec3(0, (NUMBONES - 1) * 2, 0), vec3(0.1, 0.1, 0.1), vec3(0, (NUMBONES - 1) * -2, 0));
	

	//cubeBone = Bone(NUMBONES + 1, cubePos, scale, vec3(-4.0f, 0.0f, 0.0f));
	//cubeSkeleton.loadBone(&cubeBone);

	//----------------- Make relationships ----------------
	bone[0].isRoot = true;
	bone[0].addChild(&bone[1]);
	for (int i = 1; i < NUMBONES - 1; i++)
	{
		bone[i].addParent(&bone[i - 1]);
		bone[i].addChild(&bone[i + 1]);
	}
	bone[NUMBONES - 1].addParent(&bone[NUMBONES - 2]);
	

	//------------------ Load Skeleton ------------------------
	for (int i = 0; i < NUMBONES; i++)
		skeleton.loadBone(&bone[i]);

	// ---------------- loop -------------------
	do{

		// Measure speed
		double currentTime = glfwGetTime();
		float deltaTime = (float)(currentTime - lastFrameTime);
		lastFrameTime = currentTime;
		nbFrames++;
		if (currentTime - lastTime >= 1.0){ // If last prinf() was more than 1sec ago
			// printf and reset
			//printf("%f ms/frame\n", 1000.0 / double(nbFrames));
			nbFrames = 0;
			lastTime += 1.0;
		}
		//std::cout << "currentTime: " << currentTime << std::endl;
		//std::cout << "deltaTime: " << deltaTime << std::endl;

		// Clear the screen
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		glUseProgram(programID);



		//------------- Update Camera --------------------

		//camMoved = false; // this line disables 
		ViewMatrix = glm::lookAt(eye, center, glm::vec3(0, 1, 0)); // lab 4 initial view matrix
		if (camMoved)
		{
			glm::quat view_rotation(radians(view_angles));
			ViewOrientation = ViewOrientation * view_rotation;
			ViewRotationMatrix = toMat4(ViewOrientation);
			camMoved = false;
		}
		ViewMatrix = ViewMatrix * ViewRotationMatrix; // switch multiplication to switch to first person



		//update target
		targetTranslationMatrix = translate(mat4(1.0), targetPos);


		//------------- Draw Spheres-----------------------
		drawSkeleton(skeleton, ProjectionMatrix, ViewMatrix);
		drawTarget(ProjectionMatrix, ViewMatrix);

		drawBall(ProjectionMatrix, ViewMatrix, ballPos, ballScale);

		drawGong(ProjectionMatrix, ViewMatrix, gongPos, gongScale);

		drawCC(ProjectionMatrix, ViewMatrix, ccPos, ccScale);

		drawGrass(ProjectionMatrix, ViewMatrix, grassPos, grassScale);
		// ------------ draw cube ------------------
		//drawCube(ProjectionMatrix, ViewMatrix);


		// check for user input
		checkKeys(ProjectionMatrix, ViewMatrix, deltaTime);


		// ------ follow curve ----------------
		float t = glm::mod((float)currentTime, 10.0f) / (5.0f);
		//std::cout << "t: " << t << std::endl;
		
		if (t >= 1)
		{
			curveOne = false;
			t = t - 1;
		}
		else if (t < 1)
			curveOne = true;

		if (followCurve)
			targetPos = bezierCurve(t, p0, p1, p2, p3, p4, p5);
	

		if (thrown)
		{
			timePassed += deltaTime;
			//std::cout << "deltaTime " << deltaTime << std::endl;
			//std::cout << "timePassed " << timePassed << std::endl;
			

			if (velocity0_b.y < 0)
				gravity = gravityDown;
			else gravity = gravityUp;


			//std::cout << "gravityUP " << gravityUp << "\t\tgravityDown " << gravityDown << std::endl;
			std::cout << "velocity " << velocity0_b.y << std::endl;

			if (timeToZero)
			{
				timePassed = 0;
				timeToZero = false;
			}

			if (hitting)
			{
				velocity_t.y = velocity0_t.y + (gravity * gravityWeight * (timePassed / timeWeight));
				velocity_t.z = velocity0_t.z;

				targetPos.z += velocity0_t.z * (timePassed / timeWeight);
				targetPos.y += velocity0_t.y * (timePassed / timeWeight) + (((gravity * gravityWeight) / 2) * (pow((timePassed / timeWeight), 2)));

			}
			else
			{
				velocity_b.y = velocity0_b.y + (gravity * gravityWeight * (timePassed / timeWeight));
				velocity_b.z = velocity0_b.z;

				ballPos.z += velocity0_b.z * (timePassed / timeWeight);
				ballPos.y += velocity0_b.y * (timePassed / timeWeight) + (((gravity * gravityWeight) / 2) * (pow((timePassed / timeWeight), 2)));

				//velocity_t.y = velocity0_t.y + (gravity * gravityWeight * (timePassed / timeWeight));
				//velocity_t.z = velocity0_t.z;
				if (velocity0_t.y < 0)
					velocity0_t.y += 0.1;

				if (velocity0_t.z < 0)
					velocity0_t.z += 0.1;

				if (velocity0_t.y >= 0 && velocity0_t.z >= 0)
				{
					targetPos.z += velocity0_t.z * (timePassed / timeWeight);
					targetPos.y += velocity0_t.y * (timePassed / timeWeight) + (((gravity * gravityWeight) / 2) * (pow((timePassed / timeWeight), 2)));
				}
				//targetPos = ballPos;
			}

			//targetPos = ballPos;

			//std::cout << "velocity: [" << velocity.x << ", " << velocity.y << ", " << velocity.z << "]\t";
			//std::cout << "\tballPos: [" << ballPos.x << ", " << ballPos.y << ", " << ballPos.z << "]\n";

			if (ballPos.y >= (gongPos.y - (gongScale.y/2)) && velocity0_b.y > 0)
			{
				squash = true;
				exaggerateGong = true;
			}

			if (ballPos.y <= 1 && velocity_b.y < 0 && !hitting)
				squash = true;

			if (hitting && targetPos.y <= 1 && velocity_t.y < 0)
			{
				std::cout << "Go!\n";
				velocity0_b.y = abs(velocity_t.y);
				velocity0_t.y = abs(velocity_t.y);
				velocity0_t.z = -abs(velocity_t.z);
				
				timeToZero = true;
				//timePassed = 0;
				hitting = false;
			}
			
			//center.z = ballPos.z;
			//center.y = (ballPos.y - 5);
		}

		if (revert)
		{
			ballScale.x += 0.5;
			ballScale.y -= 0.5;
			if (ballScale.x == 1.0 && ballScale.y == 1.0)
				revert = false;
		}

		if (stretch)
		{
			ballScale.x -= 0.5;
			ballScale.y += 0.5;
			if (ballScale.x == 0.5 && ballScale.y == 1.5)
			{
				stretch = false;
				revert = true;
			}
		}

		if (squash)
		{
			ballScale.x += 0.5;
			ballScale.y -= 0.5;
			//hitting bottom
			if (ballPos.y <= 0.5)
			{
				squash = false;
				stretch = true;
				//if (!ballIsZero)
				velocity0_b.y = abs(velocity_b.y);
				//gravityUp++;
				//if (gravityUp <= 0)
					//gravityUp = 0;
				velocity0_b.z = 0;

				timeToZero = true;
				//timePassed = 0;
				//simulateForce = true;
			}
			//hitting gong
			else if (ballPos.y >= (gongPos.y - (gongScale.y / 2) - 1))
			{
				squash = false;
				stretch = true;
				//exaggerateGong = false;
				//shrinkGong = true;
				//if (!ballIsZero)
				velocity0_b.y = -abs(velocity_b.y);
				velocity0_b.y++;
				if (velocity0_b.y > 0)
				{
					velocity0_b.y = 0;
					gravity = 0;
				}
				//gravityDown--;
				//if (gravityDown <= 0)
					//gravityDown = 0;
				//velocity0_b.y++;
				//if (velocity0_b.y >= 0)
					//velocity0_b.y = 0;

				//timePassed = 0;
				timeToZero = true;
			}
		}


		if (shrinkGong)
		{
			if (gongScale.x > 3.0)
				gongScale.x -= 0.1;
			if (gongScale.y > 3.0)
				gongScale.y -= 0.1;
			if (gongScale.x == 3.0f && gongScale.y == 3.0f)
				shrinkGong = false;
		}

		if (exaggerateGong)
		{
			gongScale.x += 0.1;
			gongScale.y += 0.1;
			if (gongScale.x >= 4.0 && gongScale.y >= 4.0)
			{
				exaggerateGong = false;
				shrinkGong = true;
			}
			simulateForce = true;
		}

		// if end effector is not at target, calculate CCD
		if (targetPos != bone[NUMBONES - 1].pivotPoint)
			CCD();
			


		//glDisableVertexAttribArray(0);
		//glDisableVertexAttribArray(1);
		
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

	glDeleteBuffers(1, &vb_cube);
	glDeleteBuffers(1, &ub_cube);
	//glDeleteProgram(programID_cube);
	glDeleteTextures(1, &textureID_cube);
	glDeleteVertexArrays(1, &vertexArrayID_cube);

	glDeleteTextures(1, &textureID_gong);

	// Close OpenGL window and terminate GLFW
	glfwTerminate();

	return 0;
}


// ----- Cyclic Coordinate Descent ---------------
// Updates all bones in skeleton according to CCD algorithm
void CCD()
{
	vec3 targetVector, endEffector, endEffectorVector, rotAxis;
	float angleBetween;
	endEffector = bone[NUMBONES - 1].pivotPoint;
	for (int j = 0; j < 50; j++)
	{
		if (targetPos == endEffector)
			return;

		for (int i = NUMBONES - 2; i > -1; i--)
		{
			if (targetPos != endEffector) // if end effector has reached target stop
			{
				targetVector = vec3(vec3(targetPos.x, targetPos.y, targetPos.z) - vec3(bone[i].pivotPoint.x, bone[i].pivotPoint.y, bone[i].pivotPoint.z));
				endEffectorVector = vec3(vec3(endEffector.x, endEffector.y, endEffector.z) - vec3(bone[i].pivotPoint.x, bone[i].pivotPoint.y, bone[i].pivotPoint.z));
				angleBetween = acos(dot(normalize(targetVector), normalize(endEffectorVector))); // angle between the EEvector and Tvector
				rotAxis = vec3(cross(normalize(targetVector), normalize(endEffectorVector))); // the normal of the plane that EEvector and Tvector sit on

				if (angleBetween >= 0 || angleBetween < 0) // to counteract if the angle is NAN 
					skeleton.update(&bone[i], nullVec, angleBetween, rotAxis);

				//else std::cout << angleBetween << std::endl;
			}
			endEffector = bone[NUMBONES - 1].pivotPoint;
		}
	}
}

// returns a position on the curve relative to t
vec3 bezierCurve(float t, vec3 p0, vec3 p1, vec3 p2, vec3 p3, vec3 p4, vec3 p5)
{
	if (curveOne)
		return (pow((1 - t), 3) * p0) + (3 * t*pow((1 - t), 2) * p1) + (3 * t*pow((1 - t), 2) * p2) + (pow(t, 3) * p3);
	else
		return (pow((1 - t), 3) * p3) + (3 * t*pow((1 - t), 2) * p4) + (3 * t*pow((1 - t), 2) * p5) + (pow(t, 3) * p0);
}

void drawSkeleton(Skeleton skeleton, mat4 ProjectionMatrix, mat4 ViewMatrix)
{
	// Use our shader
	//glUseProgram(programID);
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

	for (int i = 0; i < skeleton.numBones; i++)
	{
		ModelMatrix = skeleton.myBone[i]->modelMatrix;
		MVP = ProjectionMatrix * ViewMatrix * ModelMatrix;
		glUniformMatrix4fv(MatrixID, 1, GL_FALSE, &MVP[0][0]);
		glDrawArrays(GL_TRIANGLES, 0, vertices.size());
	}
	glDisableVertexAttribArray(0);
	glDisableVertexAttribArray(1);
}

void drawCube(mat4 ProjectionMatrix, mat4 ViewMatrix)
{
	glUseProgram(programID);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, texture_cube);
	glUniform1i(textureID_cube, 0);

	// 1rst attribute buffer : vertices
	glEnableVertexAttribArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, vb_cube);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);

	// 2nd attribute buffer : UVs
	glEnableVertexAttribArray(1);
	glBindBuffer(GL_ARRAY_BUFFER, ub_cube);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 0, (void*)0);

	//cubeTranslationMatrix = translate(mat4(1.0), cubePos);
	//cubeModelMatrix = cubeTranslationMatrix * mat4(1.0) * scale(mat4(), vec3(1.0f, 1.0f, 1.0f));
	cubeModelMatrix = cubeSkeleton.myBone[NUMBONES + 1]->modelMatrix;
	//std::cout << cubePos.x << " " << cubePos.y << " " << cubePos.z << std::endl;

	mvpCube = ProjectionMatrix * ViewMatrix * cubeModelMatrix;
	glUniformMatrix4fv(matrixID_cube, 1, GL_FALSE, &mvpCube[0][0]);
	glDrawArrays(GL_TRIANGLES, 0, vertices_cube.size());
	glDisableVertexAttribArray(0);
	glDisableVertexAttribArray(1);
}

void drawTarget(mat4 ProjectionMatrix, mat4 ViewMatrix)
{
	// Use our shader
	//glUseProgram(programID);
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

	targetModelMatrix = targetTranslationMatrix * mat4(1.0) * scale(mat4(), targetScale);
	MVP = ProjectionMatrix * ViewMatrix * targetModelMatrix;
	glUniformMatrix4fv(MatrixID, 1, GL_FALSE, &MVP[0][0]);
	glDrawArrays(GL_TRIANGLES, 0, vertices.size());

	glDisableVertexAttribArray(0);
	glDisableVertexAttribArray(1);
}

void drawBall(mat4 ProjectionMatrix, mat4 ViewMatrix, vec3 ballPos, vec3 ballScale)
{
	// Use our shader
	//glUseProgram(programID);
	// Bind our texture in Texture Unit 0
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, texture_cube);
	// Set our "myTextureSampler" sampler to user Texture Unit 0
	glUniform1i(textureID_cube, 0);

	// 1rst attribute buffer : vertices
	glEnableVertexAttribArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, vertexbuffer);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);

	// 2nd attribute buffer : UVs
	glEnableVertexAttribArray(1);
	glBindBuffer(GL_ARRAY_BUFFER, uvbuffer);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 0, (void*)0);

	//targetModelMatrix = targetTranslationMatrix * mat4(1.0) * scale(mat4(), targetScale);
	//cubeModelMatrix = cubeSkeleton.myBone[NUMBONES + 1]->modelMatrix;
	ballModelMatrix = translate(mat4(1.0), ballPos) * mat4(1.0)  * scale(mat4(1.0), ballScale);
	MVP = ProjectionMatrix * ViewMatrix * ballModelMatrix;
	glUniformMatrix4fv(MatrixID, 1, GL_FALSE, &MVP[0][0]);
	glDrawArrays(GL_TRIANGLES, 0, vertices.size());

	glDisableVertexAttribArray(0);
	glDisableVertexAttribArray(1);
}

void drawGong(mat4 ProjectionMatrix, mat4 ViewMatrix, vec3 gongPos, vec3 gongScale)
{
	// Use our shader
	//glUseProgram(programID);
	// Bind our texture in Texture Unit 0
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, texture_gong);
	// Set our "myTextureSampler" sampler to user Texture Unit 0
	glUniform1i(textureID_gong, 0);

	// 1rst attribute buffer : vertices
	glEnableVertexAttribArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, vertexbuffer);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);

	// 2nd attribute buffer : UVs
	glEnableVertexAttribArray(1);
	glBindBuffer(GL_ARRAY_BUFFER, uvbuffer);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 0, (void*)0);

	gongModelMatrix = translate(mat4(1.0), gongPos) * mat4(1.0)  * scale(mat4(1.0), gongScale);
	MVP = ProjectionMatrix * ViewMatrix * gongModelMatrix;
	glUniformMatrix4fv(MatrixID, 1, GL_FALSE, &MVP[0][0]);
	glDrawArrays(GL_TRIANGLES, 0, vertices.size());

	glDisableVertexAttribArray(0);
	glDisableVertexAttribArray(1);
}

void drawCC(mat4 ProjectionMatrix, mat4 ViewMatrix, vec3 ccPos, vec3 ccScale)
{
	// Use our shader
	//glUseProgram(programID);
	// Bind our texture in Texture Unit 0
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, texture_cc);
	// Set our "myTextureSampler" sampler to user Texture Unit 0
	glUniform1i(textureID_cc, 0);

	// 1rst attribute buffer : vertices
	glEnableVertexAttribArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, vertexbuffer);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);

	// 2nd attribute buffer : UVs
	glEnableVertexAttribArray(1);
	glBindBuffer(GL_ARRAY_BUFFER, uvbuffer);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 0, (void*)0);

	ccModelMatrix = translate(mat4(1.0), ccPos) * mat4(1.0)  * scale(mat4(1.0), ccScale);
	MVP = ProjectionMatrix * ViewMatrix * ccModelMatrix;
	glUniformMatrix4fv(MatrixID, 1, GL_FALSE, &MVP[0][0]);
	glDrawArrays(GL_TRIANGLES, 0, vertices.size());

	glDisableVertexAttribArray(0);
	glDisableVertexAttribArray(1);
}

void drawGrass(mat4 ProjectionMatrix, mat4 ViewMatrix, vec3 grassPos, vec3 grassScale)
{
	// Use our shader
	//glUseProgram(programID);
	// Bind our texture in Texture Unit 0
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, texture_grass);
	// Set our "myTextureSampler" sampler to user Texture Unit 0
	glUniform1i(textureID_grass, 0);

	// 1rst attribute buffer : vertices
	glEnableVertexAttribArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, vertexbuffer);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);

	// 2nd attribute buffer : UVs
	glEnableVertexAttribArray(1);
	glBindBuffer(GL_ARRAY_BUFFER, uvbuffer);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 0, (void*)0);

	grassModelMatrix = translate(mat4(1.0), grassPos) * mat4(1.0)  * scale(mat4(1.0), grassScale);
	MVP = ProjectionMatrix * ViewMatrix * grassModelMatrix;
	glUniformMatrix4fv(MatrixID, 1, GL_FALSE, &MVP[0][0]);
	glDrawArrays(GL_TRIANGLES, 0, vertices.size());

	glDisableVertexAttribArray(0);
	glDisableVertexAttribArray(1);

}

void checkKeys(mat4 ProjectionMatrix, mat4 ViewMatrix, float deltaTime)
{
	if (glfwGetKey(window, GLFW_KEY_B) == GLFW_PRESS)
		bounce = true;
	if (glfwGetKey(window, GLFW_KEY_T) == GLFW_PRESS)
	{
		thrown = true;
	}
	// ---------- Curve controls ---------------------------
	if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS)
		followCurve = true;

	if (glfwGetKey(window, GLFW_KEY_ENTER) == GLFW_PRESS)
		followCurve = false;

	// ------------ target controls --------------------------
	if ((glfwGetKey(window, GLFW_KEY_1) == GLFW_PRESS) && (glfwGetKey(window, GLFW_KEY_9) == GLFW_PRESS)){
		targetPos += right;
		updated = true;
	}
	if ((glfwGetKey(window, GLFW_KEY_1) == GLFW_PRESS) && (glfwGetKey(window, GLFW_KEY_0) == GLFW_PRESS)){
		targetPos += left;
		updated = true;
	}
	if ((glfwGetKey(window, GLFW_KEY_2) == GLFW_PRESS) && (glfwGetKey(window, GLFW_KEY_9) == GLFW_PRESS)){
		targetPos += up;
		updated = true;
	}
	if ((glfwGetKey(window, GLFW_KEY_2) == GLFW_PRESS) && (glfwGetKey(window, GLFW_KEY_0) == GLFW_PRESS)){
		targetPos += down;
		updated = true;
	}
	if ((glfwGetKey(window, GLFW_KEY_3) == GLFW_PRESS) && (glfwGetKey(window, GLFW_KEY_0) == GLFW_PRESS)){
		targetPos += back;
		updated = true;
	}
	if ((glfwGetKey(window, GLFW_KEY_3) == GLFW_PRESS) && (glfwGetKey(window, GLFW_KEY_9) == GLFW_PRESS)){
		targetPos += forth;
		updated = true;
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
