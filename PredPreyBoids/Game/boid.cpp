#include "boid.h"
#include <Windows.h>
#include "drawdata.h"
#define _USE_MATH_DEFINES
#include <math.h>


Boid::Boid()
{

}

void Boid::initialize()
{

	ID3D11Device* GD = GameData::p3d;
	int m_size = 11;

	//calculate number of vertices and primatives
	int numVerts = 6 * 6 * (m_size - 1) * (m_size - 1);
	m_numPrims = numVerts / 3;
	m_vertices = new myVertex[numVerts];
	WORD* indices = new WORD[numVerts];

	//as using the standard VB shader Set the tex-coords somewhere safe
	for (int i = 0; i<numVerts; i++)
	{
		indices[i] = i;
		m_vertices[i].texCoord = Vector2::One;
	}

	//in each loop create the two traingles for the matching sub-square on each of the six faces
	int vert = 0;
	for (int i = -(m_size - 1) / 2; i<(m_size - 1) / 2; i++)
	{
		for (int j = -(m_size - 1) / 2; j<(m_size - 1) / 2; j++)
		{
			//top
			m_vertices[vert].Color = Color(1.0f, 0.0f, 0.0f, 1.0f);
			m_vertices[vert++].Pos = Vector3((float)i, 0.5f * (float)(m_size - 1), (float)j);
			m_vertices[vert].Color = Color(1.0f, 0.0f, 0.0f, 1.0f);
			m_vertices[vert++].Pos = Vector3((float)i, 0.5f * (float)(m_size - 1), (float)(j + 1));
			m_vertices[vert].Color = Color(1.0f, 0.0f, 0.0f, 1.0f);
			m_vertices[vert++].Pos = Vector3((float)(i + 1), 0.5f * (float)(m_size - 1), (float)j);

			m_vertices[vert].Color = Color(1.0f, 0.0f, 0.0f, 1.0f);
			m_vertices[vert++].Pos = Vector3((float)(i + 1), 0.5f * (float)(m_size - 1), (float)j);
			m_vertices[vert].Color = Color(1.0f, 0.0f, 0.0f, 1.0f);
			m_vertices[vert++].Pos = Vector3((float)i, 0.5f * (float)(m_size - 1), (float)(j + 1));
			m_vertices[vert].Color = Color(1.0f, 0.0f, 0.0f, 1.0f);
			m_vertices[vert++].Pos = Vector3((float)(i + 1), 0.5f * (float)(m_size - 1), (float)(j + 1));

			//back
			m_vertices[vert].Color = Color(0.0f, 1.0f, 0.0f, 1.0f);
			m_vertices[vert++].Pos = Vector3((float)i, (float)j, 0.5f * (float)(m_size - 1));
			m_vertices[vert].Color = Color(0.0f, 1.0f, 0.0f, 1.0f);
			m_vertices[vert++].Pos = Vector3((float)(i + 1), (float)j, 0.5f * (float)(m_size - 1));
			m_vertices[vert].Color = Color(0.0f, 1.0f, 0.0f, 1.0f);
			m_vertices[vert++].Pos = Vector3((float)i, (float)(j + 1), 0.5f * (float)(m_size - 1));

			m_vertices[vert].Color = Color(0.0f, 1.0f, 0.0f, 1.0f);
			m_vertices[vert++].Pos = Vector3((float)(i + 1), (float)j, 0.5f * (float)(m_size - 1));
			m_vertices[vert].Color = Color(0.0f, 1.0f, 0.0f, 1.0f);
			m_vertices[vert++].Pos = Vector3((float)(i + 1), (float)(j + 1), 0.5f * (float)(m_size - 1));
			m_vertices[vert].Color = Color(0.0f, 1.0f, 0.0f, 1.0f);
			m_vertices[vert++].Pos = Vector3((float)i, (float)(j + 1), 0.5f * (float)(m_size - 1));

			//right
			m_vertices[vert].Color = Color(0.0f, 0.0f, 1.0f, 1.0f);
			m_vertices[vert++].Pos = Vector3(0.5f * (float)(m_size - 1), (float)i, (float)j);
			m_vertices[vert].Color = Color(0.0f, 0.0f, 1.0f, 1.0f);
			m_vertices[vert++].Pos = Vector3(0.5f * (float)(m_size - 1), (float)(i + 1), (float)j);
			m_vertices[vert].Color = Color(0.0f, 0.0f, 1.0f, 1.0f);
			m_vertices[vert++].Pos = Vector3(0.5f * (float)(m_size - 1), (float)i, (float)(j + 1));

			m_vertices[vert].Color = Color(0.0f, 0.0f, 1.0f, 1.0f);
			m_vertices[vert++].Pos = Vector3(0.5f * (float)(m_size - 1), (float)(i + 1), (float)j);
			m_vertices[vert].Color = Color(0.0f, 0.0f, 1.0f, 1.0f);
			m_vertices[vert++].Pos = Vector3(0.5f * (float)(m_size - 1), (float)(i + 1), (float)(j + 1));
			m_vertices[vert].Color = Color(0.0f, 0.0f, 1.0f, 1.0f);
			m_vertices[vert++].Pos = Vector3(0.5f * (float)(m_size - 1), (float)i, (float)(j + 1));

			//Bottom
			m_vertices[vert].Color = Color(1.0f, 1.0f, 0.0f, 1.0f);
			m_vertices[vert++].Pos = Vector3((float)j, -0.5f * (float)(m_size - 1), (float)i);
			m_vertices[vert].Color = Color(1.0f, 1.0f, 0.0f, 1.0f);
			m_vertices[vert++].Pos = Vector3((float)(j + 1), -0.5f * (float)(m_size - 1), (float)i);
			m_vertices[vert].Color = Color(1.0f, 1.0f, 0.0f, 1.0f);
			m_vertices[vert++].Pos = Vector3((float)j, -0.5f * (float)(m_size - 1), (float)(i + 1));

			m_vertices[vert].Color = Color(1.0f, 1.0f, 0.0f, 1.0f);
			m_vertices[vert++].Pos = Vector3((float)j, -0.5f * (float)(m_size - 1), (float)(i + 1));
			m_vertices[vert].Color = Color(1.0f, 1.0f, 0.0f, 1.0f);
			m_vertices[vert++].Pos = Vector3((float)(j + 1), -0.5f * (float)(m_size - 1), (float)i);
			m_vertices[vert].Color = Color(1.0f, 1.0f, 0.0f, 1.0f);
			m_vertices[vert++].Pos = Vector3((float)(j + 1), -0.5f * (float)(m_size - 1), (float)(i + 1));

			//front
			m_vertices[vert].Color = Color(0.0f, 1.0f, 1.0f, 1.0f);
			m_vertices[vert++].Pos = Vector3((float)j, (float)i, -0.5f * (float)(m_size - 1));
			m_vertices[vert].Color = Color(0.0f, 1.0f, 1.0f, 1.0f);
			m_vertices[vert++].Pos = Vector3((float)j, (float)(i + 1), -0.5f * (float)(m_size - 1));
			m_vertices[vert].Color = Color(0.0f, 1.0f, 1.0f, 1.0f);
			m_vertices[vert++].Pos = Vector3((float)(j + 1), (float)i, -0.5f * (float)(m_size - 1));

			m_vertices[vert].Color = Color(0.0f, 1.0f, 1.0f, 1.0f);
			m_vertices[vert++].Pos = Vector3((float)j, (float)(i + 1), -0.5f * (float)(m_size - 1));
			m_vertices[vert].Color = Color(0.0f, 1.0f, 1.0f, 1.0f);
			m_vertices[vert++].Pos = Vector3((float)(j + 1), (float)(i + 1), -0.5f * (float)(m_size - 1));
			m_vertices[vert].Color = Color(0.0f, 1.0f, 1.0f, 1.0f);
			m_vertices[vert++].Pos = Vector3((float)(j + 1), (float)i, -0.5f * (float)(m_size - 1));

			//left
			m_vertices[vert].Color = Color(1.0f, 0.0f, 1.0f, 1.0f);
			m_vertices[vert++].Pos = Vector3(-0.5f * (float)(m_size - 1), (float)j, (float)i);
			m_vertices[vert].Color = Color(1.0f, 0.0f, 1.0f, 1.0f);
			m_vertices[vert++].Pos = Vector3(-0.5f * (float)(m_size - 1), (float)j, (float)(i + 1));
			m_vertices[vert].Color = Color(1.0f, 0.0f, 1.0f, 1.0f);
			m_vertices[vert++].Pos = Vector3(-0.5f * (float)(m_size - 1), (float)(j + 1), (float)i);

			m_vertices[vert].Color = Color(1.0f, 0.0f, 1.0f, 1.0f);
			m_vertices[vert++].Pos = Vector3(-0.5f * (float)(m_size - 1), (float)j, (float)(i + 1));
			m_vertices[vert].Color = Color(1.0f, 0.0f, 1.0f, 1.0f);
			m_vertices[vert++].Pos = Vector3(-0.5f * (float)(m_size - 1), (float)(j + 1), (float)(i + 1));
			m_vertices[vert].Color = Color(1.0f, 0.0f, 1.0f, 1.0f);
			m_vertices[vert++].Pos = Vector3(-0.5f * (float)(m_size - 1), (float)(j + 1), (float)i);
		}
	}
	
	//Here I just change the BOIDs to be different colour spheres
	if (m_type == 0)
	{
		SphereTransform(Color(1.0f, 1.0f, 1.0f));
	}
	else if (m_type == 1)
	{
		ConeTransform(Color(0.0f, 0.0f, 1.0f));
	}
	else if (m_type == 2)
	{
		ConeTransform(Color(0.0f, 1.0f, 0.0f));
	}
	else if (m_type == 3)
	{
		ConeTransform(Color(1.0f, 0.0f, 0.0f));
	}
	else if (m_type == 4)
	{
		ConeTransform(Color(0.0f, 1.0f, 1.0f));
	}
	else if (m_type == 5)
	{
		ConeTransform(Color(1.0f, 1.0f, 1.0f));
	}

	//Calculate the normals for the basic lighting in the base shader
	for (int i = 0; i<m_numPrims; i++)
	{
		WORD V1 = 3 * i;
		WORD V2 = 3 * i + 1;
		WORD V3 = 3 * i + 2;

		//Build normals
		Vector3 norm;
		Vector3 vec1 = m_vertices[V1].Pos - m_vertices[V2].Pos;
		Vector3 vec2 = m_vertices[V3].Pos - m_vertices[V2].Pos;
		norm = vec1.Cross(vec2);
		norm.Normalize();

		m_vertices[V1].Norm = norm;
		m_vertices[V2].Norm = norm;
		m_vertices[V3].Norm = norm;
	}

	BuildIB(GD, indices);
	BuildVB(GD, numVerts, m_vertices);

	delete[] m_vertices;
}

Boid::~Boid()
{

}


void Boid::Eat(){
	//Increase scale by 0.05x
	m_scale.x += (m_weight * 0.05);
	m_scale.y += (m_weight * 0.05);
	m_scale.z += (m_weight * 0.05);
	//Decrease max speed by 2.0 units
	max_speed -= 2.0f;
	//Increment weight
	m_weight++;
	//Set last kill time to now
	lastKillTickCount = GetTickCount64();
}
void Boid::Starve(){
	if (m_weight > 0){
		//Decrease scale by 0.05x
		m_scale.x -= (m_weight * 0.05);
		m_scale.y -= (m_weight * 0.05);
		m_scale.z -= (m_weight * 0.05);
		//Increase max speed by 2.0 units
		max_speed += 2.0f;
		//Decrement weight
		m_weight--;
	}
	//Set last kill time to now
	lastKillTickCount = GetTickCount64();
}

void Boid::Tick(GameData* GD)
{
	//If this BOID is not an obstacle
	if (m_type != 0){
		//Starvation
		if (m_weight > 0){
			if (GetTickCount64() - lastKillTickCount > SimulationParameters::starvationTime){
				Starve();
			}
		}
		//For safety always set direction y component to 0
		m_direction.y = 0.0f;
		//Normalize direction
		m_direction.Normalize();
		//Move BOID in direction
		m_pos += m_direction * m_speed * GD->dt;
		//Smoothly look toward direction
		smooth_yaw = (smooth_yaw * 0.9f) + (atan2(m_direction.x, m_direction.z) * GD->dt);
		m_yaw = smooth_yaw;
		//Slow down BOID to half max speed
		if (m_speed > max_speed / 2)
		{
			m_speed -= (m_speed - max_speed) * GD->dt;
		}
		else if (m_speed < max_speed /2)
		{
			m_speed += GD->dt;
		}

	}
	VBGO::Tick(GD);
}

void Boid::Draw(DrawData* DD)
{
	VBGO::Draw(DD);

	if (SimulationParameters::showDebug)
	{
		lineVertices[0].Pos = Vector3::Zero;
		lineVertices[0].Color = Color(1.0f, 1.0f, 1.0f, 1.0f);
		lineVertices[1].Pos = m_direction * m_speed;
		lineVertices[1].Color = Color(1.0f, 1.0f, 1.0f, 1.0f);

		BuildLineVB(GameData::p3d, 2, lineVertices);

		//Set raster state
		ID3D11RasterizerState* useRasterS = m_pRasterState ? m_pRasterState : s_pRasterState;
		DD->pd3dImmediateContext->RSSetState(useRasterS);

		//Set standard Constant Buffer to match this object
		Matrix newWorld;
		Vector3 basicScale = Vector3(1.0f, 1.0f, 1.0f);

		//build up the world matrix depending on the new position of the GameObject
		//the assumption is that this class will be inherited by the class that ACTUALLY changes this
		Matrix tempScale = Matrix::CreateScale(basicScale);
		Matrix tempRot = Matrix::CreateFromYawPitchRoll(m_yaw, m_pitch, m_roll);//possible not the best way of doing this!
		Matrix tempTrans = Matrix::CreateTranslation(m_pos);

		newWorld = tempScale * tempRot * tempTrans;

		s_pCB->world = newWorld.Transpose();
		s_pCB->rot = m_rotMat.Transpose();

		//Set vertex buffer
		UINT stride = sizeof(myVertex);
		UINT offSet = 0;

		DD->pd3dImmediateContext->IASetVertexBuffers(0, 1, &m_LineVertexBuffer, &stride, &offSet);

		// Set index buffer
		//DD->pd3dImmediateContext->IASetIndexBuffer(m_IndexBuffer, DXGI_FORMAT_R16_UINT, 0);

		//update the Constant Buffer in classes that inherit from this then push to the GPU here
		ID3D11Buffer* useCB = m_pConstantBuffer ? m_pConstantBuffer : s_pConstantBuffer;
		void* useCBData = m_pCB ? m_pCB : s_pCB;

		DD->pd3dImmediateContext->UpdateSubresource(useCB, 0, NULL, useCBData, 0, 0);
		DD->pd3dImmediateContext->VSSetConstantBuffers(0, 1, &useCB);
		DD->pd3dImmediateContext->PSSetConstantBuffers(0, 1, &useCB);

		//unless it has it own Set them to the static defaults

		//Set primitive type used
		DD->pd3dImmediateContext->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_LINELIST);

		//and draw
		DD->pd3dImmediateContext->DrawIndexed(2, 0, 0);//number here will need to change depending on the primative topology!

		if (m_LineVertexBuffer)
		{
			m_LineVertexBuffer->Release();
			m_LineVertexBuffer = nullptr;
		}
	}
}

