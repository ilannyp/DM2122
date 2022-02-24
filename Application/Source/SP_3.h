#ifndef SP_3_H
#define SP_3_H
#pragma once
#include "Scene.h"
#include "Camera_SP_LVL3.h"
#include "Camera2.h"
#include "Mesh.h"
#include "MatrixStack.h"
#include "Light.h"
#include "Player.h"


class SP_3 : public Scene
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
		GEO_BLOOD,
		GEO_COIN,
		GEO_COIN_ICON,
		GEO_SWORD,
		GEO_FIREBASKET,
		GEO_TAXI,
		GEO_FULLHEALTH,
		GEO_THREEHEALTH,
		GEO_HALFHEALTH,
		GEO_ONEHEALTH,
		GEO_ZEROHEALTH,

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
		GEO_BULLET2,
		GEO_BULLET3,
		GEO_BULLET4,
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

		//light2
		U_LIGHT2_POSITION,
		U_LIGHT2_COLOR,
		U_LIGHT2_POWER,
		U_LIGHT2_KC,
		U_LIGHT2_KL,
		U_LIGHT2_KQ,
		U_LIGHT2_TYPE,
		U_LIGHT2_SPOTDIRECTION,
		U_LIGHT2_COSCUTOFF,
		U_LIGHT2_COSINNER,
		U_LIGHT2_EXPONENT,

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


	Camera_SP_LVL3 camera;
	MS modelStack, viewStack, projectionStack;
	Light light[3];


	void RenderSkybox();
	void RenderScammer();
	void RenderFloor();
	void RenderLeftSide();
	void RenderBackSide();
	void RenderFrontSide();

	void sword1();
	void sword2();
	void sword3();
	void sword4();

	void Phase1Attack();
	void Phase2Attack();
	void Phase3Attack();
	void Phase4Attack();

	void RenderText(Mesh* mesh, std::string text, Color color);
	void RenderTextOnScreen(Mesh* mesh, std::string text, Color color, float size, float x, float y);
	void RenderMeshOnScreen(Mesh* mesh, int x, int y, int sizex, int sizey);




	//sp stuff
	bool tut_text = true, scammaer_talk = false, 
		sword1_enable = true, sword2_enable = true, sword3_enable = true, sword4_enable = true, 
		blood_ui = false,sword1_press=true, sword2_press = true, sword3_press = true, sword4_press = true,
		collect_coin=false;
	Vector3 scammer_pos = Vector3(-35, 0, -35);
	Vector3 sword1_pos = Vector3(-75, 0, 0);
	Vector3 sword2_pos = Vector3(-75, 0, 50);
	Vector3 sword3_pos = Vector3(2, 0, 30);
	Vector3 sword4_pos = Vector3(0, 0, -21);
	std::string scammer_text, sword1_text, sword2_text, sword3_text, sword4_text;
	//-------------------------------------//
	float rotateAngle;
	float translateX = 1;
	float translateZ = 1;
	float translateY;
	float scaleAll;
	float LSPEED = 10.f;

	float rotatebullet = 90;
	float phase1translationX = 15;
	float phase2translationZ = -32;
	float phase2translationZ_2 = -44;
	float phase2translationZ_3 = -56;
	float phase3translationX = -85;

	int playerhealth = 20;//slight change to make it into currency
	int enemyz = 0;//i assume this is where the arrow line -irfan
	int enemy_health = 5;
	int sword_count=4;
	bool battlestart = false;
	bool win = false;
	bool die = false;
	bool hit = false;

	Vector3 bullet1;
	Vector3 bullet2;
	Vector3 bullet3;
	Vector3 bullet4;
	Vector3 bullet5;
	Vector3 bullet6;
	Vector3 bullet7;
	Vector3 bullet8;
	Vector3 bullet9;
	Vector3 bullet10;
	Vector3 bullet11;

	Vector3 bullet12;
	Vector3 bullet13;
	Vector3 bullet14;
	Vector3 bullet15;
	Vector3 bullet16;
	Vector3 bullet17;
	Vector3 bullet18;
	Vector3 bullet19;
	Vector3 bullet20;
	Vector3 bullet21;

	Vector3 bullet22;
	Vector3 bullet23;
	Vector3 bullet24;
	Vector3 bullet25;
	Vector3 bullet26;
	Vector3 bullet27;
	Vector3 bullet28;
	Vector3 bullet29;
	Vector3 bullet30;
	Vector3 bullet31;
	Vector3 bullet32;

	Vector3 player;

public:
	SP_3();
	~SP_3();


	//Player yourself;
	virtual void Init();
	virtual void Update(double dt);
	virtual void Render();
	virtual void Exit();
	bool colisiondetectionbullets(Vector3 bullet, Vector3 player);
	std::string FPS;
	std::string camerax;
	std::string cameraz;
	bool endscreen = false;

};

#endif