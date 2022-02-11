#include "Scene2.h"
#include "GL\glew.h"

#include "shader.hpp"
#include "Mtx44.h"


Scene2::Scene2()
{
}

Scene2::~Scene2()
{
}

void Scene2::Init()
{
	rotateAngle = 30;
	translateX = 2;
	scaleAll = 8;
	// Init VBO here
	
	// Set background colour to dark blue
	glClearColor(0.0f, 0.0f, 0.4f, 0.0f);
	// Enable depth test
	glEnable(GL_DEPTH_TEST);
	// Generate a default VAO for now
	glGenVertexArrays(1, &m_vertexArrayID);
	glBindVertexArray(m_vertexArrayID);

	//generate buffers
	glGenBuffers(NUM_GEOMETRY, &m_vertexBuffer[0]);
	glGenBuffers(NUM_GEOMETRY, &m_colorbuffer[0]);

	//An array of 3 vectors which represents 3 vertices
	static const GLfloat vertex_buffer_data[] =
	{
		0.0f, 0.2f, 0.0f,
		-0.3f, 0.0f, 0.0f,
		0.0f, -0.2f, 0.0f,
		0.3f, 0.0f, 0.0f,

	};
	//Set the current active buffer
	glBindBuffer(GL_ARRAY_BUFFER, m_vertexBuffer[GEO_TRIANGLE_1]);
	//Transfer vertices to OpenGL
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertex_buffer_data), vertex_buffer_data, GL_STATIC_DRAW);
	//An array of 3 vectors which represents the colors of the 3 vertices
	static const GLfloat color_buffer_data[] = {
		0.2f, 0.6f, 0.4f,
		0.4f, 0.2f, 0.6f,
		0.6f, 0.4f, 0.2f,
		0.4f, 0.2f, 0.6f,

	};
	glBindBuffer(GL_ARRAY_BUFFER, m_colorbuffer[GEO_TRIANGLE_1]);
	glBufferData(GL_ARRAY_BUFFER, sizeof(color_buffer_data),
		color_buffer_data, GL_STATIC_DRAW);

	//An array of 3 vectors which represents 3 vertices
	static const GLfloat vertex_buffer_data_2[] =
	{
	-1.0f, 1.0f, -0.5f, //vertex 0 of triangle
	-1.0f, -1.0f, -0.5f, //vertex 1 of triangle
	1.0f, 1.0f, -0.5f,  //vertex 2 of triangle	
	1.0f, -1.0f, -0.5f	//vertex 3
	-1.0f, -1.0f, -0.5f,  //vertex 2 of triangle	
	1.0f, 1.0f, -0.5f
	};
	//Set the current active buffer
	glBindBuffer(GL_ARRAY_BUFFER, m_vertexBuffer[GEO_TRIANGLE_2]);
	//Transfer vertices to OpenGL
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertex_buffer_data_2), vertex_buffer_data_2, GL_STATIC_DRAW);
	//An array of 3 vectors which represents the colors of the 3 vertices
	static const GLfloat color_buffer_data_2[] = {
		0.3f, 0.3f, 0.5f,			//color of vertex 0
		0.3f, 0.3f, 0.5f,			//color of vertex 1
		0.3f, 0.3f, 0.5f,		//color of vertex 2
		0.3f, 0.3f, 0.5f,
		0.3f, 0.3f, 0.5f,		//color of vertex 2
		0.3f, 0.3f, 0.5f
	};
	glBindBuffer(GL_ARRAY_BUFFER, m_colorbuffer[GEO_TRIANGLE_2]);
	glBufferData(GL_ARRAY_BUFFER, sizeof(color_buffer_data_2), color_buffer_data_2, GL_STATIC_DRAW);

	m_programID = LoadShaders("Shader//TransformVertexShader.vertexshader", "Shader//SimpleFragmentShader.fragmentshader");
	m_parameters[U_MVP] = glGetUniformLocation(m_programID, "MVP");
	glUseProgram(m_programID);

}

void Scene2::Update(double dt)
{
	rotateAngle += (float)(10 * dt);
	translateX += (float)(1 * dt);
	scaleAll += (float)(2 * dt);
}

void Scene2::Render()
{
	// Render VBO here
	//Clear color buffer every frame
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glEnableVertexAttribArray(0); // 1st attribute buffer :
	glEnableVertexAttribArray(1); // 2nd attribute buffer : colors

	Mtx44 translate, rotate, scale;
	Mtx44 model;
	Mtx44 view;
	Mtx44 projection;
	Mtx44 MVP;
	translate.SetToIdentity();
	rotate.SetToIdentity();
	scale.SetToIdentity();
	model.SetToIdentity();
	view.SetToIdentity();		//no need for camera now, set it at World's origin
	projection.SetToOrtho(-10, 10, -10, 10, -10, 10);

	scale.SetToScale(8, 2, 1);
	rotate.SetToRotation(rotateAngle, 0, 0, 1);
	translate.SetToTranslation(0, 0, 0);
	model = translate * rotate * scale; //scale, followed by rotate, then lastly translate
	MVP = projection * view * model; // MVP = PVM, other way around
	glUniformMatrix4fv(m_parameters[U_MVP], 1, GL_FALSE, &MVP.a[0]); //update the shader with new MVP

	glBindBuffer(GL_ARRAY_BUFFER, m_vertexBuffer[GEO_TRIANGLE_1]);
	glVertexAttribPointer(0,	//attribute 0, must match layout of shader, usually 0 is for vertex
		3,			//size	
		GL_FLOAT,	//type
		GL_FALSE,	//normalized?
		0,			//stride
		0);			//array buffer offset
	glBindBuffer(GL_ARRAY_BUFFER, m_colorbuffer[GEO_TRIANGLE_1]);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, 0);	
	

	glDrawArrays(GL_TRIANGLES, 0, 3);
	
	scale.SetToScale(8, 14, 1);
	rotate.SetToRotation(5, 0, 0, 1);
	translate.SetToTranslation(translateX, -5, 0);
	model = translate * rotate * scale; //scale, followed by rotate, then lastly translate
	MVP = projection * view * model; // MVP = PVM, other way around
	glUniformMatrix4fv(m_parameters[U_MVP], 1, GL_FALSE, &MVP.a[0]); //update the shader with new MVP

	glBindBuffer(GL_ARRAY_BUFFER, m_vertexBuffer[GEO_TRIANGLE_1]);
	glVertexAttribPointer(0,	//attribute 0, must match layout of shader, usually 0 is for vertex
		3,			//size	
		GL_FLOAT,	//type
		GL_FALSE,	//normalized?
		0,			//stride
		0);			//array buffer offset
	glBindBuffer(GL_ARRAY_BUFFER, m_colorbuffer[GEO_TRIANGLE_1]);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, 0);

	
	glDrawArrays(GL_TRIANGLES, 0, 3);
	
	scale.SetToScale(scaleAll, 6, 1);
	rotate.SetToRotation(5, 0, 0, 1);
	translate.SetToTranslation(10, -1, 0);
	model = translate * rotate * scale; //scale, followed by rotate, then lastly translate
	MVP = projection * view * model; // MVP = PVM, other way around
	glUniformMatrix4fv(m_parameters[U_MVP], 1, GL_FALSE, &MVP.a[0]); //update the shader with new MVP

	glBindBuffer(GL_ARRAY_BUFFER, m_vertexBuffer[GEO_TRIANGLE_1]);
	glVertexAttribPointer(0,	//attribute 0, must match layout of shader, usually 0 is for vertex
		3,			//size	
		GL_FLOAT,	//type
		GL_FALSE,	//normalized?
		0,			//stride
		0);			//array buffer offset
	glBindBuffer(GL_ARRAY_BUFFER, m_colorbuffer[GEO_TRIANGLE_1]);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, 0);


	glDrawArrays(GL_TRIANGLES, 0, 3);
	glBindBuffer(GL_ARRAY_BUFFER, m_vertexBuffer[GEO_TRIANGLE_2]);
	glVertexAttribPointer(0,	//attribute 0, must match layout of shader, usually 0 is for vertex
		3,			//size	
		GL_FLOAT,	//type
		GL_FALSE,	//normalized?
		0,			//stride
		0);			//array buffer offset
	glBindBuffer(GL_ARRAY_BUFFER, m_colorbuffer[GEO_TRIANGLE_2]);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, 0);
	glDrawArrays(GL_TRIANGLES, 0, 6);
	

	glDisableVertexAttribArray(0);
	glDisableVertexAttribArray(1);
}

void Scene2::Exit()
{
	// Cleanup VBO here
	glDeleteBuffers(NUM_GEOMETRY, &m_vertexBuffer[0]);
	glDeleteBuffers(NUM_GEOMETRY, &m_colorbuffer[0]);
	glDeleteVertexArrays(1, &m_vertexArrayID);
	glDeleteProgram(m_programID);
}
