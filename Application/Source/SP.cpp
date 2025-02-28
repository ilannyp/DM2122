#include "SP.h"
#include "GL\glew.h"
#include "Application.h"
#include "shader.hpp"
#include "Mtx44.h"
#include "MeshBuilder.h"
#include "Utility.h"
#include "LoadTGA.h"
#include "Light.h"
#include <sstream>

SP::SP()
{

}

SP::~SP()
{
}

void SP::RenderMeshOnScreen(Mesh* mesh, int x, int y, int sizex, int sizey)
{
	glDisable(GL_DEPTH_TEST);
	Mtx44 ortho;
	ortho.SetToOrtho(0, 80, 0, 60, -10, 10); //size of screen UI
	projectionStack.PushMatrix();
	projectionStack.LoadMatrix(ortho);
	viewStack.PushMatrix();
	viewStack.LoadIdentity(); //No need camera for ortho mode
	modelStack.PushMatrix();
	modelStack.LoadIdentity();
	//to do: scale and translate accordingly
	modelStack.Translate(x, y, 0);
	modelStack.Scale(sizex, sizey, 1);
	RenderMesh(mesh, false); //UI should not have light
	projectionStack.PopMatrix();
	viewStack.PopMatrix();
	modelStack.PopMatrix();
	glEnable(GL_DEPTH_TEST);
}

void SP::RenderQuest()
{
	if(intquest == 1)
		RenderTextOnScreen(meshList[GEO_TEXT],"Interact with the man", Color(1, 1, 1) ,2, 2, 45);
	if (intquest == 3)
		RenderTextOnScreen(meshList[GEO_TEXT], "Give the coins to the person", Color(1, 1, 1), 2, 2, 43);
	if (intquest == 4)
		RenderTextOnScreen(meshList[GEO_TEXT], "Get to the taxi, Carefull!", Color(1, 1, 1), 2, 2, 45);



}

void SP::RenderPress_E()
{
	if (coin1dis <= 10 && !coin1_collected)
	{
		modelStack.PushMatrix();
		modelStack.Translate(coin1_pos.x - 1, coin1_pos.y + 1, coin1_pos.z - 2);
		modelStack.Rotate(-90, 0, 1, 0);
		modelStack.Scale(0.2f, 0.2f, 0.2f);
		RenderText(meshList[GEO_TEXT],"Press E to collect", Color(1, 1, 1));
		modelStack.PopMatrix();
	}

	if (coin2dis <= 12 && !coin2_collected)
	{
		modelStack.PushMatrix();
		modelStack.Translate(coin2_pos.x - 2, coin2_pos.y + 1, coin2_pos.z - 3);
		modelStack.Rotate(-100, 0, 1, 0);
		modelStack.Scale(0.2f, 0.2f, 0.2f);
		RenderText(meshList[GEO_TEXT], "Press E to collect", Color(1, 1, 1));
		modelStack.PopMatrix();
	}

	if (coin3dis <= 10 && !coin3_collected)
	{
		modelStack.PushMatrix();
		
		modelStack.Translate(coin3_pos.x - 1 , coin3_pos.y + 1, coin3_pos.z -2);
		modelStack.Rotate(-70, 0, 1, 0);
		modelStack.Scale(0.2f, 0.2f, 0.2f);
		RenderText(meshList[GEO_TEXT], "Press E to collect", Color(1, 1, 1));
		modelStack.PopMatrix();
	}
	if (scammerdis <= 10 && intquest == 3)
	{
		modelStack.PushMatrix();
		modelStack.Translate(-16.f,2.5f,83.f);
		modelStack.Rotate(90, 0, 1, 0);
		modelStack.Scale(0.2f, 0.2f, 0.2f);
		RenderText(meshList[GEO_TEXT], "Press E to give the coins", Color(1, 1, 1));
		modelStack.PopMatrix();
	}
	if (cabTP() <= 10)
	{
		modelStack.PushMatrix();
		modelStack.Translate(cab_pos.x - 2,cab_pos.y+3, cab_pos.z + 4);
		modelStack.Rotate(0, 0, 1, 0);
		modelStack.Scale(0.2f, 0.2f, 0.2f);
		RenderText(meshList[GEO_TEXT], "Press E to ride", Color(1, 1, 1));
		modelStack.PopMatrix();
	}
}

void SP::RenderMesh(Mesh* mesh, bool enableLight)
{
	Mtx44 MVP, modelView, modelView_inverse_transpose;

	MVP = projectionStack.Top() * viewStack.Top() * modelStack.Top();
	glUniformMatrix4fv(m_parameters[U_MVP], 1, GL_FALSE, &MVP.a[0]);
	modelView = viewStack.Top() * modelStack.Top();
	glUniformMatrix4fv(m_parameters[U_MODELVIEW], 1, GL_FALSE, &modelView.a[0]);
	if (enableLight)
	{
		glUniform1i(m_parameters[U_LIGHTENABLED], 1);
		modelView_inverse_transpose = modelView.GetInverse().GetTranspose();
		glUniformMatrix4fv(m_parameters[U_MODELVIEW_INVERSE_TRANSPOSE], 1, GL_FALSE, &modelView_inverse_transpose.a[0]);

		//load material
		glUniform3fv(m_parameters[U_MATERIAL_AMBIENT], 1, &mesh->material.kAmbient.r);
		glUniform3fv(m_parameters[U_MATERIAL_DIFFUSE], 1, &mesh->material.kDiffuse.r);
		glUniform3fv(m_parameters[U_MATERIAL_SPECULAR], 1, &mesh->material.kSpecular.r);
		glUniform1f(m_parameters[U_MATERIAL_SHININESS], mesh->material.kShininess);
	}
	else
	{
		glUniform1i(m_parameters[U_LIGHTENABLED], 0);
	}
	if (mesh->textureID > 0)
	{
		glUniform1i(m_parameters[U_COLOR_TEXTURE_ENABLED], 1);
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, mesh->textureID);
		glUniform1i(m_parameters[U_COLOR_TEXTURE], 0);
	}
	else
	{
		glUniform1i(m_parameters[U_COLOR_TEXTURE_ENABLED], 0);
	}
	mesh->Render();
	if (mesh->textureID > 0)
	{
		glBindTexture(GL_TEXTURE_2D, 0);
	}
}

void SP::RenderSkybox()
{
	const float OFFSET = 499;

	modelStack.PushMatrix();
	//to do: transformation code here
	modelStack.Translate(0 + camera.position.x, 0 + camera.position.y, -OFFSET + camera.position.z);
	//modelStack.Rotate(180, 0, 0, 1);
	modelStack.Scale(1000, 1000, 1000);
	RenderMesh(meshList[GEO_FRONT], false);
	modelStack.PopMatrix();

	modelStack.PushMatrix();
	//to do: transformation code here
	modelStack.Translate(-OFFSET + camera.position.x, 0 + camera.position.y, 0 + camera.position.z);
	modelStack.Rotate(90, 0, 1, 0);
	//modelStack.Rotate(-180, 0, 0, 1);
	modelStack.Scale(1000, 1000, 1000);
	RenderMesh(meshList[GEO_RIGHT], false);
	modelStack.PopMatrix();

	modelStack.PushMatrix();
	//to do: transformation code here
	modelStack.Translate(OFFSET + camera.position.x, 0 + camera.position.y, 0.25 + camera.position.z);
	modelStack.Rotate(270, 0, 1, 0);
	//modelStack.Rotate(180, 0, 0, 1);
	modelStack.Scale(1000, 1000, 1000);
	RenderMesh(meshList[GEO_LEFT], false);
	modelStack.PopMatrix();

	modelStack.PushMatrix();
	//to do: transformation code here
	modelStack.Translate(0 + camera.position.x, 0 + camera.position.y, OFFSET + camera.position.z);
	modelStack.Rotate(180, 0, 1, 0);
	//modelStack.Rotate(180, 0, 0, 1);
	modelStack.Scale(1000, 1000, 1000);
	RenderMesh(meshList[GEO_BACK], false);
	modelStack.PopMatrix();

	modelStack.PushMatrix();
	//to do: transformation code here
	modelStack.Translate(0 + camera.position.x, OFFSET + camera.position.y, 0 + camera.position.z);
	modelStack.Rotate(90, 1, 0, 0);
	modelStack.Rotate(270, 0, 0, 1);
	modelStack.Scale(1000, 1000, 1000);
	RenderMesh(meshList[GEO_TOP], false);
	modelStack.PopMatrix();

	modelStack.PushMatrix();
	//to do: transformation code here
	modelStack.Translate(0 + camera.position.x, -OFFSET + camera.position.y, 0 + camera.position.z);
	modelStack.Rotate(-90, 1, 0, 0);
	modelStack.Rotate(-90, 0, 0, 1);
	modelStack.Scale(1000, 1000, 1000);
	RenderMesh(meshList[GEO_BOTTOM], false);
	modelStack.PopMatrix();
}

void SP::RenderFloor()
{
	modelStack.PushMatrix();
	modelStack.Translate(0, 0, 0);
	modelStack.Rotate(-90, 1, 0, 0);
	modelStack.Rotate(0, 0, 1, 0);
	modelStack.Scale(1000, 1000, 1000);
	RenderMesh(meshList[GEO_QUAD], true);
	modelStack.PopMatrix();

	modelStack.PushMatrix();
	modelStack.Translate(-59.f, 0.01f, 66.f);
	modelStack.Scale(53.f, 0.1f, 100.f);
	RenderMesh(meshList[GEO_ROAD_FOR_BULLETS], true);
	modelStack.PopMatrix();

	modelStack.PushMatrix();
	modelStack.Translate(-100.f, 0.01f, 66.f);
	modelStack.Scale(15.f, 0.1f, 100.f);
	RenderMesh(meshList[GEO_ROAD_FOR_BULLETS], true);
	modelStack.PopMatrix();


}

void SP::RenderRightSide()
{
	//right side
	modelStack.PushMatrix();
	modelStack.Translate(-30, 0, 0);
	modelStack.Rotate(0, 0, 0, 1);
	modelStack.Scale(50, 50, 50);
	RenderMesh(meshList[GEO_ROCKS], true);
	modelStack.PopMatrix();
	
	modelStack.PushMatrix();
	modelStack.Translate(-20, 0, 35);
	modelStack.Rotate(90, 0, 1, 0);
	modelStack.Scale(60, 30, 30);
	RenderMesh(meshList[GEO_TALLROCK], true);
	modelStack.PopMatrix();
	/*
	modelStack.PushMatrix();
	modelStack.Translate(-16, 0, 67);
	modelStack.Rotate(0, 0, 1, 0);
	modelStack.Scale(10, 7, 10);
	RenderMesh(meshList[GEO_GRAVE], true);
	modelStack.PopMatrix();*/

	/*modelStack.PushMatrix();
	modelStack.Translate(-24, 0, 73);
	modelStack.Rotate(0, 0, 1, 0);
	modelStack.Scale(10, 10, 10);
	RenderMesh(meshList[GEO_LANTERN], true);
	modelStack.PopMatrix();*/

	modelStack.PushMatrix();
	modelStack.Translate(cab_pos.x, cab_pos.y, cab_pos.z);
	//modelStack.Translate(-100, 0, 22);
	modelStack.Rotate(180, 0, 1, 0);
	modelStack.Scale(1, 1, 1);
	RenderMesh(meshList[GEO_TAXI], true);
	modelStack.PopMatrix();


}
void SP::RenderLeftSide()
{
	//left side
	modelStack.PushMatrix();
	modelStack.Translate(40, 0, 0);
	modelStack.Rotate(90, 0, 1, 0);
	modelStack.Scale(50, 50, 50);
	RenderMesh(meshList[GEO_ROCKS], true);
	modelStack.PopMatrix();

	modelStack.PushMatrix();
	modelStack.Translate(40, 0, 60);
	modelStack.Rotate(270, 0, 1, 0);
	modelStack.Scale(100, 40, 40);
	RenderMesh(meshList[GEO_TALLROCK], true);
	modelStack.PopMatrix();

	modelStack.PushMatrix();
	modelStack.Translate(30, 0, -60);
	modelStack.Rotate(270, 0, 1, 0);
	modelStack.Rotate(10, 0, 1, 0);
	modelStack.Scale(100, 40, 40);
	RenderMesh(meshList[GEO_TALLROCK], true);
	modelStack.PopMatrix();

	modelStack.PushMatrix();
	modelStack.Translate(24, 0, -18);
	modelStack.Rotate(95, 0, 1, 0);
	modelStack.Scale(30, 30, 30);
	RenderMesh(meshList[GEO_DEBRIS], true);
	modelStack.PopMatrix();

	/*modelStack.PushMatrix();
	modelStack.Translate(25, 0, 55);
	modelStack.Rotate(0, 0, 1, 0);
	modelStack.Scale(10, 7, 10);
	RenderMesh(meshList[GEO_CRYPT], true);
	modelStack.PopMatrix();

	modelStack.PushMatrix();
	modelStack.Translate(25, 0, 35);
	modelStack.Rotate(0, 0, 1, 0);
	modelStack.Scale(10, 7, 10);
	RenderMesh(meshList[GEO_CRYPT], true);
	modelStack.PopMatrix();*/

	modelStack.PushMatrix();
	modelStack.Translate(35, 0, 25);
	modelStack.Rotate(90, 0, 1, 0);
	modelStack.Scale(180, 10, 10);
	RenderMesh(meshList[GEO_STONEWALL], true);
	modelStack.PopMatrix();


	modelStack.PushMatrix();
	modelStack.Translate(7, 0, 45);
	modelStack.Rotate(0, 0, 1, 0);
	modelStack.Scale(10, 10, 10);
	RenderMesh(meshList[GEO_LAMP], true);
	modelStack.PopMatrix();



}
void SP::RenderBackSide()
{
	//back side
	/*modelStack.PushMatrix();
	modelStack.Translate(0, 0, -81);
	modelStack.Rotate(180, 0, 1, 0);
	modelStack.Scale(50, 50, 50);
	RenderMesh(meshList[GEO_TALLROCK], true);
	modelStack.PopMatrix();*/

	modelStack.PushMatrix();
	modelStack.Translate(-25, 0, -59);
	modelStack.Rotate(0, 0, 1, 0);
	modelStack.Scale(158, 10, 10);
	RenderMesh(meshList[GEO_STONEWALL], true);
	modelStack.PopMatrix();
}
void SP::RenderFrontSide()
{
	//in front
	modelStack.PushMatrix();
	modelStack.Translate(-0, 0, 110);
	modelStack.Rotate(0, 0, 1, 0);
	modelStack.Scale(100, 10, 10);
	RenderMesh(meshList[GEO_STONEWALL], true);
	modelStack.PopMatrix();

	modelStack.PushMatrix();
	modelStack.Translate(-60, 0, 110);
	modelStack.Rotate(0, 0, 1, 0);
	modelStack.Scale(100, 10, 10);
	RenderMesh(meshList[GEO_STONEWALL], true);
	modelStack.PopMatrix();

	modelStack.PushMatrix();
	modelStack.Translate(-100, 0, 60);
	modelStack.Rotate(90, 0, 1, 0);
	modelStack.Scale(100, 10, 10);
	RenderMesh(meshList[GEO_STONEWALL], true);
	modelStack.PopMatrix();

	modelStack.PushMatrix();
	modelStack.Translate(-15, 0, 65);
	modelStack.Rotate(90, 0, 1, 0);
	modelStack.Scale(20, 10, 10);
	RenderMesh(meshList[GEO_STONEWALLDAMAGED], true);
	modelStack.PopMatrix();

	modelStack.PushMatrix();
	modelStack.Translate(-20, 0, 95);
	modelStack.Rotate(270, 0, 1, 0);
	modelStack.Scale(20, 10, 10);
	RenderMesh(meshList[GEO_STONEWALLDAMAGED], true);
	modelStack.PopMatrix();

	modelStack.PushMatrix();
	modelStack.Translate(-70, 0, 20);
	modelStack.Rotate(0, 0, 1, 0);
	modelStack.Scale(100, 10, 10);
	RenderMesh(meshList[GEO_STONEWALL], true);
	modelStack.PopMatrix();

	//gold area wall connecting rock and horizontal wall
	modelStack.PushMatrix();
	modelStack.Translate(-15.4, -0.1, -14);
	modelStack.Rotate(90, 0, 1, 0);
	modelStack.Scale(20, 10, 10);
	RenderMesh(meshList[GEO_STONEWALLDAMAGED], true);
	modelStack.PopMatrix();

	//gold area horizontal wall near rock
	modelStack.PushMatrix();
	modelStack.Translate(-70, 0, -19);
	modelStack.Rotate(0, 0, 1, 0);
	modelStack.Scale(100, 10, 10);
	RenderMesh(meshList[GEO_STONEWALL], true);
	modelStack.PopMatrix();

	//gold area vertical wall near rock
	modelStack.PushMatrix();
	modelStack.Translate(-100, 0, -18);
	modelStack.Rotate(90, 0, 1, 0);
	modelStack.Scale(100, 10, 10);
	RenderMesh(meshList[GEO_STONEWALL], true);
	modelStack.PopMatrix();

	//tunnel for the taxi
	modelStack.PushMatrix();
	modelStack.Translate(-98.f, 0.f, 16.1f);
	modelStack.Scale(21, 25, 1);
	RenderMesh(meshList[GEO_TUNNEL], true);
	modelStack.PopMatrix();

}
void SP::RenderPath()
{

	//roads
	modelStack.PushMatrix();
	modelStack.Translate(0, 0, 4);
	modelStack.Rotate(90, 0, 1, 0);
	modelStack.Scale(20, 5, 5);
	RenderMesh(meshList[GEO_ROAD], true);
	modelStack.PopMatrix();

	modelStack.PushMatrix();
	modelStack.Translate(0, 0, 18);
	modelStack.Rotate(90, 0, 1, 0);
	modelStack.Scale(20, 5, 5);
	RenderMesh(meshList[GEO_ROAD], true);
	modelStack.PopMatrix();

	modelStack.PushMatrix();
	modelStack.Translate(0, 0, 32);
	modelStack.Rotate(90, 0, 1, 0);
	modelStack.Scale(20, 5, 5);
	RenderMesh(meshList[GEO_ROAD], true);
	modelStack.PopMatrix();

	modelStack.PushMatrix();
	modelStack.Translate(0, 0, 46);
	modelStack.Rotate(90, 0, 1, 0);
	modelStack.Scale(20, 5, 5);
	RenderMesh(meshList[GEO_ROAD], true);
	modelStack.PopMatrix();


	modelStack.PushMatrix();
	modelStack.Translate(0, 0, 60);
	modelStack.Rotate(90, 0, 1, 0);
	modelStack.Scale(20, 5, 5);
	RenderMesh(meshList[GEO_ROAD], true);
	modelStack.PopMatrix();

	modelStack.PushMatrix();
	modelStack.Translate(0, 0, 74);
	modelStack.Rotate(90, 0, 1, 0);
	modelStack.Scale(20, 5, 5);
	RenderMesh(meshList[GEO_ROAD], true);
	modelStack.PopMatrix();

	modelStack.PushMatrix();
	modelStack.Translate(-6, 0, 74);
	modelStack.Rotate(-160, 0, 1, 0);
	modelStack.Scale(20, 5, 5);
	RenderMesh(meshList[GEO_ROAD], true);
	modelStack.PopMatrix();

	modelStack.PushMatrix();
	modelStack.Translate(-10, 0, 78);
	modelStack.Rotate(-160, 0, 1, 0);
	modelStack.Scale(20, 5, 5);
	RenderMesh(meshList[GEO_ROAD], true);
	modelStack.PopMatrix();

	modelStack.PushMatrix();
	modelStack.Translate(0, 0, -8);
	modelStack.Rotate(90, 0, 1, 0);
	modelStack.Scale(20, 5, 5);
	RenderMesh(meshList[GEO_ROAD], true);
	modelStack.PopMatrix();

	modelStack.PushMatrix();
	modelStack.Translate(0, 0, -20);
	modelStack.Rotate(90, 0, 1, 0);
	modelStack.Scale(20, 5, 5);
	RenderMesh(meshList[GEO_ROAD], true);
	modelStack.PopMatrix();

	modelStack.PushMatrix();
	modelStack.Translate(0, 0, -32);
	modelStack.Rotate(90, 0, 1, 0);
	modelStack.Scale(20, 5, 5);
	RenderMesh(meshList[GEO_ROAD], true);
	modelStack.PopMatrix();

	modelStack.PushMatrix();
	modelStack.Translate(0, 0, -41);
	modelStack.Rotate(90, 0, 1, 0);
	modelStack.Scale(20, 5, 5);
	RenderMesh(meshList[GEO_ROAD], true);
	modelStack.PopMatrix();

	modelStack.PushMatrix();
	modelStack.Translate(-7, 0, -44);
	modelStack.Rotate(180, 0, 1, 0);
	modelStack.Scale(20, 5, 5);
	RenderMesh(meshList[GEO_ROAD], true);
	modelStack.PopMatrix();

	modelStack.PushMatrix();
	modelStack.Translate(-19, 0, -44);
	modelStack.Rotate(180, 0, 1, 0);
	modelStack.Scale(20, 5, 5);
	RenderMesh(meshList[GEO_ROAD], true);
	modelStack.PopMatrix();

	modelStack.PushMatrix();
	modelStack.Translate(-31, 0, -44);
	modelStack.Rotate(180, 0, 1, 0);
	modelStack.Scale(20, 5, 5);
	RenderMesh(meshList[GEO_ROAD], true);
	modelStack.PopMatrix();

	modelStack.PushMatrix();
	modelStack.Translate(-43, 0, -44);
	modelStack.Rotate(180, 0, 1, 0);
	modelStack.Scale(20, 5, 5);
	RenderMesh(meshList[GEO_ROAD], true);
	modelStack.PopMatrix();

	modelStack.PushMatrix();
	modelStack.Translate(-55, 0, -44);
	modelStack.Rotate(180, 0, 1, 0);
	modelStack.Scale(20, 5, 5);
	RenderMesh(meshList[GEO_ROAD], true);
	modelStack.PopMatrix();

	modelStack.PushMatrix();
	modelStack.Translate(-67, 0, -44);
	modelStack.Rotate(180, 0, 1, 0);
	modelStack.Scale(20, 5, 5);
	RenderMesh(meshList[GEO_ROAD], true);
	modelStack.PopMatrix();

	modelStack.PushMatrix();
	modelStack.Translate(-79, 0, -44);
	modelStack.Rotate(180, 0, 1, 0);
	modelStack.Scale(20, 5, 5);
	RenderMesh(meshList[GEO_ROAD], true);
	modelStack.PopMatrix();

	modelStack.PushMatrix();
	modelStack.Translate(-91, 0, -44);
	modelStack.Rotate(180, 0, 1, 0);
	modelStack.Scale(20, 5, 5);
	RenderMesh(meshList[GEO_ROAD], true);
	modelStack.PopMatrix();
}
void SP::RenderBullet()
{
	modelStack.PushMatrix();
	modelStack.Translate(bullet.x, 2, bullet.z);
	modelStack.Rotate(rotatebullet, 0, 1, 0);
	modelStack.Scale(3, 3, 5);
	RenderMesh(meshList[GEO_BULLET], true);
	modelStack.PopMatrix();

	modelStack.PushMatrix();
	modelStack.Translate(bullet2.x, 2, bullet2.z);
	modelStack.Rotate(rotatebullet, 0, 1, 0);
	modelStack.Scale(3, 3, 5);
	RenderMesh(meshList[GEO_BULLET], true);
	modelStack.PopMatrix();

	modelStack.PushMatrix();
	modelStack.Translate(bullet3.x, 2, bullet3.z);
	modelStack.Rotate(rotatebullet, 0, 1, 0);
	modelStack.Scale(3, 3, 5);
	RenderMesh(meshList[GEO_BULLET], true);
	modelStack.PopMatrix();

	modelStack.PushMatrix();
	modelStack.Translate(bullet4.x, 2, bullet4.z);
	modelStack.Rotate(rotatebullet, 0, 1, 0);
	modelStack.Scale(3, 3, 5);
	RenderMesh(meshList[GEO_BULLET], true);
	modelStack.PopMatrix();

	modelStack.PushMatrix();
	modelStack.Translate(bullet5.x, 2, bullet5.z);
	modelStack.Rotate(rotatebullet, 0, 1, 0);
	modelStack.Scale(3, 3, 5);
	RenderMesh(meshList[GEO_BULLET], true);
	modelStack.PopMatrix();

	modelStack.PushMatrix();
	modelStack.Translate(bullet6.x, 2, bullet6.z);
	modelStack.Rotate(rotatebullet, 0, 1, 0);
	modelStack.Scale(3, 3, 5);
	RenderMesh(meshList[GEO_BULLET], true);
	modelStack.PopMatrix();

	modelStack.PushMatrix();
	modelStack.Translate(bullet7.x, 2, bullet7.z);
	modelStack.Rotate(rotatebullet, 0, 1, 0);
	modelStack.Scale(3, 3, 5);
	RenderMesh(meshList[GEO_BULLET], true);
	modelStack.PopMatrix();

	modelStack.PushMatrix();
	modelStack.Translate(bullet8.x, 2, bullet8.z);
	modelStack.Rotate(rotatebullet, 0, 1, 0);
	modelStack.Scale(3, 3, 5);
	RenderMesh(meshList[GEO_BULLET], true);
	modelStack.PopMatrix();

}
//void SP::RenderLamps()
//{
//	//my lamps
//	modelStack.PushMatrix();
//	{
//		modelStack.Translate(10, 1, 20);
//		modelStack.Scale(0.1, 0.1, 0.1);
//
//		//lamp base
//		modelStack.PushMatrix();
//		{
//
//			modelStack.Translate(0, -5, 0);
//			modelStack.Rotate(0, 0, 0, 1);
//			modelStack.Rotate(0, 0, 0, 1);
//			modelStack.Scale(12, 12, 12);
//			RenderMesh(meshList[GEO_CONE], true);
//		}
//		modelStack.PopMatrix();
//
//		modelStack.Translate(0, 30, 0);
//		modelStack.Rotate(0, 0, 0, 1);
//		modelStack.Scale(4, 80, 4);
//		RenderMesh(meshList[GEO_CAPE], true);
//	}
//	modelStack.PopMatrix();
//
//	modelStack.PushMatrix();
//	{
//		modelStack.Translate(-11, 1, 67);
//		modelStack.Scale(0.1, 0.1, 0.1);
//
//		//lamp base
//		modelStack.PushMatrix();
//		{
//			modelStack.PushMatrix();
//			{
//				//light
//				modelStack.Translate(0, 75, 0);
//				modelStack.Scale(10, 10, 10);
//				RenderMesh(meshList[GEO_LAMPLIGHT], false);
//			}
//			modelStack.PopMatrix();
//
//			modelStack.Translate(0, -5, 0);
//			modelStack.Rotate(0, 0, 0, 1);
//			modelStack.Rotate(0, 0, 0, 1);
//			modelStack.Scale(12, 12, 12);
//			RenderMesh(meshList[GEO_CONE], true);
//		}
//		modelStack.PopMatrix();
//
//		modelStack.Translate(0, 30, 0);
//		modelStack.Rotate(0, 0, 0, 1);
//		modelStack.Scale(4, 80, 4);
//		RenderMesh(meshList[GEO_CAPE], true);
//	}
//	modelStack.PopMatrix();
//
//}
void SP::RenderText(Mesh* mesh, std::string text, Color color)
{

	if (!mesh || mesh->textureID <= 0) //Proper error check
		return;
	//glDisable(GL_DEPTH_TEST); //uncomment for RenderTextOnScreen
	glUniform1i(m_parameters[U_TEXT_ENABLED], 1);
	glUniform3fv(m_parameters[U_TEXT_COLOR], 1, &color.r);
	glUniform1i(m_parameters[U_LIGHTENABLED], 0);
	glUniform1i(m_parameters[U_COLOR_TEXTURE_ENABLED], 1);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, mesh->textureID);
	glUniform1i(m_parameters[U_COLOR_TEXTURE], 0);
	for (unsigned i = 0; i < text.length(); ++i)
	{
		Mtx44 characterSpacing;
		characterSpacing.SetToTranslation(i * 1.0f, 0, 0); //1.0f is the spacing of each character, you may change this value
		Mtx44 MVP = projectionStack.Top() * viewStack.Top() * modelStack.Top() * characterSpacing;
		glUniformMatrix4fv(m_parameters[U_MVP], 1, GL_FALSE, &MVP.a[0]);
		mesh->Render((unsigned)text[i] * 6, 6);
	}
	glBindTexture(GL_TEXTURE_2D, 0);
	glUniform1i(m_parameters[U_TEXT_ENABLED], 0);
	//glEnable(GL_DEPTH_TEST); //uncomment for RenderTextOnScreen
}
void SP::RenderTextOnScreen(Mesh* mesh, std::string text, Color color, float size, float x, float y)
{
	if (!mesh || mesh->textureID <= 0) //Proper error check
		return;
	glDisable(GL_DEPTH_TEST); //uncomment for RenderTextOnScreen
	//Add these code just after glDisable(GL_DEPTH_TEST);
	Mtx44 ortho;
	ortho.SetToOrtho(0, 80, 0, 60, -10, 10); //size of screen UI
	projectionStack.PushMatrix();
	projectionStack.LoadMatrix(ortho);
	viewStack.PushMatrix();
	viewStack.LoadIdentity(); //No need camera for ortho mode
	modelStack.PushMatrix();
	modelStack.LoadIdentity(); //Reset modelStack
	modelStack.Translate(x, y, 0);
	modelStack.Scale(size, size, size);

	glUniform1i(m_parameters[U_TEXT_ENABLED], 1);
	glUniform3fv(m_parameters[U_TEXT_COLOR], 1, &color.r);
	glUniform1i(m_parameters[U_LIGHTENABLED], 0);
	glUniform1i(m_parameters[U_COLOR_TEXTURE_ENABLED], 1);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, mesh->textureID);
	glUniform1i(m_parameters[U_COLOR_TEXTURE], 0);
	for (unsigned i = 0; i < text.length(); ++i)
	{
		Mtx44 characterSpacing;
		characterSpacing.SetToTranslation(0.5f + i * 0.6f, 0.5f, 0); //1.0f is the spacing of each character, you may change this value
		Mtx44 MVP = projectionStack.Top() * viewStack.Top() * modelStack.Top() * characterSpacing;
		glUniformMatrix4fv(m_parameters[U_MVP], 1, GL_FALSE, &MVP.a[0]);
		mesh->Render((unsigned)text[i] * 6, 6);
	}
	glBindTexture(GL_TEXTURE_2D, 0);
	glUniform1i(m_parameters[U_TEXT_ENABLED], 0);
	projectionStack.PopMatrix();
	viewStack.PopMatrix();
	modelStack.PopMatrix();
	glEnable(GL_DEPTH_TEST); //uncomment for RenderTextOnScreen
}

void SP::Init()
{
	Application::yourself.set_currency(100);
	Application::yourself.set_alive();
	Application::yourself.set_in_cab(false);
	Application::yourself.set_first_scammed(false);
	Application::yourself.set_coin1_enabled(false);
	Application::yourself.set_coin2_enabled(false);
	Application::yourself.set_coin3_enabled(false);
	Application::yourself.set_coin1_obtained(false);
	Application::yourself.set_coin2_obtained(false);
	Application::yourself.set_coin3_obtained(false);
	Application::yourself.set_coins(0);

	// Set background colour to light purple
	glClearColor(0.5f, 0.0f, 0.7f, 0.0f);

	// Enable depth test
	glEnable(GL_DEPTH_TEST);

	glDisable(GL_CULL_FACE);

	//Default to fill mode
	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);


	// Enable blending
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	// Generate a default VAO for now
	glGenVertexArrays(1, &m_vertexArrayID);
	glBindVertexArray(m_vertexArrayID);

	camera.Init(Vector3(-90, 3, -44), Vector3(0, 3, -44), Vector3(0, 1, 0));

	m_programID = LoadShaders("Shader//Texture.vertexshader", "Shader//Text.fragmentshader");
	// Get a handle for our "colorTexture" uniform
	m_parameters[U_COLOR_TEXTURE_ENABLED] = glGetUniformLocation(m_programID, "colorTextureEnabled");
	m_parameters[U_COLOR_TEXTURE] = glGetUniformLocation(m_programID, "colorTexture");
	m_parameters[U_MVP] = glGetUniformLocation(m_programID, "MVP");
	m_parameters[U_MODELVIEW] = glGetUniformLocation(m_programID, "MV");
	m_parameters[U_MODELVIEW_INVERSE_TRANSPOSE] = glGetUniformLocation(m_programID, "MV_inverse_transpose");
	m_parameters[U_MATERIAL_AMBIENT] = glGetUniformLocation(m_programID, "material.kAmbient");
	m_parameters[U_MATERIAL_DIFFUSE] = glGetUniformLocation(m_programID, "material.kDiffuse");
	m_parameters[U_MATERIAL_SPECULAR] = glGetUniformLocation(m_programID, "material.kSpecular");
	m_parameters[U_MATERIAL_SHININESS] = glGetUniformLocation(m_programID, "material.kShininess");
	Mesh::SetMaterialLoc(m_parameters[U_MATERIAL_AMBIENT], m_parameters[U_MATERIAL_DIFFUSE], m_parameters[U_MATERIAL_SPECULAR], m_parameters[U_MATERIAL_SHININESS]);


	//light0 -
	m_parameters[U_LIGHT0_POSITION] = glGetUniformLocation(m_programID, "lights[0].position_cameraspace");
	m_parameters[U_LIGHT0_COLOR] = glGetUniformLocation(m_programID, "lights[0].color");
	m_parameters[U_LIGHT0_POWER] = glGetUniformLocation(m_programID, "lights[0].power");
	m_parameters[U_LIGHT0_KC] = glGetUniformLocation(m_programID, "lights[0].kC");
	m_parameters[U_LIGHT0_KL] = glGetUniformLocation(m_programID, "lights[0].kL");
	m_parameters[U_LIGHT0_KQ] = glGetUniformLocation(m_programID, "lights[0].kQ");
	m_parameters[U_LIGHTENABLED] = glGetUniformLocation(m_programID, "lightEnabled");
	m_parameters[U_LIGHT0_TYPE] = glGetUniformLocation(m_programID, "lights[0].type");
	m_parameters[U_LIGHT0_SPOTDIRECTION] = glGetUniformLocation(m_programID, "lights[0].spotDirection");
	m_parameters[U_LIGHT0_COSCUTOFF] = glGetUniformLocation(m_programID, "lights[0].cosCutoff");
	m_parameters[U_LIGHT0_COSINNER] = glGetUniformLocation(m_programID, "lights[0].cosInner");
	m_parameters[U_LIGHT0_EXPONENT] = glGetUniformLocation(m_programID, "lights[0].exponent");

	//light1 -
	m_parameters[U_LIGHT1_POSITION] = glGetUniformLocation(m_programID, "lights[1].position_cameraspace");
	m_parameters[U_LIGHT1_COLOR] = glGetUniformLocation(m_programID, "lights[1].color");
	m_parameters[U_LIGHT1_POWER] = glGetUniformLocation(m_programID, "lights[1].power");
	m_parameters[U_LIGHT1_KC] = glGetUniformLocation(m_programID, "lights[1].kC");
	m_parameters[U_LIGHT1_KL] = glGetUniformLocation(m_programID, "lights[1].kL");
	m_parameters[U_LIGHT1_KQ] = glGetUniformLocation(m_programID, "lights[1].kQ");
	m_parameters[U_LIGHTENABLED] = glGetUniformLocation(m_programID, "lightEnabled");
	m_parameters[U_LIGHT1_TYPE] = glGetUniformLocation(m_programID, "lights[1].type");
	m_parameters[U_LIGHT1_SPOTDIRECTION] = glGetUniformLocation(m_programID, "lights[1].spotDirection");
	m_parameters[U_LIGHT1_COSCUTOFF] = glGetUniformLocation(m_programID, "lights[1].cosCutoff");
	m_parameters[U_LIGHT1_COSINNER] = glGetUniformLocation(m_programID, "lights[1].cosInner");
	m_parameters[U_LIGHT1_EXPONENT] = glGetUniformLocation(m_programID, "lights[1].exponent");

	//light2 -
	m_parameters[U_LIGHT2_POSITION] = glGetUniformLocation(m_programID, "lights[2].position_cameraspace");
	m_parameters[U_LIGHT2_COLOR] = glGetUniformLocation(m_programID, "lights[2].color");
	m_parameters[U_LIGHT2_POWER] = glGetUniformLocation(m_programID, "lights[2].power");
	m_parameters[U_LIGHT2_KC] = glGetUniformLocation(m_programID, "lights[2].kC");
	m_parameters[U_LIGHT2_KL] = glGetUniformLocation(m_programID, "lights[2].kL");
	m_parameters[U_LIGHT2_KQ] = glGetUniformLocation(m_programID, "lights[2].kQ");
	m_parameters[U_LIGHTENABLED] = glGetUniformLocation(m_programID, "lightEnabled");
	m_parameters[U_LIGHT2_TYPE] = glGetUniformLocation(m_programID, "lights[2].type");
	m_parameters[U_LIGHT2_SPOTDIRECTION] = glGetUniformLocation(m_programID, "lights[2].spotDirection");
	m_parameters[U_LIGHT2_COSCUTOFF] = glGetUniformLocation(m_programID, "lights[2].cosCutoff");
	m_parameters[U_LIGHT2_COSINNER] = glGetUniformLocation(m_programID, "lights[2].cosInner");
	m_parameters[U_LIGHT2_EXPONENT] = glGetUniformLocation(m_programID, "lights[2].exponent");

	// Get a handle for our "textColor" uniform
	m_parameters[U_NUMLIGHTS] = glGetUniformLocation(m_programID, "numLights");
	m_parameters[U_TEXT_ENABLED] = glGetUniformLocation(m_programID, "textEnabled");
	m_parameters[U_TEXT_COLOR] = glGetUniformLocation(m_programID, "textColor");
	glUseProgram(m_programID);



	//Replace previous code
	glUniform1i(m_parameters[U_NUMLIGHTS], 3);
	light[0].type = Light::LIGHT_SPOT;
	light[0].position.Set(-100, 20, 25);
	light[0].color.Set(0.9, 1, 0.8);
	light[0].power = 1;
	light[0].kC = 1.f;
	light[0].kL = 0.01f;
	light[0].kQ = 0.001f;
	light[0].cosCutoff = cos(Math::DegreeToRadian(45));
	light[0].cosInner = cos(Math::DegreeToRadian(30));
	light[0].exponent = 3.f;
	light[0].spotDirection.Set(0.f, 1.f, 0.f);


	light[1].type = Light::LIGHT_DIRECTIONAL;
	light[1].position.Set(-100, 140, -100);
	light[1].color.Set(0.4, 0.9, 0.7);
	light[1].power = 0.5f;
	light[1].kC = 1.f;
	light[1].kL = 0.01f;
	light[1].kQ = 0.001f;
	light[1].cosCutoff = cos(Math::DegreeToRadian(15));
	light[1].cosInner = cos(Math::DegreeToRadian(30));
	light[1].exponent = 3.f;
	light[1].spotDirection.Set(0.f, 1.f, 0.f);

	light[2].type = Light::LIGHT_SPOT;
	light[2].position.Set(-19.0f, 15.0f, 82.0f);
	light[2].color.Set(0.4, 0.9, 0.7);
	light[2].power = 1;
	light[2].kC = 1.f;
	light[2].kL = 0.01f;
	light[2].kQ = 0.001f;
	light[2].cosCutoff = cos(Math::DegreeToRadian(15));
	light[2].cosInner = cos(Math::DegreeToRadian(30));
	light[2].exponent = 3.f;
	light[2].spotDirection.Set(0.f, 1.f, 0.f);


	// Make sure you pass uniform parameters after glUseProgram()

	glUniform1i(m_parameters[U_LIGHT0_TYPE], light[0].type);
	glUniform3fv(m_parameters[U_LIGHT0_COLOR], 1, &light[0].color.r);
	glUniform1f(m_parameters[U_LIGHT0_POWER], light[0].power);
	glUniform1f(m_parameters[U_LIGHT0_KC], light[0].kC);
	glUniform1f(m_parameters[U_LIGHT0_KL], light[0].kL);
	glUniform1f(m_parameters[U_LIGHT0_KQ], light[0].kQ);
	glUniform1f(m_parameters[U_LIGHT0_COSCUTOFF], light[0].cosCutoff);
	glUniform1f(m_parameters[U_LIGHT0_COSINNER], light[0].cosInner);
	glUniform1f(m_parameters[U_LIGHT0_EXPONENT], light[0].exponent);

	glUniform1i(m_parameters[U_LIGHT1_TYPE], light[1].type);
	glUniform3fv(m_parameters[U_LIGHT1_COLOR], 1, &light[1].color.r);
	glUniform1f(m_parameters[U_LIGHT1_POWER], light[1].power);
	glUniform1f(m_parameters[U_LIGHT1_KC], light[1].kC);
	glUniform1f(m_parameters[U_LIGHT1_KL], light[1].kL);
	glUniform1f(m_parameters[U_LIGHT1_KQ], light[1].kQ);
	glUniform1f(m_parameters[U_LIGHT1_COSCUTOFF], light[1].cosCutoff);
	glUniform1f(m_parameters[U_LIGHT1_COSINNER], light[1].cosInner);
	glUniform1f(m_parameters[U_LIGHT1_EXPONENT], light[1].exponent);

	glUniform1i(m_parameters[U_LIGHT2_TYPE], light[2].type);
	glUniform3fv(m_parameters[U_LIGHT2_COLOR], 1, &light[2].color.r);
	glUniform1f(m_parameters[U_LIGHT2_POWER], light[2].power);
	glUniform1f(m_parameters[U_LIGHT2_KC], light[2].kC);
	glUniform1f(m_parameters[U_LIGHT2_KL], light[2].kL);
	glUniform1f(m_parameters[U_LIGHT2_KQ], light[2].kQ);
	glUniform1f(m_parameters[U_LIGHT2_COSCUTOFF], light[2].cosCutoff);
	glUniform1f(m_parameters[U_LIGHT2_COSINNER], light[2].cosInner);
	glUniform1f(m_parameters[U_LIGHT2_EXPONENT], light[2].exponent);


	rotateAngle = 0;
	/*for (int i = 0; i < NUM_GEOMETRY; ++i)
	{
		meshList[i] = nullptr;
	}*/

	//***old mesh***
	{
		meshList[GEO_AXES] = MeshBuilder::GenerateAxes("reference", 1000, 1000, 1000);

		meshList[GEO_QUAD] = MeshBuilder::GenerateQuad("quad", Color(0, 0, 0), 1.f);
		meshList[GEO_QUAD]->textureID = LoadTGA("Image//floor.tga");

		/*meshList[GEO_CUBE] = MeshBuilder::GenerateCube("cube", Color(1, 1, 1), 1.f);
		meshList[GEO_CUBE]->textureID = LoadTGA("Image//color.tga");*/

		meshList[GEO_FRONT] = MeshBuilder::GenerateQuad("front", Color(1, 1, 1), 1.f);
		meshList[GEO_FRONT]->textureID = LoadTGA("Image//miramar_ft.tga");

		meshList[GEO_LEFT] = MeshBuilder::GenerateQuad("left", Color(1, 1, 1), 1.f);
		meshList[GEO_LEFT]->textureID = LoadTGA("Image//miramar_lf.tga");

		meshList[GEO_RIGHT] = MeshBuilder::GenerateQuad("right", Color(1, 1, 1), 1.f);
		meshList[GEO_RIGHT]->textureID = LoadTGA("Image//miramar_rt.tga");

		meshList[GEO_TOP] = MeshBuilder::GenerateQuad("top", Color(1, 1, 1), 1.f);
		meshList[GEO_TOP]->textureID = LoadTGA("Image//miramar_up.tga");

		meshList[GEO_BOTTOM] = MeshBuilder::GenerateQuad("bottom", Color(1, 1, 1), 1.f);
		meshList[GEO_BOTTOM]->textureID = LoadTGA("Image//miramar_dn.tga");

		meshList[GEO_BACK] = MeshBuilder::GenerateQuad("back", Color(1, 1, 1), 1.f);
		meshList[GEO_BACK]->textureID = LoadTGA("Image//miramar_bk.tga");

		/*meshList[GEO_BLEND] = MeshBuilder::GenerateQuad("blend", Color(1, 1, 1), 1.f);
		meshList[GEO_BLEND]->textureID = LoadTGA("Image//Grimm.tga");*/

		//meshList[GEO_SPHERE] = MeshBuilder::GenerateSphere("sphere", Color(0, 0, 0), 10, 20, 1.f);			//limbs

		//meshList[GEO_LAMPLIGHT] = MeshBuilder::GenerateSphere("sphere", Color(0.4, 0.4, 0.4), 10, 20, 1.f);			//limbs

		//meshList[GEO_CYLINDER] = MeshBuilder::GenerateCylinder("cylinder", Color(1, 1, 1), 20, 1.f, 1.f);		//base of head

		//meshList[GEO_HEMISPHERE] = MeshBuilder::GenerateHemisphere("hemisphere", Color(1, 1, 1), 10, 20, 1.f);  //ends of head

		//meshList[GEO_TORUS] = MeshBuilder::GenerateTorus("torus", Color(0.3, 0.3, 0.3), 20, 20, 1.f, 0.5f);			//cape head

		//meshList[GEO_CAPE] = MeshBuilder::GenerateCylinder("cylinder", Color(0.3, 0.3, 0.3), 20, 1.f, 1.f);		//base of head

		//meshList[GEO_HALFTORUS] = MeshBuilder::GenerateHalfTorus("quartertorus", Color(1, 1, 1), 10, 20, 1.f, 0.5f);   //horns

		//meshList[GEO_CONE] = MeshBuilder::GenerateCone("cone", Color(0.3, 0.3, 0.3), 20, 1.f, 1.f);   //cape end

		//meshList[GEO_LIGHTBALL] = MeshBuilder::GenerateSphere("sphere", Color(1, 1, 1), 10, 20, 1.f);		//light for lamppost

		meshList[GEO_HORNET] = MeshBuilder::GenerateOBJMTL("hornet", "OBJ//zombie.obj", "OBJ//zombie.mtl");

		meshList[GEO_ROCKS] = MeshBuilder::GenerateOBJMTL("rocks", "OBJ//rocks.obj", "OBJ//rocks.mtl");
		meshList[GEO_ROCKS]->textureID = LoadTGA("Image//rockwall_darkercolor.tga");
		meshList[GEO_TALLROCK] = MeshBuilder::GenerateOBJMTL("tallrock", "OBJ//rocksTall.obj", "OBJ//rocksTall.mtl");
		meshList[GEO_TALLROCK]->textureID = LoadTGA("Image//rockwall_darkercolor.tga");

		meshList[GEO_STONEWALL] = MeshBuilder::GenerateOBJMTL("stonewall", "OBJ//stoneWall.obj", "OBJ//stoneWall.mtl");
		meshList[GEO_STONEWALL]->textureID = LoadTGA("Image//rockwall_darkercolor.tga");

		meshList[GEO_STONEWALLDAMAGED] = MeshBuilder::GenerateOBJMTL("stonewalldamaged", "OBJ//stoneWallDamaged.obj", "OBJ//stoneWallDamaged.mtl");
		meshList[GEO_STONEWALLDAMAGED]->textureID = LoadTGA("Image//rockwall_darkercolor.tga");

		meshList[GEO_ROAD] = MeshBuilder::GenerateOBJMTL("road", "OBJ//road.obj", "OBJ//road.mtl");

		//meshList[GEO_CRYPT] = MeshBuilder::GenerateOBJMTL("crypt", "OBJ//crypt.obj", "OBJ//crypt.mtl");

		meshList[GEO_DEBRIS] = MeshBuilder::GenerateOBJMTL("debris", "OBJ//debris.obj", "OBJ//debris.mtl");
		meshList[GEO_DEBRIS]->textureID = LoadTGA("Image//rockwall_darkercolor.tga");

		meshList[GEO_LAMP] = MeshBuilder::GenerateOBJMTL("debris", "OBJ//lightpostDouble.obj", "OBJ//lightpostDouble.mtl");

		//meshList[GEO_GRAVE] = MeshBuilder::GenerateOBJMTL("grave", "OBJ//grave.obj", "OBJ//grave.mtl");

		//meshList[GEO_LANTERN] = MeshBuilder::GenerateOBJMTL("lantern", "OBJ//lanternCandle.obj", "OBJ//lanternCandle.mtl");

	/*	meshList[GEO_BULLET] = MeshBuilder::GenerateOBJMTL("bullet", "OBJ//ironFenceBar.obj", "OBJ//ironFenceBar.mtl");*/

		meshList[GEO_BULLET] = MeshBuilder::GenerateQuad("bullet", Color(1, 1, 1), 1.f);
		meshList[GEO_BULLET]->textureID = LoadTGA("Image//internetscam.tga");

		meshList[GEO_BULLET2] = MeshBuilder::GenerateQuad("bullet", Color(1, 1, 1), 1.f);
		meshList[GEO_BULLET2]->textureID = LoadTGA("Image//phonelove.tga");

		meshList[GEO_TEXT] = MeshBuilder::GenerateText("text", 16, 16);
		meshList[GEO_TEXT]->textureID = LoadTGA("Image//ArialFont.tga");

		//meshList[GEO_TEXT2] = MeshBuilder::GenerateText("text2", 16, 16);
		//meshList[GEO_TEXT2]->textureID = LoadTGA("Image//comicsans.tga");
	}

	meshList[GEO_COIN] = MeshBuilder::GenerateOBJMTL("coin", "OBJ//coin.obj", "OBJ//coin.mtl");
	meshList[GEO_COIN]->textureID = LoadTGA("Image//Coin_Gold_albedo.tga");

	meshList[GEO_COIN_ICON] = MeshBuilder::GenerateQuad("coin_icon", Color(1, 1, 1), 1.f);
	meshList[GEO_COIN_ICON]->textureID = LoadTGA("Image//coin_icon.tga");

	meshList[GEO_BLOOD] = MeshBuilder::GenerateQuad("blood_gui", Color(1, 1, 1), 1.f);
	meshList[GEO_BLOOD]->textureID = LoadTGA("Image//Blood.tga");

	meshList[GEO_LV1] = MeshBuilder::GenerateQuad("lv1", Color(1, 1, 1), 1.f);
	meshList[GEO_LV1]->textureID = LoadTGA("Image//lv1.tga");


	meshList[GEO_TAXI] = MeshBuilder::GenerateOBJ("coin", "OBJ//Taxi2.obj");
	meshList[GEO_TAXI]->textureID = LoadTGA("Image//taxi.tga");

	meshList[GEO_TUNNEL] = MeshBuilder::GenerateQuad("tunnel", Color(1, 1, 1), 1.f);
	meshList[GEO_TUNNEL]->textureID = LoadTGA("Image//tunnel.tga");

	meshList[GEO_ROAD_FOR_BULLETS] = MeshBuilder::GenerateCube("roadforbullets", Color(0.5f, 0.4f, 0.43f), 1.f);

	/*meshList[GEO_BUILDINGD] = MeshBuilder::GenerateOBJMTL("buildind", "OBJ//low_buildingD.obj", "OBJ//low_buildingD.mtl");
	meshList[GEO_BUILDINGJ] = MeshBuilder::GenerateOBJMTL("buildinj	", "OBJ//low_buildingJ.obj", "OBJ//low_buildingJ.mtl");*/

	//-----------------------------------------------------------------------
	//SP
	//meshList[GEO_SCAMMER] = MeshBuilder::GenerateOBJ("scammer", "OBJ//scammer.obj");
	//meshList[GEO_SCAMMER]->textureID = LoadTGA("Image//scammer.tga");

	//An array of 3 vectors which represents the colors of the 3 vertices

	Mtx44 projection;
	projection.SetToPerspective(45.f, 4.f / 3.f, 0.1f, 1000.f);
	projectionStack.LoadMatrix(projection);



	enemyz = 25;
	bullet.x = -65;
	bullet.z = enemyz;

	bullet2.x = -85;
	bullet2.z = enemyz;

	bullet3.x = -45;
	bullet3.z = enemyz;

	bullet4.x = -35;
	bullet4.z = enemyz;

	bullet5.x = -55;
	bullet5.z = enemyz;

	bullet6.x = -95;
	bullet6.z = enemyz;

	bullet7.x = -75;
	bullet7.z = enemyz;

	bullet8.x = -70;
	bullet8.z = enemyz;

	intquest = 0;
	scammed = false;
	count = 0;

}

void SP::Update(double dt)
{
	scammerpos = scammer_pos - camera.position;
	scammerdis = sqrt(pow(scammerpos.x, 2) + pow(scammerpos.y, 2) + pow(scammerpos.z, 2));
	Vector3 coin1pos = coin1_pos - camera.position;
	coin1dis = sqrt(pow(coin1pos.x, 2) + pow(coin1pos.y, 2) + pow(coin1pos.z, 2));
	Vector3 coin2pos = coin2_pos - camera.position;
	coin2dis = sqrt(pow(coin2pos.x, 2) + pow(coin2pos.y, 2) + pow(coin2pos.z, 2));
	Vector3 coin3pos = coin3_pos - camera.position;
	coin3dis = sqrt(pow(coin3pos.x, 2) + pow(coin3pos.y, 2) + pow(coin3pos.z, 2));

	count++;
	








	/*******************************************************************************************************/
	//tutorial boundary check
	if (camera.position.x > -102 && camera.position.x<-18 && camera.position.z>-59 && camera.position.z < -25)
	{
		tut_text = true;
		
	}
	else
	{
		tut_text = false;
	}
	if (!tut_text && !scammaer_talk)
	{
		intquest = 1;
	}
	
	if (scammerdis <= 10)
		interactednpc = true;

	if (interactednpc)
		intquest = 2;
	if (Application::yourself.get_coin1_obtained() && Application::yourself.get_coin2_obtained() && Application::yourself.get_coin3_obtained())
		intquest = 3;

	if (Application::yourself.get_coin1_obtained() && Application::yourself.get_coin2_obtained() && Application::yourself.get_coin3_obtained() && scammed)

		intquest = 4;


	static bool scammaer_talk = false;
	static int scam_count = 0;
	static int buy = 0;
	if (Application::IsKeyPressed('E') && scammerdis <= 10)
	{
		scammaer_talk = true;
		
		
	}
	else if (!Application::IsKeyPressed('E'))
	{
		scammaer_talk = false;
	}

	if (scammerdis <= 10)
	{
		if (scammaer_talk)
		{
			scam_count++;
		}
		if (Application::yourself.get_first_scammed() == false)
		{
			//talk text
			scammer_text = "Give me 3 Gold Coins worth $300 and I'll give you a prize";
			if (Application::yourself.get_currency() > 300 && Application::IsKeyPressed('E'))
			{
				buy = 1;
				scam_count = 0;
				//scammer_text = "Press E to talk";
				Application::yourself.currency_deducted(300);
				Application::yourself.set_first_scammed(true);


			}
		}
		else
		{
			scammer_text = "Press E to talk";
		}
		if (Application::yourself.get_first_scammed() == true)
		{
			scammer_text = "Haha I was lying. Suck it loser!";
			scammed = true;
		}

		std::cout << scam_count << std::endl;

	}
	else
	{
		scammer_text = " ";
	}


	if (coin1dis <= 10)
	{
		if (Application::IsKeyPressed('E'))
		{
			Application::yourself.set_coin1_obtained(true);
			coin1_collected = true;
		}

		if (Application::yourself.get_coin1_obtained() == true)
		{
			Application::yourself.set_coin1_enabled(true);
		}
	
		if (Application::IsKeyPressed('E') && Application::yourself.get_coin1_obtained() == true)
		{
			if (Application::yourself.get_currency_added_from_C1() == false)
			{
				Application::yourself.currency_added(100);
				Application::yourself.add_coins(1);
				Application::yourself.set_currency_added_from_C1(true);
			}

		}
	}

	if (coin2dis <= 12)
	{
		if (Application::IsKeyPressed('E'))
		{
			Application::yourself.set_coin2_obtained(true);
			coin2_collected = true;
		}

		if (Application::yourself.get_coin2_obtained() == true)
		{
			Application::yourself.set_coin2_enabled(true);
		}
		
		
		if (Application::IsKeyPressed('E') && Application::yourself.get_coin2_obtained() == true)
		{
			if (Application::yourself.get_currency_added_from_C2() == false)
			{
				Application::yourself.currency_added(100);
				Application::yourself.add_coins(1);
				Application::yourself.set_currency_added_from_C2(true);
			}

		}
	}

	if (coin3dis <= 10)
	{
		if (Application::IsKeyPressed('E'))
		{
			Application::yourself.set_coin3_obtained(true);
			coin3_collected = true;
		}

		if (Application::yourself.get_coin3_obtained() == true)
		{
			Application::yourself.set_coin3_enabled(true);
		}
		else
		{
			scammer_text = "Press E to collect";
		}
		if (Application::IsKeyPressed('E') && Application::yourself.get_coin3_obtained() == true)
		{
			if (Application::yourself.get_currency_added_from_C3() == false)
			{
				Application::yourself.currency_added(100);
				Application::yourself.add_coins(1);
				Application::yourself.set_currency_added_from_C3(true);
			}

		}
	}
	/*if (cabTP() <= 10)
	{
		scammer_text = "Press E to ride";
	}*/
	if ((Application::yourself.get_in_cab() == false) && (cabTP() <= 10))
	{
		if (Application::IsKeyPressed('E'))
		{
			Application::yourself.set_in_cab(true);
		}
		
	}

	//std::cout << coin1_count << std::endl;
	/**********************************************************************************************************/





	

	// 3rd interaction : fighting
	if (Application::yourself.get_first_scammed() == true)
	{
		battlestart = true;
	}
	if (battlestart)
	{
		bullet.z += (float)(60 * dt);
		bullet2.z += (float)(50 * dt);
		bullet3.z += (float)(50 * dt);
		bullet4.z += (float)(70 * dt);
		bullet5.z += (float)(70 * dt);
		bullet6.z += (float)(30 * dt);
		bullet7.z += (float)(50 * dt);
		bullet8.z += (float)(70 * dt);
		

		rotatebullet += (float)(120 * dt);

		if (bullet.z > 105)
		{
			bullet.z = enemyz;
		}
		if (bullet2.z > 105)
		{
			bullet2.z = enemyz;
		}
		if (bullet3.z > 105)
		{
			bullet3.z = enemyz;
		}
		if (bullet4.z > 105)
		{
			bullet4.z = enemyz;
		}
		if (bullet5.z > 105)
		{
			bullet5.z = enemyz;
		}
		if (bullet6.z > 105)
		{
			bullet6.z = enemyz;
		}
		if (bullet7.z > 105)
		{
			bullet7.z = enemyz;
		}

		if (bullet8.z > 105)
		{
			bullet8.z = enemyz;
		}
	}

	//if player is in radius of bullet
	if (((bullet.x + 2) > camera.position.x && (bullet.x - 2) < camera.position.x && (bullet.z + 4) > camera.position.z && (bullet.z - 2) < camera.position.z)
		|| ((bullet2.x + 2) > camera.position.x && (bullet2.x - 2) < camera.position.x && (bullet2.z + 4) > camera.position.z && (bullet2.z - 2) < camera.position.z)
		|| ((bullet3.x + 2) > camera.position.x && (bullet3.x - 2) < camera.position.x && (bullet3.z + 4) > camera.position.z && (bullet3.z - 2) < camera.position.z)
		|| ((bullet4.x + 2) > camera.position.x && (bullet4.x - 2) < camera.position.x && (bullet4.z + 4) > camera.position.z && (bullet4.z - 2) < camera.position.z)
		|| ((bullet5.x + 2) > camera.position.x && (bullet5.x - 2) < camera.position.x && (bullet5.z + 4) > camera.position.z && (bullet5.z - 2) < camera.position.z)
		|| ((bullet6.x + 2) > camera.position.x && (bullet6.x - 2) < camera.position.x && (bullet6.z + 4) > camera.position.z && (bullet6.z - 2) < camera.position.z)
		|| ((bullet7.x + 2) > camera.position.x && (bullet7.x - 2) < camera.position.x && (bullet7.z + 4) > camera.position.z && (bullet7.z - 2) < camera.position.z)
		|| ((bullet8.x + 2) > camera.position.x && (bullet8.x - 2) < camera.position.x && (bullet8.z + 4) > camera.position.z && (bullet8.z - 2) < camera.position.z))
	{
		//yourself.set_currency(yourself.get_currency() - 1);
		Application::yourself.set_currency(Application::yourself.get_currency() - 1.5f);
		blood_ui = true;
	}
	if (Application::yourself.get_currency() <= 0)
	{
		camera.Init(Vector3(-96, 3, 30), Vector3(-96, 3, 0), Vector3(0, 1, 0));
		Application::yourself.set_die();
	}
	if (Application::IsKeyPressed('R'))
	{
		camera.Init(Vector3(0, 3, 1), Vector3(0, 3, 10), Vector3(0, 1, 0));
		Application::yourself.set_currency(100);
		battlestart = false;
		die = false;
		win = false;

	}
	if (battlestart == false)
	{

		enemyz = 17;
		bullet.x = -65;
		bullet.z = enemyz;

		bullet2.x = -85;
		bullet2.z = enemyz;

		bullet3.x = -45;
		bullet3.z = enemyz;

		bullet4.x = -35;
		bullet4.z = enemyz;

		bullet5.x = -55;
		bullet5.z = enemyz;

		bullet6.x = -95;
		bullet6.z = enemyz - 40;

		bullet7.x = -75;
		bullet7.z = enemyz;

		bullet8.x = -80;
		bullet8.z = enemyz;
	}





	rotateAngle += (float)(20 * dt);
	camera.Update(dt);
	FPS = std::to_string(1.f / dt);
	camerax = std::to_string(camera.position.x);

	cameraz = std::to_string(camera.position.z);
}
void SP::RenderScammer()
{
	if (Application::yourself.get_first_scammed() == false)
	{
		/*modelStack.PushMatrix();
		modelStack.Translate(scammer_pos.x, scammer_pos.y, scammer_pos.z);
		modelStack.Scale(0.5, 0.5, 0.5);
		RenderMesh(meshList[GEO_SCAMMER], true);
		modelStack.PopMatrix();*/

		modelStack.PushMatrix();
		modelStack.Translate(scammer_pos.x, scammer_pos.y, scammer_pos.z);
		//modelStack.Translate(0, 0, 30);
		modelStack.Rotate(90, 0, 1, 0);
		modelStack.Scale(5, 5, 5);
		RenderMesh(meshList[GEO_HORNET], true);
		modelStack.PopMatrix();
	}

	if (Application::yourself.get_coin1_enabled() == false)
	{
		//beside graveyard
		modelStack.PushMatrix();
		//modelStack.Translate(25, 0, 28);
		modelStack.Translate(coin1_pos.x, coin1_pos.y, coin1_pos.z);
		modelStack.Rotate(90, 1, 0, 0);
		modelStack.Scale(0.5, 0.5, 0.5);
		RenderMesh(meshList[GEO_COIN], true);
		modelStack.PopMatrix();
	}
	else
	{

	}
	if (Application::yourself.get_coin2_enabled() == false)
	{
		//corner near the exit
		modelStack.PushMatrix();
		//modelStack.Translate(27, 0, 104);
		modelStack.Translate(coin2_pos.x, coin2_pos.y, coin2_pos.z);
		modelStack.Rotate(90, 1, 0, 0);
		modelStack.Scale(0.5, 0.5, 0.5);
		RenderMesh(meshList[GEO_COIN], true);
		modelStack.PopMatrix();
	}
	else
	{

	}
	if (Application::yourself.get_coin3_enabled() == false)
	{
		//behind under rocks
		modelStack.PushMatrix();
		//modelStack.Translate(23, 0, -17);
		modelStack.Translate(coin3_pos.x, coin3_pos.y, coin3_pos.z);
		modelStack.Rotate(90, 1, 0, 0);
		modelStack.Scale(0.5, 0.5, 0.5);
		RenderMesh(meshList[GEO_COIN], true);
		modelStack.PopMatrix();
	}
	else
	{

	}

	//RenderTextOnScreen(meshList[GEO_TEXT], camerax, Color(0, 1, 0), 2, 6, 0);

}

void SP::Render()
{


	// Render VBO here
	//Clear color buffer every frame
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	viewStack.LoadIdentity();
	viewStack.LookAt(camera.position.x, camera.position.y,
		camera.position.z, camera.target.x, camera.target.y,
		camera.target.z, camera.up.x, camera.up.y, camera.up.z);
	modelStack.LoadIdentity();
	Mtx44 MVP = projectionStack.Top() * viewStack.Top() * modelStack.Top();
	glUniformMatrix4fv(m_parameters[U_MVP], 1, GL_FALSE, &MVP.a[0]); //update the shader with new MVP



	if (light[0].type == Light::LIGHT_DIRECTIONAL)
	{
		Vector3 lightDir(light[0].position.x, light[0].position.y, light[0].position.z);
		Vector3 lightDirection_cameraspace = viewStack.Top() * lightDir;
		glUniform3fv(m_parameters[U_LIGHT0_POSITION], 1, &lightDirection_cameraspace.x);
	}
	else if (light[0].type == Light::LIGHT_SPOT)
	{
		Position lightPosition_cameraspace = viewStack.Top() * light[0].position;
		glUniform3fv(m_parameters[U_LIGHT0_POSITION], 1, &lightPosition_cameraspace.x);
		Vector3 spotDirection_cameraspace = viewStack.Top() * light[0].spotDirection;
		glUniform3fv(m_parameters[U_LIGHT0_SPOTDIRECTION], 1, &spotDirection_cameraspace.x);
	}
	else
	{
		Position lightPosition_cameraspace = viewStack.Top() * light[0].position;
		glUniform3fv(m_parameters[U_LIGHT0_POSITION], 1, &lightPosition_cameraspace.x);
	}


	if (light[1].type == Light::LIGHT_DIRECTIONAL)
	{
		Vector3 lightDir(light[1].position.x, light[1].position.y, light[1].position.z);
		Vector3 lightDirection_cameraspace = viewStack.Top() * lightDir;
		glUniform3fv(m_parameters[U_LIGHT1_POSITION], 1, &lightDirection_cameraspace.x);
	}
	else if (light[1].type == Light::LIGHT_SPOT)
	{
		Position lightPosition_cameraspace = viewStack.Top() * light[1].position;
		glUniform3fv(m_parameters[U_LIGHT1_POSITION], 1, &lightPosition_cameraspace.x);
		Vector3 spotDirection_cameraspace = viewStack.Top() * light[1].spotDirection;
		glUniform3fv(m_parameters[U_LIGHT1_SPOTDIRECTION], 1, &spotDirection_cameraspace.x);
	}
	else
	{
		Position lightPosition_cameraspace = viewStack.Top() * light[1].position;
		glUniform3fv(m_parameters[U_LIGHT1_POSITION], 1, &lightPosition_cameraspace.x);
	}



	if (light[2].type == Light::LIGHT_DIRECTIONAL)
	{
		Vector3 lightDir(light[2].position.x, light[2].position.y, light[2].position.z);
		Vector3 lightDirection_cameraspace = viewStack.Top() * lightDir;
		glUniform3fv(m_parameters[U_LIGHT2_POSITION], 1, &lightDirection_cameraspace.x);
	}
	else if (light[2].type == Light::LIGHT_SPOT)
	{
		Position lightPosition_cameraspace = viewStack.Top() * light[2].position;
		glUniform3fv(m_parameters[U_LIGHT2_POSITION], 1, &lightPosition_cameraspace.x);
		Vector3 spotDirection_cameraspace = viewStack.Top() * light[2].spotDirection;
		glUniform3fv(m_parameters[U_LIGHT2_SPOTDIRECTION], 1, &spotDirection_cameraspace.x);
	}
	else
	{
		Position lightPosition_cameraspace = viewStack.Top() * light[2].position;
		glUniform3fv(m_parameters[U_LIGHT2_POSITION], 1, &lightPosition_cameraspace.x);
	}
	//**************************************************************************************************************


	/*modelStack.PushMatrix();
	modelStack.Translate(light[0].position.x, light[0].position.y, light[0].position.z);
	RenderMesh(meshList[GEO_LIGHTBALL], false);
	modelStack.PopMatrix();*/

	/*modelStack.PushMatrix();
	modelStack.Translate(light[1].position.x, light[1].position.y, light[1].position.z);
	RenderMesh(meshList[GEO_LIGHTBALL], true);
	modelStack.PopMatrix();*/

	/*modelStack.PushMatrix();
	modelStack.Translate(light[2].position.x, light[2].position.y, light[2].position.z);
	RenderMesh(meshList[GEO_LIGHTBALL], true);
	modelStack.PopMatrix();*/

	RenderSkybox();

	RenderRightSide();

	RenderLeftSide();

	RenderBackSide();

	RenderFloor();

	RenderFrontSide();

	

	RenderScammer();

	//RenderLamps();
	

	

	RenderPath();
	
	

	RenderBullet();

	

	RenderPress_E();

	


	if (die)
	{
		//endscreen = true;
		if (true)
		{
			Application::IsKeyPressed(VK_F3) == true;
		}
		battlestart = false;
		modelStack.PushMatrix();
		modelStack.Translate(-100, 3, 17);
		modelStack.Rotate(0, 0, 1, 0);
		modelStack.Scale(1, 1, 1);
		RenderText(meshList[GEO_TEXT], "You Died!", Color(1, 0, 0));
		modelStack.PopMatrix();
		modelStack.PushMatrix();

		modelStack.Translate(-102, 1, 17);
		modelStack.Rotate(0, 0, 1, 0);
		modelStack.Scale(0.6, 0.6, 0.6);
		RenderText(meshList[GEO_TEXT], "Press 'R' to restart", Color(1, 0, 0));
		modelStack.PopMatrix();
	}
	if (win)
	{
		//endscreen = true;
		battlestart = false;
		modelStack.PushMatrix();
		modelStack.Translate(-100, 3, 17);
		modelStack.Rotate(0, 0, 1, 0);
		modelStack.Scale(1, 1, 1);
		RenderText(meshList[GEO_TEXT], "You Win!", Color(0, 0, 1));
		modelStack.PopMatrix();
		modelStack.PushMatrix();

		modelStack.Translate(-102, 1, 17);
		modelStack.Rotate(0, 0, 1, 0);
		modelStack.Scale(0.6, 0.6, 0.6);
		RenderText(meshList[GEO_TEXT], "Press 'R' to restart", Color(0, 0, 1));
		modelStack.PopMatrix();
	}
	//----------------------------sp--------------------------------------
	if (blood_ui)
	{
		int i;
		for (i = 0; i < 10; i++)
		{
			RenderMeshOnScreen(meshList[GEO_BLOOD], 40, 30, 85, 65);
		}
		i = 0;
		blood_ui = false;
	}
	if (tut_text == true)
	{
		RenderTextOnScreen(meshList[GEO_TEXT], "Use WASD to move", Color(0.3f, 0.4f, 0.2f), 2, 30, 55);
		RenderTextOnScreen(meshList[GEO_TEXT], "Use Arrow keys to look around", Color(0.3f, 0.4f, 0.2f), 2, 25, 53);
		RenderTextOnScreen(meshList[GEO_TEXT], "<-- is your health..", Color(0.0f, 0.9f, 0.5f), 3, 7, 49);
	}
	//modelStack.PushMatrix();// 
	////scale, translate, rotate
	//modelStack.Translate(0, 3, 18);
	//modelStack.Rotate(90, 0, 1, 0);
	//modelStack.Scale(5, 5, 5);
	//RenderMesh(meshList[GEO_SCAMMER], true);
	//modelStack.PopMatrix();
	//----------------------------sp--------------------------------------


	//UI
	RenderQuest();
	/*RenderTextOnScreen(meshList[GEO_TEXT], camerax, Color(0, 1, 0), 2, 6, 0);
	RenderTextOnScreen(meshList[GEO_TEXT], "X:", Color(0, 1, 0), 2, 0, 0);
	RenderTextOnScreen(meshList[GEO_TEXT], cameraz, Color(0, 1, 0), 2, 6, 2);
	RenderTextOnScreen(meshList[GEO_TEXT], "Z:", Color(0, 1, 0), 2, 0, 2);*/
	RenderTextOnScreen(meshList[GEO_TEXT], FPS, Color(0, 1, 0), 2, 47, 0);
	RenderTextOnScreen(meshList[GEO_TEXT], "FPS:", Color(0, 1, 0), 2, 40, 0);


	//currency on screen
	RenderTextOnScreen(meshList[GEO_TEXT], "$", Color(0, 1, 0), 3, 1, 49);
	RenderTextOnScreen(meshList[GEO_TEXT], std::to_string(Application::yourself.get_currency()), Color(0, 1, 0), 3, 2, 49);
	//RenderTextOnScreen(meshList[GEO_TEXT], "Currency:", Color(0, 1, 0), 2, 0, 50);



	/*RenderTextOnScreen(meshList[GEO_TEXT], std::to_string(playerhealth), Color(0, 1, 0), 2, 15, 50);
	RenderTextOnScreen(meshList[GEO_TEXT], "Health:", Color(0, 1, 0), 2, 0, 50);*/
	RenderTextOnScreen(meshList[GEO_TEXT], scammer_text, Color(0, 1, 0), 2, 0, 10);
	RenderMeshOnScreen(meshList[GEO_COIN_ICON], 5, 56, 10, 10);

	//int test;
	//test = intquest;
	//std::ostringstream ss;
	//ss.precision(3);
	//ss.str("");//clear ss
	//ss << "quest int :" << intquest;

	//RenderTextOnScreen(meshList[GEO_TEXT], ss.str(), Color(1, 1, 1), 2, 1, 40);

	//amt of coins collected on screen
	if(intquest > 1 && intquest < 4)
		RenderTextOnScreen(meshList[GEO_TEXT], "Coins collected : " + std::to_string(Application::yourself.get_coins_collected()) + " / 3", Color(0, 1, 0), 2, 2, 45);

	if (count < 150)
	{
		RenderMeshOnScreen(meshList[GEO_LV1], 40, 30, 85, 65);
	}

}

float SP::cabTP()
{
	Vector3 cabpos = cab_pos - camera.position;
	float cabdis = sqrt(pow(cabpos.x, 2) + pow(cabpos.y, 2) + pow(cabpos.z, 2)); 

	return cabdis;
}
void SP::Exit()
{
	// Cleanup VBO here
	for (int i = 0; i < NUM_GEOMETRY; ++i)
	{
		if (meshList[i])
		{
			delete meshList[i];
		}
	}
	glDeleteVertexArrays(1, &m_vertexArrayID);
	glDeleteProgram(m_programID);
}



