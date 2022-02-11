#pragma once
#include "Vertex.h"

struct Light
{

	Position position;
	Color color;
	float power;
	float kC, kL, kQ;
	Light() { color.Set(1, 1, 1); power = 1.f; kC = 1.f; kL = 0; kQ = 0; };

	//add these after existing parameters 
	enum LIGHT_TYPE
	{
		LIGHT_POINT = 0,
		LIGHT_DIRECTIONAL,
		LIGHT_SPOT,
	};
	LIGHT_TYPE type;
	Vector3 spotDirection;
	float cosCutoff;
	float cosInner;
	float exponent;
};
