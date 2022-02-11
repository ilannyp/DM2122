#include "Assignment1.h"
#include "GL\glew.h"
#include "Application.h"
#include "shader.hpp"
#include "Mtx44.h"
#include "MeshBuilder.h"
#include "Utility.h"





Assignment1::Assignment1()
{

}

Assignment1::~Assignment1()
{
}

void Assignment1::RenderMesh(Mesh* mesh, bool enableLight)
{
	Mtx44 MVP, modelView, modelView_inverse_transpose;

	MVP = projectionStack.Top() * viewStack.Top() * modelStack.Top();
	glUniformMatrix4fv(m_parameters[U_MVP], 1, GL_FALSE, &MVP.a[0]);
	modelView = viewStack.Top() * modelStack.Top();
	glUniformMatrix4fv(m_parameters[U_MODELVIEW], 1, GL_FALSE, &modelView.a[0]);
	if (enableLight)
	{
		glUniform1i(m_parameters[U_LIGHTENABLED], 1);
		modelView_inverse_transpose = modelView.GetInverse().GetTranspose();
		glUniformMatrix4fv(m_parameters[U_MODELVIEW_INVERSE_TRANSPOSE], 1, GL_FALSE, &modelView_inverse_transpose.a[0]);

		//load material
		glUniform3fv(m_parameters[U_MATERIAL_AMBIENT], 1, &mesh->material.kAmbient.r);
		glUniform3fv(m_parameters[U_MATERIAL_DIFFUSE], 1, &mesh->material.kDiffuse.r);
		glUniform3fv(m_parameters[U_MATERIAL_SPECULAR], 1, &mesh->material.kSpecular.r);
		glUniform1f(m_parameters[U_MATERIAL_SHININESS], mesh->material.kShininess);
	}
	else
	{
		glUniform1i(m_parameters[U_LIGHTENABLED], 0);
	}
	mesh->Render();
}

void Assignment1::Init()
{


	// Set background colour to light purple
	glClearColor(0.1f, 0.0f, 0.2f, 0.0f);
	// Enable depth test
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_CULL_FACE);
	camera.Init(Vector3(100, 80, 80), Vector3(0, 0, 0), Vector3(0, 1, 0));

	m_programID = LoadShaders("Shader//Shading.vertexshader", "Shader//LightSource.fragmentshader");
	m_parameters[U_MVP] = glGetUniformLocation(m_programID, "MVP");
	m_parameters[U_MODELVIEW] = glGetUniformLocation(m_programID, "MV");
	m_parameters[U_MODELVIEW_INVERSE_TRANSPOSE] = glGetUniformLocation(m_programID, "MV_inverse_transpose");
	m_parameters[U_MATERIAL_AMBIENT] = glGetUniformLocation(m_programID, "material.kAmbient");
	m_parameters[U_MATERIAL_DIFFUSE] = glGetUniformLocation(m_programID, "material.kDiffuse");
	m_parameters[U_MATERIAL_SPECULAR] = glGetUniformLocation(m_programID, "material.kSpecular");
	m_parameters[U_MATERIAL_SHININESS] = glGetUniformLocation(m_programID, "material.kShininess");

	//Replace previous code
	light[0].type = Light::LIGHT_SPOT;
	light[0].position.Set(25, 75, 0);
	light[0].color.Set(1, 1, 1);
	light[0].power = 10;
	light[0].kC = 1.f;
	light[0].kL = 0.01f;
	light[0].kQ = 0.001f;
	light[0].cosCutoff = cos(Math::DegreeToRadian(45));
	light[0].cosInner = cos(Math::DegreeToRadian(30));
	light[0].exponent = 3.f;
	light[0].spotDirection.Set(0.f, 1.f, 0.f);


	m_parameters[U_LIGHT0_POSITION] = glGetUniformLocation(m_programID, "lights[0].position_cameraspace");
	m_parameters[U_LIGHT0_COLOR] = glGetUniformLocation(m_programID, "lights[0].color");
	m_parameters[U_LIGHT0_POWER] = glGetUniformLocation(m_programID, "lights[0].power");
	m_parameters[U_LIGHT0_KC] = glGetUniformLocation(m_programID, "lights[0].kC");
	m_parameters[U_LIGHT0_KL] = glGetUniformLocation(m_programID, "lights[0].kL");
	m_parameters[U_LIGHT0_KQ] = glGetUniformLocation(m_programID, "lights[0].kQ");
	m_parameters[U_LIGHT0_TYPE] = glGetUniformLocation(m_programID, "lights[0].type");
	m_parameters[U_LIGHT0_SPOTDIRECTION] = glGetUniformLocation(m_programID, "lights[0].spotDirection");
	m_parameters[U_LIGHT0_COSCUTOFF] = glGetUniformLocation(m_programID, "lights[0].cosCutoff");
	m_parameters[U_LIGHT0_COSINNER] = glGetUniformLocation(m_programID, "lights[0].cosInner");
	m_parameters[U_LIGHT0_EXPONENT] = glGetUniformLocation(m_programID, "lights[0].exponent");

	m_parameters[U_LIGHTENABLED] = glGetUniformLocation(m_programID, "lightEnabled");
	m_parameters[U_NUMLIGHTS] = glGetUniformLocation(m_programID, "numLights");

	glUseProgram(m_programID);

	// Make sure you pass uniform parameters after glUseProgram()
	// Make sure you pass uniform parameters after glUseProgram()
	glUniform1i(m_parameters[U_NUMLIGHTS], 1);
	glUniform1i(m_parameters[U_LIGHT0_TYPE], light[0].type);
	glUniform3fv(m_parameters[U_LIGHT0_COLOR], 1, &light[0].color.r);
	glUniform1f(m_parameters[U_LIGHT0_POWER], light[0].power);
	glUniform1f(m_parameters[U_LIGHT0_KC], light[0].kC);
	glUniform1f(m_parameters[U_LIGHT0_KL], light[0].kL);
	glUniform1f(m_parameters[U_LIGHT0_KQ], light[0].kQ);
	glUniform1f(m_parameters[U_LIGHT0_COSCUTOFF], light[0].cosCutoff);
	glUniform1f(m_parameters[U_LIGHT0_COSINNER], light[0].cosInner);
	glUniform1f(m_parameters[U_LIGHT0_EXPONENT], light[0].exponent);

	// Generate a default VAO for now
	glGenVertexArrays(1, &m_vertexArrayID);
	glBindVertexArray(m_vertexArrayID);	
	rotateAngle = 0;
	for (int i = 0; i < NUM_GEOMETRY; ++i)
	{
		meshList[i] = nullptr;
	}

	meshList[GEO_AXES] = MeshBuilder::GenerateAxes("reference", 1000, 1000, 1000);

	meshList[GEO_SPHERE] = MeshBuilder::GenerateSphere("sphere", Color(0, 0, 0), 10, 20, 1.f);			//limbs

	meshList[GEO_SPHERE2] = MeshBuilder::GenerateSphere("sphere", Color(1, 1, 1), 10, 20, 1.f);			//SPELL

	meshList[GEO_LAMPLIGHT] = MeshBuilder::GenerateSphere("sphere", Color(0.4, 0.4, 0.4), 10, 20, 1.f);			//limbs

	meshList[GEO_CYLINDER] = MeshBuilder::GenerateCylinder("cylinder", Color(1, 1, 1), 20, 1.f, 1.f);		//base of head

	meshList[GEO_BODY] = MeshBuilder::GenerateCylinder("cylinder", Color(0, 0,0), 20, 1.f, 1.f);		//body

	meshList[GEO_HEMISPHERE] = MeshBuilder::GenerateHemisphere("hemisphere", Color(1, 1, 1), 10, 20, 1.f);  //ends of head

	meshList[GEO_HEMISPHERE2] = MeshBuilder::GenerateHemisphere("hemisphere", Color(0, 0, 0), 10, 20, 1.f);  //waist

	meshList[GEO_HEMISPHERE3] = MeshBuilder::GenerateHemisphere("hemisphere", Color(0.6, 0.6, 0.6), 10, 20, 1.f);  //waist

	meshList[GEO_QUAD] = MeshBuilder::GenerateQuad("quad", Color(0.5, 0.5, 0.5));		//scene
	
	meshList[GEO_TORUS] = MeshBuilder::GenerateTorus("torus", Color(0.3, 0.3, 0.3), 20, 20, 1.f,0.5f);			//cape head

	meshList[GEO_CAPE] = MeshBuilder::GenerateCylinder("cylinder", Color(0.3, 0.3, 0.3), 20, 1.f, 1.f);		//base of head

	meshList[GEO_HALFTORUS] = MeshBuilder::GenerateHalfTorus("quartertorus", Color(1, 1, 1), 10, 20, 1.f, 0.5f);   //horns

	meshList[GEO_CONE] = MeshBuilder::GenerateCone("cone", Color(0.3, 0.3, 0.3), 20, 1.f, 1.f);   //cape end
	
	meshList[GEO_SWORD] = MeshBuilder::GenerateCone("cone", Color(1, 1, 1), 20, 1.f, 1.f);   //cape ends

	meshList[GEO_LIGHTBALL] = MeshBuilder::GenerateSphere("sphere", Color(1, 1, 1), 10, 20, 1.f);		//light for lamppost
	//An array of 3 vectors which represents the colors of the 3 vertices

	Mtx44 projection;
	projection.SetToPerspective(45.f, 4.f / 3.f, 0.1f, 1000.f);
	projectionStack.LoadMatrix(projection);

}

void Assignment1::Update(double dt)
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
	if (Application::IsKeyPressed('I'))
		light[0].position.z -= (float)(LSPEED * dt);
	if (Application::IsKeyPressed('K'))
		light[0].position.z += (float)(LSPEED * dt);
	if (Application::IsKeyPressed('J'))
		light[0].position.x -= (float)(LSPEED * dt);
	if (Application::IsKeyPressed('L'))
		light[0].position.x += (float)(LSPEED * dt);
	if (Application::IsKeyPressed('O'))
		light[0].position.y -= (float)(LSPEED * dt);
	if (Application::IsKeyPressed('P'))
		light[0].position.y += (float)(LSPEED * dt);

	if (Application::IsKeyPressed('5'))
	{
		
		light[0].type = Light::LIGHT_POINT;
		glUniform1i(m_parameters[U_LIGHT0_TYPE], light[0].type);
		glUniform3fv(m_parameters[U_LIGHT0_COLOR], 1, &light[0].color.r);

	}
	else if (Application::IsKeyPressed('6'))
	{
		light[0].type = Light::LIGHT_DIRECTIONAL;
		glUniform1i(m_parameters[U_LIGHT0_TYPE], light[0].type);
		glUniform3fv(m_parameters[U_LIGHT0_COLOR], 1, &light[0].color.r);
		//to do: switch light type to DIRECTIONAL and pass the information to shader
	}
	else if (Application::IsKeyPressed('7'))
	{
		light[0].type = Light::LIGHT_SPOT;
		glUniform1i(m_parameters[U_LIGHT0_TYPE], light[0].type);
		glUniform3fv(m_parameters[U_LIGHT0_COLOR], 1, &light[0].color.r);
		//to do: switch light type to SPOT and pass the information to shader
	}

	if (Application::IsKeyPressed('T'))
	{
		
		rotateAngle += (float)(80 * dt * reverseRotate);
		if (rotateAngle < -10)
		{
			reverseRotate = 1;
			
		}
		if (rotateAngle > 40)
		{
			reverseRotate = -1;
		}
		swordRotate = 360;
		swordRotate2 = 90;
		swordTranslateX = 8;
		swordTranslateY = -7;
		armTranslateX = 0;
		armTranslateY = 0;
		armTranslateZ = 0;
		
	}

	if (Application::IsKeyPressed('Z'))			//go forward
	{
		
		translateZ += (float)(20 * dt * translateX);
		firstlegRotate += (float)(180 * dt * reverseRotateLeg);
		if (firstlegRotate < -15)
		{
			reverseRotateLeg = 1;
		}
		if (firstlegRotate > 15)
		{
			reverseRotateLeg = -1;
		}

		secondlegRotate += (float)(180 * dt * reverseRotateLeg2);
		if (secondlegRotate < -15)
		{
			reverseRotateLeg2 = 1;
		}
		if (secondlegRotate > 15)
		{
			reverseRotateLeg2 = -1;
		}
		rotateBody = 1;
	}

	if (Application::IsKeyPressed('X'))			//go backwards
	{
		
		translateZ += (float)(20 * dt * -translateX);
		firstlegRotate += (float)( 180 * dt * reverseRotateLeg3);
		if (firstlegRotate < -15)
		{
			reverseRotateLeg3 = 1;
		}
		if (firstlegRotate > 15)
		{
			reverseRotateLeg3 = -1;
		}
		secondlegRotate += (float)(180 * dt* reverseRotateLeg4);
		if (secondlegRotate < -15)
		{
			reverseRotateLeg4 = 1;
		}
		if (secondlegRotate > 15)
		{
			reverseRotateLeg4 = -1;
		}
		rotateBody = 180;
	}

	if (Application::IsKeyPressed('C'))			//spawn spell
	{
		spellx = 0;
		spellswordtranslate = -40;
		
		spellbodyUp += (float)(90 * dt);
		if (spellbodyUp > 15) 
		{
			spellarmTranslateZ = -5;
			spelllegTranslateZ = -3;
			spellbodyUp = 15;
			spellheadrotate = 25;
			spelly = 0;
			spellz += (float)(240 * dt);
			spellarmrotate = 40;
			spelllegTranslateY = -5;
			spellcaperotate = 90;
			spellcapetranslateY = 8;
			spellcapetranslateZ = -5;
			spellcaperotate2 += (float)(120 * dt * reversespellcaperotate2);
			if (spellcaperotate2 < -30)
			{
				reversespellcaperotate2 = 1;
			}
			if (spellcaperotate2 > 30)
			{
				reversespellcaperotate2 = -1;
			}

			spellcaperotate3 += (float)(240 * dt * reversespellcaperotate3);
			if (spellcaperotate3 < -30)
			{
				reversespellcaperotate3 = 1;
			}
			if (spellcaperotate3 > 30)
			{
				reversespellcaperotate3 = -1;
			}

			spellcaperotate4 += (float)(180 * dt * reversespellcaperotate4);
			if (spellcaperotate4 < -60)
			{
				reversespellcaperotate4 = 1;
			}
			if (spellcaperotate4 > 60)
			{
				reversespellcaperotate4 = -1;
			}
			
			spellcaperotate5 += (float)(100 * dt * reversespellcaperotate5);
			if (spellcaperotate5 < -30)
			{
				reversespellcaperotate5 = 1;
			}
			if (spellcaperotate5 > 30)
			{
				reversespellcaperotate5 = -1;
			}

			spellcaperotate6 += (float)(100 * dt * reversespellcaperotate6);
			if (spellcaperotate6 < -30)
			{
				reversespellcaperotate6 = 1;
			}
			if (spellcaperotate6 > 30)
			{
				reversespellcaperotate6 = -1;
			}
		}
	}

	if (Application::IsKeyPressed('T') == false)
	{
		rotateAngle = 0;
		swordRotate = 0;
		swordRotate2 = 0;
		swordTranslateX = 0;
		swordTranslateY = 0;
		armTranslateX = 0;
		armTranslateY = 0;
		armTranslateZ = 0;
	}

	if (Application::IsKeyPressed('C') == false)			//despawn spell
	{
		spellx = 0;
		spelly = -80;
		spellz = 0;
		spellbodyUp = 0;
		spellheadrotate = 0;
		spellswordtranslate = 0;
		spellarmrotate = 0;
		spellarmTranslateZ = 0;
		spelllegTranslateY = 0;
		spelllegTranslateZ = 0;
		spellcapetranslateY = 0;
		spellcapetranslateZ = 0;
		spellcaperotate = 0;
		spellcaperotate2 = 0;
		spellcaperotate3 = 0;
		spellcaperotate4 = 0;
		spellcaperotate5 = 0;
		spellcaperotate6 = 0;
	}

	if (Application::IsKeyPressed('Z') == false && Application::IsKeyPressed('X') == false)			//go forward
	{

		firstlegRotate = 0;
		secondlegRotate = 0;

	}

	

	camera.Update(dt);

}

void Assignment1::Render()
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

	if (light[0].type == Light::LIGHT_DIRECTIONAL)
	{
		Vector3 lightDir(light[0].position.x, light[0].position.y, light[0].position.z);
		Vector3 lightDirection_cameraspace = viewStack.Top() * lightDir;
		glUniform3fv(m_parameters[U_LIGHT0_POSITION], 1, &lightDirection_cameraspace.x);
	}
	else if (light[0].type == Light::LIGHT_SPOT)
	{
		Position lightPosition_cameraspace = viewStack.Top() * light[0].position;
		glUniform3fv(m_parameters[U_LIGHT0_POSITION], 1, &lightPosition_cameraspace.x);
		Vector3 spotDirection_cameraspace = viewStack.Top() * light[0].spotDirection;
		glUniform3fv(m_parameters[U_LIGHT0_SPOTDIRECTION], 1, &spotDirection_cameraspace.x);
	}
	else
	{
		Position lightPosition_cameraspace = viewStack.Top() * light[0].position;
		glUniform3fv(m_parameters[U_LIGHT0_POSITION], 1, &lightPosition_cameraspace.x);
	}
	//**************************************************************************************************************

	//body
	modelStack.PushMatrix();
	{
		modelStack.Translate(0, 0, translateZ);
		modelStack.Translate(0, spellbodyUp, 0);
		modelStack.Rotate(rotateBody, 0, 1,0);

		//head
		modelStack.PushMatrix();
		{
			modelStack.Translate(0,15, 0);
			modelStack.Rotate(spellheadrotate, -1, 0, 0);

			//base of down head 
			modelStack.PushMatrix();
			{
				modelStack.Translate(0, -5, 0);
				modelStack.Rotate(0, 0, 0, 1);
				modelStack.Scale(8, 3, 6);
				RenderMesh(meshList[GEO_HEMISPHERE], true);
			}
			modelStack.PopMatrix();

			//base of up head 
			modelStack.PushMatrix();
			{
				modelStack.Translate(0, 5, 0);
				modelStack.Rotate(180, 0, 0, 1);
				modelStack.Scale(8, 3, 6);
				RenderMesh(meshList[GEO_HEMISPHERE], true);
			}
			modelStack.PopMatrix();



			//eyes
			modelStack.PushMatrix();
			{
				modelStack.Translate(3, 0, 4);
				modelStack.Rotate(0, 0, 0, 1);
				modelStack.Scale(3, 4, 3);
				RenderMesh(meshList[GEO_SPHERE], true);
			}
			modelStack.PopMatrix();
			//end eyes 

			//eyes
			modelStack.PushMatrix();
			{
				modelStack.Translate(-3, 0, 4);
				modelStack.Rotate(0, 0, 0, 1);
				modelStack.Scale(3, 4, 3);
				RenderMesh(meshList[GEO_SPHERE], true);
			}
			modelStack.PopMatrix();
			//end eyes 

			//horns
			modelStack.PushMatrix();
			{
				modelStack.Translate(3, 9, 0);
				modelStack.Rotate(90, 1, 0, 0);
				modelStack.Rotate(-45, 0, 1, 0);

				modelStack.PushMatrix();
				{
					modelStack.Translate(0, 0, -4);
					modelStack.Rotate(270, 0.1, 0.1, 10);
					modelStack.Scale(2, 1, 2);
					RenderMesh(meshList[GEO_HEMISPHERE], true);
				}
				modelStack.PopMatrix();

				modelStack.Scale(4,4,4);
				RenderMesh(meshList[GEO_HALFTORUS], true);
			}
			modelStack.PopMatrix();

			//horn 2
			modelStack.PushMatrix();
			{
				modelStack.Translate(-3, 9, 0);
				modelStack.Rotate(90, 1, 0, 0);
				modelStack.Rotate(180, 0, 0, 1);
				modelStack.Rotate(-45, 0, 1, 0);

				modelStack.PushMatrix();
				{
					modelStack.Translate(0, 0, -4);
					modelStack.Rotate(270, 0.1, 0.1, 10);
					modelStack.Scale(2, 1, 2);
					RenderMesh(meshList[GEO_HEMISPHERE], true);
				}
				modelStack.PopMatrix();

				modelStack.Scale(4, 4, 4);
				RenderMesh(meshList[GEO_HALFTORUS], true);
			}
			modelStack.PopMatrix();

			//end horns

			modelStack.Scale(8, 10, 6);
			RenderMesh(meshList[GEO_CYLINDER], true);
			//end head
		}
		modelStack.PopMatrix();

		//left arm
		modelStack.PushMatrix();
		{

			modelStack.Translate(4, 2, 0);
			modelStack.Translate(0, 0, spellarmTranslateZ);
			modelStack.Rotate(20, 0, 0, 1);
			modelStack.Rotate(spellarmrotate, 1, 0, 0);
			modelStack.Scale(2, 8, 2);
			RenderMesh(meshList[GEO_SPHERE], true);
		}
		modelStack.PopMatrix();
		//left arm end
	

		//right arm
		modelStack.PushMatrix();
		{
			modelStack.Translate(armTranslateX,armTranslateY, armTranslateZ);
			modelStack.Translate(0, 0, spellarmTranslateZ);
			modelStack.Rotate(rotateAngle, 0, 1, 0);
			modelStack.Rotate(swordRotate, 1, 0, 0);
			modelStack.Rotate(spellarmrotate, 1, 0, 0);
			
			//sword hilt
			modelStack.PushMatrix();
			{
				modelStack.Translate(-14, -4, 13.5);
				modelStack.Translate(swordTranslateX,swordTranslateY,0);
				modelStack.Translate(0, spellswordtranslate, 0);
				modelStack.Rotate(90, 1, 0, 0);
				modelStack.Rotate(swordRotate, 0, 1, 0);
				modelStack.Rotate(swordRotate2, 0, 1, 0);
			
				//blade
				modelStack.PushMatrix();
				{
					modelStack.Translate(8, 2, 0);
					modelStack.Rotate(90, 0, 1, 0);
					modelStack.Scale(3, 20, 2);
					RenderMesh(meshList[GEO_SWORD], true);
				}
				modelStack.PopMatrix();

				modelStack.Translate(8, -8, 0);
				modelStack.Rotate(0, 0, 0, 1);
				modelStack.Scale(0.5, 8, 0.5);
				RenderMesh(meshList[GEO_CYLINDER], true);
			}
			modelStack.PopMatrix();

			modelStack.Translate(-4, 2, 0);
			modelStack.Rotate(20, 0, 0, -1);
			modelStack.Scale(2, 8, 2);
			RenderMesh(meshList[GEO_SPHERE], true);
			
		}
		modelStack.PopMatrix();
		//right arm end
		

		//right leg
		modelStack.PushMatrix();
		{
			modelStack.Translate(2, -6, 0);
			modelStack.Translate(0, 0, spelllegTranslateZ);
			modelStack.Translate(0, spelllegTranslateY, 0);
			modelStack.Rotate(firstlegRotate, 1, 0, 0);
			modelStack.Rotate(spellarmrotate, 1, 0, 0);
			modelStack.Scale(2, 8, 2);
			RenderMesh(meshList[GEO_SPHERE], true);
		}
		modelStack.PopMatrix();
		//right leg done
	

		//left leg
		modelStack.PushMatrix();
		{
			modelStack.Translate(-2, -6, 0);
			modelStack.Translate(0, 0, spelllegTranslateZ);
			modelStack.Translate(0, spelllegTranslateY, 0);

			modelStack.Rotate(secondlegRotate, 1, 0, 0);
			modelStack.Rotate(spellarmrotate, 1, 0, 0);
			modelStack.Scale(2, 8, 2);
			RenderMesh(meshList[GEO_SPHERE], true);
			
		}
		modelStack.PopMatrix();
		//left leg done
	

		//waist
		modelStack.PushMatrix();
		{
			modelStack.Translate(0, -7.4, 0);
			modelStack.Rotate(0, 0, 0, 1);
			modelStack.Scale(4, 3, 4);
			RenderMesh(meshList[GEO_HEMISPHERE2], true);
		}
		modelStack.PopMatrix();
		//end waist

		//cape
		modelStack.PushMatrix();
		{
			modelStack.Translate(0, 6.5, 0);

			//back 1
			modelStack.PushMatrix();
			{
				modelStack.Translate(0, -6.4, -4.6);
				modelStack.Translate(0, spellcapetranslateY, spellcapetranslateZ);
				modelStack.Rotate(12, 1, 0, 0);
				modelStack.Rotate(spellcaperotate, 1, 0, 0);

				modelStack.PushMatrix();
				{
					modelStack.Translate(0, -6, -0.5);
					modelStack.Rotate(25, 1, 0, 0);
					modelStack.Rotate(spellcaperotate2, 1, 0, 0);

					modelStack.PushMatrix();
					{
						modelStack.Translate(0, -2.8, -0.3);
						modelStack.Rotate(5, 1, 0, 0);
						modelStack.Rotate(180, 1, 0, 0);
						modelStack.Rotate(spellcaperotate3, 1, 0, 0);
						modelStack.Scale(0.8, 4, 0.8);
						RenderMesh(meshList[GEO_CONE], true);
					}
					modelStack.PopMatrix();

					modelStack.Scale(1, 2, 1);
					RenderMesh(meshList[GEO_CAPE], true);
				}
				modelStack.PopMatrix();

				modelStack.Scale(1, 12, 1);
				RenderMesh(meshList[GEO_CAPE], true);
			}
			modelStack.PopMatrix();
			//back 2
			modelStack.PushMatrix();
			{
				modelStack.Translate(-1.5, -6.4, -4.2);
				modelStack.Translate(0, spellcapetranslateY, spellcapetranslateZ);
				modelStack.Rotate(spellcaperotate, 1, 0, 0);
				modelStack.Rotate(12, 1, 0, 0);
				

				modelStack.PushMatrix();
				{
					modelStack.Translate(0, -6, -0.5);
					modelStack.Rotate(spellcaperotate2, 1, 0, 0);
					modelStack.Rotate(25, 1, 0, 0);

					modelStack.PushMatrix();
					{
						modelStack.Translate(0, -2.8, -0.3);
						modelStack.Rotate(5, 1, 0, 0);
						modelStack.Rotate(180, 1, 0, 0);
						modelStack.Rotate(spellcaperotate3, 1, 0, 0);
						modelStack.Scale(0.8, 4, 0.8);
						RenderMesh(meshList[GEO_CONE], true);
					}
					modelStack.PopMatrix();

					modelStack.Scale(1, 2, 1);
					RenderMesh(meshList[GEO_CAPE], true);
				}
				modelStack.PopMatrix();

				modelStack.Scale(1, 12, 1);
				RenderMesh(meshList[GEO_CAPE], true);
			}
			modelStack.PopMatrix();

			//back 3
			modelStack.PushMatrix();
			{
				modelStack.Translate(1.7, -6.4, -4.3);
				modelStack.Translate(0, spellcapetranslateY, spellcapetranslateZ);
				modelStack.Rotate(spellcaperotate, 1, 0, 0);
				modelStack.Rotate(14, 1, 0, 0);

				modelStack.PushMatrix();
				{
					modelStack.Translate(0, -6, -0.5);
					modelStack.Rotate(25, 1, 0, 0);
					modelStack.Rotate(spellcaperotate2, 1, 0, 0);

					modelStack.PushMatrix();
					{
						modelStack.Translate(0, -2.8, -0.3);
						modelStack.Rotate(5, 1, 0, 0);
						modelStack.Rotate(180, 1, 0, 0);
						modelStack.Rotate(spellcaperotate3, 1, 0, 0);
						modelStack.Scale(0.8, 4, 0.8);
						RenderMesh(meshList[GEO_CONE], true);
					}
					modelStack.PopMatrix();

					modelStack.Scale(1, 2, 1);
					RenderMesh(meshList[GEO_CAPE], true);
				}
				modelStack.PopMatrix();

				modelStack.Scale(1, 12, 1);
				RenderMesh(meshList[GEO_CAPE], true);
			}
			modelStack.PopMatrix();
			

			//back 4
			modelStack.PushMatrix();
			{
				modelStack.Translate(3.2, -6.4, -3.6);
				modelStack.Translate(0, spellcapetranslateY, spellcapetranslateZ);
				modelStack.Rotate(spellcaperotate, 1, 0, 0);
				modelStack.Rotate(17, 1, 0, 0);
				modelStack.Rotate(3, 0, 0, 1);
				modelStack.Rotate(-5, 0, 1, 0);


				modelStack.PushMatrix();
				{
					modelStack.Translate(0, -6, -0.5);
					modelStack.Rotate(25, 1, 0, 0);
					modelStack.Rotate(spellcaperotate4, 1, 0, 0);

					modelStack.PushMatrix();
					{
						modelStack.Translate(0, -2.8, -0.3);
						modelStack.Rotate(5, 1, 0, 0);
						modelStack.Rotate(180, 1, 0, 0);
						modelStack.Rotate(spellcaperotate5, 1, 0, 0);
						modelStack.Scale(0.8, 4, 0.8);
						RenderMesh(meshList[GEO_CONE], true);
					}
					modelStack.PopMatrix();

					modelStack.Scale(1, 2, 1);
					RenderMesh(meshList[GEO_CAPE], true);
				}
				modelStack.PopMatrix();

				modelStack.Scale(1, 12, 1);
				RenderMesh(meshList[GEO_CAPE], true);
			}
			modelStack.PopMatrix();

			//back 5
			modelStack.PushMatrix();
			{
				modelStack.Translate(-3.2, -6.4, -3.6);
				modelStack.Translate(0, spellcapetranslateY, spellcapetranslateZ);
				modelStack.Rotate(spellcaperotate, 1, 0, 0);
				modelStack.Rotate(17, 1, 0, 0);
				modelStack.Rotate(-3, 0, 0, 1);
				modelStack.Rotate(-5, 0, 1, 0);

				modelStack.PushMatrix();
				{
					modelStack.Translate(0, -6, -0.5);
					modelStack.Rotate(25, 1, 0, 0);
					modelStack.Rotate(spellcaperotate4, 1, 0, 0);

					modelStack.PushMatrix();
					{
						modelStack.Translate(0, -2.8, -0.3);
						modelStack.Rotate(5, 1, 0, 0);
						modelStack.Rotate(180, 1, 0, 0);
						modelStack.Rotate(spellcaperotate5, 1, 0, 0);
						modelStack.Scale(0.8, 4, 0.8);
						RenderMesh(meshList[GEO_CONE], true);
					}
					modelStack.PopMatrix();

					modelStack.Scale(1, 2, 1);
					RenderMesh(meshList[GEO_CAPE], true);
				}
				modelStack.PopMatrix();

				modelStack.Scale(1, 12, 1);
				RenderMesh(meshList[GEO_CAPE], true);
			}
			modelStack.PopMatrix();

			//back 6
			modelStack.PushMatrix();
			{
				modelStack.Translate(-4.2, -6.4, -3);
				modelStack.Translate(0, spellcapetranslateY, spellcapetranslateZ);
				modelStack.Rotate(spellcaperotate, 1, 0, 0);
				modelStack.Rotate(17, 1, 0, 0);
				modelStack.Rotate(-14, 0, 0, 1);
				modelStack.Rotate(-5, 0, 1, 0);

				modelStack.PushMatrix();
				{
					modelStack.Translate(0, -6, -0.5);
					modelStack.Rotate(25, 1, 0, 0);
					modelStack.Rotate(spellcaperotate2, 1, 0, 0);

					modelStack.PushMatrix();
					{
						modelStack.Translate(0, -2.8, -0.3);
						modelStack.Rotate(5, 1, 0, 0);
						modelStack.Rotate(180, 1, 0, 0);
						modelStack.Rotate(spellcaperotate5, 1, 0, 0);
						modelStack.Scale(0.8, 4, 0.8);
						RenderMesh(meshList[GEO_CONE], true);
					}
					modelStack.PopMatrix();

					modelStack.Scale(1, 2, 1);
					RenderMesh(meshList[GEO_CAPE], true);
				}
				modelStack.PopMatrix();

				modelStack.Scale(1, 12, 1);
				RenderMesh(meshList[GEO_CAPE], true);
			}
			modelStack.PopMatrix();

			//side 1
			modelStack.PushMatrix();
			{
				modelStack.Translate(-5.2, -6.4, -2.5);
				modelStack.Translate(0, spellcapetranslateY, spellcapetranslateZ);
				modelStack.Rotate(spellcaperotate, 1, 0, 0);
				modelStack.Rotate(17, 1, 0, 0);
				modelStack.Rotate(-19, 0, 0, 1);
				modelStack.Rotate(-15, 0, 1, 0);

				modelStack.PushMatrix();
				{
					modelStack.Translate(0, -6, -0.5);
					modelStack.Rotate(25, 1, 0, 0);
					modelStack.Rotate(spellcaperotate2, 1, 0, 0);

					modelStack.PushMatrix();
					{
						modelStack.Translate(0, -2.8, -0.3);
						modelStack.Rotate(5, 1, 0, 0);
						modelStack.Rotate(180, 1, 0, 0);
						modelStack.Rotate(spellcaperotate4, 1, 0, 0);
						modelStack.Scale(0.8, 4, 0.8);
						RenderMesh(meshList[GEO_CONE], true);
					}
					modelStack.PopMatrix();

					modelStack.Scale(1, 2, 1);
					RenderMesh(meshList[GEO_CAPE], true);
				}
				modelStack.PopMatrix();

				modelStack.Scale(1, 12, 1);
				RenderMesh(meshList[GEO_CAPE], true);
			}
			modelStack.PopMatrix();

			//back 7
			modelStack.PushMatrix();
			{
				modelStack.Translate(4.2, -6.4, -3);
				modelStack.Translate(0, spellcapetranslateY, spellcapetranslateZ);
				modelStack.Rotate(spellcaperotate, 1, 0, 0);
				modelStack.Rotate(17, 1, 0, 0);
				modelStack.Rotate(14, 0, 0, 1);
				modelStack.Rotate(5, 0, 1, 0);

				modelStack.PushMatrix();
				{
					modelStack.Translate(0, -6, -0.5);
					modelStack.Rotate(25, 1, 0, 0);
					modelStack.Rotate(spellcaperotate4, 1, 0, 0);

					modelStack.PushMatrix();
					{
						modelStack.Translate(0, -2.8, -0.3);
						modelStack.Rotate(5, 1, 0, 0);
						modelStack.Rotate(180, 1, 0, 0);
						modelStack.Rotate(spellcaperotate5, 1, 0, 0);
						modelStack.Scale(0.8, 4, 0.8);
						RenderMesh(meshList[GEO_CONE], true);
					}
					modelStack.PopMatrix();

					modelStack.Scale(1, 2, 1);
					RenderMesh(meshList[GEO_CAPE], true);
				}
				modelStack.PopMatrix();

				modelStack.Scale(1, 12, 1);
				RenderMesh(meshList[GEO_CAPE], true);
			}
			modelStack.PopMatrix();

			//side 2
			modelStack.PushMatrix();
			{
				modelStack.Translate(5.2, -6.4, -2.5);
				modelStack.Translate(0, spellcapetranslateY, spellcapetranslateZ);
				modelStack.Rotate(spellcaperotate, 1, 0, 0);
				modelStack.Rotate(17, 1, 0, 0);
				modelStack.Rotate(19, 0, 0, 1);
				modelStack.Rotate(15, 0, 1, 0);

				modelStack.PushMatrix();
				{
					modelStack.Translate(0, -6, -0.5);
					modelStack.Rotate(25, 1, 0, 0);
					modelStack.Rotate(spellcaperotate2, 1, 0, 0);

					modelStack.PushMatrix();
					{
						modelStack.Translate(0, -2.8, -0.3);
						modelStack.Rotate(5, 1, 0, 0);
						modelStack.Rotate(180, 1, 0, 0);
						modelStack.Rotate(spellcaperotate3, 1, 0, 0);
						modelStack.Scale(0.8, 4, 0.8);
						RenderMesh(meshList[GEO_CONE], true);
					}
					modelStack.PopMatrix();

					modelStack.Scale(1, 2, 1);
					RenderMesh(meshList[GEO_CAPE], true);
				}
				modelStack.PopMatrix();

				modelStack.Scale(1, 12, 1);
				RenderMesh(meshList[GEO_CAPE], true);
			}
			modelStack.PopMatrix();
		
			modelStack.Rotate(0, 0, 0, 1);
			modelStack.Scale(3, 3, 3);
			RenderMesh(meshList[GEO_TORUS], true);
			
		}
		modelStack.PopMatrix();
		//end cape

		//SPELL: Vengeful Spirit
		modelStack.PushMatrix();
		{
			//spell
			modelStack.Translate(spellx, spelly, spellz);
			modelStack.Rotate(0, 0, 0, 1);
			
			//first tentacle
			modelStack.PushMatrix();
			{
				modelStack.Translate(0, 0, -20);
				modelStack.Rotate(90, 1, 0, 0);
				modelStack.Scale(2, 40, 2);
				RenderMesh(meshList[GEO_HEMISPHERE], true);
			}
			modelStack.PopMatrix();

			//second tentacle
			modelStack.PushMatrix();
			{
				modelStack.Translate(5, 3, -15);
				modelStack.Rotate(90, 1, 0, 0);
				modelStack.Scale(2, 40, 2);
				RenderMesh(meshList[GEO_HEMISPHERE], true);
			}
			modelStack.PopMatrix();

			//third tentacle
			modelStack.PushMatrix();
			{
				modelStack.Translate(10, -6, -5);
				modelStack.Rotate(90, 1, 0, 0);
				modelStack.Scale(2, 40, 2);
				RenderMesh(meshList[GEO_HEMISPHERE], true);
			}
			modelStack.PopMatrix();

			//fourth tentacle
			modelStack.PushMatrix();
			{
				modelStack.Translate(10, 10, 5);
				modelStack.Rotate(90, 1, 0, 0);
				modelStack.Scale(2, 40, 2);
				RenderMesh(meshList[GEO_HEMISPHERE], true);
			}
			modelStack.PopMatrix();


			//fifth tentacle
			modelStack.PushMatrix();
			{
				modelStack.Translate(-2, 10, 3);
				modelStack.Rotate(90, 1, 0, 0);
				modelStack.Scale(2, 40, 2);
				RenderMesh(meshList[GEO_HEMISPHERE], true);
			}
			modelStack.PopMatrix();

			//sixth tentacle
			modelStack.PushMatrix();
			{
				modelStack.Translate(-14, 7, -7);
				modelStack.Rotate(90, 1, 0, 0);
				modelStack.Scale(2, 40, 2);
				RenderMesh(meshList[GEO_HEMISPHERE], true);
			}
			modelStack.PopMatrix();


			//seventh tentacle
			modelStack.PushMatrix();
			{
				modelStack.Translate(-14, -7, -7);
				modelStack.Rotate(90, 1, 0, 0);
				modelStack.Scale(2, 40, 2);
				RenderMesh(meshList[GEO_HEMISPHERE], true);
			}
			modelStack.PopMatrix();

			//eigth tentacle
			modelStack.PushMatrix();
			{
				modelStack.Translate(14, 2, -5);
				modelStack.Rotate(90, 1, 0, 0);
				modelStack.Scale(2, 40, 2);
				RenderMesh(meshList[GEO_HEMISPHERE], true);
			}
			modelStack.PopMatrix();

			//ninth tentacle
			modelStack.PushMatrix();
			{
				modelStack.Translate(-5, -4, -10);
				modelStack.Rotate(90, 1, 0, 0);
				modelStack.Scale(2, 40, 2);
				RenderMesh(meshList[GEO_HEMISPHERE], true);
			}
			modelStack.PopMatrix();

			//ninth tentacle
			modelStack.PushMatrix();
			{
				modelStack.Translate(-5, 6, -10);
				modelStack.Rotate(90, 1, 0, 0);
				modelStack.Scale(2, 40, 2);
				RenderMesh(meshList[GEO_HEMISPHERE], true);
			}
			modelStack.PopMatrix();

			//eyes
			modelStack.PushMatrix();
			{
				modelStack.Translate(-6, 3, 19);
				modelStack.Rotate(55, 0, 0, 1);
				modelStack.Rotate(-45, 1, 0, 0);
				modelStack.Scale(5, 7, 5);
				RenderMesh(meshList[GEO_SPHERE], true);
			}
			modelStack.PopMatrix();

			modelStack.PushMatrix();
			{
				modelStack.Translate(6, 3, 19);
				modelStack.Rotate(-55, 1, 0, 0);
				modelStack.Rotate(-45, 0, 0, 1);
				modelStack.Scale(5, 7, 5);
				RenderMesh(meshList[GEO_SPHERE], true);
			}
			modelStack.PopMatrix();

			modelStack.Scale(20, 15, 25);
			RenderMesh(meshList[GEO_SPHERE2], true);
		}
		modelStack.PopMatrix();

		modelStack.Rotate(0, 0, 0, 1);
		modelStack.Scale(4, 15, 4);
		RenderMesh(meshList[GEO_BODY], true);
		modelStack.PopMatrix();
		//end body
	}
	
//*****************************************************************************************************************************88
	//Environmentz

	//Lamppost
	modelStack.PushMatrix();
	{
		modelStack.Translate(25, -3, 0);
		
		//lamp base
		modelStack.PushMatrix();
		{
			modelStack.PushMatrix();
			{
				//light
				modelStack.Translate(light[0].position.x-25, light[0].position.y, light[0].position.z);
				modelStack.Scale(10, 10, 10);
				RenderMesh(meshList[GEO_SPHERE2], false);
			}
			modelStack.PopMatrix();
		
			modelStack.Translate(0, -5, 0);
			modelStack.Rotate(0, 0, 0, 1);
			modelStack.Rotate(0, 0, 0, 1);
			modelStack.Scale(12, 12, 12);
			RenderMesh(meshList[GEO_CONE], true);
		}
		modelStack.PopMatrix();

		modelStack.Translate(0, 30, 0);
		modelStack.Rotate(0, 0, 0, 1);
		modelStack.Scale(4, 80, 4);
		RenderMesh(meshList[GEO_CAPE], true);
	}
	modelStack.PopMatrix();

	//Lamppost 2
	modelStack.PushMatrix();
	{
		modelStack.Translate(-50, -3, 100);

		//lamp base
		modelStack.PushMatrix();
		{
			modelStack.PushMatrix();
			{
				//light
				modelStack.Translate(0,75,0);
				modelStack.Scale(10, 10, 10);
				RenderMesh(meshList[GEO_LAMPLIGHT], false);
			}
			modelStack.PopMatrix();

			modelStack.Translate(0, -5, 0);
			modelStack.Rotate(0, 0, 0, 1);
			modelStack.Rotate(0, 0, 0, 1);
			modelStack.Scale(12, 12, 12);
			RenderMesh(meshList[GEO_CONE], true);
		}
		modelStack.PopMatrix();

		modelStack.Translate(0, 30, 0);
		modelStack.Rotate(0, 0, 0, 1);
		modelStack.Scale(4, 80, 4);
		RenderMesh(meshList[GEO_CAPE], true);
	}
	modelStack.PopMatrix();

	//Lamppost 3
	modelStack.PushMatrix();
	{
		modelStack.Translate(-50, -3, -100);

		//lamp base
		modelStack.PushMatrix();
		{
			modelStack.PushMatrix();
			{
				//light
				modelStack.Translate(0, 75, 0);
				modelStack.Scale(10, 10, 10);
				RenderMesh(meshList[GEO_LAMPLIGHT], false);
			}
			modelStack.PopMatrix();

			modelStack.Translate(0, -5, 0);
			modelStack.Rotate(0, 0, 0, 1);
			modelStack.Rotate(0, 0, 0, 1);
			modelStack.Scale(12, 12, 12);
			RenderMesh(meshList[GEO_CONE], true);
		}
		modelStack.PopMatrix();

		modelStack.Translate(0, 30, 0);
		modelStack.Rotate(0, 0, 0, 1);
		modelStack.Scale(4, 80, 4);
		RenderMesh(meshList[GEO_CAPE], true);
	}
	modelStack.PopMatrix();


	modelStack.PushMatrix();
	{
		modelStack.Translate(130, -20, 0);
		modelStack.Rotate(180, 0, 0, 1);
		modelStack.Scale(50,50,50);
		RenderMesh(meshList[GEO_HEMISPHERE3], true);

	}
	modelStack.PopMatrix();

	modelStack.PushMatrix();
	{
		modelStack.Translate(-130, -20, 0);
		modelStack.Rotate(180, 0, 0, 1);
		modelStack.Scale(50, 70, 50);
		RenderMesh(meshList[GEO_HEMISPHERE3], true);

	}
	modelStack.PopMatrix();

	modelStack.PushMatrix();
	{
		modelStack.Translate(-130, -20, 75);
		modelStack.Rotate(180, 0, 0, 1);
		modelStack.Scale(50, 70, 50);
		RenderMesh(meshList[GEO_HEMISPHERE3], true);
	}
	modelStack.PopMatrix();


	modelStack.PushMatrix();
	{
		modelStack.Translate(-130, -20, -75);
		modelStack.Rotate(180, 0, 0, 1);
		modelStack.Scale(50, 35, 50);
		RenderMesh(meshList[GEO_HEMISPHERE3], true);
	}
	modelStack.PopMatrix();

	modelStack.PushMatrix();
	{
		modelStack.Translate(150, -20, -85);
		modelStack.Rotate(180, 0, 0, 1);
		modelStack.Scale(50, 25, 50);
		RenderMesh(meshList[GEO_HEMISPHERE3], true);
	}
	modelStack.PopMatrix();

	//***********************************************************************************************************************************************
	RenderMesh(meshList[GEO_AXES], false);
	


	modelStack.PushMatrix();
	//to do: transformation code here
	modelStack.Translate(0, -14, 0);
	modelStack.Rotate(270, 1, 0, 0);
	modelStack.Scale(500, 500, 500);
	RenderMesh(meshList[GEO_QUAD], true);
	modelStack.PopMatrix();
}

void Assignment1::Exit()
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
