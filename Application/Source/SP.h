#ifndef SP_H
#define SP_H
#pragma once
#include "Scene.h"
#include "Camera3.h"
#include "Camera2.h"
#include "Mesh.h"
#include "MatrixStack.h"
#include "Light.h"


class SP : public Scene
{
	enum GEOMETRY_TYPE
	{
		GEO_TRIANGLE_1 = 0,
		GEO_AXES,
		GEO_QUAD,
		GEO_CUBE,
		GEO_CIRCLE,
		GEO_SPHERE,
		GEO_LAMPLIGHT,
		GEO_LIGHTBALL,
		GEO_CYLINDER,
		GEO_CAPE,
		GEO_CONE,
		GEO_HEMISPHERE,
		GEO_TORUS,
		GEO_HALFTORUS,
		GEO_LEFT,
		GEO_RIGHT,
		GEO_TOP,
		GEO_BOTTOM,
		GEO_FRONT,
		GEO_BACK,
		GEO_BLEND,
		//**********SP************
		GEO_SCAMMER,

		//*********obj models*********
		GEO_HOLLOW,
		GEO_HORNET,
		GEO_ROCKS,
		GEO_TALLROCK,
		GEO_STONEWALL,
		GEO_STONEWALLDAMAGED,
		GEO_CRYPT,
		GEO_ROAD,
		GEO_DEBRIS,
		GEO_LAMP,
		GEO_GRAVE,
		GEO_LANTERN,
		GEO_BULLET,
		GEO_TEXT,
		GEO_TEXT2,






		NUM_GEOMETRY,

	};
	enum UNIFORM_TYPE
	{
		U_MVP = 0,
		U_MODELVIEW,
		U_MODELVIEW_INVERSE_TRANSPOSE,
		U_MATERIAL_AMBIENT,
		U_MATERIAL_DIFFUSE,
		U_MATERIAL_SPECULAR,
		U_MATERIAL_SHININESS,
		U_LIGHTENABLED,
		U_NUMLIGHTS,


		//light0
		U_LIGHT0_POSITION,
		U_LIGHT0_COLOR,
		U_LIGHT0_POWER,
		U_LIGHT0_KC,
		U_LIGHT0_KL,
		U_LIGHT0_KQ,
		U_LIGHT0_TYPE,
		U_LIGHT0_SPOTDIRECTION,
		U_LIGHT0_COSCUTOFF,
		U_LIGHT0_COSINNER,
		U_LIGHT0_EXPONENT,

		//light1
		U_LIGHT1_POSITION,
		U_LIGHT1_COLOR,
		U_LIGHT1_POWER,
		U_LIGHT1_KC,
		U_LIGHT1_KL,
		U_LIGHT1_KQ,
		U_LIGHT1_TYPE,
		U_LIGHT1_SPOTDIRECTION,
		U_LIGHT1_COSCUTOFF,
		U_LIGHT1_COSINNER,
		U_LIGHT1_EXPONENT,

		U_COLOR_TEXTURE_ENABLED,
		U_COLOR_TEXTURE,
		
		
		U_TEXT_ENABLED,
		U_TEXT_COLOR,
		U_TOTAL,
	


	};
	
	
private:
	void RenderMesh(Mesh* mesh, bool enableLight);
	unsigned m_vertexArrayID;
	Mesh* meshList[NUM_GEOMETRY];


	unsigned m_programID;
	unsigned m_parameters[U_TOTAL];


	Camera3 camera;
	MS modelStack, viewStack, projectionStack;
	Light light[2];

	
	void RenderSkybox();
	void RenderScammer();
	void RenderScammer_E();
	void RenderFloor();
	void RenderRightSide();
	void RenderLeftSide();
	void RenderBackSide();
	void RenderFrontSide();
	void RenderRoads();
	void RenderBullet();
	void RenderLamps();

	void RenderText(Mesh* mesh, std::string text, Color color);
	void RenderTextOnScreen(Mesh* mesh, std::string text, Color color, float size, float x, float y);
	



	//sp stuff
	bool tut_text = true;

	//-------------------------------------//
	float rotateAngle;
	float translateX = 1;
	float translateZ = 1;
	float translateY;
	float scaleAll;
	float LSPEED = 10.f;






	int playerhealth = 20;//slight change to make it into currency
	int enemyz = 0;//i assume this is where the arrow line -irfan
	bool battlestart = false;
	bool win = false;
	bool die = false;
	bool hit = false;

		


	Vector3 bullet;
	Vector3 bullet2;
	Vector3 bullet3;
	Vector3 bullet4;
	Vector3 bullet5;
	Vector3 bullet6;
	Vector3 bullet7;
	Vector3 bullet8;
	

	
	
	
public:
	SP();
	~SP();
	

	virtual void Init();
	virtual void Update(double dt);
	virtual void Render();
	virtual void Exit();
	std::string FPS;
	std::string camerax;
	std::string cameraz;
	bool endscreen = false;
	

};

#endif