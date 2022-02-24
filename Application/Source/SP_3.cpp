#include "SP_3.h"
#include "GL\glew.h"
#include "Application.h"
#include "shader.hpp"
#include "Mtx44.h"
#include "MeshBuilder.h"
#include "Utility.h"
#include "LoadTGA.h"
#include "Light.h"


SP_3::SP_3()
{

}

SP_3::~SP_3()
{
}

void SP_3::RenderMeshOnScreen(Mesh* mesh, int x, int y, int sizex, int sizey)
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

void SP_3::RenderMesh(Mesh* mesh, bool enableLight)
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

void SP_3::RenderSkybox()
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



void SP_3::RenderFloor()
{
	modelStack.PushMatrix();
	modelStack.Translate(0, 0, 0);
	modelStack.Rotate(-90, 1, 0, 0);
	modelStack.Rotate(0, 0, 1, 0);
	modelStack.Scale(1000, 1000, 1000);
	RenderMesh(meshList[GEO_QUAD], true);
	modelStack.PopMatrix();
}
void SP_3::RenderLeftSide()
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
	modelStack.Translate(20, 0, -60);
	modelStack.Rotate(-250, 0, 1, 0);
	modelStack.Scale(100, 40, 40);
	RenderMesh(meshList[GEO_TALLROCK], true);
	modelStack.PopMatrix();

	modelStack.PushMatrix();
	modelStack.Translate(24, 0, -18);
	modelStack.Rotate(95, 0, 1, 0);
	modelStack.Scale(30, 30, 30);
	RenderMesh(meshList[GEO_DEBRIS], true);
	modelStack.PopMatrix();
}
void SP_3::RenderBackSide()
{
	modelStack.PushMatrix();
	modelStack.Translate(-29, 0, -75);
	modelStack.Rotate(180, 0, 1, 0);
	modelStack.Scale(70, 20, 70);
	RenderMesh(meshList[GEO_TALLROCK], true);
	modelStack.PopMatrix();

	modelStack.PushMatrix();
	modelStack.Translate(-80, 0, -61);
	modelStack.Rotate(180, 0, 1, 0);
	modelStack.Scale(80, 60, 60);
	RenderMesh(meshList[GEO_TALLROCK], true);
	modelStack.PopMatrix();

	modelStack.PushMatrix();
	modelStack.Translate(-115, 0, -20);
	modelStack.Rotate(90, 0, 1, 0);
	modelStack.Scale(80, 20, 80);
	RenderMesh(meshList[GEO_TALLROCK], true);
	modelStack.PopMatrix();

	modelStack.PushMatrix();
	modelStack.Translate(-110, 0, 48);
	modelStack.Rotate(270, 0, 1, 0);
	modelStack.Scale(80, 20, 60);
	RenderMesh(meshList[GEO_TALLROCK], true);
	modelStack.PopMatrix();

	modelStack.PushMatrix();
	modelStack.Translate(-97, 0, 95);
	modelStack.Rotate(270, 0, 1, 0);
	modelStack.Scale(80, 20, 60);
	RenderMesh(meshList[GEO_TALLROCK], true);
	modelStack.PopMatrix();
	//back
	modelStack.PushMatrix();
	modelStack.Translate(-40, 0, 110);
	modelStack.Rotate(90, 0, 1, 0);
	modelStack.Scale(80, 70, 80);
	RenderMesh(meshList[GEO_TALLROCK], true);
	modelStack.PopMatrix();

	modelStack.PushMatrix();
	modelStack.Translate(15, 0, 100);
	modelStack.Rotate(90, 0, 1, 0);
	modelStack.Scale(80, 35, 80);
	RenderMesh(meshList[GEO_TALLROCK], true);
	modelStack.PopMatrix();



	modelStack.PushMatrix();
	modelStack.Translate(-63, 0, -59);
	modelStack.Rotate(0, 0, 1, 0);
	modelStack.Scale(108, 10, 10);
	RenderMesh(meshList[GEO_STONEWALL], true);
	modelStack.PopMatrix();

	modelStack.PushMatrix();
	modelStack.Translate(-100, 0, -35);
	modelStack.Rotate(90, 0, 1, 0);
	modelStack.Scale(100, 10, 10);
	RenderMesh(meshList[GEO_STONEWALL], true);
	modelStack.PopMatrix();
}
void SP_3::RenderFrontSide()
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
}
void SP_3::sword1()
{
	modelStack.PushMatrix();
	{
		//modelStack.Translate(-75, 0, 0);
		modelStack.Translate(sword1_pos.x, sword1_pos.y, sword1_pos.z);
		modelStack.Rotate(0, 0, 1, 0);
		modelStack.Scale(0.5, 0.5, 0.5);
		//sword hilt
		modelStack.PushMatrix();
		{
			modelStack.Translate(0, 5, 0);
			modelStack.Rotate(180, 1, 0, 0);
			modelStack.Rotate(90, 0, 1, 0);
			modelStack.Scale(3, 3, 3);
			//blade
			modelStack.PushMatrix();
			{
				modelStack.Translate(0, 0, 0);
				modelStack.Rotate(90, 0, 1, 0);
				modelStack.Scale(3, 20, 2);
				modelStack.Scale(0.1, 0.1, 0.1);
				RenderMesh(meshList[GEO_SWORD], true);
			}
			modelStack.PopMatrix();

			modelStack.Translate(0, -1, 0);
			modelStack.Rotate(0, 0, 0, 1);
			modelStack.Scale(0.5, 8, 0.5);
			modelStack.Scale(0.1, 0.1, 0.1);
			RenderMesh(meshList[GEO_CYLINDER], true);
		}
		modelStack.PopMatrix();

		modelStack.Scale(20, 20, 20);
		RenderMesh(meshList[GEO_FIREBASKET], true);
	}
	modelStack.PopMatrix();
}
void SP_3::sword2()
{
	modelStack.PushMatrix();
	{
		//modelStack.Translate(-75, 0, 0);
		modelStack.Translate(sword2_pos.x, sword2_pos.y, sword2_pos.z);
		modelStack.Rotate(0, 0, 1, 0);
		modelStack.Scale(0.5, 0.5, 0.5);
		//sword hilt
		modelStack.PushMatrix();
		{
			modelStack.Translate(0, 5, 0);
			modelStack.Rotate(180, 1, 0, 0);
			modelStack.Rotate(90, 0, 1, 0);
			modelStack.Scale(3, 3, 3);
			//blade
			modelStack.PushMatrix();
			{
				modelStack.Translate(0, 0, 0);
				modelStack.Rotate(90, 0, 1, 0);
				modelStack.Scale(3, 20, 2);
				modelStack.Scale(0.1, 0.1, 0.1);
				RenderMesh(meshList[GEO_SWORD], true);
			}
			modelStack.PopMatrix();

			modelStack.Translate(0, -1, 0);
			modelStack.Rotate(0, 0, 0, 1);
			modelStack.Scale(0.5, 8, 0.5);
			modelStack.Scale(0.1, 0.1, 0.1);
			RenderMesh(meshList[GEO_CYLINDER], true);
		}
		modelStack.PopMatrix();

		modelStack.Scale(20, 20, 20);
		RenderMesh(meshList[GEO_FIREBASKET], true);
	}
	modelStack.PopMatrix();
}
void SP_3::sword3()
{
	modelStack.PushMatrix();
	{
		//modelStack.Translate(-75, 0, 0);
		modelStack.Translate(sword3_pos.x, sword3_pos.y, sword3_pos.z);
		modelStack.Rotate(0, 0, 1, 0);
		modelStack.Scale(0.5, 0.5, 0.5);
		//sword hilt
		modelStack.PushMatrix();
		{
			modelStack.Translate(0, 5, 0);
			modelStack.Rotate(180, 1, 0, 0);
			modelStack.Rotate(90, 0, 1, 0);
			modelStack.Scale(3, 3, 3);
			//blade
			modelStack.PushMatrix();
			{
				modelStack.Translate(0, 0, 0);
				modelStack.Rotate(90, 0, 1, 0);
				modelStack.Scale(3, 20, 2);
				modelStack.Scale(0.1, 0.1, 0.1);
				RenderMesh(meshList[GEO_SWORD], true);
			}
			modelStack.PopMatrix();

			modelStack.Translate(0, -1, 0);
			modelStack.Rotate(0, 0, 0, 1);
			modelStack.Scale(0.5, 8, 0.5);
			modelStack.Scale(0.1, 0.1, 0.1);
			RenderMesh(meshList[GEO_CYLINDER], true);
		}
		modelStack.PopMatrix();

		modelStack.Scale(20, 20, 20);
		RenderMesh(meshList[GEO_FIREBASKET], true);
	}
	modelStack.PopMatrix();
}
void SP_3::sword4()
{
	modelStack.PushMatrix();
	{
		//modelStack.Translate(-75, 0, 0);
		modelStack.Translate(sword4_pos.x, sword4_pos.y, sword4_pos.z);
		modelStack.Rotate(0, 0, 1, 0);
		modelStack.Scale(0.5, 0.5, 0.5);
		//sword hilt
		modelStack.PushMatrix();
		{
			modelStack.Translate(0, 5, 0);
			modelStack.Rotate(180, 1, 0, 0);
			modelStack.Rotate(90, 0, 1, 0);
			modelStack.Scale(3, 3, 3);
			//blade
			modelStack.PushMatrix();
			{
				modelStack.Translate(0, 0, 0);
				modelStack.Rotate(90, 0, 1, 0);
				modelStack.Scale(3, 20, 2);
				modelStack.Scale(0.1, 0.1, 0.1);
				RenderMesh(meshList[GEO_SWORD], true);
			}
			modelStack.PopMatrix();

			modelStack.Translate(0, -1, 0);
			modelStack.Rotate(0, 0, 0, 1);
			modelStack.Scale(0.5, 8, 0.5);
			modelStack.Scale(0.1, 0.1, 0.1);
			RenderMesh(meshList[GEO_CYLINDER], true);
		}
		modelStack.PopMatrix();

		modelStack.Scale(20, 20, 20);
		RenderMesh(meshList[GEO_FIREBASKET], true);
	}
	modelStack.PopMatrix();
}
void SP_3::Phase1Attack()
{

	modelStack.PushMatrix();
	modelStack.Translate(phase1translationX, 2, bullet1.z);
	modelStack.Rotate(rotatebullet, 0, 1, 0);
	modelStack.Scale(3, 3, 3);
	RenderMesh(meshList[GEO_BULLET], true);
	modelStack.PopMatrix();

	modelStack.PushMatrix();
	modelStack.Translate(phase1translationX, 2, bullet2.z);
	modelStack.Rotate(rotatebullet, 0, 1, 0);
	modelStack.Scale(3, 3, 3);
	RenderMesh(meshList[GEO_BULLET], true);
	modelStack.PopMatrix();

	modelStack.PushMatrix();
	modelStack.Translate(phase1translationX, 2, bullet3.z);
	modelStack.Rotate(rotatebullet, 0, 1, 0);
	modelStack.Scale(3, 3, 3);
	RenderMesh(meshList[GEO_BULLET], true);
	modelStack.PopMatrix();

	modelStack.PushMatrix();
	modelStack.Translate(phase1translationX, 2, bullet4.z);
	modelStack.Rotate(rotatebullet, 0, 1, 0);
	modelStack.Scale(3, 3, 3);
	RenderMesh(meshList[GEO_BULLET], true);
	modelStack.PopMatrix();

	modelStack.PushMatrix();
	modelStack.Translate(phase1translationX, 2, bullet5.z);
	modelStack.Rotate(rotatebullet, 0, 1, 0);
	modelStack.Scale(3, 3, 3);
	RenderMesh(meshList[GEO_BULLET], true);
	modelStack.PopMatrix();

	modelStack.PushMatrix();
	modelStack.Translate(phase1translationX, 2, bullet6.z);
	modelStack.Rotate(rotatebullet, 0, 1, 0);
	modelStack.Scale(3, 3, 3);
	RenderMesh(meshList[GEO_BULLET], true);
	modelStack.PopMatrix();

	modelStack.PushMatrix();
	modelStack.Translate(phase1translationX, 2, bullet7.z);
	modelStack.Rotate(rotatebullet, 0, 1, 0);
	modelStack.Scale(3, 3, 3);
	RenderMesh(meshList[GEO_BULLET], true);
	modelStack.PopMatrix();

	modelStack.PushMatrix();
	modelStack.Translate(phase1translationX, 2, bullet8.z);
	modelStack.Rotate(rotatebullet, 0, 1, 0);
	modelStack.Scale(3, 3, 3);
	RenderMesh(meshList[GEO_BULLET], true);
	modelStack.PopMatrix();

	modelStack.PushMatrix();
	modelStack.Translate(phase1translationX, 2, bullet9.z);
	modelStack.Rotate(rotatebullet, 0, 1, 0);
	modelStack.Scale(3, 3, 3);
	RenderMesh(meshList[GEO_BULLET], true);
	modelStack.PopMatrix();

	modelStack.PushMatrix();
	modelStack.Translate(phase1translationX, 2, bullet10.z);
	modelStack.Rotate(rotatebullet, 0, 1, 0);
	modelStack.Scale(3, 3, 3);
	RenderMesh(meshList[GEO_BULLET], true);
	modelStack.PopMatrix();

	modelStack.PushMatrix();
	modelStack.Translate(phase1translationX, 2, bullet11.z);
	modelStack.Rotate(rotatebullet, 0, 1, 0);
	modelStack.Scale(3, 3, 3);
	RenderMesh(meshList[GEO_BULLET], true);
	modelStack.PopMatrix();

	modelStack.PushMatrix();
	modelStack.Translate(phase1translationX, 2, bullet12.z);
	modelStack.Rotate(rotatebullet, 0, 1, 0);
	modelStack.Scale(3, 3, 3);
	RenderMesh(meshList[GEO_BULLET], true);
	modelStack.PopMatrix();

	modelStack.PushMatrix();
	modelStack.Translate(phase1translationX, 2, bullet13.z);
	modelStack.Rotate(rotatebullet, 0, 1, 0);
	modelStack.Scale(3, 3, 3);
	RenderMesh(meshList[GEO_BULLET], true);
	modelStack.PopMatrix();

	modelStack.PushMatrix();
	modelStack.Translate(phase1translationX, 2, bullet14.z);
	modelStack.Rotate(rotatebullet, 0, 1, 0);
	modelStack.Scale(3, 3, 3);
	RenderMesh(meshList[GEO_BULLET], true);
	modelStack.PopMatrix();

	modelStack.PushMatrix();
	modelStack.Translate(phase1translationX, 2, bullet15.z);
	modelStack.Rotate(rotatebullet, 0, 1, 0);
	modelStack.Scale(3, 3, 3);
	RenderMesh(meshList[GEO_BULLET], true);
	modelStack.PopMatrix();

	modelStack.PushMatrix();
	modelStack.Translate(phase1translationX, 2, bullet16.z);
	modelStack.Rotate(rotatebullet, 0, 1, 0);
	modelStack.Scale(3, 3, 3);
	RenderMesh(meshList[GEO_BULLET], true);
	modelStack.PopMatrix();

	modelStack.PushMatrix();
	modelStack.Translate(phase1translationX, 2, bullet17.z);
	modelStack.Rotate(rotatebullet, 0, 1, 0);
	modelStack.Scale(3, 3, 3);
	RenderMesh(meshList[GEO_BULLET], true);
	modelStack.PopMatrix();


}
void SP_3::Phase2Attack()
{
	{//third row
	modelStack.PushMatrix();
	modelStack.Translate(bullet22.x, 2, phase2translationZ_3);
	modelStack.Rotate(0, 0, 1, 0);
	modelStack.Scale(3, 3, 3);
	RenderMesh(meshList[GEO_BULLET3], true);
	modelStack.PopMatrix();

	modelStack.PushMatrix();
	modelStack.Translate(bullet23.x, 2, phase2translationZ_3);
	modelStack.Rotate(0, 0, 1, 0);
	modelStack.Scale(3, 3, 3);
	RenderMesh(meshList[GEO_BULLET3], true);
	modelStack.PopMatrix();

	modelStack.PushMatrix();
	modelStack.Translate(bullet24.x, 2, phase2translationZ_3);
	modelStack.Rotate(0, 0, 1, 0);
	modelStack.Scale(3, 3, 3);
	RenderMesh(meshList[GEO_BULLET3], true);
	modelStack.PopMatrix();

	modelStack.PushMatrix();
	modelStack.Translate(bullet25.x, 2, phase2translationZ_3);
	modelStack.Rotate(0, 0, 1, 0);
	modelStack.Scale(3, 3, 3);
	RenderMesh(meshList[GEO_BULLET3], true);
	modelStack.PopMatrix();

	modelStack.PushMatrix();
	modelStack.Translate(bullet26.x, 2, phase2translationZ_3);
	modelStack.Rotate(0, 0, 1, 0);
	modelStack.Scale(3, 3, 3);
	RenderMesh(meshList[GEO_BULLET3], true);
	modelStack.PopMatrix();

	modelStack.PushMatrix();
	modelStack.Translate(bullet27.x, 2, phase2translationZ_3);
	modelStack.Rotate(0, 0, 1, 0);
	modelStack.Scale(3, 3, 3);
	RenderMesh(meshList[GEO_BULLET3], true);
	modelStack.PopMatrix();

	modelStack.PushMatrix();
	modelStack.Translate(bullet28.x, 2, phase2translationZ_3);
	modelStack.Rotate(0, 0, 1, 0);
	modelStack.Scale(3, 3, 3);
	RenderMesh(meshList[GEO_BULLET3], true);
	modelStack.PopMatrix();

	modelStack.PushMatrix();
	modelStack.Translate(bullet29.x, 2, phase2translationZ_3);
	modelStack.Rotate(0, 0, 1, 0);
	modelStack.Scale(3, 3, 3);
	RenderMesh(meshList[GEO_BULLET3], true);
	modelStack.PopMatrix();

	modelStack.PushMatrix();
	modelStack.Translate(bullet30.x, 2, phase2translationZ_3);
	modelStack.Rotate(0, 0, 1, 0);
	modelStack.Scale(3, 3, 3);
	RenderMesh(meshList[GEO_BULLET3], true);
	modelStack.PopMatrix();

	modelStack.PushMatrix();
	modelStack.Translate(bullet31.x, 2, phase2translationZ_3);
	modelStack.Rotate(0, 0, 1, 0);
	modelStack.Scale(3, 3, 3);
	RenderMesh(meshList[GEO_BULLET3], true);
	modelStack.PopMatrix();

	modelStack.PushMatrix();
	modelStack.Translate(bullet32.x, 2, phase2translationZ_3);
	modelStack.Rotate(0, 0, 1, 0);
	modelStack.Scale(3, 3, 3);
	RenderMesh(meshList[GEO_BULLET3], true);
	modelStack.PopMatrix();
	}

	{//second row
		modelStack.PushMatrix();
		modelStack.Translate(bullet12.x, 2, phase2translationZ_2);
		modelStack.Rotate(0, 0, 1, 0);
		modelStack.Scale(3, 3, 3);
		RenderMesh(meshList[GEO_BULLET], true);
		modelStack.PopMatrix();

		modelStack.PushMatrix();
		modelStack.Translate(bullet13.x, 2, phase2translationZ_2);
		modelStack.Rotate(0, 0, 1, 0);
		modelStack.Scale(3, 3, 3);
		RenderMesh(meshList[GEO_BULLET], true);
		modelStack.PopMatrix();

		modelStack.PushMatrix();
		modelStack.Translate(bullet14.x, 2, phase2translationZ_2);
		modelStack.Rotate(0, 0, 1, 0);
		modelStack.Scale(3, 3, 3);
		RenderMesh(meshList[GEO_BULLET], true);
		modelStack.PopMatrix();

		modelStack.PushMatrix();
		modelStack.Translate(bullet15.x, 2, phase2translationZ_2);
		modelStack.Rotate(0, 0, 1, 0);
		modelStack.Scale(3, 3, 3);
		RenderMesh(meshList[GEO_BULLET], true);
		modelStack.PopMatrix();

		modelStack.PushMatrix();
		modelStack.Translate(bullet16.x, 2, phase2translationZ_2);
		modelStack.Rotate(0, 0, 1, 0);
		modelStack.Scale(3, 3, 3);
		RenderMesh(meshList[GEO_BULLET], true);
		modelStack.PopMatrix();

		modelStack.PushMatrix();
		modelStack.Translate(bullet17.x, 2, phase2translationZ_2);
		modelStack.Rotate(0, 0, 1, 0);
		modelStack.Scale(3, 3, 3);
		RenderMesh(meshList[GEO_BULLET], true);
		modelStack.PopMatrix();

		modelStack.PushMatrix();
		modelStack.Translate(bullet18.x, 2, phase2translationZ_2);
		modelStack.Rotate(0, 0, 1, 0);
		modelStack.Scale(3, 3, 3);
		RenderMesh(meshList[GEO_BULLET], true);
		modelStack.PopMatrix();

		modelStack.PushMatrix();
		modelStack.Translate(bullet19.x, 2, phase2translationZ_2);
		modelStack.Rotate(0, 0, 1, 0);
		modelStack.Scale(3, 3, 3);
		RenderMesh(meshList[GEO_BULLET], true);
		modelStack.PopMatrix();

		modelStack.PushMatrix();
		modelStack.Translate(bullet20.x, 2, phase2translationZ_2);
		modelStack.Rotate(0, 0, 1, 0);
		modelStack.Scale(3, 3, 3);
		RenderMesh(meshList[GEO_BULLET], true);
		modelStack.PopMatrix();

		modelStack.PushMatrix();
		modelStack.Translate(bullet21.x, 2, phase2translationZ_2);
		modelStack.Rotate(0, 0, 1, 0);
		modelStack.Scale(3, 3, 3);
		RenderMesh(meshList[GEO_BULLET], true);
		modelStack.PopMatrix();
	}

	{//first row
		modelStack.PushMatrix();
		modelStack.Translate(bullet1.x, 2, phase2translationZ);
		modelStack.Rotate(0, 0, 1, 0);
		modelStack.Scale(3, 3, 3);
		RenderMesh(meshList[GEO_BULLET2], true);
		modelStack.PopMatrix();

		modelStack.PushMatrix();
		modelStack.Translate(bullet2.x, 2, phase2translationZ);
		modelStack.Rotate(0, 0, 1, 0);
		modelStack.Scale(3, 3, 3);
		RenderMesh(meshList[GEO_BULLET2], true);
		modelStack.PopMatrix();

		modelStack.PushMatrix();
		modelStack.Translate(bullet3.x, 2, phase2translationZ);
		modelStack.Rotate(0, 0, 1, 0);
		modelStack.Scale(3, 3, 3);
		RenderMesh(meshList[GEO_BULLET2], true);
		modelStack.PopMatrix();

		modelStack.PushMatrix();
		modelStack.Translate(bullet4.x, 2, phase2translationZ);
		modelStack.Rotate(0, 0, 1, 0);
		modelStack.Scale(3, 3, 3);
		RenderMesh(meshList[GEO_BULLET2], true);
		modelStack.PopMatrix();

		modelStack.PushMatrix();
		modelStack.Translate(bullet5.x, 2, phase2translationZ);
		modelStack.Rotate(0, 0, 1, 0);
		modelStack.Scale(3, 3, 3);
		RenderMesh(meshList[GEO_BULLET2], true);
		modelStack.PopMatrix();

		modelStack.PushMatrix();
		modelStack.Translate(bullet6.x, 2, phase2translationZ);
		modelStack.Rotate(0, 0, 1, 0);
		modelStack.Scale(3, 3, 3);
		RenderMesh(meshList[GEO_BULLET2], true);
		modelStack.PopMatrix();

		modelStack.PushMatrix();
		modelStack.Translate(bullet7.x, 2, phase2translationZ);
		modelStack.Rotate(0, 0, 1, 0);
		modelStack.Scale(3, 3, 3);
		RenderMesh(meshList[GEO_BULLET2], true);
		modelStack.PopMatrix();

		modelStack.PushMatrix();
		modelStack.Translate(bullet8.x, 2, phase2translationZ);
		modelStack.Rotate(0, 0, 1, 0);
		modelStack.Scale(3, 3, 3);
		RenderMesh(meshList[GEO_BULLET2], true);
		modelStack.PopMatrix();

		modelStack.PushMatrix();
		modelStack.Translate(bullet9.x, 2, phase2translationZ);
		modelStack.Rotate(0, 0, 1, 0);
		modelStack.Scale(3, 3, 3);
		RenderMesh(meshList[GEO_BULLET2], true);
		modelStack.PopMatrix();

		modelStack.PushMatrix();
		modelStack.Translate(bullet10.x, 2, phase2translationZ);
		modelStack.Rotate(0, 0, 1, 0);
		modelStack.Scale(3, 3, 3);
		RenderMesh(meshList[GEO_BULLET2], true);
		modelStack.PopMatrix();

		modelStack.PushMatrix();
		modelStack.Translate(bullet11.x, 2, phase2translationZ);
		modelStack.Rotate(0, 0, 1, 0);
		modelStack.Scale(3, 3, 3);
		RenderMesh(meshList[GEO_BULLET2], true);
		modelStack.PopMatrix();
	}

}
void SP_3::Phase3Attack()
{
	modelStack.PushMatrix();
	modelStack.Translate(phase1translationX, 2, bullet1.z);
	modelStack.Rotate(rotatebullet, 0, 1, 0);
	modelStack.Scale(3, 3, 3);
	RenderMesh(meshList[GEO_BULLET], true);
	modelStack.PopMatrix();

	modelStack.PushMatrix();
	modelStack.Translate(phase3translationX, 2, bullet2.z);
	modelStack.Rotate(rotatebullet, 0, 1, 0);
	modelStack.Scale(3, 3, 3);
	RenderMesh(meshList[GEO_BULLET], true);
	modelStack.PopMatrix();

	modelStack.PushMatrix();
	modelStack.Translate(phase1translationX, 2, bullet3.z);
	modelStack.Rotate(rotatebullet, 0, 1, 0);
	modelStack.Scale(3, 3, 3);
	RenderMesh(meshList[GEO_BULLET], true);
	modelStack.PopMatrix();

	modelStack.PushMatrix();
	modelStack.Translate(phase3translationX, 2, bullet4.z);
	modelStack.Rotate(rotatebullet, 0, 1, 0);
	modelStack.Scale(3, 3, 3);
	RenderMesh(meshList[GEO_BULLET], true);
	modelStack.PopMatrix();

	modelStack.PushMatrix();
	modelStack.Translate(phase1translationX, 2, bullet5.z);
	modelStack.Rotate(rotatebullet, 0, 1, 0);
	modelStack.Scale(3, 3, 3);
	RenderMesh(meshList[GEO_BULLET], true);
	modelStack.PopMatrix();

	modelStack.PushMatrix();
	modelStack.Translate(phase3translationX, 2, bullet6.z);
	modelStack.Rotate(rotatebullet, 0, 1, 0);
	modelStack.Scale(3, 3, 3);
	RenderMesh(meshList[GEO_BULLET], true);
	modelStack.PopMatrix();

	modelStack.PushMatrix();
	modelStack.Translate(phase1translationX, 2, bullet7.z);
	modelStack.Rotate(rotatebullet, 0, 1, 0);
	modelStack.Scale(3, 3, 3);
	RenderMesh(meshList[GEO_BULLET], true);
	modelStack.PopMatrix();

	modelStack.PushMatrix();
	modelStack.Translate(phase3translationX, 2, bullet8.z);
	modelStack.Rotate(rotatebullet, 0, 1, 0);
	modelStack.Scale(3, 3, 3);
	RenderMesh(meshList[GEO_BULLET], true);
	modelStack.PopMatrix();

	modelStack.PushMatrix();
	modelStack.Translate(phase1translationX, 2, bullet9.z);
	modelStack.Rotate(rotatebullet, 0, 1, 0);
	modelStack.Scale(3, 3, 3);
	RenderMesh(meshList[GEO_BULLET], true);
	modelStack.PopMatrix();

	modelStack.PushMatrix();
	modelStack.Translate(phase3translationX, 2, bullet10.z);
	modelStack.Rotate(rotatebullet, 0, 1, 0);
	modelStack.Scale(3, 3, 3);
	RenderMesh(meshList[GEO_BULLET], true);
	modelStack.PopMatrix();

	modelStack.PushMatrix();
	modelStack.Translate(phase1translationX, 2, bullet11.z);
	modelStack.Rotate(rotatebullet, 0, 1, 0);
	modelStack.Scale(3, 3, 3);
	RenderMesh(meshList[GEO_BULLET], true);
	modelStack.PopMatrix();

	modelStack.PushMatrix();
	modelStack.Translate(phase3translationX, 2, bullet12.z);
	modelStack.Rotate(rotatebullet, 0, 1, 0);
	modelStack.Scale(3, 3, 3);
	RenderMesh(meshList[GEO_BULLET], true);
	modelStack.PopMatrix();

	modelStack.PushMatrix();
	modelStack.Translate(phase1translationX, 2, bullet13.z);
	modelStack.Rotate(rotatebullet, 0, 1, 0);
	modelStack.Scale(3, 3, 3);
	RenderMesh(meshList[GEO_BULLET], true);
	modelStack.PopMatrix();

	modelStack.PushMatrix();
	modelStack.Translate(phase3translationX, 2, bullet14.z);
	modelStack.Rotate(rotatebullet, 0, 1, 0);
	modelStack.Scale(3, 3, 3);
	RenderMesh(meshList[GEO_BULLET], true);
	modelStack.PopMatrix();

	modelStack.PushMatrix();
	modelStack.Translate(phase1translationX, 2, bullet15.z);
	modelStack.Rotate(rotatebullet, 0, 1, 0);
	modelStack.Scale(3, 3, 3);
	RenderMesh(meshList[GEO_BULLET], true);
	modelStack.PopMatrix();

	modelStack.PushMatrix();
	modelStack.Translate(phase3translationX, 2, bullet16.z);
	modelStack.Rotate(rotatebullet, 0, 1, 0);
	modelStack.Scale(3, 3, 3);
	RenderMesh(meshList[GEO_BULLET], true);
	modelStack.PopMatrix();

	modelStack.PushMatrix();
	modelStack.Translate(phase1translationX, 2, bullet17.z);
	modelStack.Rotate(rotatebullet, 0, 1, 0);
	modelStack.Scale(3, 3, 3);
	RenderMesh(meshList[GEO_BULLET], true);
	modelStack.PopMatrix();

}
void SP_3::Phase4Attack()
{

}
bool SP_3::colisiondetectionbullets(Vector3 bullet, Vector3 player)
{

	if ((player.x > bullet.x - 2 && player.x < bullet.x + 2) && (player.z > bullet.z - 4 && player.z < bullet.z + 4))
	{
		blood_ui = true;
		return true;

	}
	else
	{

		return false;

	}


}

void SP_3::RenderText(Mesh* mesh, std::string text, Color color)
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
void SP_3::RenderTextOnScreen(Mesh* mesh, std::string text, Color color, float size, float x, float y)
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


void SP_3::Init()
{
	sword1_enable=true;
	enemy_health = 5;
	sword_count = 4;
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

	camera.Init(Vector3(-35, 3, 35), Vector3(-35, 3, -35), Vector3(0, 1, 0));

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
	light[0].position.Set(-60.f, 60.f, 25.f);
	light[0].color.Set(0.9, 1, 0.8);
	light[0].power = 1;
	light[0].kC = 1.f;
	light[0].kL = 0.01f;
	light[0].kQ = 0.001f;
	light[0].cosCutoff = cos(Math::DegreeToRadian(80));
	light[0].cosInner = cos(Math::DegreeToRadian(30));
	light[0].exponent = 3.f;
	light[0].spotDirection.Set(0.f, 1.f, 0.f);


	light[1].type = Light::LIGHT_POINT;
	light[1].position.Set(9.9f, 60.f, 20.f);
	light[1].color.Set(0.4, 0.9, 0.7);
	light[1].power = 1;
	light[1].kC = 1.f;
	light[1].kL = 0.01f;
	light[1].kQ = 0.001f;
	light[1].cosCutoff = cos(Math::DegreeToRadian(15));
	light[1].cosInner = cos(Math::DegreeToRadian(30));
	light[1].exponent = 3.f;
	light[1].spotDirection.Set(0.f, 1.f, 0.f);

	light[2].type = Light::LIGHT_SPOT;
	light[2].position.Set(17.0f, 60.0f, -24.0f);
	light[2].color.Set(0.4, 0.9, 0.7);
	light[2].power = 1;
	light[2].kC = 1.f;
	light[2].kL = 0.01f;
	light[2].kQ = 0.001f;
	light[2].cosCutoff = cos(Math::DegreeToRadian(80));
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
	{
		
		meshList[GEO_FIREBASKET] = MeshBuilder::GenerateOBJMTL("basket", "OBJ//fireBasket.obj", "OBJ//fireBasket.mtl");
		meshList[GEO_SWORD] = MeshBuilder::GenerateCone("cone", Color(1, 1, 1), 20, 1.f, 1.f);

		meshList[GEO_AXES] = MeshBuilder::GenerateAxes("reference", 1000, 1000, 1000);

		meshList[GEO_QUAD] = MeshBuilder::GenerateQuad("quad", Color(0, 0, 0), 1.f);
		meshList[GEO_QUAD]->textureID = LoadTGA("Image//floor.tga");

		meshList[GEO_CUBE] = MeshBuilder::GenerateCube("cube", Color(1, 1, 1), 1.f);
		meshList[GEO_CUBE]->textureID = LoadTGA("Image//color.tga");

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

		meshList[GEO_BLEND] = MeshBuilder::GenerateQuad("blend", Color(1, 1, 1), 1.f);
		meshList[GEO_BLEND]->textureID = LoadTGA("Image//Grimm.tga");

		meshList[GEO_SPHERE] = MeshBuilder::GenerateSphere("sphere", Color(0, 0, 0), 10, 20, 1.f);			//limbs

		meshList[GEO_LAMPLIGHT] = MeshBuilder::GenerateSphere("sphere", Color(0.4, 0.4, 0.4), 10, 20, 1.f);			//limbs

		meshList[GEO_CYLINDER] = MeshBuilder::GenerateCylinder("cylinder", Color(1, 1, 1), 20, 1.f, 1.f);		//base of head

		meshList[GEO_HEMISPHERE] = MeshBuilder::GenerateHemisphere("hemisphere", Color(1, 1, 1), 10, 20, 1.f);  //ends of head

		meshList[GEO_TORUS] = MeshBuilder::GenerateTorus("torus", Color(0.3, 0.3, 0.3), 20, 20, 1.f, 0.5f);			//cape head

		meshList[GEO_CAPE] = MeshBuilder::GenerateCylinder("cylinder", Color(0.3, 0.3, 0.3), 20, 1.f, 1.f);		//base of head

		meshList[GEO_HALFTORUS] = MeshBuilder::GenerateHalfTorus("quartertorus", Color(1, 1, 1), 10, 20, 1.f, 0.5f);   //horns

		meshList[GEO_CONE] = MeshBuilder::GenerateCone("cone", Color(0.3, 0.3, 0.3), 20, 1.f, 1.f);   //cape end

		meshList[GEO_LIGHTBALL] = MeshBuilder::GenerateSphere("sphere", Color(1, 1, 1), 10, 20, 1.f);		//light for lamppost

		meshList[GEO_HORNET] = MeshBuilder::GenerateOBJMTL("hornet", "OBJ//zombie.obj", "OBJ//zombie.mtl");

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

		meshList[GEO_BULLET] = MeshBuilder::GenerateQuad("bullet", Color(1, 1, 1), 1.f);
		meshList[GEO_BULLET]->textureID = LoadTGA("Image//internetscam.tga");
		meshList[GEO_BULLET2] = MeshBuilder::GenerateQuad("bullet", Color(1, 1, 1), 1.f);
		meshList[GEO_BULLET2]->textureID = LoadTGA("Image//phonelove.tga");
		meshList[GEO_BULLET3] = MeshBuilder::GenerateQuad("bullet", Color(1, 1, 1), 1.f);
		meshList[GEO_BULLET3]->textureID = LoadTGA("Image//scam1.tga");
		meshList[GEO_BULLET4] = MeshBuilder::GenerateQuad("bullet", Color(1, 1, 1), 1.f);
		meshList[GEO_BULLET4]->textureID = LoadTGA("Image//scam2.tga");

		meshList[GEO_TEXT] = MeshBuilder::GenerateText("text", 16, 16);
		meshList[GEO_TEXT]->textureID = LoadTGA("Image//RockwellFont.tga");



		meshList[GEO_TEXT2] = MeshBuilder::GenerateText("text2", 16, 16);
		meshList[GEO_TEXT2]->textureID = LoadTGA("Image//comicsans.tga");
	}

	meshList[GEO_COIN] = MeshBuilder::GenerateOBJMTL("coin", "OBJ//coin.obj", "OBJ//coin.mtl");
	meshList[GEO_COIN]->textureID = LoadTGA("Image//Coin_Gold_albedo.tga");

	meshList[GEO_COIN_ICON] = MeshBuilder::GenerateQuad("coin_icon", Color(1, 1, 1), 1.f);
	meshList[GEO_COIN_ICON]->textureID = LoadTGA("Image//coin_icon.tga");

	meshList[GEO_BLOOD] = MeshBuilder::GenerateQuad("blood_gui", Color(1, 1, 1), 1.f);
	meshList[GEO_BLOOD]->textureID = LoadTGA("Image//Blood.tga");

	meshList[GEO_FULLHEALTH] = MeshBuilder::GenerateQuad("Full_health", Color(1, 1, 1), 1.f);
	meshList[GEO_FULLHEALTH]->textureID = LoadTGA("Image//Full_health.tga");
	meshList[GEO_THREEHEALTH] = MeshBuilder::GenerateQuad("Three_health", Color(1, 1, 1), 1.f);
	meshList[GEO_THREEHEALTH]->textureID = LoadTGA("Image//three_health.tga");
	meshList[GEO_HALFHEALTH] = MeshBuilder::GenerateQuad("Half_health", Color(1, 1, 1), 1.f);
	meshList[GEO_HALFHEALTH]->textureID = LoadTGA("Image//Half_health.tga");
	meshList[GEO_ONEHEALTH] = MeshBuilder::GenerateQuad("One_health", Color(1, 1, 1), 1.f);
	meshList[GEO_ONEHEALTH]->textureID = LoadTGA("Image//One_health.tga");
	meshList[GEO_ZEROHEALTH] = MeshBuilder::GenerateQuad("Zero_health", Color(1, 1, 1), 1.f);
	meshList[GEO_ZEROHEALTH]->textureID = LoadTGA("Image//Zero_health.tga");



	meshList[GEO_TAXI] = MeshBuilder::GenerateOBJ("coin", "OBJ//Taxi2.obj");
	meshList[GEO_TAXI]->textureID = LoadTGA("Image//taxi.tga");


	//-----------------------------------------------------------------------
	//SP
	//An array of 3 vectors which represents the colors of the 3 vertices

	Mtx44 projection;
	projection.SetToPerspective(45.f, 4.f / 3.f, 0.1f, 1000.f);
	projectionStack.LoadMatrix(projection);




	//Player yourself;



}

void SP_3::Update(double dt)
{
	if (Application::yourself.get_currency() <= 0)
	{
		//Application::IsKeyPressed(VK_F3) == true;
		Application::yourself.set_die();
	}

	Vector3 scammerpos = scammer_pos - camera.position;
	float scammerdis = sqrt(pow(scammerpos.x, 2) + pow(scammerpos.y, 2) + pow(scammerpos.z, 2));
	Vector3 sword1pos = sword1_pos - camera.position;
	float sword1dis = sqrt(pow(sword1pos.x, 2) + pow(sword1pos.y, 2) + pow(sword1pos.z, 2));
	Vector3 sword2pos = sword2_pos - camera.position;
	float sword2dis = sqrt(pow(sword2pos.x, 2) + pow(sword2pos.y, 2) + pow(sword2pos.z, 2));
	Vector3 sword3pos = sword3_pos - camera.position;
	float sword3dis = sqrt(pow(sword3pos.x, 2) + pow(sword3pos.y, 2) + pow(sword3pos.z, 2));
	Vector3 sword4pos = sword4_pos - camera.position;
	float sword4dis = sqrt(pow(sword4pos.x, 2) + pow(sword4pos.y, 2) + pow(sword4pos.z, 2));
	static bool sword1state = false;
	static bool sword2state = false;
	static bool sword3state = false;
	static bool sword4state = false;

	//intialising player vector
	player.x = camera.position.x;
	player.y = camera.position.y;
	player.z = camera.position.z;

	//if (Application::IsKeyPressed('1'))
	//{
	//	glEnable(GL_CULL_FACE);
	//}
	//if (Application::IsKeyPressed('2'))
	//{
	//	glDisable(GL_CULL_FACE);
	//}
	//if (Application::IsKeyPressed('3'))
	//{
	//	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL); //default fill mode
	//}
	//if (Application::IsKeyPressed('4'))
	//{
	//	glPolygonMode(GL_FRONT_AND_BACK, GL_LINE); //wireframe mode
	//}


	if (Application::IsKeyPressed('I'))
		light[0].position.z -= (float)(LSPEED * dt);
	if (Application::IsKeyPressed('K'))
		light[0].position.z += (float)(LSPEED * dt);
	if (Application::IsKeyPressed('J'))
		light[0].position.x -= (float)(LSPEED * dt);
	if (Application::IsKeyPressed('L'))
		light[0].position.x += (float)(LSPEED * dt);
	if (Application::IsKeyPressed('O'))
		light[0].position.y -= (float)(LSPEED * dt);
	if (Application::IsKeyPressed('P'))
	{
		light[0].position.y += (float)(LSPEED * dt);
	}
	static bool steal_coin = false;
	static int steal_count = 0;
	if (Application::IsKeyPressed('E'))
	{
		steal_coin = true;
	}
	else if (!Application::IsKeyPressed('E'))
	{
		steal_coin = false;
	}
	if (enemy_health<2 && scammerdis<10)
	{
		collect_coin = true;
		if (steal_coin&& steal_count==0)
		{
			Application::yourself.currency_added(300);
			steal_count++;
		}
	}
	else
	{
		collect_coin = false;
	}

	static bool sword1_check = false;
	static int sword1_count = 0;
	if (Application::IsKeyPressed('E'))
	{
		sword1_check = true;
	}
	else if (!Application::IsKeyPressed('E'))
	{
		sword1_check = false;
	}
	if (sword1dis<=10)
	{
		if (sword1_enable)
		{
			sword1_text = "Press E to attack";
		}
		else
		{
			sword1_text = "";
		}
		if (sword1_check)
		{
			sword1_count++;
		}
		if (sword1_press&& sword1_count>=1)
		{
			if (!sword1state /*&& Application::IsKeyPressed('E')*/)
			{
				sword1_enable = false;
				enemy_health --;
				sword1state = true;
			}
			else if (sword1state /*&& !Application::IsKeyPressed('E')*/)
			{
				sword1state = false;
			}
		}
	}
	else
	{
		sword1_text = "";
	}

	if (sword2dis <= 10)
	{
		if (sword2_enable)
		{
			sword2_text = "Press E to attack";
		}
		else
		{
			sword2_text = "";
		}
		if (sword2_press)
		{
			if (!sword2state && Application::IsKeyPressed('E'))
			{
				sword2_enable = false;
				enemy_health--;
				sword2state = true;
			}
			else if (sword2state && !Application::IsKeyPressed('E'))
			{
				sword2state = false;
			}

		}
	}
	else
	{
		sword2_text = "";
	}

	if (sword3dis <= 10)
	{
		if (sword3_enable)
		{
			sword3_text = "Press E to attack";
		}
		else
		{
			sword3_text = "";
		}
		//sword_text = "Press E to attack";
		if (sword3_press)
		{
			if (!sword3state && Application::IsKeyPressed('E'))
			{
				sword3_enable = false;
				enemy_health--;
				sword3state = true;
			}
			else if (sword3state && !Application::IsKeyPressed('E'))
			{
				sword3state = false;
			}

		}
	}
	else
	{
		sword3_text = "";
	}

	if (sword4dis <= 10)
	{
		if (sword4_enable)
		{
			sword4_text = "Press E to attack";
		}
		else
		{
			sword4_text = "";
		}
		if (sword4_press)
		{
			if (!sword4state && Application::IsKeyPressed('E'))
			{
				sword4_enable = false;
				enemy_health--;
				sword4state = true;
			}
			else if (sword4state && !Application::IsKeyPressed('E'))
			{
				sword4state = false;
			}

		}
	}
	else
	{
		sword4_text = "";
	}
	/*******************************************************************************************************/
	//tutorial boundary check
	//static bool scammaer_talk = false;

	//boss fight Phase 1: 
	if (enemy_health == 5)
	{
		bullet1.x = 15;
		bullet1.z = 59;

		bullet2.x = 15;
		bullet2.z = 54;

		bullet3.x = 15;
		bullet3.z = 39;

		bullet4.x = 15;
		bullet4.z = 34;

		bullet5.x = 15;
		bullet5.z = 29;

		bullet6.x = 15;
		bullet6.z = 24;

		bullet7.x = 15;
		bullet7.z = 19;

		bullet8.x = 15;
		bullet8.z = 14;

		bullet9.x = 15;
		bullet9.z = 9;

		bullet10.x = 15;
		bullet10.z = 4;

		bullet11.x = 15;
		bullet11.z = -1;

		bullet12.x = 15;
		bullet12.z = -6;

		bullet13.x = 15;
		bullet13.z = -11;

		bullet14.x = 15;
		bullet14.z = -16;

		bullet15.x = 15;
		bullet15.z = -21;

		bullet16.x = 15;
		bullet16.z = -26;

		bullet17.x = 15;
		bullet17.z = -31;

		bullet1.x = phase1translationX;
		bullet2.x = phase1translationX;
		bullet3.x = phase1translationX;
		bullet4.x = phase1translationX;
		bullet5.x = phase1translationX;
		bullet6.x = phase1translationX;
		bullet7.x = phase1translationX;
		bullet8.x = phase1translationX;
		bullet9.x = phase1translationX;
		bullet10.x = phase1translationX;
		bullet11.x = phase1translationX;
		bullet12.x = phase1translationX;
		bullet13.x = phase1translationX;
		bullet14.x = phase1translationX;
		bullet15.x = phase1translationX;
		bullet16.x = phase1translationX;
		bullet17.x = phase1translationX;

		phase1translationX -= (float)(20 * dt);

		if (phase1translationX <= -85)
		{
			phase1translationX = 15;
		}
	}
	//boss fight Phase 2:
	else if (enemy_health == 4)
	{
		//first row
		bullet1.x = -85;
		bullet1.z = -32;

		bullet2.x = -75;
		bullet2.z = -32;

		bullet3.x = -65;
		bullet3.z = -32;

		bullet4.x = -55;
		bullet4.z = -32;

		bullet5.x = -45;
		bullet5.z = -32;

		bullet6.x = -35;
		bullet6.z = -32;

		bullet7.x = -25;
		bullet7.z = -32;

		bullet8.x = -15;
		bullet8.z = -32;

		bullet9.x = -5;
		bullet9.z = -32;

		bullet10.x = 5;
		bullet10.z = -32;

		bullet11.x = 15;
		bullet11.z = -32;

		//the second row
		bullet12.x = -80;
		bullet12.z = -36;

		bullet13.x = -70;
		bullet13.z = -36;

		bullet14.x = -60;
		bullet14.z = -36;

		bullet15.x = -50;
		bullet15.z = -36;

		bullet16.x = -40;
		bullet16.z = -36;

		bullet17.x = -30;
		bullet17.z = -36;

		bullet18.x = -20;
		bullet18.z = -36;
		
		bullet19.x = -10;
		bullet19.z = -36;

		bullet20.x = 0;
		bullet20.z = -36;

		bullet21.x = 10;
		bullet21.z = -36;

		//third row
		bullet22.x = -85;
		bullet22.z = -32;

		bullet23.x = -75;
		bullet23.z = -32;

		bullet24.x = -65;
		bullet24.z = -32;

		bullet25.x = -55;
		bullet25.z = -32;

		bullet26.x = -45;
		bullet26.z = -32;

		bullet27.x = -35;
		bullet27.z = -32;

		bullet28.x = -25;
		bullet28.z = -32;

		bullet29.x = -15;
		bullet29.z = -32;

		bullet30.x = -5;
		bullet30.z = -32;

		bullet31.x = 5;
		bullet31.z = -32;

		bullet32.x = 15;
		bullet32.z = -32;

		//first row
		bullet1.z = phase2translationZ;
		bullet2.z = phase2translationZ;
		bullet3.z = phase2translationZ;
		bullet4.z = phase2translationZ;
		bullet5.z = phase2translationZ;
		bullet6.z = phase2translationZ;
		bullet7.z = phase2translationZ;
		bullet8.z = phase2translationZ;
		bullet9.z = phase2translationZ;
		bullet10.z = phase2translationZ;
		bullet11.z = phase2translationZ;


		//second row
		bullet12.z = phase2translationZ_2;
		bullet13.z = phase2translationZ_2;
		bullet14.z = phase2translationZ_2;
		bullet15.z = phase2translationZ_2;
		bullet16.z = phase2translationZ_2;
		bullet17.z = phase2translationZ_2;
		bullet18.z = phase2translationZ_2;
		bullet19.z = phase2translationZ_2;
		bullet20.z = phase2translationZ_2;
		bullet21.z = phase2translationZ_2;

		//third row
		bullet22.z = phase2translationZ_3;
		bullet23.z = phase2translationZ_3;
		bullet24.z = phase2translationZ_3;
		bullet25.z = phase2translationZ_3;
		bullet26.z = phase2translationZ_3;
		bullet27.z = phase2translationZ_3;
		bullet28.z = phase2translationZ_3;
		bullet29.z = phase2translationZ_3;
		bullet30.z = phase2translationZ_3;
		bullet31.z = phase2translationZ_3;
		bullet32.z = phase2translationZ_3;

		phase2translationZ += (float)(35 * dt);
		phase2translationZ_2 += (float)(35 * dt);
		phase2translationZ_3 += (float)(35 * dt);
		if (phase2translationZ >= 61)
		{
			phase2translationZ = -32;
		}
		if (phase2translationZ_2 >= 61)
		{
			phase2translationZ_2 = -32;
		}
		if (phase2translationZ_3 >= 61)
		{
			phase2translationZ_3 = -32;
		}

	}
	//boss fight Phase 3:
	else if (enemy_health == 3)
	{
		bullet1.x = 15;
		bullet1.z = 59;

		bullet2.x = 15;
		bullet2.z = 54;

		bullet3.x = 15;
		bullet3.z = 39;

		bullet4.x = 15;
		bullet4.z = 34;

		bullet5.x = 15;
		bullet5.z = 29;

		bullet6.x = 15;
		bullet6.z = 24;

		bullet7.x = 15;
		bullet7.z = 19;

		bullet8.x = 15;
		bullet8.z = 14;

		bullet9.x = 15;
		bullet9.z = 9;

		bullet10.x = 15;
		bullet10.z = 4;

		bullet11.x = 15;
		bullet11.z = -1;

		bullet12.x = 15;
		bullet12.z = -6;

		bullet13.x = 15;
		bullet13.z = -11;

		bullet14.x = 15;
		bullet14.z = -16;

		bullet15.x = 15;
		bullet15.z = -21;

		bullet16.x = 15;
		bullet16.z = -26;

		bullet17.x = 15;
		bullet17.z = -31;

		bullet18.x = 80;
		bullet18.z = -32;

		bullet19.x = 80;
		bullet19.z = -32;

		bullet20.x = 80;
		bullet20.z = -32;

		bullet21.x = 80;
		bullet21.z = -32;

		bullet22.x = 80;
		bullet22.z = -32;

		bullet23.x = 80;
		bullet23.z = -32;

		bullet24.x = 80;
		bullet24.z = -32;

		bullet25.x = 80;
		bullet25.z = -32;

		bullet26.x = 80;
		bullet26.z = -32;

		bullet27.x = 80;
		bullet27.z = -32;

		bullet28.x = 80;
		bullet28.z = -32;

		bullet29.x = 80;
		bullet29.z = -32;

		bullet30.x = 80;
		bullet30.z = -32;

		bullet31.x = 80;
		bullet31.z = -32;

		bullet32.x = 80;
		bullet32.z = -32;

		bullet1.x = phase1translationX;
		bullet2.x = phase3translationX;
		bullet3.x = phase1translationX;
		bullet4.x = phase3translationX;
		bullet5.x = phase1translationX;
		bullet6.x = phase3translationX;
		bullet7.x = phase1translationX;
		bullet8.x = phase3translationX;
		bullet9.x = phase1translationX;
		bullet10.x = phase3translationX;
		bullet11.x = phase1translationX;
		bullet12.x = phase3translationX;
		bullet13.x = phase1translationX;
		bullet14.x = phase3translationX;
		bullet15.x = phase1translationX;
		bullet16.x = phase3translationX;
		bullet17.x = phase1translationX;

		phase1translationX -= (float)(30 * dt);
		phase3translationX += (float)(30 * dt);
		if (phase1translationX <= -85)
		{
			phase1translationX = 15;
		}
		if (phase3translationX >= 15)
		{
			phase3translationX = -85;
		}
	}
	//boss fight Phase 4:
	else if (enemy_health == 2)
	{
		
	}
	else if (enemy_health < 2)
	{

		bullet1.x = 84;
		bullet1.z = -32;

		bullet2.x = 80;
		bullet2.z = -32;

		bullet3.x = 80;
		bullet3.z = -32;

		bullet4.x = 80;
		bullet4.z = -32;

		bullet5.x = 80;
		bullet5.z = -32;

		bullet6.x = 80;
		bullet6.z = -32;

		bullet7.x = 80;
		bullet7.z = -32;

		bullet8.x = 80;
		bullet8.z = -32;

		bullet9.x = 80;
		bullet9.z = -32;

		bullet10.x = 80;
		bullet10.z = -32;

		bullet11.x = 80;
		bullet11.z = -32;

		bullet12.x = 80;
		bullet12.z = -32;

		bullet13.x = 80;
		bullet13.z = -32;

		bullet14.x = 80;
		bullet14.z = -32;

		bullet15.x = 80;
		bullet15.z = -32;

		bullet16.x = 80;
		bullet16.z = -32;

		bullet17.x = 80;
		bullet17.z = -32;

		bullet18.x = 80;
		bullet18.z = -32;

		bullet19.x = 80;
		bullet19.z = -32;

		bullet20.x = 80;
		bullet20.z = -32;

		bullet21.x = 80;
		bullet21.z = -32;

		bullet22.x = 80;
		bullet22.z = -32;
		
		bullet23.x = 80;
		bullet23.z = -32;

		bullet24.x = 80;
		bullet24.z = -32;

		bullet25.x = 80;
		bullet25.z = -32;

		bullet26.x = 80;
		bullet26.z = -32;

		bullet27.x = 80;
		bullet27.z = -32;

		bullet28.x = 80;
		bullet28.z = -32;

		bullet29.x = 80;
		bullet29.z = -32;

		bullet30.x = 80;
		bullet30.z = -32;

		bullet31.x = 80;
		bullet31.z = -32;

		bullet32.x = 80;
		bullet32.z = -32;
	}

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
		if (colisiondetectionbullets(bullet22, player))
		{

			Application::yourself.currency_deducted(1.5f);
		}
		if (colisiondetectionbullets(bullet23, player))
		{

			Application::yourself.currency_deducted(1.5f);
		}
		if (colisiondetectionbullets(bullet24, player))
		{

			Application::yourself.currency_deducted(1.5f);
		}
		if (colisiondetectionbullets(bullet25, player))
		{

			Application::yourself.currency_deducted(1.5f);
		}
		if (colisiondetectionbullets(bullet26, player))
		{

			Application::yourself.currency_deducted(1.5f);
		}
		if (colisiondetectionbullets(bullet27, player))
		{

			Application::yourself.currency_deducted(1.5f);
		}
		if (colisiondetectionbullets(bullet28, player))
		{

			Application::yourself.currency_deducted(1.5f);
		}
		if (colisiondetectionbullets(bullet29, player))
		{

			Application::yourself.currency_deducted(1.5f);
		}
		if (colisiondetectionbullets(bullet30, player))
		{

			Application::yourself.currency_deducted(1.5f);
		}
		if (colisiondetectionbullets(bullet31, player))
		{

			Application::yourself.currency_deducted(1.5f);
		}
		if (colisiondetectionbullets(bullet32, player))
		{

			Application::yourself.currency_deducted(1.5f);
		}











	}
	

	if (Application::IsKeyPressed('R'))
	{
		camera.Init(Vector3(0, 3, 1), Vector3(0, 3, 10), Vector3(0, 1, 0));
		Application::yourself.set_currency(100);
		battlestart = false;
		die = false;
		win = false;

	}

	rotateAngle += (float)(20 * dt);
	camera.Update(dt);
	FPS = std::to_string(1.f / dt);
	camerax = std::to_string(camera.position.x);
	cameraz = std::to_string(camera.position.z);
}
void SP_3::RenderScammer()
{
	modelStack.PushMatrix();
	modelStack.Translate(scammer_pos.x, scammer_pos.y, scammer_pos.z);
	if (enemy_health<=1)
	{
		modelStack.Rotate(90, 1, 0, 0);
		modelStack.Rotate(180, 0, 1, 0);
		modelStack.Rotate(180, 0, 0, 1);
	}
	else
	{

	}
	modelStack.Scale(5, 5, 5);
	RenderMesh(meshList[GEO_HORNET], true);
	modelStack.PopMatrix();
}

void SP_3::Render()
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


	modelStack.PushMatrix();
	modelStack.Translate(light[0].position.x, light[0].position.y, light[0].position.z);
	RenderMesh(meshList[GEO_LIGHTBALL], false);
	modelStack.PopMatrix();

	modelStack.PushMatrix();
	modelStack.Translate(light[1].position.x, light[1].position.y, light[1].position.z);
	RenderMesh(meshList[GEO_LIGHTBALL], true);
	modelStack.PopMatrix();

	modelStack.PushMatrix();
	modelStack.Translate(light[2].position.x, light[2].position.y, light[2].position.z);
	RenderMesh(meshList[GEO_LIGHTBALL], true);
	modelStack.PopMatrix();

	RenderSkybox();

	RenderFloor();

	RenderScammer();

	RenderLeftSide();

	RenderBackSide();

	RenderFrontSide();

	if (sword1_enable)
	{
		sword1();
	}
	else
	{
		sword1_press = false;
	}
	if (sword2_enable)
	{
		sword2();
	}
	else
	{
		sword2_press = false;
	}
	if (sword3_enable)
	{
		sword3();
	}
	else
	{
		sword3_press = false;
	}
	if (sword4_enable)
	{
		sword4();
	}
	else
	{
		sword4_press = false;
	}
	

	if (die)
	{
		//endscreen = true;
		if (true)
		{
			Application::IsKeyPressed(VK_F3) == true;
		}
		
	
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
	if (enemy_health == 5)
	{
		Phase1Attack();
		RenderMeshOnScreen(meshList[GEO_FULLHEALTH], 45, 56, 35, 2);
	}
	else if (enemy_health == 4)
	{
		Phase2Attack();
		RenderMeshOnScreen(meshList[GEO_THREEHEALTH], 45, 56, 35, 2);
	}
	else if (enemy_health == 3)
	{
		Phase3Attack();
		RenderMeshOnScreen(meshList[GEO_HALFHEALTH], 45, 56, 35, 2);
	}
	else if (enemy_health == 2)
	{
		RenderMeshOnScreen(meshList[GEO_ONEHEALTH], 45, 56, 35, 2);
	}
	else /*if (enemy_health == 2)*/
	{
		RenderMeshOnScreen(meshList[GEO_ZEROHEALTH], 45, 56, 35, 2);
	}

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
	//----------------------------sp--------------------------------------


	//UI
	RenderTextOnScreen(meshList[GEO_TEXT], camerax, Color(0, 1, 0), 2, 6, 0);
	RenderTextOnScreen(meshList[GEO_TEXT], "X:", Color(0, 1, 0), 2, 0, 0);
	RenderTextOnScreen(meshList[GEO_TEXT], cameraz, Color(0, 1, 0), 2, 6, 2);
	RenderTextOnScreen(meshList[GEO_TEXT], "Z:", Color(0, 1, 0), 2, 0, 2);
	RenderTextOnScreen(meshList[GEO_TEXT], FPS, Color(0, 1, 0), 2, 47, 0);
	RenderTextOnScreen(meshList[GEO_TEXT], "FPS:", Color(0, 1, 0), 2, 40, 0);


	RenderTextOnScreen(meshList[GEO_TEXT], std::to_string(Application::yourself.get_currency()), Color(0, 1, 0), 2, 2, 50);
	//RenderTextOnScreen(meshList[GEO_TEXT], "Currency:", Color(0, 1, 0), 2, 0, 50);



	/*RenderTextOnScreen(meshList[GEO_TEXT], std::to_string(playerhealth), Color(0, 1, 0), 2, 15, 50);
	RenderTextOnScreen(meshList[GEO_TEXT], "Health:", Color(0, 1, 0), 2, 0, 50);*/
	RenderTextOnScreen(meshList[GEO_TEXT], sword1_text, Color(0, 1, 0), 2, 0, 10);
	RenderTextOnScreen(meshList[GEO_TEXT], sword2_text, Color(0, 1, 0), 2, 0, 10);
	RenderTextOnScreen(meshList[GEO_TEXT], sword3_text, Color(0, 1, 0), 2, 0, 10);
	RenderTextOnScreen(meshList[GEO_TEXT], sword4_text, Color(0, 1, 0), 2, 0, 10);
	if (collect_coin)
	{
		RenderTextOnScreen(meshList[GEO_TEXT], "Press E to take coins back", Color(0, 1, 0), 2, 0, 10);
	}
	if (enemy_health>1)
	{
		RenderTextOnScreen(meshList[GEO_TEXT], "Use the swords to attack the scammer", Color(0, 1, 0), 2, 0, 8);
	}
	else
	{
		RenderTextOnScreen(meshList[GEO_TEXT], "Get your coins back from the scammer", Color(0, 1, 0), 2, 0, 8);
	}
	RenderMeshOnScreen(meshList[GEO_COIN_ICON], 5, 56, 10, 10);


}
void SP_3::Exit()
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