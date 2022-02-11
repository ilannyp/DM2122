#include "Scene4.h"
#include "GL\glew.h"
#include "Application.h"
#include "shader.hpp"
#include "Mtx44.h"
#include "MeshBuilder.h"


Scene4::Scene4()
{

}

Scene4::~Scene4()
{
}

void Scene4::Init()
{

	// Set background colour to light purple
	glClearColor(0.5f, 0.0f, 0.7f, 0.0f);
	// Enable depth test
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_CULL_FACE);
	camera.Init(Vector3(4, 3, 3), Vector3(0, 0, 0), Vector3(0, 1, 0));

	m_programID = LoadShaders("Shader//TransformVertexShader.vertexshader", "Shader//SimpleFragmentShader.fragmentshader");
	m_parameters[U_MVP] = glGetUniformLocation(m_programID, "MVP");
	glUseProgram(m_programID);

	// Generate a default VAO for now
	glGenVertexArrays(1, &m_vertexArrayID);
	glBindVertexArray(m_vertexArrayID);	

	//meshList[GEO_AXES] = MeshBuilder::GenerateAxes("reference",1000,1000,1000);
	/*meshList[GEO_QUAD] = MeshBuilder::GenerateQuad("quad", Color(1, 1, 0));*/
	meshList[GEO_CUBE] = MeshBuilder::GenerateCube("cube",1,1);
	
	//An array of 3 vectors which represents the colors of the 3 vertices

	

}

void Scene4::Update(double dt)
{
	rotateAngle += (float)(60 * dt);
	//rotateAngleStars += (float)(-360 * dt);
	//translateX += (float)(20 * dt);
	//if (translateX > 140)
	//{
	//	translateX -= 140;
	//}
	//smoke += (float)(10 * dt);
	//if (smoke > 60)
	//{
	//	smoke -= 60;
	//}
	//scaleAll += (float)(reverseScale * dt);
	//if(scaleAll > 4)
	//{
	//	reverseScale *= -1;
	//}
	//else if (scaleAll < 1)
	//{
	//	reverseScale *= -1;
	//}
	if (Application::IsKeyPressed('1'))
	{
		glEnable(GL_CULL_FACE);
	}
	if (Application::IsKeyPressed('2'))
	{
		glDisable(GL_CULL_FACE);
	}
	if (Application::IsKeyPressed('3'))
	{
		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL); //default fill mode
	}
	if (Application::IsKeyPressed('4'))
	{
		glPolygonMode(GL_FRONT_AND_BACK, GL_LINE); //wireframe mode
	}
	camera.Update(dt);

}

void Scene4::Render()
{
	// Render VBO here
	//Clear color buffer every frame
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	Mtx44 translate, rotate, scale;
	Mtx44 model;
	Mtx44 view;
	Mtx44 projection;
	Mtx44 MVP;
	translate.SetToIdentity();
	rotate.SetToIdentity();
	scale.SetToIdentity();
	model.SetToIdentity();
	view.SetToLookAt(camera.position.x, camera.position.y, camera.position.z,camera.target.x,camera.target.y,camera.target.z,camera.up.x,camera.up.y,camera.up.z);		//no need for camera now, set it at World's origin
	//projection.SetToOrtho(-40, 40, -30, 30, -10, 10);
	
	projection.SetToPerspective(45.0f, 4.0f / 3.0f, 0.1f, 1000.0f); //FOV, Aspect Ratio, Near plane, Far plane


	//model = translate * rotate * scale; //scale, followed by rotate, then lastly translate
	//MVP = projection * view * model; // MVP = PVM, other way around
	//glUniformMatrix4fv(m_parameters[U_MVP], 1, GL_FALSE, &MVP.a[0]); //update the shader with new MVP
	//meshList[GEO_AXES]->Render();

	//scale.SetToScale(2, 2, 2);
	//rotate.SetToRotation(rotateAngle, 0, 0, 1);
	//translate.SetToTranslation(0, 0, 0);
	//model = translate * rotate * scale; //scale, followed by rotate, then lastly translate
	//MVP = projection * view * model; // MVP = PVM, other way around
	//glUniformMatrix4fv(m_parameters[U_MVP], 1, GL_FALSE, &MVP.a[0]); //update the shader with new MVP
	//meshList[GEO_QUAD]->Render();

	scale.SetToScale(2, 2, 2);
	rotate.SetToRotation(rotateAngle, 0, 0, 1);
	translate.SetToTranslation(0, 0, 0);
	model = translate * rotate * scale; //scale, followed by rotate, then lastly translate
	MVP = projection * view * model; // MVP = PVM, other way around
	glUniformMatrix4fv(m_parameters[U_MVP], 1, GL_FALSE, &MVP.a[0]); //update the shader with new MVP
	meshList[GEO_CUBE]->Render();

}

void Scene4::Exit()
{
	// Cleanup VBO here
	/*delete meshList[GEO_AXES];*/
	//delete meshList[GEO_QUAD];
	delete meshList[GEO_CUBE];
	glDeleteVertexArrays(1, &m_vertexArrayID);
	glDeleteProgram(m_programID);
}
