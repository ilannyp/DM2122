#include "Scene3.h"
#include "GL\glew.h"

#include "shader.hpp"
#include "Mtx44.h"


Scene3::Scene3()
{
}

Scene3::~Scene3()
{
}

void Scene3::Init()
{
	rotateAngle = 30;
	rotateAngleStars = 30;
	translateX = 2;
	scaleAll = 1;
	reverseScale = 1;
	smoke = 2;
	// Init VBO here
	
	// Set background colour to light purple
	glClearColor(0.5f, 0.0f, 0.7f, 0.0f);
	// Enable depth test
	glEnable(GL_DEPTH_TEST);
	// Generate a default VAO for now
	glGenVertexArrays(1, &m_vertexArrayID);
	glBindVertexArray(m_vertexArrayID);

	//generate buffers
	glGenBuffers(NUM_GEOMETRY, &m_vertexBuffer[0]);
	glGenBuffers(NUM_GEOMETRY, &m_colorbuffer[0]);




	//An array of 3 vectors which represents 3 vertices
	

	// *************** LEAVES *****************************
	static const GLfloat vertex_buffer_data[] =
	{
		-1.0f, 0.0f, 0.0f,
		0.0f, 1.0f, 0.0f,
		1.0f, 0.0f, 0.0f,
		-1.0f, -0.3f, 0.0f,
		0.0f, 1.0f, 0.0f,
		1.0f, -0.3f, 0.0f

	};
	//Set the current active buffer
	glBindBuffer(GL_ARRAY_BUFFER, m_vertexBuffer[GEO_TRIANGLE_1]);
	//Transfer vertices to OpenGL
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertex_buffer_data), vertex_buffer_data, GL_STATIC_DRAW);
	//An array of 3 vectors which represents the colors of the 3 vertices
	static const GLfloat color_buffer_data[] = {
		0.2f, 0.6f, 0.0f,
		0.2f, 0.6f, 0.0f,
		0.2f, 0.6f, 0.0f,
		0.2f, 0.6f, 0.0f,
		0.2f, 0.6f, 0.0f,
		0.2f, 0.6f, 0.0f


	};
	glBindBuffer(GL_ARRAY_BUFFER, m_colorbuffer[GEO_TRIANGLE_1]);
	glBufferData(GL_ARRAY_BUFFER, sizeof(color_buffer_data),
		color_buffer_data, GL_STATIC_DRAW);

	//An array of 3 vectors which represents 3 vertices



	//******************* FLOOR **********************
	static const GLfloat vertex_buffer_data_2[] =
	{
	-1.0f, 0.0f, -0.5f, //vertex 0 of triangle
	1.0f, 0.0f, -0.5f, //vertex 1 of triangle
	-1.0f, -1.0f, -0.5f,  //vertex 2 of triangle	
	1.0f, 0.0f, -0.5f,	//vertex 3
	1.0f, -1.0f, -0.5f,  //vertex 2 of triangle	
	-1.0f, -1.0f, -0.5f
	};
	glBindBuffer(GL_ARRAY_BUFFER, m_vertexBuffer[GEO_TRIANGLE_2]);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertex_buffer_data_2), vertex_buffer_data_2, GL_STATIC_DRAW);

	static const GLfloat color_buffer_data_2[] = {
		0.6f, 1.0f, 0.0f,			//color of vertex 0
		0.6f, 1.0f, 0.0f,			//color of vertex 1
		0.9f, 1.0f, 0.0f,		//color of vertex 2
		0.8f, 1.0f, 0.0f,
		0.8f, 1.0f, 0.0f,		//color of vertex 2
		0.8f, 1.0f, 0.0f
	};
	glBindBuffer(GL_ARRAY_BUFFER, m_colorbuffer[GEO_TRIANGLE_2]);
	glBufferData(GL_ARRAY_BUFFER, sizeof(color_buffer_data_2), color_buffer_data_2, GL_STATIC_DRAW);




	//************** TREE WOOD ***************************
	static const GLfloat vertex_buffer_data_3[] =
	{
	-0.8f, -0.8f, 0.0f, //vertex 0 of triangle
	-0.6f, -0.8f, 0.0f, //vertex 1 of triangle
	-0.8f, -0.4f, 0.0f,  //vertex 2 of triangle	
	-0.6f, -0.4f, 0.0f,	//vertex 3
	-0.8f, -0.4f, 0.0f,  //vertex 2 of triangle	
	-0.6f, -0.8f, 0.0f

	};
	//Set the current active buffer
	glBindBuffer(GL_ARRAY_BUFFER, m_vertexBuffer[GEO_TRIANGLE_3]);
	//Transfer vertices to OpenGL
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertex_buffer_data_3), vertex_buffer_data_3, GL_STATIC_DRAW);
	//An array of 3 vectors which represents the colors of the 3 vertices
	static const GLfloat color_buffer_data_3[] = {
		0.8f, 0.4f, 0.4f,			//color of vertex 0
		0.7f, 0.3f, 0.3f,			//color of vertex 1
		0.8f, 0.4f, 0.4f,		//color of vertex 2
		0.8f, 0.4f, 0.4f,
		0.7f, 0.3f, 0.3f,		//color of vertex 2
		0.8f, 0.4f, 0.4f,
	
	};
	glBindBuffer(GL_ARRAY_BUFFER, m_colorbuffer[GEO_TRIANGLE_3]);
	glBufferData(GL_ARRAY_BUFFER, sizeof(color_buffer_data_3), color_buffer_data_3, GL_STATIC_DRAW);

	//*************************** STAR ******************************************************************************************
	static const GLfloat vertex_buffer_data_4[] =
	{
	0.0f, -1.0f, -0.3f, //vertex 0 of triangle
	-1.0f, 0.6f, -0.3f, //vertex 1 of triangle
	1.0f, 0.6f, -0.3f,  //vertex 2 of triangle	
	0.0f, 1.0f, -0.3f,	//vertex 3
	1.0f, -0.6f, -0.3f,  //vertex 2 of triangle	
	-1.0f, -0.6f, -0.3f
	};
	glBindBuffer(GL_ARRAY_BUFFER, m_vertexBuffer[GEO_TRIANGLE_4]);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertex_buffer_data_4), vertex_buffer_data_4, GL_STATIC_DRAW);

	static const GLfloat color_buffer_data_4[] = {
		0.9f, 0.8f, 0.5f,			//color of vertex 0
		0.9f, 0.8f, 0.5f,			//color of vertex 1
		0.9f, 0.8f, 0.5f,		//color of vertex 2
		0.9f, 0.8f, 0.5f,
		0.9f, 0.8f, 0.5f,		//color of vertex 2
		0.9f, 0.8f, 0.5f
	};
	glBindBuffer(GL_ARRAY_BUFFER, m_colorbuffer[GEO_TRIANGLE_4]);
	glBufferData(GL_ARRAY_BUFFER, sizeof(color_buffer_data_4), color_buffer_data_4, GL_STATIC_DRAW);


	//**************************************** SKY *******************************************************
	static const GLfloat vertex_buffer_data_5[] =
	{
	-1.0f, 0.0f, -0.5f, //vertex 0 of triangle
	1.0f, 0.0f, -0.5f, //vertex 1 of triangle
	-1.0f, -1.0f, -0.5f,  //vertex 2 of triangle	
	1.0f, 0.0f, -0.5f,	//vertex 3
	1.0f, -1.0f, -0.5f,  //vertex 2 of triangle	
	-1.0f, -1.0f, -0.5f
	};
	glBindBuffer(GL_ARRAY_BUFFER, m_vertexBuffer[GEO_TRIANGLE_5]);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertex_buffer_data_5), vertex_buffer_data_5, GL_STATIC_DRAW);

	static const GLfloat color_buffer_data_5[] = {
		0.5f, 0.0f, 0.6f,			//color of vertex 0
		0.5f, 0.0f, 0.6f,			//color of vertex 1
		0.5f, 0.0f, 0.6f,		//color of vertex 2
		0.3f, 0.0f, 0.6f,
		0.3f, 0.0f, 0.6f,		//color of vertex 2
		0.3f, 0.0f, 0.6f
	};
	glBindBuffer(GL_ARRAY_BUFFER, m_colorbuffer[GEO_TRIANGLE_5]);
	glBufferData(GL_ARRAY_BUFFER, sizeof(color_buffer_data_5), color_buffer_data_5, GL_STATIC_DRAW);


	//*********************************** HOUSE ************************************************************************
	static const GLfloat vertex_buffer_data_6[] =
	{

	-0.8f, 0.3f, 0.0f,	//roof
	0.0f, 1.0f, 0.0f,
	0.8f, 0.3f, 0.0f,

	-0.5f, 0.5f, 0.0f, //house
	0.5f, 0.5f, 0.0f, 
	0.5f, -0.5f, 0.0f, 
	-0.5f, -0.5f, 0.0f,	
	-0.5f, 0.5f, 0.0f,  
	0.5f, -0.5, 0.0f,

	0.2f, 0.9f, 0.0f, //chimney
	0.45f, 0.9f, 0.0f,
	0.45f, -0.5f, 0.0f,  	
	0.2f, -0.5f, 0.0f,	
	0.2f, 0.9f, 0.0f,  
	0.45f, -0.5, 0.0f,

	-0.4f, 0.2f, 0.3f, //window
	0.1f, 0.2f, 0.3f, 
	0.1f, -0.2f, 0.3f,  
	-0.4f, -0.2f, 0.3f,	
	-0.4f, 0.2f, 0.3f, 
	0.1f, -0.2f, 0.3f,


	};
	//Set the current active buffer
	glBindBuffer(GL_ARRAY_BUFFER, m_vertexBuffer[GEO_TRIANGLE_6]);
	//Transfer vertices to OpenGL
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertex_buffer_data_6), vertex_buffer_data_6, GL_STATIC_DRAW);
	//An array of 3 vectors which represents the colors of the 3 vertices
	static const GLfloat color_buffer_data_6[] = {
		0.9f, 0.6f, 0.4f,
		0.9f, 0.6f, 0.4f,		//color of vertex 2
		0.9f, 0.6f, 0.4f,
		0.7f, 0.6f, 0.8f,			//color of vertex 0
		0.7f, 0.6f, 0.8f,			//color of vertex 1
		0.7f, 0.6f, 0.8f,		//color of vertex 2
		0.7f, 0.6f, 0.8f,
		0.7f, 0.6f, 0.8f,		//color of vertex 2
		0.7f, 0.6f, 0.8f,

		0.8f, 0.6f, 0.5f,			//color of vertex 0
		0.8f, 0.6f, 0.5f,			//color of vertex 1
		0.8f, 0.6f, 0.5f,		//color of vertex 2
		0.8f, 0.6f, 0.5f,
		0.8f, 0.6f, 0.5f,		//color of vertex 2
		0.8f, 0.6f, 0.5f,

		0.0f, 0.5f, 1.0f,			//color of vertex 0
		0.0f, 0.5f, 1.0f,			//color of vertex 1
		0.0f, 0.5f, 1.0f,		//color of vertex 2
		0.0f, 0.5f, 1.0f,
		0.0f, 0.5f, 1.0f,		//color of vertex 2
		0.0f, 0.5f, 1.0f,

	};
	glBindBuffer(GL_ARRAY_BUFFER, m_colorbuffer[GEO_TRIANGLE_6]);
	glBufferData(GL_ARRAY_BUFFER, sizeof(color_buffer_data_6), color_buffer_data_6, GL_STATIC_DRAW);

//**************************************** STEAM **********************************************************************************************
	static const GLfloat vertex_buffer_data_7[] =
	{
	-0.5f, 0.5f, -0.4f, //square	
	0.5f, 0.5f, -0.4f,
	0.5f, -0.5f, -0.4f,
	-0.5f, -0.5f, -0.4f,
	-0.5f, 0.5f, -0.4f,
	0.5f, -0.5, -0.4f,
	};
	glBindBuffer(GL_ARRAY_BUFFER, m_vertexBuffer[GEO_TRIANGLE_7]);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertex_buffer_data_7), vertex_buffer_data_7, GL_STATIC_DRAW);

	static const GLfloat color_buffer_data_7[] = {
		1.0f, 1.0f, 1.0f,			//color of vertex 0
		1.0f, 1.0f, 1.0f,			//color of vertex 1
		1.0f, 1.0f, 1.0f,		//color of vertex 2
		1.0f, 1.0f, 1.0f,
		1.0f, 1.0f, 1.0f,		//color of vertex 2
		1.0f, 1.0f, 1.0f
	};
	glBindBuffer(GL_ARRAY_BUFFER, m_colorbuffer[GEO_TRIANGLE_7]);
	glBufferData(GL_ARRAY_BUFFER, sizeof(color_buffer_data_7), color_buffer_data_7, GL_STATIC_DRAW);

//*********************************************************************************************************************************************************
	m_programID = LoadShaders("Shader//TransformVertexShader.vertexshader", "Shader//SimpleFragmentShader.fragmentshader");
	m_parameters[U_MVP] = glGetUniformLocation(m_programID, "MVP");
	glUseProgram(m_programID);

}

void Scene3::Update(double dt)
{
	rotateAngle += (float)(-6 * dt);
	rotateAngleStars += (float)(-360 * dt);
	translateX += (float)(20 * dt);
	if (translateX > 140)
	{
		translateX -= 140;
	}
	smoke += (float)(10 * dt);
	if (smoke > 60)
	{
		smoke -= 60;
	}
	scaleAll += (float)(reverseScale * dt);
	if(scaleAll > 4)
	{
		reverseScale *= -1;
	}
	else if (scaleAll < 1)
	{
		reverseScale *= -1;
	}
}

void Scene3::Render()
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
	projection.SetToOrtho(-40, 40, -30, 30, -10, 10);


	//******************************* TREES **********************************************************8
	scale.SetToScale(7, 10, 1);
	rotate.SetToRotation(0, 0, 0, 1);
	translate.SetToTranslation(-24, -12, 0);
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
	glDrawArrays(GL_TRIANGLES, 0, 6);

	scale.SetToScale(20, 35, 1);
	rotate.SetToRotation(0, 0, 0, 1);
	translate.SetToTranslation(-10, 5, 0);
	model = translate * rotate * scale; //scale, followed by rotate, then lastly translate
	MVP = projection * view * model; // MVP = PVM, other way around
	glUniformMatrix4fv(m_parameters[U_MVP], 1, GL_FALSE, &MVP.a[0]); //update the shader with new MVP
	glBindBuffer(GL_ARRAY_BUFFER, m_vertexBuffer[GEO_TRIANGLE_3]);
	glVertexAttribPointer(0,	//attribute 0, must match layout of shader, usually 0 is for vertex
		3,			//size	
		GL_FLOAT,	//type
		GL_FALSE,	//normalized?
		0,			//stride
		0);			//array buffer offset
	glBindBuffer(GL_ARRAY_BUFFER, m_colorbuffer[GEO_TRIANGLE_3]);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, 0);
	glDrawArrays(GL_TRIANGLES, 0, 6);
	//-----------------------------------------------------------------------------------------------------------------------
	scale.SetToScale(7, 10, 1);
	rotate.SetToRotation(0, 0, 0, 1);
	translate.SetToTranslation(-4, -9, 0);
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
	glDrawArrays(GL_TRIANGLES, 0, 6);

	scale.SetToScale(20, 35, 1);
	rotate.SetToRotation(0, 0, 0, 1);
	translate.SetToTranslation(10, 4, 0);
	model = translate * rotate * scale; //scale, followed by rotate, then lastly translate
	MVP = projection * view * model; // MVP = PVM, other way around
	glUniformMatrix4fv(m_parameters[U_MVP], 1, GL_FALSE, &MVP.a[0]); //update the shader with new MVP
	glBindBuffer(GL_ARRAY_BUFFER, m_vertexBuffer[GEO_TRIANGLE_3]);
	glVertexAttribPointer(0,	//attribute 0, must match layout of shader, usually 0 is for vertex
		3,			//size	
		GL_FLOAT,	//type
		GL_FALSE,	//normalized?
		0,			//stride
		0);			//array buffer offset
	glBindBuffer(GL_ARRAY_BUFFER, m_colorbuffer[GEO_TRIANGLE_3]);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, 0);
	glDrawArrays(GL_TRIANGLES, 0, 6);

//-----------------------------------------------------------------------------------------------------

//*************************************************************************************************	
	// GROUND


	scale.SetToScale(80, 80, 1);
	rotate.SetToRotation(0, 0, 0, 1);
	translate.SetToTranslation(10, -10, 0);
	model = translate * rotate * scale; //scale, followed by rotate, then lastly translate
	MVP = projection * view * model; // MVP = PVM, other way around
	glUniformMatrix4fv(m_parameters[U_MVP], 1, GL_FALSE, &MVP.a[0]); //update the shader with new MVP
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

//********************************** STAR **********************************************************************
	scale.SetToScale(2, 2, 1);
	rotate.SetToRotation(rotateAngleStars, 0, 0, 1);
	translate.SetToTranslation(translateX - 70, 20, 0);
	model = translate * rotate * scale; //scale, followed by rotate, then lastly translate
	MVP = projection * view * model; // MVP = PVM, other way around
	glUniformMatrix4fv(m_parameters[U_MVP], 1, GL_FALSE, &MVP.a[0]); //update the shader with new MVP
	glBindBuffer(GL_ARRAY_BUFFER, m_vertexBuffer[GEO_TRIANGLE_4]);
	glVertexAttribPointer(0,	//attribute 0, must match layout of shader, usually 0 is for vertex
		3,			//size	
		GL_FLOAT,	//type
		GL_FALSE,	//normalized?
		0,			//stride
		0);			//array buffer offset
	glBindBuffer(GL_ARRAY_BUFFER, m_colorbuffer[GEO_TRIANGLE_4]);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, 0);
	glDrawArrays(GL_TRIANGLES, 0, 6);
//-----------------------------------------------------------------------------------------------------------------
	scale.SetToScale(2, 2, 1);
	rotate.SetToRotation(rotateAngleStars, 0, 0, 1);
	translate.SetToTranslation(translateX - 80, 15, 0);
	model = translate * rotate * scale; //scale, followed by rotate, then lastly translate
	MVP = projection * view * model; // MVP = PVM, other way around
	glUniformMatrix4fv(m_parameters[U_MVP], 1, GL_FALSE, &MVP.a[0]); //update the shader with new MVP
	glBindBuffer(GL_ARRAY_BUFFER, m_vertexBuffer[GEO_TRIANGLE_4]);
	glVertexAttribPointer(0,	//attribute 0, must match layout of shader, usually 0 is for vertex
		3,			//size	
		GL_FLOAT,	//type
		GL_FALSE,	//normalized?
		0,			//stride
		0);			//array buffer offset
	glBindBuffer(GL_ARRAY_BUFFER, m_colorbuffer[GEO_TRIANGLE_4]);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, 0);
	glDrawArrays(GL_TRIANGLES, 0, 6);
//----------------------------------------------------------------------------------------------------------------------
	scale.SetToScale(2, 2, 1);
	rotate.SetToRotation(rotateAngleStars, 0, 0, 1);
	translate.SetToTranslation(translateX - 60, 13, 0);
	model = translate * rotate * scale; //scale, followed by rotate, then lastly translate
	MVP = projection * view * model; // MVP = PVM, other way around
	glUniformMatrix4fv(m_parameters[U_MVP], 1, GL_FALSE, &MVP.a[0]); //update the shader with new MVP
	glBindBuffer(GL_ARRAY_BUFFER, m_vertexBuffer[GEO_TRIANGLE_4]);
	glVertexAttribPointer(0,	//attribute 0, must match layout of shader, usually 0 is for vertex
		3,			//size	
		GL_FLOAT,	//type
		GL_FALSE,	//normalized?
		0,			//stride
		0);			//array buffer offset
	glBindBuffer(GL_ARRAY_BUFFER, m_colorbuffer[GEO_TRIANGLE_4]);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, 0);
	glDrawArrays(GL_TRIANGLES, 0, 6);

//************************************* SKY **************************************************************************************************

	scale.SetToScale(80, 80, 1);
	rotate.SetToRotation(rotateAngle, 0, 0, 1);
	translate.SetToTranslation(5, -15, 0);
	model = translate * rotate * scale; //scale, followed by rotate, then lastly translate
	MVP = projection * view * model; // MVP = PVM, other way around
	glUniformMatrix4fv(m_parameters[U_MVP], 1, GL_FALSE, &MVP.a[0]); //update the shader with new MVP
	glBindBuffer(GL_ARRAY_BUFFER, m_vertexBuffer[GEO_TRIANGLE_5]);
	glVertexAttribPointer(0,	//attribute 0, must match layout of shader, usually 0 is for vertex
		3,			//size	
		GL_FLOAT,	//type
		GL_FALSE,	//normalized?
		0,			//stride
		0);			//array buffer offset
	glBindBuffer(GL_ARRAY_BUFFER, m_colorbuffer[GEO_TRIANGLE_5]);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, 0);
	glDrawArrays(GL_TRIANGLES, 0, 6);
//*********************************************** HOUSE ******************************************************************************************************
	scale.SetToScale(20, 20, 1);
	rotate.SetToRotation(0, 0, 0, 1);
	translate.SetToTranslation(25, -18, 0);
	model = translate * rotate * scale; //scale, followed by rotate, then lastly translate
	MVP = projection * view * model; // MVP = PVM, other way around
	glUniformMatrix4fv(m_parameters[U_MVP], 1, GL_FALSE, &MVP.a[0]); //update the shader with new MVP
	glBindBuffer(GL_ARRAY_BUFFER, m_vertexBuffer[GEO_TRIANGLE_6]);
	glVertexAttribPointer(0,	//attribute 0, must match layout of shader, usually 0 is for vertex
		3,			//size	
		GL_FLOAT,	//type
		GL_FALSE,	//normalized?
		0,			//stride
		0);			//array buffer offset
	glBindBuffer(GL_ARRAY_BUFFER, m_colorbuffer[GEO_TRIANGLE_6]);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, 0);
	glDrawArrays(GL_TRIANGLES, 0, 21);
//*********************************************** SMOKE ****************************************************************
	scale.SetToScale(scaleAll, scaleAll, 1);
	rotate.SetToRotation(rotateAngle, 0, 0, 1);
	translate.SetToTranslation(31, smoke, 0);
	model = translate * rotate * scale; //scale, followed by rotate, then lastly translate
	MVP = projection * view * model; // MVP = PVM, other way around
	glUniformMatrix4fv(m_parameters[U_MVP], 1, GL_FALSE, &MVP.a[0]); //update the shader with new MVP
	glBindBuffer(GL_ARRAY_BUFFER, m_vertexBuffer[GEO_TRIANGLE_7]);
	glVertexAttribPointer(0,	//attribute 0, must match layout of shader, usually 0 is for vertex
		3,			//size	
		GL_FLOAT,	//type
		GL_FALSE,	//normalized?
		0,			//stride
		0);			//array buffer offset
	glBindBuffer(GL_ARRAY_BUFFER, m_colorbuffer[GEO_TRIANGLE_7]);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, 0);
	glDrawArrays(GL_TRIANGLES, 0, 6);

	glDisableVertexAttribArray(0);
	glDisableVertexAttribArray(1);
}

void Scene3::Exit()
{
	// Cleanup VBO here
	glDeleteBuffers(NUM_GEOMETRY, &m_vertexBuffer[0]);
	glDeleteBuffers(NUM_GEOMETRY, &m_colorbuffer[0]);
	glDeleteVertexArrays(1, &m_vertexArrayID);
	glDeleteProgram(m_programID);
}
