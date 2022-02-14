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
	enum UNIFORM_TYPE
	{
		U_MVP = 0,
		U_MODELVIEW,
		U_MODELVIEW_INVERSE_TRANSPOSE,
		U_MATERIAL_AMBIENT,
		U_MATERIAL_DIFFUSE,
		U_MATERIAL_SPECULAR,
		U_MATERIAL_SHININESS,

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
		GEO_BODY,
		GEO_SPHERE2,
		GEO_LAMPLIGHT,
		GEO_LIGHTBALL,
		GEO_CYLINDER,
		GEO_CAPE,
		GEO_CONE,
		GEO_HEMISPHERE,
		GEO_HEMISPHERE2,
		GEO_HEMISPHERE3,
		GEO_TORUS,
		GEO_HALFTORUS,
		GEO_SWORD,
		GEO_LEFT,
		GEO_RIGHT,
		GEO_TOP,
		GEO_BOTTOM,
		GEO_FRONT,
		GEO_BACK,
		GEO_BLEND,

	//*********obj models*********
		GEO_HOLLOW,
		GEO_HORNET,
		GEO_BENCH,
		GEO_ROCKS,
		GEO_TALLROCK,
		GEO_STONEWALL,
		GEO_BROKENBENCH,
		GEO_STONEWALLDAMAGED,
		GEO_CRYPT,
		GEO_ROAD,
		GEO_DEBRIS,
		GEO_LAMP,
		GEO_GRAVE,
		GEO_LANTERN,
		GEO_BULLET,
		GEO_FIREBASKET,
		GEO_TEXT,
		GEO_TEXT2,
		NUM_GEOMETRY,
		
	};
	
private:
	//sp stuff
	bool tut_text=true;

	//-------------------------------------//
	float rotateAngle;
	float translateX = 1;
	float translateZ = 1;
	float translateY;
	float scaleAll;
	float LSPEED = 10.f;


	int spellbodyUp = 0;
	int spellheadrotate = 1;
	int spelllegrotate = 1;
	int spellarmrotate = 1;

	int spellcaperotate = 1;
	int spellcaperotate2 = 1;
	int spellcaperotate3 = 1;
	int spellcaperotate4 = 1;
	int spellcaperotate5 = 1;
	int spellcaperotate6 = 1;

	int reversespellcaperotate = 1;
	int reversespellcaperotate2 = 1;
	int reversespellcaperotate3 = 1;
	int reversespellcaperotate4 = 1;
	int reversespellcaperotate5 = 1;
	int reversespellcaperotate6 = 1;

	int spellcapetranslateZ = 0;
	int spellcapetranslateX = 0;
	int spellcapetranslateY = 0;

	int spellswordtranslate = 0;

	int spellarmtranslateX = 0;
	int spelllegTranslateY = 0;
	int spellarmTranslateZ = 0;
	int spelllegTranslateZ = 0;


	int swordTranslateX = 0;
	int swordTranslateY = 0;
	int swordTranslateZ = 0;
	int armTranslateX = 0;
	int armTranslateY = 0;
	int armTranslateZ = 0;
	int firstlegRotate = 1;
	int secondlegRotate = 1;
	int reverseRotate = 1;
	int reverseRotateLeg = 1;
	int reverseRotateLeg2 = -1;
	int reverseRotateLeg3 = 1;
	int reverseRotateLeg4 = -1;
	int rotateBody = 1;
	int swordRotate = 1;
	int swordRotate2 = 1;
	int spellx = 1;
	int spelly = -40;
	int spellz = 20;
	float timer;
	//for bench
	int isonbench = 0;
	bool issitting = false;
	bool isclosetobench = false;
	bool benchinteract = false;
	bool benchcam;
	int benchposx = 0;
	int benchposz = 0;
	
	
	//for corpse npc
	bool isclosetonpc = false;
	bool npcinteract = false;
	int npcx = 0;
	int npcz = 0;

	int characterRotate;
	//Interaction for battle
	bool isclosetobasket;
	bool basketinteract;
	int basketx;
	int basketz;
	int playerhealth = 20;
	int enemyhealth = 100;
	int enemyx = 0;
	int enemyz = 0;
	bool battlestart = false;
	bool win = false;
	bool die = false;
	bool hit = false;
	int npchealth;
	
	int translatebasketz = 0;

	Vector3 bullet;
	Vector3 bullet2;
	Vector3 bullet3;
	Vector3 bullet4;
	Vector3 bullet5;
	Vector3 bullet6;
	Vector3 bullet7;
	Vector3 bullet8;
	Vector3 spell;
	Light light[3];
	void RenderMesh(Mesh* mesh, bool enableLight);
	void RenderSkybox();
	void RenderText(Mesh* mesh, std::string text, Color color);
	void RenderTextOnScreen(Mesh* mesh, std::string text, Color color, float size, float x, float y);
	unsigned m_vertexArrayID;
	unsigned m_programID;
	unsigned m_parameters[U_TOTAL];
	Mesh* meshList[NUM_GEOMETRY];
	Camera3 camera;
	Camera2 benchcamera;
	MS modelStack, viewStack, projectionStack;
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