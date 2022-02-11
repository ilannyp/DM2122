#include "Scene5.h"
#include "GL\glew.h"
#include "Application.h"
#include "shader.hpp"
#include "Mtx44.h"
#include "MeshBuilder.h"


Scene5::Scene5()
{

}

Scene5::~Scene5()
{
}

void Scene5::Init()
{


	// Set background colour to light purple
	glClearColor(0.5f, 0.0f, 0.7f, 0.0f);
	// Enable depth test
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_CULL_FACE);
	camera.Init(Vector3(100, 80, 80), Vector3(0, 0, 0), Vector3(0, 1, 0));

	m_programID = LoadShaders("Shader//TransformVertexShader.vertexshader", "Shader//SimpleFragmentShader.fragmentshader");
	m_parameters[U_MVP] = glGetUniformLocation(m_programID, "MVP");
	glUseProgram(m_programID);

	// Generate a default VAO for now
	glGenVertexArrays(1, &m_vertexArrayID);
	glBindVertexArray(m_vertexArrayID);	
	rotateAngle = 0;
	for (int i = 0; i < NUM_GEOMETRY; ++i)
	{
		meshList[i] = nullptr;
	}
	meshList[GEO_AXES] = MeshBuilder::GenerateAxes("reference",1000,1000,1000);
	/*meshList[GEO_QUAD] = MeshBuilder::GenerateQuad("quad", Color(1, 1, 0));*/
	meshList[GEO_CUBE] = MeshBuilder::GenerateCube("cube", Color(1, 0, 0), 1.f);
	meshList[GEO_CIRCLE] = MeshBuilder::GenerateCircle("circle", Color(0.2, 0.7, 1), 20, 1.f);
	meshList[GEO_SPHERE] = MeshBuilder::GenerateSphere("sphere", Color(1, 0.4, 0), 10, 20, 1.f);	
	meshList[GEO_SPHERE1] = MeshBuilder::GenerateSphere("sphere", Color(0, 1, 0), 10, 20, 1.f);
	meshList[GEO_SPHERE2] = MeshBuilder::GenerateSphere("sphere", Color(1, 0, 1), 10, 20, 1.f);
	meshList[GEO_SPHERE3] = MeshBuilder::GenerateSphere("sphere", Color(1, 1, 0.5), 10, 20, 1.f);
	meshList[GEO_SPHERE4] = MeshBuilder::GenerateSphere("sphere", Color(0, 0, 1), 10, 20, 1.f);
	//An array of 3 vectors which represents the colors of the 3 vertices

	Mtx44 projection;
	projection.SetToPerspective(45.f, 4.f / 3.f, 0.1f, 1000.f);
	projectionStack.LoadMatrix(projection);

}

void Scene5::Update(double dt)
{

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
	rotateAngle += (float)(20 * dt);
	translateY += (float)(5 * dt);
	camera.Update(dt);

}

void Scene5::Render()
{
	// Render VBO here
	//Clear color buffer every frame
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	viewStack.LoadIdentity();
	viewStack.LookAt(camera.position.x, camera.position.y,
		camera.position.z, camera.target.x, camera.target.y,
		camera.target.z, camera.up.x, camera.up.y, camera.up.z);
	modelStack.LoadIdentity();

	Mtx44 MVP = projectionStack.Top() * viewStack.Top() * modelStack.Top();
	glUniformMatrix4fv(m_parameters[U_MVP], 1, GL_FALSE, &MVP.a[0]); //update the shader with new MVP
	meshList[GEO_AXES]->Render();
	
	//Sun system
	modelStack.PushMatrix();
	{
		modelStack.Translate(0, 15, 0);

		//Planet system
		modelStack.PushMatrix();
		{
			modelStack.Rotate(rotateAngle, 0, 1, 0);
			modelStack.Translate(20, 0, 0);


			//Moon system
			modelStack.PushMatrix();
			{
				modelStack.Rotate(rotateAngle, 1, 0, 0);
				modelStack.Translate(0, 10, 0);
				meshList[GEO_SPHERE2]->Render();
			}
			modelStack.PopMatrix();
			//end moon

			modelStack.Rotate(rotateAngle, 0, 0, 1);
			modelStack.Scale(5, 5, 5);
			meshList[GEO_SPHERE3]->Render();
		}
		modelStack.PopMatrix();
		//end planet

		//Ring planet
		modelStack.PushMatrix();
		{
			modelStack.Rotate(-rotateAngle, 0, rotateAngle+200, rotateAngle);
			modelStack.Translate(30, 40, 0);

			//ring
			modelStack.PushMatrix();
			{
				modelStack.Rotate(rotateAngle, rotateAngle, 0, 0);
				modelStack.Scale(5, 5, 5);
				meshList[GEO_CIRCLE]->Render();
			}
			modelStack.PopMatrix();
			//end ring 
			
			//moon
			modelStack.PushMatrix();
			{
				modelStack.Rotate(rotateAngle,-rotateAngle,0,rotateAngle);
				modelStack.Translate(10, 5, 0);
				modelStack.Scale(1, 1, 1);
				meshList[GEO_SPHERE1]->Render();
			}
			modelStack.PopMatrix();
			//end moon


			modelStack.Rotate(0, 0, rotateAngle, 0);
			modelStack.Scale(3, 3, 3);
			meshList[GEO_SPHERE4]->Render();
		}
		modelStack.PopMatrix();

		modelStack.Rotate(rotateAngle, 0, 0, 1);
		modelStack.Scale(10, 10, 10);
		meshList[GEO_SPHERE]->Render();
	}
	modelStack.PopMatrix();
	//end sun
	
}

void Scene5::Exit()
{
	// Cleanup VBO here
	for (int i = 0; i < NUM_GEOMETRY; ++i)
	{
		if (meshList[i])
		{
			delete meshList[i];
		}
	}
	glDeleteVertexArrays(1, &m_vertexArrayID);
	glDeleteProgram(m_programID);
}
