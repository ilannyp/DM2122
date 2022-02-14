#ifndef SP_START_H
#define SP_START_H
#pragma once
#include "Scene.h"
#include "Camera3.h"
#include "Camera2.h"
#include "Camera_Start.h"
#include "Mesh.h"
#include "MatrixStack.h"
#include "Light.h"


class SP_Start : public Scene
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
		GEO_QUAD,
		GEO_BLEND,
		GEO_LIGHTBALL,
		GEO_TEXT,
		NUM_GEOMETRY,
	};
	
private:
	
	float LSPEED = 10.f;

	Light light[1];
	void RenderMesh(Mesh* mesh, bool enableLight);
	void RenderText(Mesh* mesh, std::string text, Color color);
	void RenderTextOnScreen(Mesh* mesh, std::string text, Color color, float size, float x, float y);
	void RenderMeshOnScreen(Mesh* mesh, int x, int y, int sizex, int sizey);
	unsigned m_vertexArrayID;
	unsigned m_programID;
	unsigned m_parameters[U_TOTAL];
	Mesh* meshList[NUM_GEOMETRY];
	Camera_Start camera;
	MS modelStack, viewStack, projectionStack;
public:
	SP_Start();
	~SP_Start();


	virtual void Init();
	virtual void Update(double dt);
	virtual void Render();
	virtual void Exit();
	std::string FPS;
};

#endif