#include "Scene_LV2.h"
#include "GL\glew.h"
#include "Application.h"
#include "shader.hpp"
#include "Mtx44.h"
#include "MeshBuilder.h"
#include "Utility.h"
#include "LoadTGA.h"
#include "SP.h"
#include "Player.h"


Scene_LV2::Scene_LV2()
{

}

Scene_LV2::~Scene_LV2()
{
}

void Scene_LV2::RenderMeshOnScreen(Mesh* mesh, int x, int y, int sizex, int sizey)
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
void Scene_LV2::RenderMesh(Mesh* mesh, bool enableLight)
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
		glUniform1i(m_parameters[U_COLOR_TEXTURE],0);
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
void Scene_LV2::RenderSkybox()
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
void Scene_LV2::RenderFloor()
{
	modelStack.PushMatrix();
	modelStack.Translate(0, 0, 0);
	modelStack.Rotate(-90, 1, 0, 0);
	modelStack.Rotate(0, 0, 1, 0);
	modelStack.Scale(1000, 1000, 1000);
	RenderMesh(meshList[GEO_QUAD], true);
	modelStack.PopMatrix();
}

void Scene_LV2::RenderRoads()
{
	//SPAWN
	modelStack.PushMatrix();
	modelStack.Translate(0, 0.1f, -222);
	modelStack.Rotate(90, 0, 1, 0);
	modelStack.Scale(15.f, 0.1f, 100.f);
	RenderMesh(meshList[GEO_ROADFORBULLETS], true);
	modelStack.PopMatrix();

	modelStack.PushMatrix();
	modelStack.Translate(-37.4f, 6.f, -222);
	modelStack.Rotate(90, 0, 1, 0);
	modelStack.Scale(21, 25, 1);
	RenderMesh(meshList[GEO_TUNNEL], true);
	modelStack.PopMatrix();

	//first bunch
	modelStack.PushMatrix();
	modelStack.Translate(0, 0.1f, -157);
	modelStack.Rotate(90, 0, 1, 0);
	modelStack.Scale(58.f, 0.1f, 100.f);
	RenderMesh(meshList[GEO_ROADFORBULLETS], true);
	modelStack.PopMatrix();

	//second bunch
	modelStack.PushMatrix();
	modelStack.Translate(0.f, 0.1f, -96.5f);
	modelStack.Rotate(90, 0, 1, 0);
	modelStack.Scale(34.f, 0.1f, 100.f);
	RenderMesh(meshList[GEO_ROADFORBULLETS], true);
	modelStack.PopMatrix();

	//third bunch
	modelStack.PushMatrix();
	modelStack.Translate(0, 0.1f, -37.5f);
	modelStack.Rotate(90, 0, 1, 0);
	modelStack.Scale(56.f, 0.1f, 100.f);
	RenderMesh(meshList[GEO_ROADFORBULLETS], true);
	modelStack.PopMatrix();

	//fourth bunch
	modelStack.PushMatrix();
	modelStack.Translate(0, 0.1f, 50.5f);
	modelStack.Rotate(90, 0, 1, 0);
	modelStack.Scale(92.f, 0.1f, 100.f);
	RenderMesh(meshList[GEO_ROADFORBULLETS], true);
	modelStack.PopMatrix();


	
}


void Scene_LV2::RenderWall()
{
	//left wall
	
	modelStack.PushMatrix();
	modelStack.Translate(40, 1, 0);
	modelStack.Scale(5, 33, 550);
	RenderMesh(meshList[GEO_CUBE], true);
	modelStack.PopMatrix();



	//end wall
	modelStack.PushMatrix();
	modelStack.Translate(0, 1, 140);
	modelStack.Scale(100, 33, 5);
	RenderMesh(meshList[GEO_CUBE], true);
	modelStack.PopMatrix();


	//right wall
	modelStack.PushMatrix();
	modelStack.Translate(-40, 1, 0);
	modelStack.Scale(5, 33, 550);
	RenderMesh(meshList[GEO_CUBE], true);
	modelStack.PopMatrix();

	//spawn wall
	modelStack.PushMatrix();
	modelStack.Translate(0, 1, -250);
	modelStack.Scale(100, 33, 5);
	RenderMesh(meshList[GEO_CUBE], true);
	modelStack.PopMatrix();
}
void Scene_LV2::RenderTunnel()
{
	//spawn
	
	//left side
	modelStack.PushMatrix();
	modelStack.Rotate(-90, 0, 1, 0);
	modelStack.Translate(-180.f, 2.f, -37.1f);
	modelStack.Scale(19, 27, 1);
	RenderMesh(meshList[GEO_TUNNEL], false);//1.1
	modelStack.Translate(0.6f, 0.f, 0.1f);
	RenderMesh(meshList[GEO_TUNNEL], false);//1.2
	modelStack.Translate(0.6f, 0.f, 0.1f);
	RenderMesh(meshList[GEO_TUNNEL], false);//1.3
	modelStack.Translate(0.6f, 0.f, 0.1f);
	RenderMesh(meshList[GEO_TUNNEL], false);//1.4
	modelStack.Translate(0.6f, 0.f, 0.1f);
	RenderMesh(meshList[GEO_TUNNEL], false);//1.5
	modelStack.Translate(1.4f, 0.f, -0.79f);
	RenderMesh(meshList[GEO_TUNNEL], false);//2.1
	modelStack.Translate(0.6f, 0.f, 0.1f);
	RenderMesh(meshList[GEO_TUNNEL], false);//2.2
	modelStack.Translate(0.6f, 0.f, 0.1f);
	RenderMesh(meshList[GEO_TUNNEL], false);//2.3
	modelStack.Translate(1.3f, 0.f, -0.1f);
	RenderMesh(meshList[GEO_TUNNEL], false);//3.1
	modelStack.Translate(0.6f, 0.f, 0.1f);
	RenderMesh(meshList[GEO_TUNNEL], false);//3.2
	modelStack.Translate(0.6f, 0.f, 0.1f);
	RenderMesh(meshList[GEO_TUNNEL], false);//3.3
	modelStack.Translate(0.6f, 0.f, 0.1f);
	RenderMesh(meshList[GEO_TUNNEL], false);//3.4
	modelStack.Translate(0.6f, 0.f, 0.1f);
	RenderMesh(meshList[GEO_TUNNEL], false);//3.5
	modelStack.Translate(1.3f, 0.f, -0.5f);
	RenderMesh(meshList[GEO_TUNNEL], false);//4.1
	modelStack.Translate(0.6f, 0.f, 0.1f);
	RenderMesh(meshList[GEO_TUNNEL], false);//4.2
	modelStack.Translate(0.6f, 0.f, 0.1f);
	RenderMesh(meshList[GEO_TUNNEL], false);//4.3
	modelStack.Translate(0.6f, 0.f, 0.1f);
	RenderMesh(meshList[GEO_TUNNEL], false);//4.4
	modelStack.Translate(0.6f, 0.f, 0.1f);
	RenderMesh(meshList[GEO_TUNNEL], false);//4.5
	modelStack.Translate(0.6f, 0.f, 0.1f);
	RenderMesh(meshList[GEO_TUNNEL], false);//4.6
	modelStack.Translate(0.6f, 0.f, 0.1f);
	RenderMesh(meshList[GEO_TUNNEL], false);//4.7
	modelStack.Translate(0.6f, 0.f, 0.1f);
	RenderMesh(meshList[GEO_TUNNEL], false);//4.8
	modelStack.PopMatrix();


	//right side
	modelStack.PushMatrix();
	modelStack.Rotate(90, 0, 1, 0);
	modelStack.Translate(180.f, 2.f, -37.3f);
	modelStack.Scale(19, 27, 1);
	RenderMesh(meshList[GEO_TUNNEL], false);//1.1
	modelStack.Translate(-0.6f, 0.f, 0.1f);
	RenderMesh(meshList[GEO_TUNNEL], false);//1.2
	modelStack.Translate(-0.6f, 0.f, 0.1f);
	RenderMesh(meshList[GEO_TUNNEL], false);//1.3
	modelStack.Translate(-0.6f, 0.f, 0.1f);
	RenderMesh(meshList[GEO_TUNNEL], false);//1.4
	modelStack.Translate(-0.6f, 0.f, 0.1f);
	RenderMesh(meshList[GEO_TUNNEL], false);//1.5
	modelStack.Translate(-1.4f, 0.f, -0.59f);
	RenderMesh(meshList[GEO_TUNNEL], false);//2.1
	modelStack.Translate(-0.6f, 0.f, 0.1f);
	RenderMesh(meshList[GEO_TUNNEL], false);//2.2
	modelStack.Translate(-0.6f, 0.f, 0.1f);
	RenderMesh(meshList[GEO_TUNNEL], false);//2.3
	modelStack.Translate(-1.3f, 0.f, -0.1f);
	RenderMesh(meshList[GEO_TUNNEL], false);//3.1
	modelStack.Translate(-0.6f, 0.f, 0.1f);
	RenderMesh(meshList[GEO_TUNNEL], false);//3.2
	modelStack.Translate(-0.6f, 0.f, 0.1f);
	RenderMesh(meshList[GEO_TUNNEL], false);//3.3
	modelStack.Translate(-0.6f, 0.f, 0.1f);
	RenderMesh(meshList[GEO_TUNNEL], false);//3.4
	modelStack.Translate(-0.6f, 0.f, 0.1f);
	RenderMesh(meshList[GEO_TUNNEL], false);//3.5
	modelStack.Translate(-1.3f, 0.f, -0.5f);
	RenderMesh(meshList[GEO_TUNNEL], false);//4.1
	modelStack.Translate(-0.6f, 0.f, 0.1f);
	RenderMesh(meshList[GEO_TUNNEL], false);//4.2
	modelStack.Translate(-0.6f, 0.f, 0.1f);
	RenderMesh(meshList[GEO_TUNNEL], false);//4.3
	modelStack.Translate(-0.6f, 0.f, 0.1f);
	RenderMesh(meshList[GEO_TUNNEL], false);//4.4
	modelStack.Translate(-0.6f, 0.f, 0.1f);
	RenderMesh(meshList[GEO_TUNNEL], false);//4.5
	modelStack.Translate(-0.6f, 0.f, 0.1f);
	RenderMesh(meshList[GEO_TUNNEL], false);//4.6
	modelStack.Translate(-0.6f, 0.f, 0.1f);
	RenderMesh(meshList[GEO_TUNNEL], false);//4.7
	modelStack.Translate(-0.6f, 0.f, 0.1f);
	RenderMesh(meshList[GEO_TUNNEL], false);//4.8

	modelStack.PopMatrix();


	
	

}
void Scene_LV2::RenderPavement()
{
	modelStack.PushMatrix();
	modelStack.Translate(0, 0, -122);
	modelStack.Scale(70, 1, 10);
	RenderMesh(meshList[GEO_CUBE], true);
	modelStack.Translate(0.f, 0.f, 4.9f);
	RenderMesh(meshList[GEO_CUBE], true);
	modelStack.Translate(0.f, 0.f, 7.0f);
	RenderMesh(meshList[GEO_CUBE], true);
	modelStack.PopMatrix();

	modelStack.PushMatrix();
	modelStack.Translate(tp_sphere.x,tp_sphere.y,tp_sphere.z);
	RenderMesh(meshList[GEO_SPHERE], false);
	modelStack.PopMatrix();

	modelStack.PushMatrix();
	modelStack.Translate(shop_shpere.x, shop_shpere.y, shop_shpere.z);
	modelStack.Scale(7, 7, 7);
	RenderMesh(meshList[GEO_SHOP], true);
	modelStack.PopMatrix();

	


}
void Scene_LV2::RenderNPC()
{

	//npc1
	modelStack.PushMatrix();
	modelStack.Translate(translatenpc, 1.8f, npc1z);
	
	modelStack.Scale(2, 2.5, 1);
	RenderMesh(meshList[GEO_TORSO], true);
	//head
	{
		modelStack.PushMatrix();
		modelStack.Translate(0.f, 0.75f, 0.f);
		modelStack.Scale(0.5, 0.5, 1);
		RenderMesh(meshList[GEO_HEAD], true);
		modelStack.PopMatrix();
	}
	modelStack.PopMatrix();

	//npc2
	modelStack.PushMatrix();
	modelStack.Translate(translatenpc2, 1.8f, npc2z);

	modelStack.Scale(2, 2.5, 1);
	RenderMesh(meshList[GEO_TORSO], true);
	//head
	{
		modelStack.PushMatrix();
		modelStack.Translate(0.f, 0.75f, 0.f);
		modelStack.Scale(0.5, 0.5, 1);
		RenderMesh(meshList[GEO_HEAD], true);
		modelStack.PopMatrix();
	}
	modelStack.PopMatrix();

	//npc3
	modelStack.PushMatrix();
	modelStack.Translate(translatenpc3, 1.8f, npc3z);

	modelStack.Scale(2, 2.5, 1);
	RenderMesh(meshList[GEO_TORSO], true);
	//head
	{
		modelStack.PushMatrix();
		modelStack.Translate(0.f, 0.75f, 0.f);
		modelStack.Scale(0.5, 0.5, 1);
		RenderMesh(meshList[GEO_HEAD], true);
		modelStack.PopMatrix();
	}
	modelStack.PopMatrix();

	//npc 4
	modelStack.PushMatrix();
	modelStack.Translate(translatenpc4, 1.8f, npc4z);

	modelStack.Scale(2, 2.5, 1);
	RenderMesh(meshList[GEO_TORSO], true);
	//head
	{
		modelStack.PushMatrix();
		modelStack.Translate(0.f, 0.75f, 0.f);
		modelStack.Scale(0.5, 0.5, 1);
		RenderMesh(meshList[GEO_HEAD], true);
		modelStack.PopMatrix();
	}
	modelStack.PopMatrix();








}
void Scene_LV2::RenderBullets()
{
	//first bunch
	{
	//bullet1
	modelStack.PushMatrix();
	modelStack.Translate(bullet1.x, bullet1.y, bullet1.z);
	//modelStack.Rotate(90, 0, 0, -1);
	modelStack.Rotate(rotatebullet, 0, 1, 0);
	modelStack.Scale(3, 3, 5);
	RenderMesh(meshList[GEO_BULLET], true);
	modelStack.PopMatrix();
	//bullet2
	modelStack.PushMatrix();
	modelStack.Translate(translation_x_bullet_fast, bullet2.y, bullet2.z);
	//modelStack.Rotate(90, 0, 0, -1);
	modelStack.Rotate(rotatebullet, 0, 1, 0);
	modelStack.Scale(3, 3, 5);
	RenderMesh(meshList[GEO_BULLET2], true);
	modelStack.PopMatrix();
	//bullet3
	modelStack.PushMatrix();
	modelStack.Translate(-translation_x_bullet_medium, bullet3.y, bullet3.z);
	//modelStack.Rotate(-90, 0, 0, -1);
	modelStack.Rotate(-rotatebullet, 0, 1, 0);
	modelStack.Scale(3, 3, 5);
	RenderMesh(meshList[GEO_BULLET], true);
	modelStack.PopMatrix();
	//bullet4
	modelStack.PushMatrix();
	modelStack.Translate(translation_x_bullet_slow, bullet4.y, bullet4.z);
	//modelStack.Rotate(-90, 0, 0, -1);
	modelStack.Rotate(-rotatebullet, 0, 1, 0);
	modelStack.Scale(3, 3, 5);
	RenderMesh(meshList[GEO_BULLET2], true);
	modelStack.PopMatrix();
	//bullet5
	modelStack.PushMatrix();
	modelStack.Translate(translation_x_bullet, bullet5.y, bullet5.z);
	//modelStack.Rotate(90, 0, 0, -1);
	modelStack.Rotate(rotatebullet, 0, 1, 0);
	modelStack.Scale(3, 3, 5);
	RenderMesh(meshList[GEO_BULLET2], true);
	modelStack.PopMatrix();
	}

	//second bunch
	{
		modelStack.PushMatrix();
		modelStack.Translate(translation_x_bullet_fast, bullet6.y, bullet6.z);
		modelStack.Rotate(rotatebullet, 0, 1, 0);
		modelStack.Scale(3, 3, 5);
		RenderMesh(meshList[GEO_BULLET2], true);
		modelStack.PopMatrix();
		//bullet2
		modelStack.PushMatrix();
		modelStack.Translate(translation_x_bullet_medium, bullet7.y, bullet7.z);
		modelStack.Rotate(rotatebullet, 0, 1, 0);
		modelStack.Scale(3, 3, 5);
		RenderMesh(meshList[GEO_BULLET4], true);
		modelStack.PopMatrix();
		//bullet3
		modelStack.PushMatrix();
		modelStack.Translate(-translation_x_bullet_fast, bullet8.y, bullet8.z);
		modelStack.Rotate(-rotatebullet, 0, 1, 0);
		modelStack.Scale(3, 3, 5);
		RenderMesh(meshList[GEO_BULLET2], true);
		modelStack.PopMatrix();
	}
	//third bunch
	{
		modelStack.PushMatrix();
		modelStack.Translate(translation_x_bullet_fast, bullet9.y, bullet9.z);
		modelStack.Rotate(rotatebullet, 0, 1, 0);
		modelStack.Scale(3, 3, 5);
		RenderMesh(meshList[GEO_BULLET], true);
		modelStack.PopMatrix();
		
		modelStack.PushMatrix();
		modelStack.Translate(translation_x_bullet_medium, bullet10.y, bullet10.z);
		modelStack.Rotate(rotatebullet, 0, 1, 0);
		modelStack.Scale(3, 3, 5);
		RenderMesh(meshList[GEO_BULLET4], true);
		modelStack.PopMatrix();
		
		modelStack.PushMatrix();
		modelStack.Translate(-translation_x_bullet_fast, bullet11.y, bullet11.z);
		modelStack.Rotate(-rotatebullet, 0, 1, 0);
		modelStack.Scale(3, 3, 5);
		RenderMesh(meshList[GEO_BULLET3], true);
		modelStack.PopMatrix();
		
		modelStack.PushMatrix();
		modelStack.Translate(translation_x_bullet_medium, bullet12.y, bullet12.z);
		modelStack.Rotate(-rotatebullet, 0, 1, 0);
		modelStack.Scale(3, 3, 5);
		RenderMesh(meshList[GEO_BULLET], true);
		modelStack.PopMatrix();
	
		modelStack.PushMatrix();
		modelStack.Translate(-translation_x_bullet_fast, bullet13.y, bullet13.z);
		modelStack.Rotate(-rotatebullet, 0, 1, 0);
		modelStack.Scale(3, 3, 5);
		RenderMesh(meshList[GEO_BULLET4], true);
		modelStack.PopMatrix();
	}
	//fourth bunch
	modelStack.PushMatrix();
	modelStack.Translate(translation_x_bullet_fast, bullet14.y, bullet14.z);
	modelStack.Rotate(rotatebullet, 0, 1, 0);
	modelStack.Scale(3, 3, 5);
	RenderMesh(meshList[GEO_BULLET], true);
	modelStack.PopMatrix();

	modelStack.PushMatrix();
	modelStack.Translate(-translation_x_bullet_medium, bullet15.y, bullet15.z);
	modelStack.Rotate(-rotatebullet, 0, 1, 0);
	modelStack.Scale(3, 3, 5);
	RenderMesh(meshList[GEO_BULLET2], true);
	modelStack.PopMatrix();

	modelStack.PushMatrix();
	modelStack.Translate(translation_x_bullet, bullet16.y, bullet16.z);
	modelStack.Rotate(rotatebullet, 0, 1, 0);
	modelStack.Scale(3, 3, 5);
	RenderMesh(meshList[GEO_BULLET2], true);
	modelStack.PopMatrix();

	modelStack.PushMatrix();
	modelStack.Translate(-translation_x_bullet_slow, bullet17.y, bullet17.z);
	modelStack.Rotate(-rotatebullet, 0, 1, 0);
	modelStack.Scale(3, 3, 5);
	RenderMesh(meshList[GEO_BULLET3], true);
	modelStack.PopMatrix();

	modelStack.PushMatrix();
	modelStack.Translate(-translation_x_bullet_medium, bullet18.y, bullet18.z);
	modelStack.Rotate(-rotatebullet, 0, 1, 0);
	modelStack.Scale(3, 3, 5);
	RenderMesh(meshList[GEO_BULLET2], true);
	modelStack.PopMatrix();

	modelStack.PushMatrix();
	modelStack.Translate(-translation_x_bullet_fast, bullet19.y, bullet19.z);
	modelStack.Rotate(-rotatebullet, 0, 1, 0);
	modelStack.Scale(3, 3, 5);
	RenderMesh(meshList[GEO_BULLET], true);
	modelStack.PopMatrix();

	modelStack.PushMatrix();
	modelStack.Translate(translation_x_bullet, bullet20.y, bullet20.z);
	modelStack.Rotate(rotatebullet, 0, 1, 0);
	modelStack.Scale(3, 3, 5);
	RenderMesh(meshList[GEO_BULLET2], true);
	modelStack.PopMatrix();

	modelStack.PushMatrix();
	modelStack.Translate(-translation_x_bullet_fast, bullet21.y, bullet21.z);
	modelStack.Rotate(-rotatebullet, 0, 1, 0);
	modelStack.Scale(3, 3, 5);
	RenderMesh(meshList[GEO_BULLET3], true);
	modelStack.PopMatrix();
}
bool Scene_LV2::colisiondetectionbullets(Vector3 bullet, Vector3 player)
{

	if ((player.x > bullet.x - 4 && player.x < bullet.x + 4) && (player.z > bullet.z - 4 && player.z < bullet.z + 4))
	{
		bloodui = true;
		return true;

	}
	else
	{
		
		return false;

	}

	
}
void Scene_LV2::RenderQuestion()
{
	if (intnpc1 == 1 && onpavement(1))
		RenderMeshOnScreen(meshList[GEO_QUESTION1], 40, 30, 45, 35);
	else if (intnpc1 == 2 && onpavement(1))
		RenderMeshOnScreen(meshList[GEO_QUESTION1_CORRECT], 40, 30, 45, 35);
	else if (intnpc1 == 3 && onpavement(1))
		RenderMeshOnScreen(meshList[GEO_QUESTION1_WRONG], 40, 30, 45, 35);

	if (intnpc2 == 1 && onpavement(2))
		RenderMeshOnScreen(meshList[GEO_QUESTION2], 40, 30, 45, 35);
	else if (intnpc2 == 2 && onpavement(2))
		RenderMeshOnScreen(meshList[GEO_QUESTION2_CORRECT], 40, 30, 45, 35);
	else if (intnpc2 == 3 && onpavement(2))
		RenderMeshOnScreen(meshList[GEO_QUESTION2_WRONG], 40, 30, 45, 35);

	if (intnpc3 == 1 && onpavement(3))
		RenderMeshOnScreen(meshList[GEO_QUESTION3], 40, 30, 45, 35);
	else if (intnpc3 == 2 && onpavement(3))
		RenderMeshOnScreen(meshList[GEO_QUESTION3_CORRECT], 40, 30, 45, 35);
	else if (intnpc3 == 3 && onpavement(3))
		RenderMeshOnScreen(meshList[GEO_QUESTION3_WRONG], 40, 30, 45, 35);

	if (intnpc4 == 1 && onpavement(4))
		RenderMeshOnScreen(meshList[GEO_QUESTION4], 40, 30, 45, 35);
	else if (intnpc4 == 2 && onpavement(4))
		RenderMeshOnScreen(meshList[GEO_QUESTION4_CORRECT], 40, 30, 45, 35);
	else if (intnpc4== 3 && onpavement(4))
		RenderMeshOnScreen(meshList[GEO_QUESTION4_WRONG], 40, 30, 45, 35);

	
}
bool Scene_LV2::onpavement(int pavementint)
{
	if ((player.x < 34 && player.x > -34) && (player.z < -117 && player.z > -126) && pavementint == 1)
	{
		//std::cout << "onpavement1";
		return true;

	}
	else if ((player.x < 34 && player.x > -34) && (player.z < -67 && player.z > -78) && pavementint ==2)
	{
		//std::cout << "onpavement2";
		return true;
	}
	else if ((player.x < 34 && player.x > 2) && (player.z < 1 && player.z > -8) && pavementint == 3)
	{
		//std::cout << "onpavement3";
		return true;
	}
	else if ((player.x < -2 && player.x > -34) && (player.z < 1 && player.z > -8) && pavementint == 4)
	{
		//std::cout << "onpavement4";
		return true;
	}
	else
		return false;

		


	
}
void Scene_LV2::RenderLightpole()
{
	modelStack.PushMatrix();
	modelStack.Translate(0, 0, -2);
	modelStack.Scale(30, 30, 30);
	RenderMesh(meshList[GEO_LIGHTPOLE], true);
	modelStack.PopMatrix();
}
void Scene_LV2::RenderTaxi()
{

	modelStack.PushMatrix();
	modelStack.Translate(0, 0, -222);
	modelStack.Rotate(40, 0, 1, 0);
	RenderMesh(meshList[GEO_TAXI], true);

	modelStack.PopMatrix();
}
void Scene_LV2::RenderText(Mesh * mesh, std::string text, Color color)
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
void Scene_LV2::RenderTextOnScreen(Mesh* mesh, std::string text, Color color, float size, float x, float y)
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
void Scene_LV2::Init()
{
	

	
	// Set background colour to light purple
	glClearColor(0.5f, 0.0f, 0.7f, 0.0f);
	// Enable depth test
	glEnable(GL_DEPTH_TEST);
	glDisable(GL_CULL_FACE);
	// Enable blending
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	camera.Init(Vector3(0, 3, -220), Vector3(0, 3, 10), Vector3(0, 1, 0));

	m_programID = LoadShaders("Shader//Texture.vertexshader", "Shader//Text.fragmentshader");
	m_parameters[U_MVP] = glGetUniformLocation(m_programID, "MVP");
	m_parameters[U_MODELVIEW] = glGetUniformLocation(m_programID, "MV");
	m_parameters[U_MODELVIEW_INVERSE_TRANSPOSE] = glGetUniformLocation(m_programID, "MV_inverse_transpose");
	m_parameters[U_MATERIAL_AMBIENT] = glGetUniformLocation(m_programID, "material.kAmbient");
	m_parameters[U_MATERIAL_DIFFUSE] = glGetUniformLocation(m_programID, "material.kDiffuse");
	m_parameters[U_MATERIAL_SPECULAR] = glGetUniformLocation(m_programID, "material.kSpecular");
	m_parameters[U_MATERIAL_SHININESS] = glGetUniformLocation(m_programID, "material.kShininess");

	//Replace previous code
	light[0].type = Light::LIGHT_SPOT;
	light[0].position.Set(-100, 20, 25);
	light[0].color.Set(1, 1, 1);
	light[0].power = 1;
	light[0].kC = 1.f;
	light[0].kL = 0.01f;
	light[0].kQ = 0.001f;
	light[0].cosCutoff = cos(Math::DegreeToRadian(45));
	light[0].cosInner = cos(Math::DegreeToRadian(30));
	light[0].exponent = 3.f;
	light[0].spotDirection.Set(0.f, 1.f, 0.f);


	m_parameters[U_LIGHT0_POSITION] = glGetUniformLocation(m_programID, "lights[0].position_cameraspace");
	m_parameters[U_LIGHT0_COLOR] = glGetUniformLocation(m_programID, "lights[0].color");
	m_parameters[U_LIGHT0_POWER] = glGetUniformLocation(m_programID, "lights[0].power");
	m_parameters[U_LIGHT0_KC] = glGetUniformLocation(m_programID, "lights[0].kC");
	m_parameters[U_LIGHT0_KL] = glGetUniformLocation(m_programID, "lights[0].kL");
	m_parameters[U_LIGHT0_KQ] = glGetUniformLocation(m_programID, "lights[0].kQ");
	m_parameters[U_LIGHT0_TYPE] = glGetUniformLocation(m_programID, "lights[0].type");
	m_parameters[U_LIGHT0_SPOTDIRECTION] = glGetUniformLocation(m_programID, "lights[0].spotDirection");
	m_parameters[U_LIGHT0_COSCUTOFF] = glGetUniformLocation(m_programID, "lights[0].cosCutoff");
	m_parameters[U_LIGHT0_COSINNER] = glGetUniformLocation(m_programID, "lights[0].cosInner");
	m_parameters[U_LIGHT0_EXPONENT] = glGetUniformLocation(m_programID, "lights[0].exponent");

	//m_parameters[U_LIGHTENABLED] = glGetUniformLocation(m_programID, "lightEnabled");
	//m_parameters[U_NUMLIGHTS] = glGetUniformLocation(m_programID, "numLights");

	light[1].type = Light::LIGHT_POINT;
	light[1].position.Set(10, 8, 20);
	light[1].color.Set(1, 1, 1);
	light[1].power = 0.8;
	light[1].kC = 1.f;
	light[1].kL = 0.01f;
	light[1].kQ = 0.001f;
	light[1].cosCutoff = cos(Math::DegreeToRadian(45));
	light[1].cosInner = cos(Math::DegreeToRadian(30));
	light[1].exponent = 3.f;
	light[1].spotDirection.Set(0.f, 1.f, 0.f);


	m_parameters[U_LIGHT1_POSITION] = glGetUniformLocation(m_programID, "lights[1].position_cameraspace");
	m_parameters[U_LIGHT1_COLOR] = glGetUniformLocation(m_programID, "lights[1].color");
	m_parameters[U_LIGHT1_POWER] = glGetUniformLocation(m_programID, "lights[1].power");
	m_parameters[U_LIGHT1_KC] = glGetUniformLocation(m_programID, "lights[1].kC");
	m_parameters[U_LIGHT1_KL] = glGetUniformLocation(m_programID, "lights[1].kL");
	m_parameters[U_LIGHT1_KQ] = glGetUniformLocation(m_programID, "lights[1].kQ");
	m_parameters[U_LIGHT1_TYPE] = glGetUniformLocation(m_programID, "lights[1].type");
	m_parameters[U_LIGHT1_SPOTDIRECTION] = glGetUniformLocation(m_programID, "lights[1].spotDirection");
	m_parameters[U_LIGHT1_COSCUTOFF] = glGetUniformLocation(m_programID, "lights[1].cosCutoff");
	m_parameters[U_LIGHT1_COSINNER] = glGetUniformLocation(m_programID, "lights[1].cosInner");
	m_parameters[U_LIGHT1_EXPONENT] = glGetUniformLocation(m_programID, "lights[1].exponent");

	m_parameters[U_LIGHTENABLED] = glGetUniformLocation(m_programID, "lightEnabled");
	m_parameters[U_NUMLIGHTS] = glGetUniformLocation(m_programID, "numLights");


	// Get a handle for our "colorTexture" uniform
	m_parameters[U_COLOR_TEXTURE_ENABLED] = glGetUniformLocation(m_programID, "colorTextureEnabled");
	m_parameters[U_COLOR_TEXTURE] = glGetUniformLocation(m_programID, "colorTexture");

	// Get a handle for our "textColor" uniform
	m_parameters[U_TEXT_ENABLED] = glGetUniformLocation(m_programID, "textEnabled");
	m_parameters[U_TEXT_COLOR] = glGetUniformLocation(m_programID, "textColor");
	glUseProgram(m_programID);

	// Make sure you pass uniform parameters after glUseProgram()
	// Make sure you pass uniform parameters after glUseProgram()
	glUniform1i(m_parameters[U_NUMLIGHTS], 2);
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

	// Generate a default VAO for now
	glGenVertexArrays(1, &m_vertexArrayID);
	glBindVertexArray(m_vertexArrayID);	
	rotateAngle = 0;
	for (int i = 0; i < NUM_GEOMETRY; ++i)
	{
		meshList[i] = nullptr;
	}

	{
		//meshList[GEO_AXES] = MeshBuilder::GenerateAxes("reference", 1000, 1000, 1000);
		meshList[GEO_QUAD] = MeshBuilder::GenerateQuad("quad", Color(0, 0, 0), 1.f);
		meshList[GEO_QUAD]->textureID = LoadTGA("Image//floor.tga");
		meshList[GEO_CUBE] = MeshBuilder::GenerateCube("cube", Color(1, 1, 1), 1.f);
		meshList[GEO_CUBE]->textureID = LoadTGA("Image//color.tga");
		meshList[GEO_SPHERE] = MeshBuilder::GenerateSphere("sphere", Color(0, 0, 0), 10, 20, 1.f);
		//meshList[GEO_CYLINDER] = MeshBuilder::GenerateCylinder("cylinder", Color(1, 1, 1), 20, 1.f, 1.f);
		//meshList[GEO_HEMISPHERE] = MeshBuilder::GenerateHemisphere("hemisphere", Color(1, 1, 1), 10, 20, 1.f);
		//meshList[GEO_TORUS] = MeshBuilder::GenerateTorus("torus", Color(0.3, 0.3, 0.3), 20, 20, 1.f, 0.5f);
		//meshList[GEO_HALFTORUS] = MeshBuilder::GenerateHalfTorus("quartertorus", Color(1, 1, 1), 10, 20, 1.f, 0.5f);

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
		meshList[GEO_TUNNEL] = MeshBuilder::GenerateQuad("tunnel", Color(1, 1, 1), 1.f);
		meshList[GEO_TUNNEL]->textureID = LoadTGA("Image//tunnel.tga");


		meshList[GEO_SHOP_UI] = MeshBuilder::GenerateQuad("shop_ui", Color(1, 1, 1), 1.f);
		meshList[GEO_SHOP_UI]->textureID = LoadTGA("Image//shop_ui.tga");

		meshList[GEO_SCAMMED] = MeshBuilder::GenerateQuad("scammed", Color(1, 1, 1), 1.f);
		meshList[GEO_SCAMMED]->textureID = LoadTGA("Image//scammed.tga");

		meshList[GEO_LAMPLIGHT] = MeshBuilder::GenerateSphere("sphere", Color(0.4, 0.4, 0.4), 10, 20, 1.f);			//limbs
		meshList[GEO_CAPE] = MeshBuilder::GenerateCylinder("cylinder", Color(0.3, 0.3, 0.3), 20, 1.f, 1.f);		//base of head



		//meshList[GEO_CONE] = MeshBuilder::GenerateCone("cone", Color(0.3, 0.3, 0.3), 20, 1.f, 1.f);   //cape end

		meshList[GEO_LIGHTBALL] = MeshBuilder::GenerateSphere("sphere", Color(1, 1, 1), 10, 20, 1.f);		//light for lamppost



		meshList[GEO_HORNET] = MeshBuilder::GenerateOBJMTL("hornet", "OBJ//zombie.obj", "OBJ//zombie.mtl");


		//meshList[GEO_SHOP] = MeshBuilder::GenerateSphere("sphere", Color(1, 0, 0), 10, 20, 1.f);


		meshList[GEO_ROCKS] = MeshBuilder::GenerateOBJMTL("rocks", "OBJ//rocks.obj", "OBJ//rocks.mtl");
		meshList[GEO_ROCKS]->textureID = LoadTGA("Image//rockwall.tga");
		meshList[GEO_TALLROCK] = MeshBuilder::GenerateOBJMTL("tallrock", "OBJ//rocksTall.obj", "OBJ//rocksTall.mtl");
		meshList[GEO_TALLROCK]->textureID = LoadTGA("Image//rockwall.tga");
		meshList[GEO_STONEWALL] = MeshBuilder::GenerateOBJMTL("stonewall", "OBJ//stoneWall.obj", "OBJ//stoneWall.mtl");
		meshList[GEO_STONEWALLDAMAGED] = MeshBuilder::GenerateOBJMTL("stonewalldamaged", "OBJ//stoneWallDamaged.obj", "OBJ//stoneWallDamaged.mtl");
		meshList[GEO_ROAD] = MeshBuilder::GenerateOBJMTL("road", "OBJ//road.obj", "OBJ//road.mtl");
		meshList[GEO_CRYPT] = MeshBuilder::GenerateOBJMTL("crypt", "OBJ//crypt.obj", "OBJ//crypt.mtl");
		meshList[GEO_DEBRIS] = MeshBuilder::GenerateOBJMTL("debris", "OBJ//debris.obj", "OBJ//debris.mtl");
		meshList[GEO_DEBRIS]->textureID = LoadTGA("Image//rockwall.tga");
		meshList[GEO_LAMP] = MeshBuilder::GenerateOBJMTL("debris", "OBJ//lightpostDouble.obj", "OBJ//lightpostDouble.mtl");
		meshList[GEO_GRAVE] = MeshBuilder::GenerateOBJMTL("grave", "OBJ//grave.obj", "OBJ//grave.mtl");
		meshList[GEO_LANTERN] = MeshBuilder::GenerateOBJMTL("lantern", "OBJ//lanternCandle.obj", "OBJ//lanternCandle.mtl");
		meshList[GEO_SHOP] = MeshBuilder::GenerateOBJMTL("shop", "OBJ//stallGreen.obj", "OBJ//stallGreen.mtl");

		/*meshList[GEO_BULLET] = MeshBuilder::GenerateOBJMTL("bullet", "OBJ//ironFenceBar.obj", "OBJ//ironFenceBar.mtl");*/
		meshList[GEO_BULLET] = MeshBuilder::GenerateQuad("bullet", Color(1, 1, 1), 1.f);
		meshList[GEO_BULLET]->textureID = LoadTGA("Image//internetscam.tga");

		meshList[GEO_BULLET2] = MeshBuilder::GenerateQuad("bullet", Color(1, 1, 1), 1.f);
		meshList[GEO_BULLET2]->textureID = LoadTGA("Image//phonelove.tga");
		meshList[GEO_BULLET3] = MeshBuilder::GenerateQuad("bullet", Color(1, 1, 1), 1.f);
		meshList[GEO_BULLET3]->textureID = LoadTGA("Image//scam1.tga");
		meshList[GEO_BULLET4] = MeshBuilder::GenerateQuad("bullet", Color(1, 1, 1), 1.f);
		meshList[GEO_BULLET4]->textureID = LoadTGA("Image//scam2.tga");
	}

		meshList[GEO_TEXT] = MeshBuilder::GenerateText("text", 16, 16);
		meshList[GEO_TEXT]->textureID = LoadTGA("Image//ArialFont.tga");
		meshList[GEO_TEXT2] = MeshBuilder::GenerateText("text2", 16, 16);
		meshList[GEO_TEXT2]->textureID = LoadTGA("Image//comicsans.tga");

		meshList[GEO_COIN_ICON] = MeshBuilder::GenerateQuad("coin_icon", Color(1, 1, 1), 1.f);
		meshList[GEO_COIN_ICON]->textureID = LoadTGA("Image//coin_icon.tga");

		meshList[GEO_LIGHTPOLE] = MeshBuilder::GenerateOBJMTL("lightpole", "OBJ//light_squareDouble.obj", "OBJ//light_squareDouble.mtl");
		//-----------------------------------------------------------------------
		//SP
		/*meshList[GEO_SCAMMER] = MeshBuilder::GenerateOBJ("scam","OBJ//scammer.obj");
		meshList[GEO_SCAMMER]->textureID = LoadTGA("Image//scammer.tga");
		*/
		//***myownscammerfornow***
		meshList[GEO_HEAD] = MeshBuilder::GenerateCube("cube", Color(1, 0, 0), 1.f);
		meshList[GEO_TORSO] = MeshBuilder::GenerateCube("cube", Color(0, 0, 1), 1.f);

		//**questions & correct/wrong
		meshList[GEO_QUESTION1] = MeshBuilder::GenerateQuad("quad", Color(0, 0, 0), 1.f);
		meshList[GEO_QUESTION1]->textureID = LoadTGA("Image//question_1.tga");

		meshList[GEO_QUESTION1_CORRECT] = MeshBuilder::GenerateQuad("quad", Color(0, 0, 0), 1.f);
		meshList[GEO_QUESTION1_CORRECT]->textureID = LoadTGA("Image//question_1_correct.tga");

		meshList[GEO_QUESTION1_WRONG] = MeshBuilder::GenerateQuad("quad", Color(0, 0, 0), 1.f);
		meshList[GEO_QUESTION1_WRONG]->textureID = LoadTGA("Image//question_1_wrong.tga");

		meshList[GEO_QUESTION2] = MeshBuilder::GenerateQuad("quad", Color(0, 0, 0), 1.f);
		meshList[GEO_QUESTION2]->textureID = LoadTGA("Image//question_2.tga");

		meshList[GEO_QUESTION2_CORRECT] = MeshBuilder::GenerateQuad("quad", Color(0, 0, 0), 1.f);
		meshList[GEO_QUESTION2_CORRECT]->textureID = LoadTGA("Image//question_2_correct.tga");

		meshList[GEO_QUESTION2_WRONG] = MeshBuilder::GenerateQuad("quad", Color(0, 0, 0), 1.f);
		meshList[GEO_QUESTION2_WRONG]->textureID = LoadTGA("Image//question_2_wrong.tga");


		meshList[GEO_QUESTION3] = MeshBuilder::GenerateQuad("quad", Color(0, 0, 0), 1.f);
		meshList[GEO_QUESTION3]->textureID = LoadTGA("Image//question_3.tga");

		meshList[GEO_QUESTION3_CORRECT] = MeshBuilder::GenerateQuad("quad", Color(0, 0, 0), 1.f);
		meshList[GEO_QUESTION3_CORRECT]->textureID = LoadTGA("Image//question_3_correct.tga");

		meshList[GEO_QUESTION3_WRONG] = MeshBuilder::GenerateQuad("quad", Color(0, 0, 0), 1.f);
		meshList[GEO_QUESTION3_WRONG]->textureID = LoadTGA("Image//question_3_wrong.tga");


		meshList[GEO_QUESTION4] = MeshBuilder::GenerateQuad("quad", Color(0, 0, 0), 1.f);
		meshList[GEO_QUESTION4]->textureID = LoadTGA("Image//question_4.tga");

		meshList[GEO_QUESTION4_CORRECT] = MeshBuilder::GenerateQuad("quad", Color(0, 0, 0), 1.f);
		meshList[GEO_QUESTION4_CORRECT]->textureID = LoadTGA("Image//question_4_correct.tga");

		meshList[GEO_QUESTION4_WRONG] = MeshBuilder::GenerateQuad("quad", Color(0, 0, 0), 1.f);
		meshList[GEO_QUESTION4_WRONG]->textureID = LoadTGA("Image//question_4_wrong.tga");

	
		meshList[GEO_BLOOD] = MeshBuilder::GenerateQuad("blood_gui", Color(1, 1, 1), 1.f);
		meshList[GEO_BLOOD]->textureID = LoadTGA("Image//Blood.tga");

		meshList[GEO_LV2] = MeshBuilder::GenerateQuad("lv2", Color(1, 1, 1), 1.f);
		meshList[GEO_LV2]->textureID = LoadTGA("Image//lv2.tga");

		meshList[GEO_LV3] = MeshBuilder::GenerateQuad("lv3", Color(1, 1, 1), 1.f);
		meshList[GEO_LV3]->textureID = LoadTGA("Image//lv3.tga");

		meshList[GEO_TAXI_LOGO] = MeshBuilder::GenerateQuad("taxi_logo", Color(1, 1, 1), 1.f);
		meshList[GEO_TAXI_LOGO]->textureID = LoadTGA("Image//taxi_logo.tga");

		meshList[GEO_ROADFORBULLETS] = MeshBuilder::GenerateCube("roadforbullets", Color(0.5f, 0.4f, 0.43f), 1.f);

		meshList[GEO_TAXI] = MeshBuilder::GenerateOBJ("coin", "OBJ//Taxi2.obj");
		meshList[GEO_TAXI]->textureID = LoadTGA("Image//taxi.tga");

	

		Mtx44 projection;
		projection.SetToPerspective(45.f, 4.f / 3.f, 0.1f, 1000.f);
		projectionStack.LoadMatrix(projection);


		Mesh::SetMaterialLoc(m_parameters[U_MATERIAL_AMBIENT],
			m_parameters[U_MATERIAL_DIFFUSE],
			m_parameters[U_MATERIAL_SPECULAR],
			m_parameters[U_MATERIAL_SHININESS]);






		//npc
		interactnpc1 = false;
		intnpc1 = 0;
		npc1x = translatenpc;
		npc1z = -122;

		interactnpc2 = false;
		intnpc2 = 0;
		npc2x = translatenpc2 = 0;
		npc2z = -73;

		interactnpc3 = false;
		intnpc3 = 0;
		npc3x = translatenpc3 = 0;
		npc3z = -3.f;

		interactnpc4 = false;
		intnpc4 = 0;
		npc4x = translatenpc4 = 0;
		npc4z = -3.f;
		//player vector set
		player.x = camera.position.x;
		player.y = camera.position.y;
		player.z = camera.position.z;

		//translation for bullets
		translation_x_bullet = -38;
		//bullets vectors
		

		//first bunch
		bullet1.x = 0;
		bullet1.y = 2;
		bullet1.z = -180;
		bullet2.y = 2;
		bullet2.z = -168;
		bullet3.y = 2;
		bullet3.z = -157;
		bullet4.y = 2;
		bullet4.z = -145;
		bullet5.y = 2;
		bullet5.z = -134;

		//second bunch
		bullet6.y = 3;
		bullet6.z = -107;
		bullet7.y = 3;
		bullet7.z = -96;
		bullet8.y = 3;
		bullet8.z = -84;
		
		//third bunch
		bullet9.y = 2;
		bullet9.z = -60;
		bullet10.y = 2;
		bullet10.z = -49;
		bullet11.y = 2;
		bullet11.z = -37;
		bullet12.y = 2;
		bullet12.z = -26;
		bullet13.y = 2;
		bullet13.z = -14;

		//fourth bunch
		bullet14.y = 2;
		bullet14.z = 9;
		bullet15.y = 2;
		bullet15.z = 21;
		bullet16.y = 2;
		bullet16.z = 33;
		bullet17.y = 2;
		bullet17.z = 44;
		bullet18.y = 2;
		bullet18.z = 55;
		bullet19.y = 2;
		bullet19.z = 67;
		bullet20.y = 2;
		bullet20.z = 78;
		bullet21.y = 2;
		bullet21.z = 90;

		bloodui = false;
		count = 0;
}

void Scene_LV2::Update(double dt)
{
	count2++;
	if (count3_start == true)
	{
		count3++;
	}
	std::cout << count3 << std::endl;
	move_car++;
	
	
	npc1x = translatenpc;
	npc2x = translatenpc2;
	npc3x = translatenpc3;
	npc4x = translatenpc4;

	bullet1.x = translation_x_bullet;
	bullet5.x = translation_x_bullet;

	rotatebullet += (float)(70 * dt);
	if (Application::yourself.get_currency() <= 0)
	{
		//Application::IsKeyPressed(VK_F3) == true;
		Application::yourself.set_die();
	}

	if (shopdis < 10)
	{
		shop_speech = "Press E to open shop";
		if (shop_open == false)
		{
			if (Application::yourself.get_shop1() == false)
			{
				camera.ZOOM_SPEED = 20.0f;
			}
			else
			{
				camera.ZOOM_SPEED = 40.0f;
			}
			camera.CAMERA_SPEED = 80.0f;
			
			if (Application::IsKeyPressed('E'))
			{
				shop_open = true;
				display_shop_ui = true;
			}
		}
		else if (shop_open == true)
		{
			camera.ZOOM_SPEED = 0.0f;
			camera.CAMERA_SPEED = 0.0f;
			shop_speech = "";
			if (Application::IsKeyPressed('Q'))
			{
				shop_open = false;
				display_shop_ui = false;
			}
			if (Application::IsKeyPressed('1') && Application::yourself.get_currency() > 20 && Application::yourself.get_shop1() == false)
			{
				//faster movespd
				Application::yourself.currency_deducted(20);
				Application::yourself.set_shop1(true);
			}
			if (Application::IsKeyPressed('2') && Application::yourself.get_currency() > 20 && Application::yourself.get_shop2() == false)
			{
				//slower arrows
				Application::yourself.currency_deducted(20);
				Application::yourself.set_shop2(true);
			}
			if (Application::IsKeyPressed('3') && Application::yourself.get_currency() > 20 && Application::yourself.get_shop3() == false)
			{
				//scammed
				scammed_screen = true;
				Application::yourself.currency_deducted(20);
				Application::yourself.set_shop3(true);
			}
			if (Application::IsKeyPressed('4') && Application::yourself.get_currency() > 20 && Application::yourself.get_shop4() == false)
			{
				//scammed
				scammed_screen = true;
				Application::yourself.currency_deducted(20);
				Application::yourself.set_shop4(true);
			}
		}
	}
	else
	{
		shop_speech = "";
	}
	
	if(!interactnpc1)
	{
		translatenpc += (float)(30 * dt * dir1);
		if (translatenpc > 33)
			dir1 = -1;
		else if (translatenpc < -33)
			dir1 = 1;
	}

	if (!interactnpc2)
	{
		translatenpc2 += (float)(50 * dt * dir2);
		if (translatenpc2 > 33)
			dir2 = -1;
		else if (translatenpc2 < -33)
			dir2 = 1;
	}
	if (!interactnpc3)
	{
		translatenpc3 += (float)(50 * dt * dir3);
		if (translatenpc3 > 33)
			dir3 = -1;
		else if (translatenpc3 < 4)
			dir3 = 1;
	}

	if (!interactnpc4)
	{
		translatenpc4 += (float)(50 * dt * dir4);
		if (translatenpc4 > -4)
			dir4 = -1;
		else if (translatenpc4 < -33)
			dir4 = 1;
	}




	if ( (camera.position.x > npc1x - 4 && camera.position.x < npc1x + 4) && (camera.position.z > npc1z - 4 && camera.position.z < npc1z + 4) && !interactnpc1)
	{
		interactnpc1 = true;
		intnpc1 = 1;
		camera.setNPCinteract(intnpc1);
	}

	if ((camera.position.x > npc2x - 4 && camera.position.x < npc2x + 4) && (camera.position.z > npc2z - 4 && camera.position.z < npc2z + 4) && !interactnpc2)
	{
		interactnpc2 = true;
		
		intnpc2 = 1;
		camera.setNPCinteract(intnpc2);
	}


	if ((camera.position.x > npc3x - 1 && camera.position.x < npc3x + 1) && (camera.position.z > npc3z - 4 && camera.position.z < npc3z + 4) && !interactnpc3)
	{
		interactnpc3 = true;
		
		intnpc3 = 1;
		camera.setNPCinteract(intnpc3);
	}

	if ((camera.position.x > npc4x - 1 && camera.position.x < npc4x + 1) && (camera.position.z > npc4z - 4 && camera.position.z < npc4z + 4) && !interactnpc4)
	{
		interactnpc4 = true;
	
		intnpc4 = 1;
		camera.setNPCinteract(intnpc4);
	}
	
	//player vector set
	player.x = camera.position.x;
	player.y = camera.position.y;
	player.z = camera.position.z;

	//bullets translation
	translation_x_bullet += (float)(35 * dt);
	if (translation_x_bullet > 38)
		translation_x_bullet = -38;

	translation_x_bullet_slow += (float)(10 * dt);
	if (translation_x_bullet_slow > 38)
		translation_x_bullet_slow = -38;
	translation_x_bullet_fast += (float)(40 * dt);
	if (translation_x_bullet_fast > 38)
		translation_x_bullet_fast = -38;
	translation_x_bullet_medium += (float)(30 * dt);
	if (translation_x_bullet_medium > 38)
		translation_x_bullet_medium = -38;



	bullet1.x = translation_x_bullet;
	bullet2.x = translation_x_bullet_fast;
	bullet3.x = translation_x_bullet_medium;
	bullet4.x = translation_x_bullet_slow;
	bullet5.x = translation_x_bullet;

	bullet6.x = translation_x_bullet_fast;
	bullet7.x = translation_x_bullet_medium;
	bullet8.x = -translation_x_bullet_fast;

	bullet9.x = translation_x_bullet_fast;
	bullet10.x = translation_x_bullet_medium;
	bullet11.x = -translation_x_bullet_fast;
	bullet12.x = translation_x_bullet_medium;
	bullet13.x = -translation_x_bullet_fast;

	bullet14.x = translation_x_bullet_fast;
	bullet15.x = -translation_x_bullet_medium;
	bullet16.x = translation_x_bullet;
	bullet17.x = -translation_x_bullet_slow;
	bullet18.x = -translation_x_bullet_medium;
	bullet19.x = translation_x_bullet_fast;
	bullet20.x = translation_x_bullet;
	bullet21.x = -translation_x_bullet_fast;


	//colision detecion for bullets  
	{
		if (colisiondetectionbullets(bullet1, player))
		{

			Application::yourself.currency_deducted(1.5f);
		}
		if (colisiondetectionbullets(bullet2, player))
		{

			Application::yourself.currency_deducted(1.5f);
		}
		if (colisiondetectionbullets(bullet3, player))
		{

			Application::yourself.currency_deducted(1.5f);
		}
		if (colisiondetectionbullets(bullet4, player))
		{

			Application::yourself.currency_deducted(1.5f);
		}
		if (colisiondetectionbullets(bullet5, player))
		{

			Application::yourself.currency_deducted(1.5f);
		}

		if (colisiondetectionbullets(bullet6, player))
		{

			Application::yourself.currency_deducted(1.5f);
		}
		if (colisiondetectionbullets(bullet7, player))
		{

			Application::yourself.currency_deducted(1.5f);
		}
		if (colisiondetectionbullets(bullet8, player))
		{

			Application::yourself.currency_deducted(1.5f);
		}

		if (colisiondetectionbullets(bullet9, player))
		{

			Application::yourself.currency_deducted(1.5f);
		}
		if (colisiondetectionbullets(bullet10, player))
		{

			Application::yourself.currency_deducted(1.5f);
		}
		if (colisiondetectionbullets(bullet11, player))
		{

			Application::yourself.currency_deducted(1.5f);
		}
		if (colisiondetectionbullets(bullet12, player))
		{

			Application::yourself.currency_deducted(1.5f);
		}
		if (colisiondetectionbullets(bullet13, player))
		{

			Application::yourself.currency_deducted(1.5f);
		}

		if (colisiondetectionbullets(bullet14, player))
		{

			Application::yourself.currency_deducted(1.5f);
		}
		if (colisiondetectionbullets(bullet15, player))
		{

			Application::yourself.currency_deducted(1.5f);
		}
		if (colisiondetectionbullets(bullet16, player))
		{

			Application::yourself.currency_deducted(1.5f);
		}
		if (colisiondetectionbullets(bullet17, player))
		{

			Application::yourself.currency_deducted(1.5f);
		}
		if (colisiondetectionbullets(bullet18, player))
		{

			Application::yourself.currency_deducted(1.5f);
		}
		if (colisiondetectionbullets(bullet19, player))
		{

			Application::yourself.currency_deducted(1.5f);
		}
		if (colisiondetectionbullets(bullet20, player))
		{

			Application::yourself.currency_deducted(1.5f);
		}
		if (colisiondetectionbullets(bullet21, player))
		{

			Application::yourself.currency_deducted(1.5f);
		}


	}

	onpavement(1);
	onpavement(2);
	onpavement(3);
	onpavement(4);
	//questions
	//npc1
	if (intnpc1 == 1 && Application::IsKeyPressed('2'))//npc1
	{
		std::cout << "correct";
		intnpc1 = 2;
		camera.setNPCinteract(2);
	}
	else if(intnpc1 == 1 && (Application::IsKeyPressed('1')|| Application::IsKeyPressed('3')))
	{
		std::cout << "wrong";
		intnpc1 = 3;
		camera.setNPCinteract(3);
		Application::yourself.currency_deducted(5);
	}
	
	//npc2
	if (intnpc2 == 1 && Application::IsKeyPressed('2'))
	{
		std::cout << "correct";
		intnpc2 = 2;
		camera.setNPCinteract(2);
	}
	else if (intnpc2 == 1 && (Application::IsKeyPressed('1') || Application::IsKeyPressed('3') || Application::IsKeyPressed('4')))
	{
		std::cout << "wrong";
		intnpc2 = 3;
		camera.setNPCinteract(3);
		Application::yourself.currency_deducted(5);
	}

	//npc3
	if (intnpc3 == 1 && Application::IsKeyPressed('3'))
	{
		std::cout << "correct";
		intnpc3 = 2;
		camera.setNPCinteract(2);
	}
	else if (intnpc3 == 1 && (Application::IsKeyPressed('1') || Application::IsKeyPressed('2')))
	{
		std::cout << "wrong";
		intnpc3 = 3;
		camera.setNPCinteract(3);
		Application::yourself.currency_deducted(5);
	}

	//npc4
	if (intnpc4 == 1 && Application::IsKeyPressed('2'))
	{
		std::cout << "correct";
		intnpc4 = 2;
		camera.setNPCinteract(2);
	}
	else if (intnpc4 == 1 && Application::IsKeyPressed('1'))
	{
		std::cout << "wrong";
		intnpc4 = 3;
		camera.setNPCinteract(3);
		Application::yourself.currency_deducted(5);
	}

	//Mouse Inputs
	static bool bLButtonState = false;
	if (!bLButtonState && Application::IsMousePressed(0))
	{
		bLButtonState = true;
		//std::cout << "LBUTTON DOWN" << std::endl;
		//Converting Viewport space to UI space
		double x, y;
		double BUTTON_LEFT;
		Application::GetCursorPos(&x, &y);
		unsigned w = Application::GetWindowWidth();
		unsigned h = Application::GetWindowHeight();
		float posX = x / w * 80; //convert (0,800) to (0,80)
		float posY = 60 - (y / h * 60); //convert (600,0) to (0,60)
		//std::cout << "cursorX:" << x << " , cursorY:" << y << std::endl;
		std::cout << "posX:" << posX << " , posY:" << posY << std::endl;
		//qn 1
		if ((posX > 19 && posX < 58 && posY > 30 && posY < 35) && onpavement(1))//ans 1
		{
			std::cout << "ans 1 to q1" << std::endl;
			intnpc1 = 3;
			camera.setNPCinteract(3);
		}
		else if ((posX > 19 && posX < 58 && posY > 25 && posY < 28) && onpavement(1))//ans 2
		{
			std::cout << "ans 2 to q1" << std::endl;
			intnpc1 = 2;
			camera.setNPCinteract(2);
		}
		else if ((posX > 19 && posX < 58 && posY > 16 && posY < 21) && onpavement(1))//ans 3
		{
			std::cout << "ans 3 to q1" << std::endl;
			intnpc1 = 3;
			camera.setNPCinteract(3);
		}
		//qn2
		if ((posX > 19 && posX < 58 && posY > 31 && posY < 34) && onpavement(2))//ans 1
		{
			std::cout << "ans 1 to q2" << std::endl;
			intnpc2 = 3;
			camera.setNPCinteract(3);
		}
		else if ((posX > 19 && posX < 58 && posY > 27 && posY < 30) && onpavement(2))//ans 2
		{
			std::cout << "ans 2 to q2" << std::endl;
			intnpc2 = 2;
			camera.setNPCinteract(2);
		}
		else if ((posX > 19 && posX < 58 && posY > 23 && posY < 26) && onpavement(2))//ans 3
		{
			std::cout << "ans 3 to q2" << std::endl;
			intnpc2 = 3;
			camera.setNPCinteract(3);
		}
		else if ((posX > 19 && posX < 58 && posY > 14 && posY < 21) && onpavement(2))//ans 4
		{
			std::cout << "ans 4 to q2" << std::endl;
			intnpc2 = 3;
			camera.setNPCinteract(3);
		}

		//qn3
		if ((posX > 19 && posX < 58 && posY > 26 && posY < 29) && onpavement(3))//ans 1
		{
			std::cout << "ans 1 to q3" << std::endl;
			intnpc3 = 3;
			camera.setNPCinteract(3);

		}
		else if ((posX > 19 && posX < 58 && posY > 21 && posY < 25) && onpavement(3))//ans 2
		{
			std::cout << "ans 2 to q3" << std::endl;
			intnpc3 = 3;
			camera.setNPCinteract(3);
		}
		else if ((posX > 19 && posX < 58 && posY > 17 && posY < 20) && onpavement(3))//ans 3
		{
			std::cout << "ans 3 to q3" << std::endl;
			intnpc3 = 2;
			camera.setNPCinteract(2);
		}
		
		//qn4
		if ((posX > 19 && posX < 58 && posY > 25 && posY < 28) && onpavement(4))//ans 1
		{
			std::cout << "ans 1 to q4" << std::endl;
			intnpc4 = 3;
			camera.setNPCinteract(3);
		}
		else if ((posX > 19 && posX < 58 && posY > 21 && posY < 24) && onpavement(4))//ans 2
		{
			std::cout << "ans 2 to q4" << std::endl;
			intnpc4 = 2;
			camera.setNPCinteract(2);
		}
		

		
	}
	else if (bLButtonState && !Application::IsMousePressed(0))
	{
		bLButtonState = false;
		//std::cout << "LBUTTON UP" << std::endl;
	}



	













	
	rotateAngle += (float)(20 * dt);
	camera.Update(dt);
	FPS = std::to_string(1.f / dt);
	camerax = std::to_string(camera.position.x);
	cameraz = std::to_string(camera.position.z);
}


void Scene_LV2::Render()
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
	//**************************************************************************************************************


	/*modelStack.PushMatrix();
	modelStack.Translate(light[0].position.x, light[0].position.y, light[0].position.z);
	RenderMesh(meshList[GEO_LIGHTBALL], false);
	modelStack.PopMatrix();

	modelStack.PushMatrix();
	modelStack.Translate(light[1].position.x, light[1].position.y, light[1].position.z);
	RenderMesh(meshList[GEO_LIGHTBALL], false);
	modelStack.PopMatrix();*/

	RenderSkybox();
	RenderWall();
	RenderFloor();
	
	RenderPavement();
	
	RenderLightpole();
	
	RenderNPC();

	
	RenderRoads();

	RenderTaxi();

	RenderTunnel();

	RenderBullets();

	RenderQuestion();

		modelStack.PushMatrix();
		modelStack.Translate(-102, 1, 17);
		modelStack.Rotate(0, 0, 1, 0);
		modelStack.Scale(0.6, 0.6, 0.6);
		RenderText(meshList[GEO_TEXT], "Press 'R' to restart", Color(0, 0, 1));
		modelStack.PopMatrix();
	
	//----------------------------sp--------------------------------------
	if (tut_text==true)
	{
		//RenderTextOnScreen(meshList[GEO_TEXT], "Use WASD to move", Color(0, 1, 0), 2, 30, 55);
		RenderTextOnScreen(meshList[GEO_TEXT], "Reach the black ball to find scammer", Color(0, 1, 0), 2, 20, 53);
	}
	//modelStack.PushMatrix();
	////scale, translate, rotate
	//modelStack.Translate(0, 3, 18);
	//modelStack.Rotate(90, 0, 1, 0);
	//modelStack.Scale(5, 5, 5);
	//RenderMesh(meshList[GEO_SCAMMER], true);
	//modelStack.PopMatrix();
	//----------------------------sp--------------------------------------
	
	
	//UI
	/*RenderTextOnScreen(meshList[GEO_TEXT], camerax , Color(0, 1, 0), 2, 6, 0);
	RenderTextOnScreen(meshList[GEO_TEXT], "X:", Color(0, 1, 0), 2, 0, 0);
	RenderTextOnScreen(meshList[GEO_TEXT], cameraz, Color(0, 1, 0), 2, 6, 2);
	RenderTextOnScreen(meshList[GEO_TEXT], "Z:", Color(0, 1, 0), 2, 0, 2);
	RenderTextOnScreen(meshList[GEO_TEXT], FPS, Color(0, 1, 0), 2, 47, 0);
	RenderTextOnScreen(meshList[GEO_TEXT], "FPS:", Color(0, 1, 0), 2, 40, 0);*/

	if (intnpc1 == 1 || intnpc2 == 1 || intnpc3 == 1 || intnpc4 == 1)
		RenderTextOnScreen(meshList[GEO_TEXT], "Click on the answer or press number", Color(0.1f, 0.4f, 0.6f), 2, 23, 51);


	RenderTextOnScreen(meshList[GEO_TEXT], "$", Color(0, 1, 0), 3, 1, 49);
	RenderTextOnScreen(meshList[GEO_TEXT], std::to_string(Application::yourself.get_currency()), Color(0, 1, 0), 3, 2, 49);
	//RenderTextOnScreen(meshList[GEO_TEXT], "Currency:", Color(0, 1, 0), 2, 0, 50);
	RenderMeshOnScreen(meshList[GEO_COIN_ICON], 5, 56, 10, 10);


	//shop ui
	if (display_shop_ui == true)
	{
		RenderMeshOnScreen(meshList[GEO_SHOP_UI], 40, 30, 75, 55);
	}
	RenderTextOnScreen(meshList[GEO_TEXT], shop_speech, Color(0, 1, 0), 2, 1, 12);

	if (bloodui)
	{
		int i;
		for (i = 0; i < 10; i++)
		{
			RenderMeshOnScreen(meshList[GEO_BLOOD], 40, 30, 85, 65);
		}
		i = 0;
		bloodui = false;
	}

	if (scammed_screen == true)
	{
		int counta = 0;
		if (counta < 1500)
		{
			RenderMeshOnScreen(meshList[GEO_SCAMMED], 40, 30, 85, 65);
		}
		counta++;
		scammed_screen = false;
	}

	if (count2 < 150)
	{
		RenderMeshOnScreen(meshList[GEO_LV2], 40, 30, 100, 90);
		modelStack.PushMatrix();
		RenderMeshOnScreen(meshList[GEO_TAXI_LOGO], (move_car), 2, 45, 35);
	}

	Vector3 spherepos = tp_sphere - camera.position;
	float spheredis = sqrt(pow(spherepos.x, 2) + pow(spherepos.y, 2) + pow(spherepos.z, 2));

	shoppos = shop_shpere - camera.position;
	shopdis = sqrt(pow(shoppos.x, 2) + pow(shoppos.y, 2) + pow(shoppos.z, 2));



	if (spheredis <= 5 && lv3_cutscene == false)
	{
		count3_start = true;
		camera.CAMERA_SPEED = 0.0f;
		camera.ZOOM_SPEED = 0.0f;
		//play cutscene
		if (count3 < 150)
		{
			RenderMeshOnScreen(meshList[GEO_LV3], 40, 30, 85, 65);
		}
		else
		{
			lv3_cutscene = true;
		}
	}
	else if (spheredis <= 5 && lv3_cutscene == true)
	{
		Application::yourself.set_go_lv3(true);
	}
	
}

void Scene_LV2::Exit()
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




