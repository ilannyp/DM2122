#ifndef Scene_LV2_H
#define Scene_LV2_H
#pragma once
#include "Scene.h"
#include "Camera_SP_LVL2.h"
#include "Mesh.h"
#include "MatrixStack.h"
#include "Light.h"
#include "SP.h"
#include "Player.h"


class Scene_LV2 : public Scene , public SP
{
	enum UNIFORM_TYPE
	{
		U_MVP = 0,
		U_MODELVIEW,
		U_MODELVIEW_INVERSE_TRANSPOSE,
		U_MATERIAL_AMBIENT,
		U_MATERIAL_DIFFUSE,
		U_MATERIAL_SPECULAR,
		U_MATERIAL_SHININESS,

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
		U_LIGHTENABLED,
		U_NUMLIGHTS,
		U_TEXT_ENABLED,
		U_TEXT_COLOR,
		U_TOTAL,
	


	};
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
		GEO_COIN_ICON,
		GEO_TUNNEL,
		GEO_BLOOD,





		//***ownbuildscsammer***
		GEO_HEAD,
		GEO_TORSO,
		//***questions
		GEO_QUESTION1,
		GEO_QUESTION1_WRONG,
		GEO_QUESTION1_CORRECT,

		GEO_QUESTION2,
		GEO_QUESTION2_WRONG,
		GEO_QUESTION2_CORRECT,

		GEO_QUESTION3,
		GEO_QUESTION3_WRONG,
		GEO_QUESTION3_CORRECT,

		GEO_QUESTION4,
		GEO_QUESTION4_WRONG,
		GEO_QUESTION4_CORRECT,


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
	
private:
	//sp stuff
	bool tut_text=true;
	Vector3 scammer_pos = Vector3(0, 0, 0);
	
	//movin npc
	float translatenpc;
	float npc1x;
	float npc1z;
	int dir1 = 1;

	float translatenpc2;
	float npc2x;
	float npc2z;
	int dir2 = 1;

	float translatenpc3;
	float npc3x;
	float npc3z;
	int dir3 = 1;

	float translatenpc4;
	float npc4x;
	float npc4z;
	int dir4 = 1;

	//-------------------------------------//
	float rotateAngle;
	float translateX = 1;
	float translateZ = 1;
	float translateY;
	float scaleAll;
	float LSPEED = 10.f;




	
	
	int playerhealth = 20;//slight change to make it into currency
	int enemyz = 0;//i assume this is where the arrow line
	bool battlestart = false;
	bool win = false;
	bool die = false;
	bool hit = false;

	//npc spawn
	bool interactnpc1;
	int intnpc1;
	bool interactnpc2;
	int intnpc2;
	bool interactnpc3;
	int intnpc3;
	bool interactnpc4;
	int intnpc4;

	//bullets vectors
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


		
	Vector3 player;

	//bullets translation
	float translation_x_bullet;//speed 45
	float translation_x_bullet_slow;//speed 10
	float translation_x_bullet_fast;//speed 60
	float translation_x_bullet_medium;//speed 30



	Light light[2];
	void RenderMesh(Mesh* mesh, bool enableLight);
	void RenderSkybox();
	void RenderFloor();
	void RenderRightSide();
	void RenderLeftSide();
	void RenderBackSide();
	void RenderFrontSide();
	void RenderRoads();
	void RenderLamps();

	
	//**sp**
	void RenderWall();
	void RenderTunnel();
	void RenderPavement();
	void RenderNPC();
	void RenderBullets();
	bool colisiondetectionbullets(Vector3 bullet, Vector3 player);
	void RenderQuestion();
	bool onpavement(int pavementint);
	bool bloodui;

	void RenderText(Mesh* mesh, std::string text, Color color);
	void RenderTextOnScreen(Mesh* mesh, std::string text, Color color, float size, float x, float y);
	unsigned m_vertexArrayID;
	unsigned m_programID;
	unsigned m_parameters[U_TOTAL];
	Mesh* meshList[NUM_GEOMETRY];
	Camera_SP_LVL2 camera;
	MS modelStack, viewStack, projectionStack;

public:
	Scene_LV2();
	~Scene_LV2();
	

	virtual void Init();
	virtual void Update(double dt);
	virtual void Render();
	virtual void Exit();
	std::string FPS;
	std::string camerax;
	std::string cameraz;
	bool endscreen = false;
	
	void RenderMeshOnScreen(Mesh* mesh, int x, int y, int sizex, int sizey);
	Vector3 tp_sphere = Vector3 (-33, 0, 131);
};

#endif