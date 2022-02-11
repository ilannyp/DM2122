#ifndef ASSIGNMENT1_H
#define ASSIGNMENT1_H
#pragma once
#include "Scene.h"
#include "Camera2.h"
#include "Mesh.h"
#include "MatrixStack.h"
#include "Light.h"

class Assignment1 : public Scene
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

		U_LIGHTENABLED,
		U_NUMLIGHTS,
		U_TOTAL,

	};
	enum GEOMETRY_TYPE
	{
		GEO_TRIANGLE_1 = 0,
		GEO_AXES,
		GEO_QUAD,
		GEO_CUBE,
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
		NUM_GEOMETRY,
		
	};
	
private:
	float rotateAngle;
	float translateX = 1;
	float translateZ = 1;
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

	Light light[1];
	void RenderMesh(Mesh* mesh, bool enableLight);
	unsigned m_vertexArrayID;
	unsigned m_programID;
	unsigned m_parameters[U_TOTAL];
	Mesh* meshList[NUM_GEOMETRY];
	Camera2 camera;
	MS modelStack, viewStack, projectionStack;
public:
	Assignment1();
	~Assignment1();


	virtual void Init();
	virtual void Update(double dt);
	virtual void Render();
	virtual void Exit();
};

#endif