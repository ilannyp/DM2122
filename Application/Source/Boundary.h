#pragma once
#include "Vector3.h"

class Boundary
{
private:
	int cameraXHead;		//head
	int cameraXTail;		//tail
	int cameraYHead;		//head
	int cameraYTail;		//tail
	int cameraZHead;		//head
	int cameraZTail;		//tail

public:
	Boundary();
	~Boundary();
	void setCameraXHead(int CameraXHead);
	int getCameraXHead(void);
	void setCameraXTail(int CameraXTail);
	int getCameraXTail(void);

	void setCameraYHead(int CameraYHead);
	int getCameraYHead(void);
	void setCameraYTail(int CameraYTail);
	int getCameraYTail(void);

	void setCameraZHead(int CameraZHead);
	int getCameraZHead(void);
	void setCameraZTail(int CameraZTail);
	int getCameraZTail(void);

	void collision();

	
};

