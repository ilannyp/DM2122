#ifndef SCENE_4_H
#define SCENE_4_H
#pragma once
#include "Scene.h"
#include "Camera.h"
#include "Mesh.h"

class Scene4 : public Scene
{
	enum UNIFORM_TYPE
	{
		U_MVP = 0,
		U_TOTAL,

	};
	enum GEOMETRY_TYPE
	{
		GEO_TRIANGLE_1 = 0,
		GEO_AXES,
		GEO_QUAD,
		GEO_CUBE,
		NUM_GEOMETRY,
		
	};
	
private:
	float rotateAngle;
	float translateX;
	float scaleAll;

	
	unsigned m_vertexArrayID;
	unsigned m_programID;
	unsigned m_parameters[U_TOTAL];
	Mesh* meshList[NUM_GEOMETRY];
	Camera camera;
public:
	Scene4();
	~Scene4();


	virtual void Init();
	virtual void Update(double dt);
	virtual void Render();
	virtual void Exit();
};

#endif