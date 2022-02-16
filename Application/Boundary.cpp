#include "Boundary.h"

Boundary::Boundary()
{
    cameraXHead = 1;
    cameraXTail = 1;
    cameraYHead = 1;
    cameraYTail = 1;
    cameraZHead = 1;
    cameraZTail = 1;
}

Boundary::~Boundary()
{
}

void Boundary::setCameraXHead(int CameraXHead)
{
    this->cameraXHead = CameraXHead;
}

int Boundary::getCameraXHead(void)
{
    return cameraXHead;
}

void Boundary::setCameraXTail(int CameraXTail)
{
    this->cameraXTail = CameraXTail;
}

int Boundary::getCameraXTail(void)
{
    return cameraXTail;
}

void Boundary::setCameraYHead(int CameraYHead)
{
    this->cameraYHead = CameraYHead;
}

int Boundary::getCameraYHead(void)
{
    return cameraYHead;
}

void Boundary::setCameraYTail(int CameraYTail)
{
    this->cameraYTail = CameraYTail;
}

int Boundary::getCameraYTail(void)
{
    return cameraYTail;
}

void Boundary::setCameraZHead(int CameraZHead)
{
    this->cameraZHead = CameraZHead;
}

int Boundary::getCameraZHead(void)
{
    return cameraZHead;
}

void Boundary::setCameraZTail(int CameraZTail)
{
    this->cameraZTail = CameraZTail;
}

int Boundary::getCameraZTail(void)
{
    return cameraZTail;
}

void Boundary::collision()
{
}
