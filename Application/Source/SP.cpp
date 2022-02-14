#include "SP.h"
#include "GL\glew.h"
#include "Application.h"
#include "shader.hpp"
#include "Mtx44.h"
#include "MeshBuilder.h"
#include "Utility.h"
#include "LoadTGA.h"


SP::SP()
{

}

SP::~SP()
{
}

void SP::RenderMesh(Mesh* mesh, bool enableLight)
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
	if (mesh->textureID > 0)
	{
		glUniform1i(m_parameters[U_COLOR_TEXTURE_ENABLED], 1);
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, mesh->textureID);
		glUniform1i(m_parameters[U_COLOR_TEXTURE],0);
	}
	else
	{
		glUniform1i(m_parameters[U_COLOR_TEXTURE_ENABLED], 0);
	}
	mesh->Render();
	if (mesh->textureID > 0)
	{
		glBindTexture(GL_TEXTURE_2D, 0);
	}
}
void SP::RenderSkybox()
{
	const float OFFSET = 499;

	modelStack.PushMatrix();
	//to do: transformation code here
	modelStack.Translate(0, 0, -OFFSET);
	modelStack.Rotate(180, 0, 0, 1);
	modelStack.Scale(1000, 1000, 1000);
	RenderMesh(meshList[GEO_FRONT], false);
	modelStack.PopMatrix();

	modelStack.PushMatrix();
	//to do: transformation code here
	modelStack.Translate(-OFFSET, 0, 0);
	modelStack.Rotate(90, 0, 1, 0);
	modelStack.Rotate(180, 0, 0, 1);
	modelStack.Scale(1000, 1000, 1000);
	RenderMesh(meshList[GEO_LEFT], false);
	modelStack.PopMatrix();

	modelStack.PushMatrix();
	//to do: transformation code here
	modelStack.Translate(OFFSET, 0, 0.25);
	modelStack.Rotate(270, 0, 1, 0);
	modelStack.Rotate(180, 0, 0, 1);
	modelStack.Scale(1000, 1000, 1000);
	RenderMesh(meshList[GEO_RIGHT], false);
	modelStack.PopMatrix();

	modelStack.PushMatrix();
	//to do: transformation code here
	modelStack.Translate(0, 0, OFFSET);
	modelStack.Rotate(180, 0, 1, 0);
	modelStack.Rotate(180, 0, 0, 1);
	modelStack.Scale(1000, 1000, 1000);
	RenderMesh(meshList[GEO_BACK], false);
	modelStack.PopMatrix();

	modelStack.PushMatrix();
	//to do: transformation code here
	modelStack.Translate(0, OFFSET, 0);
	modelStack.Rotate(90, 1, 0, 0);
	modelStack.Rotate(270, 0, 0, 1);
	modelStack.Scale(1000, 1000, 1000);
	RenderMesh(meshList[GEO_TOP], false);
	modelStack.PopMatrix();

	modelStack.PushMatrix();
	//to do: transformation code here
	modelStack.Translate(0, -OFFSET, 0);
	modelStack.Rotate(-90, 1, 0, 0);
	modelStack.Rotate(-90, 0, 0, 1);
	modelStack.Scale(1000, 1000, 1000);
	RenderMesh(meshList[GEO_BOTTOM], false);
	modelStack.PopMatrix();
}

void SP::RenderText(Mesh* mesh, std::string text, Color color)
{

	if (!mesh || mesh->textureID <= 0) //Proper error check
		return;
	//glDisable(GL_DEPTH_TEST); //uncomment for RenderTextOnScreen
	glUniform1i(m_parameters[U_TEXT_ENABLED], 1);
	glUniform3fv(m_parameters[U_TEXT_COLOR], 1, &color.r);
	glUniform1i(m_parameters[U_LIGHTENABLED], 0);
	glUniform1i(m_parameters[U_COLOR_TEXTURE_ENABLED], 1);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, mesh->textureID);
	glUniform1i(m_parameters[U_COLOR_TEXTURE], 0);
	for (unsigned i = 0; i < text.length(); ++i)
	{
		Mtx44 characterSpacing;
		characterSpacing.SetToTranslation(i * 1.0f, 0, 0); //1.0f is the spacing of each character, you may change this value
		Mtx44 MVP = projectionStack.Top() * viewStack.Top() * modelStack.Top() * characterSpacing;
		glUniformMatrix4fv(m_parameters[U_MVP], 1, GL_FALSE, &MVP.a[0]);
		mesh->Render((unsigned)text[i] * 6, 6);
	}
	glBindTexture(GL_TEXTURE_2D, 0);
	glUniform1i(m_parameters[U_TEXT_ENABLED], 0);
	//glEnable(GL_DEPTH_TEST); //uncomment for RenderTextOnScreen
}



void SP::RenderTextOnScreen(Mesh* mesh, std::string text, Color color, float size, float x, float y)
{
	if (!mesh || mesh->textureID <= 0) //Proper error check
		return;
	glDisable(GL_DEPTH_TEST); //uncomment for RenderTextOnScreen
	//Add these code just after glDisable(GL_DEPTH_TEST);
	Mtx44 ortho;
	ortho.SetToOrtho(0, 80, 0, 60, -10, 10); //size of screen UI
	projectionStack.PushMatrix();
	projectionStack.LoadMatrix(ortho);
	viewStack.PushMatrix();
	viewStack.LoadIdentity(); //No need camera for ortho mode
	modelStack.PushMatrix();
	modelStack.LoadIdentity(); //Reset modelStack
	modelStack.Translate(x, y, 0);
	modelStack.Scale(size, size, size);

	glUniform1i(m_parameters[U_TEXT_ENABLED], 1);
	glUniform3fv(m_parameters[U_TEXT_COLOR], 1, &color.r);
	glUniform1i(m_parameters[U_LIGHTENABLED], 0);
	glUniform1i(m_parameters[U_COLOR_TEXTURE_ENABLED], 1);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, mesh->textureID);
	glUniform1i(m_parameters[U_COLOR_TEXTURE], 0);
	for (unsigned i = 0; i < text.length(); ++i)
	{
		Mtx44 characterSpacing;
		characterSpacing.SetToTranslation(0.5f + i * 0.6f, 0.5f, 0); //1.0f is the spacing of each character, you may change this value
		Mtx44 MVP = projectionStack.Top() * viewStack.Top() * modelStack.Top() * characterSpacing;
		glUniformMatrix4fv(m_parameters[U_MVP], 1, GL_FALSE, &MVP.a[0]);
		mesh->Render((unsigned)text[i] * 6, 6);
	}
	glBindTexture(GL_TEXTURE_2D, 0);
	glUniform1i(m_parameters[U_TEXT_ENABLED], 0);
	projectionStack.PopMatrix();
	viewStack.PopMatrix();
	modelStack.PopMatrix();
	glEnable(GL_DEPTH_TEST); //uncomment for RenderTextOnScreen
}

void SP::Init()
{


	// Set background colour to light purple
	glClearColor(0.5f, 0.0f, 0.7f, 0.0f);
	// Enable depth test
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_CULL_FACE);
	// Enable blending
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	camera.Init(Vector3(0, 3, 1), Vector3(0, 3, 10), Vector3(0, 1, 0));

	m_programID = LoadShaders("Shader//Texture.vertexshader", "Shader//Text.fragmentshader");
	m_parameters[U_MVP] = glGetUniformLocation(m_programID, "MVP");
	m_parameters[U_MODELVIEW] = glGetUniformLocation(m_programID, "MV");
	m_parameters[U_MODELVIEW_INVERSE_TRANSPOSE] = glGetUniformLocation(m_programID, "MV_inverse_transpose");
	m_parameters[U_MATERIAL_AMBIENT] = glGetUniformLocation(m_programID, "material.kAmbient");
	m_parameters[U_MATERIAL_DIFFUSE] = glGetUniformLocation(m_programID, "material.kDiffuse");
	m_parameters[U_MATERIAL_SPECULAR] = glGetUniformLocation(m_programID, "material.kSpecular");
	m_parameters[U_MATERIAL_SHININESS] = glGetUniformLocation(m_programID, "material.kShininess");

	//Replace previous code
	light[0].type = Light::LIGHT_SPOT;
	light[0].position.Set(-100, 20, 25);
	light[0].color.Set(1, 1, 1);
	light[0].power = 1;
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

	//m_parameters[U_LIGHTENABLED] = glGetUniformLocation(m_programID, "lightEnabled");
	//m_parameters[U_NUMLIGHTS] = glGetUniformLocation(m_programID, "numLights");

	light[1].type = Light::LIGHT_POINT;
	light[1].position.Set(10, 8, 20);
	light[1].color.Set(1, 1, 1);
	light[1].power = 0.8;
	light[1].kC = 1.f;
	light[1].kL = 0.01f;
	light[1].kQ = 0.001f;
	light[1].cosCutoff = cos(Math::DegreeToRadian(45));
	light[1].cosInner = cos(Math::DegreeToRadian(30));
	light[1].exponent = 3.f;
	light[1].spotDirection.Set(0.f, 1.f, 0.f);


	m_parameters[U_LIGHT1_POSITION] = glGetUniformLocation(m_programID, "lights[1].position_cameraspace");
	m_parameters[U_LIGHT1_COLOR] = glGetUniformLocation(m_programID, "lights[1].color");
	m_parameters[U_LIGHT1_POWER] = glGetUniformLocation(m_programID, "lights[1].power");
	m_parameters[U_LIGHT1_KC] = glGetUniformLocation(m_programID, "lights[1].kC");
	m_parameters[U_LIGHT1_KL] = glGetUniformLocation(m_programID, "lights[1].kL");
	m_parameters[U_LIGHT1_KQ] = glGetUniformLocation(m_programID, "lights[1].kQ");
	m_parameters[U_LIGHT1_TYPE] = glGetUniformLocation(m_programID, "lights[1].type");
	m_parameters[U_LIGHT1_SPOTDIRECTION] = glGetUniformLocation(m_programID, "lights[1].spotDirection");
	m_parameters[U_LIGHT1_COSCUTOFF] = glGetUniformLocation(m_programID, "lights[1].cosCutoff");
	m_parameters[U_LIGHT1_COSINNER] = glGetUniformLocation(m_programID, "lights[1].cosInner");
	m_parameters[U_LIGHT1_EXPONENT] = glGetUniformLocation(m_programID, "lights[1].exponent");

	m_parameters[U_LIGHTENABLED] = glGetUniformLocation(m_programID, "lightEnabled");
	m_parameters[U_NUMLIGHTS] = glGetUniformLocation(m_programID, "numLights");


	// Get a handle for our "colorTexture" uniform
	m_parameters[U_COLOR_TEXTURE_ENABLED] = glGetUniformLocation(m_programID, "colorTextureEnabled");
	m_parameters[U_COLOR_TEXTURE] = glGetUniformLocation(m_programID, "colorTexture");

	// Get a handle for our "textColor" uniform
	m_parameters[U_TEXT_ENABLED] = glGetUniformLocation(m_programID, "textEnabled");
	m_parameters[U_TEXT_COLOR] = glGetUniformLocation(m_programID, "textColor");
	glUseProgram(m_programID);

	// Make sure you pass uniform parameters after glUseProgram()
	// Make sure you pass uniform parameters after glUseProgram()
	glUniform1i(m_parameters[U_NUMLIGHTS], 2);
	glUniform1i(m_parameters[U_LIGHT0_TYPE], light[0].type);
	glUniform3fv(m_parameters[U_LIGHT0_COLOR], 1, &light[0].color.r);
	glUniform1f(m_parameters[U_LIGHT0_POWER], light[0].power);
	glUniform1f(m_parameters[U_LIGHT0_KC], light[0].kC);
	glUniform1f(m_parameters[U_LIGHT0_KL], light[0].kL);
	glUniform1f(m_parameters[U_LIGHT0_KQ], light[0].kQ);
	glUniform1f(m_parameters[U_LIGHT0_COSCUTOFF], light[0].cosCutoff);
	glUniform1f(m_parameters[U_LIGHT0_COSINNER], light[0].cosInner);
	glUniform1f(m_parameters[U_LIGHT0_EXPONENT], light[0].exponent);

	glUniform1i(m_parameters[U_LIGHT1_TYPE], light[1].type);
	glUniform3fv(m_parameters[U_LIGHT1_COLOR], 1, &light[1].color.r);
	glUniform1f(m_parameters[U_LIGHT1_POWER], light[1].power);
	glUniform1f(m_parameters[U_LIGHT1_KC], light[1].kC);
	glUniform1f(m_parameters[U_LIGHT1_KL], light[1].kL);
	glUniform1f(m_parameters[U_LIGHT1_KQ], light[1].kQ);
	glUniform1f(m_parameters[U_LIGHT1_COSCUTOFF], light[1].cosCutoff);
	glUniform1f(m_parameters[U_LIGHT1_COSINNER], light[1].cosInner);
	glUniform1f(m_parameters[U_LIGHT1_EXPONENT], light[1].exponent);

	// Generate a default VAO for now
	glGenVertexArrays(1, &m_vertexArrayID);
	glBindVertexArray(m_vertexArrayID);	
	rotateAngle = 0;
	for (int i = 0; i < NUM_GEOMETRY; ++i)
	{
		meshList[i] = nullptr;
	}

	meshList[GEO_AXES] = MeshBuilder::GenerateAxes("reference", 1000, 1000, 1000);

	meshList[GEO_QUAD] = MeshBuilder::GenerateQuad("quad", Color(0, 0, 0), 1.f);
	meshList[GEO_QUAD]->textureID = LoadTGA("Image//Stone_Floor.tga");

	meshList[GEO_CUBE] = MeshBuilder::GenerateCube("cube", Color(1, 1, 1), 1.f);
	meshList[GEO_CUBE]->textureID = LoadTGA("Image//color.tga");

	meshList[GEO_FRONT] = MeshBuilder::GenerateQuad("front", Color(1, 1, 1), 1.f);
	meshList[GEO_FRONT]->textureID = LoadTGA("Image//space_ft.tga");

	meshList[GEO_LEFT] = MeshBuilder::GenerateQuad("left", Color(1, 1, 1), 1.f);
	meshList[GEO_LEFT]->textureID = LoadTGA("Image//space_lf.tga");

	meshList[GEO_RIGHT] = MeshBuilder::GenerateQuad("right", Color(1, 1, 1), 1.f);
	meshList[GEO_RIGHT]->textureID = LoadTGA("Image//space_rt.tga");

	meshList[GEO_TOP] = MeshBuilder::GenerateQuad("top", Color(1, 1, 1), 1.f);
	meshList[GEO_TOP]->textureID = LoadTGA("Image//space_up.tga");

	meshList[GEO_BOTTOM] = MeshBuilder::GenerateQuad("bottom", Color(1, 1, 1), 1.f);
	meshList[GEO_BOTTOM]->textureID = LoadTGA("Image//space_dn.tga");

	meshList[GEO_BACK] = MeshBuilder::GenerateQuad("back", Color(1, 1, 1), 1.f);
	meshList[GEO_BACK]->textureID = LoadTGA("Image//space_bk.tga");

	meshList[GEO_BLEND] = MeshBuilder::GenerateQuad("blend", Color(1, 1, 1), 1.f);
	meshList[GEO_BLEND]->textureID = LoadTGA("Image//Grimm.tga");

	meshList[GEO_SPHERE] = MeshBuilder::GenerateSphere("sphere", Color(0, 0, 0), 10, 20, 1.f);			//limbs

	meshList[GEO_SPHERE2] = MeshBuilder::GenerateSphere("sphere", Color(1, 1, 1), 10, 20, 1.f);			//SPELL

	meshList[GEO_LAMPLIGHT] = MeshBuilder::GenerateSphere("sphere", Color(0.4, 0.4, 0.4), 10, 20, 1.f);			//limbs

	meshList[GEO_CYLINDER] = MeshBuilder::GenerateCylinder("cylinder", Color(1, 1, 1), 20, 1.f, 1.f);		//base of head

	meshList[GEO_BODY] = MeshBuilder::GenerateCylinder("cylinder", Color(0, 0, 0), 20, 1.f, 1.f);		//body

	meshList[GEO_HEMISPHERE] = MeshBuilder::GenerateHemisphere("hemisphere", Color(1, 1, 1), 10, 20, 1.f);  //ends of head

	meshList[GEO_HEMISPHERE2] = MeshBuilder::GenerateHemisphere("hemisphere", Color(0, 0, 0), 10, 20, 1.f);  //waist

	meshList[GEO_HEMISPHERE3] = MeshBuilder::GenerateHemisphere("hemisphere", Color(0.6, 0.6, 0.6), 10, 20, 1.f);  //waist

	meshList[GEO_TORUS] = MeshBuilder::GenerateTorus("torus", Color(0.3, 0.3, 0.3), 20, 20, 1.f, 0.5f);			//cape head

	meshList[GEO_CAPE] = MeshBuilder::GenerateCylinder("cylinder", Color(0.3, 0.3, 0.3), 20, 1.f, 1.f);		//base of head

	meshList[GEO_HALFTORUS] = MeshBuilder::GenerateHalfTorus("quartertorus", Color(1, 1, 1), 10, 20, 1.f, 0.5f);   //horns

	meshList[GEO_CONE] = MeshBuilder::GenerateCone("cone", Color(0.3, 0.3, 0.3), 20, 1.f, 1.f);   //cape end

	meshList[GEO_SWORD] = MeshBuilder::GenerateCone("cone", Color(1, 1, 1), 20, 1.f, 1.f);   //cape ends

	meshList[GEO_LIGHTBALL] = MeshBuilder::GenerateSphere("sphere", Color(1, 1, 1), 10, 20, 1.f);		//light for lamppost
	
	meshList[GEO_HOLLOW] = MeshBuilder::GenerateOBJMTL("hollow", "OBJ//Hollow.obj", "OBJ//Hollow.mtl");
	//meshList[GEO_HOLLOW]->textureID = LoadTGA("Image//Hollow-text.tga");

	meshList[GEO_HORNET] = MeshBuilder::GenerateOBJMTL("hornet", "OBJ//zombie.obj", "OBJ//zombie.mtl");
	

	meshList[GEO_BENCH] = MeshBuilder::GenerateOBJMTL("bench", "OBJ//bench.obj","OBJ//bench.mtl");

	meshList[GEO_ROCKS] = MeshBuilder::GenerateOBJMTL("rocks", "OBJ//rocks.obj", "OBJ//rocks.mtl");
	//meshList[GEO_ROCKS]->textureID = LoadTGA("Image//rockwall.tga");
	meshList[GEO_TALLROCK] = MeshBuilder::GenerateOBJMTL("tallrock", "OBJ//rocksTall.obj", "OBJ//rocksTall.mtl");
	//meshList[GEO_TALLROCK]->textureID = LoadTGA("Image//rockwall.tga");	

	meshList[GEO_BROKENBENCH] = MeshBuilder::GenerateOBJMTL("benchdamaged", "OBJ//benchDamaged.obj", "OBJ//benchDamaged.mtl");

	meshList[GEO_STONEWALL] = MeshBuilder::GenerateOBJMTL("stonewall", "OBJ//stoneWall.obj", "OBJ//stoneWall.mtl");

	meshList[GEO_STONEWALLDAMAGED] = MeshBuilder::GenerateOBJMTL("stonewalldamaged", "OBJ//stoneWallDamaged.obj", "OBJ//stoneWallDamaged.mtl");

	meshList[GEO_ROAD] = MeshBuilder::GenerateOBJMTL("road", "OBJ//road.obj", "OBJ//road.mtl");

	meshList[GEO_CRYPT] = MeshBuilder::GenerateOBJMTL("crypt", "OBJ//crypt.obj", "OBJ//crypt.mtl");

	meshList[GEO_DEBRIS] = MeshBuilder::GenerateOBJMTL("debris", "OBJ//debris.obj", "OBJ//debris.mtl");
	//meshList[GEO_DEBRIS]->textureID = LoadTGA("Image//rockwall.tga");

	meshList[GEO_LAMP] = MeshBuilder::GenerateOBJMTL("debris", "OBJ//lightpostDouble.obj", "OBJ//lightpostDouble.mtl");

	meshList[GEO_GRAVE] = MeshBuilder::GenerateOBJMTL("grave", "OBJ//grave.obj", "OBJ//grave.mtl");

	meshList[GEO_LANTERN] = MeshBuilder::GenerateOBJMTL("lantern", "OBJ//lanternCandle.obj", "OBJ//lanternCandle.mtl");

	meshList[GEO_BULLET] = MeshBuilder::GenerateOBJMTL("bullet", "OBJ//ironFenceBar.obj", "OBJ//ironFenceBar.mtl");

	meshList[GEO_FIREBASKET] = MeshBuilder::GenerateOBJMTL("basket", "OBJ//fireBasket.obj", "OBJ//fireBasket.mtl");

	meshList[GEO_TEXT] = MeshBuilder::GenerateText("text", 16,16);
	//meshList[GEO_TEXT]->textureID = LoadTGA("Image//RockwellFont.tga");

	meshList[GEO_TEXT2] = MeshBuilder::GenerateText("text2", 16, 16);
	//meshList[GEO_TEXT2]->textureID = LoadTGA("Image//comicsans.tga");

	//An array of 3 vectors which represents the colors of the 3 vertices

	Mtx44 projection;
	projection.SetToPerspective(45.f, 4.f / 3.f, 0.1f, 1000.f);
	projectionStack.LoadMatrix(projection);
	
	
	Mesh::SetMaterialLoc(m_parameters[U_MATERIAL_AMBIENT],
		m_parameters[U_MATERIAL_DIFFUSE],
		m_parameters[U_MATERIAL_SPECULAR],
		m_parameters[U_MATERIAL_SHININESS]);

	benchposx = 10;
	benchposz = 50;
	npcx = -7;
	npcz = 85;
	
		
	enemyx = -65;
	enemyz = 25;
	bullet.x = -65;
	bullet.z = enemyz;

	bullet2.x = -85;
	bullet2.z = enemyz;

	bullet3.x = -45;
	bullet3.z = enemyz;

	bullet4.x = -35;
	bullet4.z = enemyz;

	bullet5.x = -55;
	bullet5.z = enemyz;
		
	bullet6.x = -95;
	bullet6.z = enemyz;

	bullet7.x = -50;
	bullet7.z = enemyz;

	bullet8.x = -70;
	bullet8.z = enemyz;
	
	spell.x = spellx;
	spell.y = -45;
	spell.z = spellz;
	
	basketz = 25;
	basketx = -100;

}

void SP::Update(double dt)
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

	//if (Application::IsKeyPressed('Z'))			//go forward
	//{

	//	translateZ += (float)(20 * dt * translateX);
	//	firstlegRotate += (float)(180 * dt * reverseRotateLeg);
	//	if (firstlegRotate < -15)
	//	{
	//		reverseRotateLeg = 1;
	//	}
	//	if (firstlegRotate > 15)
	//	{
	//		reverseRotateLeg = -1;
	//	}

	//	secondlegRotate += (float)(180 * dt * reverseRotateLeg2);
	//	if (secondlegRotate < -15)
	//	{
	//		reverseRotateLeg2 = 1;
	//	}
	//	if (secondlegRotate > 15)
	//	{
	//		reverseRotateLeg2 = -1;
	//	}
	//	rotateBody = 1;
	//}

	//if (Application::IsKeyPressed('X'))			//go backwards
	//{

	//	translateZ += (float)(20 * dt * -translateX);
	//	firstlegRotate += (float)(180 * dt * reverseRotateLeg3);
	//	if (firstlegRotate < -15)
	//	{
	//		reverseRotateLeg3 = 1;
	//	}
	//	if (firstlegRotate > 15)
	//	{
	//		reverseRotateLeg3 = -1;
	//	}
	//	secondlegRotate += (float)(180 * dt * reverseRotateLeg4);
	//	if (secondlegRotate < -15)
	//	{
	//		reverseRotateLeg4 = 1;
	//	}
	//	if (secondlegRotate > 15)
	//	{
	//		reverseRotateLeg4 = -1;
	//	}
	//	rotateBody = 180;
	//}

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

	static bool spellstate = false;
	if (/*!spellstate && */Application::IsKeyPressed('C'))			//spawn spell
	{
		spellx = 0;
		spellswordtranslate = -80;

		spellbodyUp += (float)(90 * dt);
		if (spellbodyUp > 3)
		{
			spellarmTranslateZ = -5;
			spelllegTranslateZ = -3;
			spellbodyUp = 3;
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
	//if (/*!spellstate && */Application::IsKeyPressed('C'))			//spawn spell
	//{
	//	spellx = 0;
	//	spellswordtranslate = -80;

	//	spellbodyUp += (float)(90 * dt);
	//	if (spellbodyUp > 3)
	//	{
	//		spellarmTranslateZ = -5;
	//		spelllegTranslateZ = -3;
	//		spellbodyUp = 3;
	//		spellheadrotate = 25;
	//		spelly = 0;
	//		spell.z -= (float)(240 * dt);
	//}

	if (/*spellstate && */Application::IsKeyPressed('C') == false)			//despawn spell
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
		//spellstate = false;
	}

	if (Application::IsKeyPressed('Z') == false && Application::IsKeyPressed('X') == false)			//go forward
	{

		firstlegRotate = 0;
		secondlegRotate = 0;

	}

	/*******************************************************************************************************/
	//boundary check
	//if (camera.position.x > 18)
	//{
	//	camera.position.x = 18;
	//}
	//if (camera.position.x < -100)
	//{
	//	camera.position.x = -100;
	//}
	//if (camera.position.z > 103)
	//{
	//	camera.position.z = 103;
	//}
	////if (camera.position.z < -14)
	////{
	////	camera.position.z = -14;
	////}


	//if (camera.position.x < -7 && camera.position.x >= -34 && camera.position.z < 72)
	//{
	//	if (camera.position.x < -20)
	//	{
	//		camera.position.x = -34;
	//	}
	//	else
	//	{
	//		camera.position.x = -7;
	//	}
	//}
	////x = -34, -7
	//if (camera.position.z < 74 && camera.position.x >= -32 && camera.position.x < -7)
	//{
	//	camera.position.z = 74;
	//}


	//if (camera.position.z <= 17 && camera.position.x <= -33)
	//{
	//	camera.position.z = 17;
	//}

	//
	//if (camera.position.x <= -13 && camera.position.x >= -17 && camera.position.z <= 103 && camera.position.z >= 84)
	//{
	//	if (camera.position.x < -15)
	//	{
	//		camera.position.x = -17;
	//	}
	//	else
	//	{
	//		camera.position.x = -13;
	//	}
	//}

	/**********************************************************************************************************/
	//making character move with camera
	if (camera.target.z - camera.position.z >= 0 && camera.target.x - camera.position.x <= 0) 
	{
		characterRotate = 180 + Math::rotateAngle(camera.target.z - camera.position.z, camera.target.x - camera.position.x);
	}

	else if (camera.target.z - camera.position.z >= 0 && camera.target.x - camera.position.x >= 0)
	{
		characterRotate = 360 - Math::rotateAngle(camera.target.z - camera.position.z, camera.target.x - camera.position.x);
	}

	else if (camera.target.z - camera.position.z <= 0 && camera.target.x - camera.position.x <= 0)
	{
		characterRotate = 180 + Math::rotateAngle(camera.target.z - camera.position.z, camera.target.x - camera.position.x);
	}

	//0 - 90
	else if (camera.target.z - camera.position.z <= 0 && camera.target.x - camera.position.x >= 0)
	{
		characterRotate = -Math::rotateAngle(camera.target.z - camera.position.z, camera.target.x - camera.position.x);
	}
	/**********************************************************************************************************/


	//bench 
	static bool benchstate = false;
	if (camera.position.z < (benchposz + 5) && camera.position.z >(benchposz - 5) && (camera.position.x < (benchposx + 5) && camera.position.x >(benchposx - 5)))
	{
		if (!benchstate && Application::IsKeyPressed('E'))
		{
			isonbench++;
			benchstate = true;
			benchinteract = !benchinteract;
			if (isonbench > 3)
			{
				isonbench = 1;
			}
		} 
		else if (benchstate && !Application::IsKeyPressed('E'))
		{
			benchstate = false;
		}
		if (isonbench == 1)
		{
			camera.Init(Vector3(benchposx, 3, benchposz), Vector3(benchposx - 5, 3, benchposz), Vector3(0, 1, 0));
			playerhealth = 20;
			isonbench++;
		}
		if (isonbench == 3)		//means not on bench
		{
			camera.Init(Vector3(benchposx - 5, 3, benchposz), Vector3(benchposx - 5, 3, benchposz+5), Vector3(0, 1, 0));
			isonbench++;
		}
		isclosetobench = true;
	}
	else
	{
		isclosetobench = false;
	}

	//npc
	if (camera.position.z < (npcz + 5) && camera.position.z >(npcz - 5) && (camera.position.x < (npcx + 5) && camera.position.x >(npcx - 5)))
	{
		isclosetonpc = true;
	}
	else
	{
		isclosetonpc = false;
	}

	
	// 3rd interaction : fighting
	if (camera.position.x <= -9 && camera.position.z >= 72)
	{
		battlestart = true;
	}
	if(battlestart)
	{
		bullet.z += (float)(60 * dt);
		bullet2.z += (float)(120 * dt);
		bullet3.z += (float)(50 * dt);
		bullet4.z += (float)(70 * dt);
		bullet5.z += (float)(100 * dt);
		bullet6.z += (float)(30 * dt);
		bullet7.z += (float)(90 * dt);
		bullet8.z += (float)(100 * dt);

		if (bullet.z > 112)
		{
			bullet.z = enemyz;
		}
		if (bullet2.z > 112)
		{
			bullet2.z = enemyz;
		}
		if (bullet3.z > 112)
		{
			bullet3.z = enemyz;
		}
		if (bullet4.z > 112)
		{
			bullet4.z = enemyz;
		}
		if (bullet5.z > 112)
		{
			bullet5.z = enemyz;
		}
		if (bullet6.z > 112)
		{
			bullet6.z = enemyz;
		}
		if (bullet7.z > 112)
		{
			bullet7.z = enemyz;
		}

		if (bullet8.z > 112)
		{
			bullet8.z = enemyz;
		}
	}

	if (((bullet.x + 2) > camera.position.x && (bullet.x - 2) < camera.position.x && (bullet.z + 4) > camera.position.z && (bullet.z - 2) < camera.position.z)
		|| ((bullet2.x + 2) > camera.position.x && (bullet2.x - 2) < camera.position.x && (bullet2.z + 4) > camera.position.z && (bullet2.z - 2) < camera.position.z)
		|| ((bullet3.x + 2) > camera.position.x && (bullet3.x - 2) < camera.position.x && (bullet3.z + 4) > camera.position.z && (bullet3.z - 2) < camera.position.z)
		|| ((bullet4.x + 2) > camera.position.x && (bullet4.x - 2) < camera.position.x && (bullet4.z + 4) > camera.position.z && (bullet4.z - 2) < camera.position.z)
		|| ((bullet5.x + 2) > camera.position.x && (bullet5.x - 2) < camera.position.x && (bullet5.z + 4) > camera.position.z && (bullet5.z - 2) < camera.position.z)
		|| ((bullet6.x + 2) > camera.position.x && (bullet6.x - 2) < camera.position.x && (bullet6.z + 4) > camera.position.z && (bullet6.z - 2) < camera.position.z)
		|| ((bullet7.x + 2) > camera.position.x && (bullet7.x - 2) < camera.position.x && (bullet7.z + 4) > camera.position.z && (bullet7.z - 2) < camera.position.z)
		|| ((bullet8.x + 2) > camera.position.x && (bullet8.x - 2) < camera.position.x && (bullet8.z + 4) > camera.position.z && (bullet8.z - 2) < camera.position.z))
	{
		playerhealth -= (1 * dt);
	}
	if (playerhealth <= 0)
	{
		camera.Init(Vector3(-96, 3, 30), Vector3(-96, 3, 0), Vector3(0, 1, 0));
		die = true;
	
	}

	if (enemyhealth <= 0)
	{
		camera.Init(Vector3(-96, 3, 30), Vector3(-96, 3, 0), Vector3(0, 1, 0));
		win = true;

	}
	if (Application::IsKeyPressed('R'))
	{
		camera.Init(Vector3(0, 3, 1), Vector3(0, 3, 10), Vector3(0, 1, 0));
		playerhealth = 20;
		battlestart = false;
		enemyhealth = 100;
		die = false;
		win = false;

	}
	if (battlestart == false)
	{
		enemyx = -65;
		enemyz = 25;
		bullet.x = -65;
		bullet.z = enemyz;

		bullet2.x = -85;
		bullet2.z = enemyz;

		bullet3.x = -45;
		bullet3.z = enemyz;

		bullet4.x = -35;
		bullet4.z = enemyz;

		bullet5.x = -55;
		bullet5.z = enemyz;

		bullet6.x = -95;
		bullet6.z = enemyz - 40;

		bullet7.x = -50;
		bullet7.z = enemyz;
	}
	
	if (camera.position.z < (basketz + 5) && camera.position.z >(basketz - 5) && (camera.position.x < (basketx + 5) && camera.position.x >(basketx - 5)))
	{
		
		isclosetobasket = true;
		if (Application::IsKeyPressed('E'))
		{
			enemyhealth -= 25;
			/*camera.position.x = -10;
			camera.position.z = 75;
			camera.target.x = -100;
			camera.target.z = 72;
			camera.target.y = 3;*/
			camera.Init(Vector3(-10, 3, 75), Vector3(-100, 3, 72), Vector3(0, 1, 0));
		
		}
	}
	else
	{
		isclosetobasket = false;
	}


	if (die || win)
	{
		translatebasketz = -60;
		
	}
	else
	{
		translatebasketz = 0;
	}
	rotateAngle += (float)(20 * dt);
	camera.Update(dt);
	FPS = std::to_string(1.f / dt);
	camerax = std::to_string(camera.position.x);
	
	cameraz = std::to_string(camera.position.z);
}


void SP::Render()
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


	modelStack.PushMatrix();
	modelStack.Translate(light[0].position.x, light[0].position.y, light[0].position.z);
	RenderMesh(meshList[GEO_LIGHTBALL], false);
	modelStack.PopMatrix();

	modelStack.PushMatrix();
	modelStack.Translate(light[1].position.x, light[1].position.y, light[1].position.z);
	RenderMesh(meshList[GEO_LIGHTBALL], false);
	modelStack.PopMatrix();

	RenderSkybox();

	////body
	modelStack.PushMatrix();
	{
		modelStack.Translate(0, 1.4, 0);
		modelStack.Translate(camera.position.x, 0, camera.position.z);
		modelStack.Translate(0, spellbodyUp, 0);
		modelStack.Rotate(characterRotate, 0, 1, 0);
		modelStack.Rotate(90,0,1,0);
		modelStack.Rotate(rotateBody, 0, 1, 0);
		modelStack.Scale(0.1, 0.1, 0.1);

		//head
		modelStack.PushMatrix();
		{
			modelStack.Translate(0, 15, 0);
			modelStack.Translate(0, -85, 0);
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

				modelStack.Scale(4, 4, 4);
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
			modelStack.Translate(armTranslateX, armTranslateY, armTranslateZ);
			modelStack.Translate(0, 0, spellarmTranslateZ);
			modelStack.Rotate(rotateAngle, 0, 1, 0);
			modelStack.Rotate(swordRotate, 1, 0, 0);
			modelStack.Rotate(spellarmrotate, 1, 0, 0);

			//sword hilt
			modelStack.PushMatrix();
			{
				modelStack.Translate(-14, -4, 13.5);
				modelStack.Translate(swordTranslateX, swordTranslateY, 0);
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





	modelStack.PushMatrix();
	modelStack.Translate(0, 0, 0);
	modelStack.Rotate(-90, 1, 0, 0);
	modelStack.Rotate(0, 0, 1, 0);
	modelStack.Scale(1000, 1000, 1000);
	RenderMesh(meshList[GEO_QUAD], true);
	modelStack.PopMatrix();
	/**************************************************/
	//NPCS and interactables
	modelStack.PushMatrix();
	modelStack.Translate(npcx, 0.3, npcz);
	modelStack.Rotate(90, 0, 0, 1);
	modelStack.Rotate(45, 1, 0, 0);
	modelStack.Scale(0.5, 0.5, 0.5);
	RenderMesh(meshList[GEO_HOLLOW], true);
	modelStack.PopMatrix();

	modelStack.PushMatrix();
	modelStack.Translate(enemyx, 0, enemyz);
	modelStack.Rotate(0, 0, 0, 1);
	modelStack.Scale(10, 10, 10);
	RenderMesh(meshList[GEO_HORNET], true);
	modelStack.PopMatrix();

	modelStack.PushMatrix();
	modelStack.Translate(benchposx, 0, benchposz);
	modelStack.Rotate(270, 0, 1, 0);
	modelStack.Scale(6, 6, 6);
	RenderMesh(meshList[GEO_BENCH], true);
	modelStack.PopMatrix();
	/*************************************************************/

	//right side
	modelStack.PushMatrix();
	modelStack.Translate(-30, 0, 0);
	modelStack.Rotate(0, 0, 0, 1);
	modelStack.Scale(50, 50, 50);
	RenderMesh(meshList[GEO_ROCKS], true);
	modelStack.PopMatrix();

	modelStack.PushMatrix();
	modelStack.Translate(-20, 0, 35);
	modelStack.Rotate(90, 0, 1, 0);
	modelStack.Scale(60, 30, 30);
	RenderMesh(meshList[GEO_TALLROCK], true);
	modelStack.PopMatrix();

	modelStack.PushMatrix();
	modelStack.Translate(-16, 0, 67);
	modelStack.Rotate(0, 0, 1, 0);
	modelStack.Scale(10, 7, 10);
	RenderMesh(meshList[GEO_GRAVE], true);
	modelStack.PopMatrix();

	//left side
	modelStack.PushMatrix();
	modelStack.Translate(40, 0, 0);
	modelStack.Rotate(90, 0, 1, 0);
	modelStack.Scale(50, 50, 50);
	RenderMesh(meshList[GEO_ROCKS], true);
	modelStack.PopMatrix();

	modelStack.PushMatrix();
	modelStack.Translate(40, 0, 60);
	modelStack.Rotate(270, 0, 1, 0);
	modelStack.Scale(100, 40, 40);
	RenderMesh(meshList[GEO_TALLROCK], true);
	modelStack.PopMatrix();

	modelStack.PushMatrix();
	modelStack.Translate(24, 0, -18);
	modelStack.Rotate(95, 0, 1, 0);
	modelStack.Scale(30, 30, 30);
	RenderMesh(meshList[GEO_DEBRIS], true);
	modelStack.PopMatrix();

	modelStack.PushMatrix();
	modelStack.Translate(25, 0, 55);
	modelStack.Rotate(0, 0, 1, 0);
	modelStack.Scale(10, 7, 10);
	RenderMesh(meshList[GEO_CRYPT], true);
	modelStack.PopMatrix();

	modelStack.PushMatrix();
	modelStack.Translate(25, 0, 35);
	modelStack.Rotate(0, 0, 1, 0);
	modelStack.Scale(10, 7, 10);
	RenderMesh(meshList[GEO_CRYPT], true);
	modelStack.PopMatrix();


	modelStack.PushMatrix();
	modelStack.Translate(25, 0, 45);
	modelStack.Rotate(0, 0, 1, 0);
	modelStack.Scale(10, 10, 10);
	RenderMesh(meshList[GEO_LAMP], true);
	modelStack.PopMatrix();


	modelStack.PushMatrix();
	modelStack.Translate(-24, 0, 73);
	modelStack.Rotate(0, 0, 1, 0);
	modelStack.Scale(10, 10, 10);
	RenderMesh(meshList[GEO_LANTERN], true);
	modelStack.PopMatrix();

	//back side
	modelStack.PushMatrix();
	modelStack.Translate(0, 0, -35);
	modelStack.Rotate(180, 0, 1, 0);
	modelStack.Scale(50, 50, 50);
	RenderMesh(meshList[GEO_TALLROCK], true);
	modelStack.PopMatrix();	



	modelStack.PushMatrix();
	modelStack.Translate(5, 0, 20);
	modelStack.Rotate(270, 0, 1, 0);
	modelStack.Scale(6, 6, 6);
	RenderMesh(meshList[GEO_BROKENBENCH], true);
	modelStack.PopMatrix();
	
	//in front
	modelStack.PushMatrix();
	modelStack.Translate(-0, 0, 110);
	modelStack.Rotate(0, 0, 1, 0);
	modelStack.Scale(100, 10, 10);
	RenderMesh(meshList[GEO_STONEWALL], true);
	modelStack.PopMatrix();

	modelStack.PushMatrix();
	modelStack.Translate(-60, 0, 110);
	modelStack.Rotate(0, 0, 1, 0);
	modelStack.Scale(100, 10, 10);
	RenderMesh(meshList[GEO_STONEWALL], true);
	modelStack.PopMatrix();

	modelStack.PushMatrix();
	modelStack.Translate(-100, 0, 60);
	modelStack.Rotate(90, 0, 1, 0);
	modelStack.Scale(100, 10, 10);
	RenderMesh(meshList[GEO_STONEWALL], true);
	modelStack.PopMatrix();

	modelStack.PushMatrix();
	modelStack.Translate(-15, 0, 65);
	modelStack.Rotate(90, 0, 1, 0);
	modelStack.Scale(20, 10, 10);
	RenderMesh(meshList[GEO_STONEWALLDAMAGED], true);
	modelStack.PopMatrix();

	modelStack.PushMatrix();
	modelStack.Translate(-20, 0, 95);
	modelStack.Rotate(270, 0, 1, 0);
	modelStack.Scale(20, 10, 10);
	RenderMesh(meshList[GEO_STONEWALLDAMAGED], true);
	modelStack.PopMatrix();

	modelStack.PushMatrix();
	modelStack.Translate(-70, 0, 20);
	modelStack.Rotate(0, 0, 1, 0);
	modelStack.Scale(100, 10, 10);
	RenderMesh(meshList[GEO_STONEWALL], true);
	modelStack.PopMatrix();

	//roads
	modelStack.PushMatrix();
	modelStack.Translate(0, 0, 4);
	modelStack.Rotate(90, 0, 1, 0);
	modelStack.Scale(20, 5, 5);
	RenderMesh(meshList[GEO_ROAD], true);
	modelStack.PopMatrix();

	modelStack.PushMatrix();
	modelStack.Translate(0, 0, 18);
	modelStack.Rotate(90, 0, 1, 0);
	modelStack.Scale(20, 5, 5);
	RenderMesh(meshList[GEO_ROAD], true);
	modelStack.PopMatrix();

	modelStack.PushMatrix();
	modelStack.Translate(0, 0, 32);
	modelStack.Rotate(90, 0, 1, 0);
	modelStack.Scale(20, 5, 5);
	RenderMesh(meshList[GEO_ROAD], true);
	modelStack.PopMatrix();

	modelStack.PushMatrix();
	modelStack.Translate(0, 0, 46);
	modelStack.Rotate(90, 0, 1, 0);
	modelStack.Scale(20, 5, 5);
	RenderMesh(meshList[GEO_ROAD], true);
	modelStack.PopMatrix();


	modelStack.PushMatrix();
	modelStack.Translate(0, 0, 60);
	modelStack.Rotate(90, 0, 1, 0);
	modelStack.Scale(20, 5, 5);
	RenderMesh(meshList[GEO_ROAD], true);
	modelStack.PopMatrix();

	modelStack.PushMatrix();
	modelStack.Translate(0, 0, 74);
	modelStack.Rotate(90, 0, 1, 0);
	modelStack.Scale(20, 5, 5);
	RenderMesh(meshList[GEO_ROAD], true);
	modelStack.PopMatrix();

	modelStack.PushMatrix();
	modelStack.Translate(-6, 0, 74);
	modelStack.Rotate(-160, 0, 1, 0);
	modelStack.Scale(20, 5, 5);
	RenderMesh(meshList[GEO_ROAD], true);
	modelStack.PopMatrix();

	modelStack.PushMatrix();
	modelStack.Translate(-10, 0, 78);
	modelStack.Rotate(-160, 0, 1, 0);
	modelStack.Scale(20, 5, 5);
	RenderMesh(meshList[GEO_ROAD], true);
	modelStack.PopMatrix();

	modelStack.PushMatrix();
	modelStack.Translate(bullet.x, 1, bullet.z);
	modelStack.Rotate(90, 1, 0, 0);
	modelStack.Scale(20, 5, 20);
	RenderMesh(meshList[GEO_BULLET], true);
	modelStack.PopMatrix();

	modelStack.PushMatrix();
	modelStack.Translate(bullet2.x, 1, bullet2.z);
	modelStack.Rotate(90, 1, 0, 0);
	modelStack.Scale(20, 5, 20);
	RenderMesh(meshList[GEO_BULLET], true);
	modelStack.PopMatrix();

	modelStack.PushMatrix();
	modelStack.Translate(bullet3.x, 1, bullet3.z);
	modelStack.Rotate(90, 1, 0, 0);
	modelStack.Scale(20, 5, 20);
	RenderMesh(meshList[GEO_BULLET], true);
	modelStack.PopMatrix();

	modelStack.PushMatrix();
	modelStack.Translate(bullet4.x, 1, bullet4.z);
	modelStack.Rotate(90, 1, 0, 0);
	modelStack.Scale(20, 5, 20);
	RenderMesh(meshList[GEO_BULLET], true);
	modelStack.PopMatrix();

	modelStack.PushMatrix();
	modelStack.Translate(bullet5.x, 1, bullet5.z);
	modelStack.Rotate(90, 1, 0, 0);
	modelStack.Scale(20, 5, 20);
	RenderMesh(meshList[GEO_BULLET], true);
	modelStack.PopMatrix();

	modelStack.PushMatrix();
	modelStack.Translate(bullet6.x, 1, bullet6.z);
	modelStack.Rotate(90, 1, 0, 0);
	modelStack.Scale(20, 5, 20);
	RenderMesh(meshList[GEO_BULLET], true);
	modelStack.PopMatrix();

	modelStack.PushMatrix();
	modelStack.Translate(bullet7.x, 1, bullet7.z);
	modelStack.Rotate(90, 1, 0, 0);
	modelStack.Scale(20, 5, 20);
	RenderMesh(meshList[GEO_BULLET], true);
	modelStack.PopMatrix();

	modelStack.PushMatrix();
	{
		modelStack.Translate(basketx, 0, basketz);
		modelStack.Translate(0, 0, translatebasketz);
		modelStack.Rotate(0, 0, 1, 0);
		
		//sword hilt
		modelStack.PushMatrix();
		{
			modelStack.Translate(0, 5, 0);
			modelStack.Rotate(180, 1, 0, 0);
			modelStack.Rotate(90, 0, 1, 0);
			modelStack.Scale(3, 3, 3);
			//blade
			modelStack.PushMatrix();
			{
				modelStack.Translate(0, 0, 0);
				modelStack.Rotate(90, 0, 1, 0);
				modelStack.Scale(3, 20, 2);
				modelStack.Scale(0.1, 0.1, 0.1);
				RenderMesh(meshList[GEO_SWORD], true);
			}
			modelStack.PopMatrix();

			modelStack.Translate(0, -1, 0);
			modelStack.Rotate(0, 0, 0, 1);
			modelStack.Scale(0.5, 8, 0.5);
			modelStack.Scale(0.1, 0.1, 0.1);
			RenderMesh(meshList[GEO_CYLINDER], true);
		}
		modelStack.PopMatrix();

		modelStack.Scale(20, 20, 20);
		RenderMesh(meshList[GEO_FIREBASKET], true);
	}
	modelStack.PopMatrix();



	

	//my lamps
	modelStack.PushMatrix();
	{
		modelStack.Translate(10, 1, 20);
		modelStack.Scale(0.1, 0.1, 0.1);

		//lamp base
		modelStack.PushMatrix();
		{
			//modelStack.PushMatrix();
			//{
			//	//light
			//	modelStack.Translate(0, 75, 0);
			//	modelStack.Scale(10, 10, 10);
			//	RenderMesh(meshList[GEO_LAMPLIGHT], false);
			//}
			//modelStack.PopMatrix();

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
		modelStack.Translate(-11, 1, 67);
		modelStack.Scale(0.1, 0.1, 0.1);

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



	//modelStack.PushMatrix();
	//modelStack.Translate(0, 5, 50);
	//modelStack.Rotate(180, 0, 1, 0);
	//modelStack.Rotate(grimmx, 0, 1, 0);
	//modelStack.Rotate(grimmz, 0, 1, 0);

	//modelStack.Scale(10, 10, 10);
	//RenderMesh(meshList[GEO_BLEND], false);
	//modelStack.PopMatrix();

	
	//**********************************************************************************************************************************************************
	//bench and npc interactions
	if (isclosetobench && benchinteract == false)
	{
		RenderTextOnScreen(meshList[GEO_TEXT], "Press 'E' to sit.", Color(0, 1, 0), 2, 0, 15);
	
	}
	if (isclosetobench && benchinteract)
	{

		RenderTextOnScreen(meshList[GEO_TEXT], "Press 'E' to get up.", Color(0, 1, 0), 2, 0, 15);
		
	}
	if (isclosetonpc && npcinteract == false)
	{
		RenderTextOnScreen(meshList[GEO_TEXT], "Press 'E' to talk.", Color(0, 1, 0), 2, 0, 15);
		if (Application::IsKeyPressed('E'))
		{
			npcinteract = true;
		}
	}
	if (isclosetonpc && npcinteract)
	{
		RenderTextOnScreen(meshList[GEO_TEXT2], "Corpse: man im dead. go get", Color(0, 1, 0), 3, 0, 15);
		RenderTextOnScreen(meshList[GEO_TEXT2], "        revenge for me.", Color(0, 1, 0), 3, 0, 11);
	}
	else
	{
		npcinteract = false;
	}

	if (isclosetobasket)
	{
		RenderTextOnScreen(meshList[GEO_TEXT], "Press 'E' to attack!", Color(1, 0, 0), 2, 0, 15);

	}

	if (isclosetobench)
	{
		modelStack.PushMatrix();
		modelStack.Translate(benchposx + 5, 8, benchposz -5);
		modelStack.Rotate(270, 0, 1, 0);
		modelStack.Scale(1, 1, 1);
		RenderText(meshList[GEO_TEXT], "Sit down to heal.", Color(0, 1, 0));
		modelStack.PopMatrix();
	}

	modelStack.PushMatrix();
	modelStack.Translate(6, 4, 104);
	modelStack.Rotate(180, 0, 1, 0);
	modelStack.Scale(1, 1, 1);
	RenderText(meshList[GEO_TEXT], "Run for the sword!", Color(0, 0.6, 1));
	modelStack.PopMatrix();
	

	modelStack.PushMatrix();
	modelStack.Translate(enemyx - 10, 15, enemyz);
	modelStack.Rotate(0, 0, 1, 0);
	modelStack.Scale(1, 1, 1);
	RenderText(meshList[GEO_TEXT], "Enemy Health:", Color(1, 0, 0));
	modelStack.PopMatrix();

	modelStack.PushMatrix();
	modelStack.Translate(enemyx + 4, 15, enemyz);
	modelStack.Rotate(0, 0, 1, 0);
	modelStack.Scale(1, 1, 1);
	RenderText(meshList[GEO_TEXT], std::to_string(enemyhealth), Color(1, 0, 0));
	modelStack.PopMatrix();


	if (die)
	{
		battlestart = false;
		modelStack.PushMatrix();
		modelStack.Translate(-100, 3, 17);
		modelStack.Rotate(0, 0, 1, 0);
		modelStack.Scale(1, 1, 1);
		RenderText(meshList[GEO_TEXT], "You Died!", Color(1, 0, 0));
		modelStack.PopMatrix();
		modelStack.PushMatrix();

		modelStack.Translate(-102, 1, 17);
		modelStack.Rotate(0, 0, 1, 0);
		modelStack.Scale(0.6, 0.6, 0.6);
		RenderText(meshList[GEO_TEXT], "Press 'R' to restart", Color(1, 0, 0));
		modelStack.PopMatrix();
	}
	if (win)
	{
		battlestart = false;
		modelStack.PushMatrix();
		modelStack.Translate(-100, 3, 17);
		modelStack.Rotate(0, 0, 1, 0);
		modelStack.Scale(1, 1, 1);
		RenderText(meshList[GEO_TEXT], "You Win!", Color(0, 0, 1));
		modelStack.PopMatrix();
		modelStack.PushMatrix();

		modelStack.Translate(-102, 1, 17);
		modelStack.Rotate(0, 0, 1, 0);
		modelStack.Scale(0.6, 0.6, 0.6);
		RenderText(meshList[GEO_TEXT], "Press 'R' to restart", Color(0, 0, 1));
		modelStack.PopMatrix();
	}


	RenderTextOnScreen(meshList[GEO_TEXT], camerax , Color(0, 1, 0), 2, 6, 0);
	RenderTextOnScreen(meshList[GEO_TEXT], "X:", Color(0, 1, 0), 2, 0, 0);
	RenderTextOnScreen(meshList[GEO_TEXT], cameraz, Color(0, 1, 0), 2, 6, 2);
	RenderTextOnScreen(meshList[GEO_TEXT], "Z:", Color(0, 1, 0), 2, 0, 2);
	RenderTextOnScreen(meshList[GEO_TEXT], FPS, Color(0, 1, 0), 2, 47, 0);
	RenderTextOnScreen(meshList[GEO_TEXT], "FPS:", Color(0, 1, 0), 2, 40, 0);

	RenderTextOnScreen(meshList[GEO_TEXT], std::to_string(playerhealth), Color(0, 1, 0), 2, 15, 50);
	RenderTextOnScreen(meshList[GEO_TEXT], "Health:", Color(0, 1, 0), 2, 0, 50);

}

void SP::Exit()
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
