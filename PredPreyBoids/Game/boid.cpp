#include "boid.h"
#include <Windows.h>
#include "drawdata.h"
#define _USE_MATH_DEFINES
#include <math.h>


Boid::Boid(Waypoint* _finish, Waypoint* _outpost)
{
	finish = _finish;
	outpost = _outpost;
}

void Boid::initialize()
{
	//Size of the boid
	int m_size = 11;
	//Calculate number of vertices and primatives
	int numVerts = 6 * 6 * (m_size - 1) * (m_size - 1);
	m_numPrims = numVerts / 3;
	m_vertices = new myVertex[numVerts];
	WORD* indices = new WORD[numVerts];

	//Set the tex-coords somewhere safe
	for (int i = 0; i<numVerts; i++)
	{
		indices[i] = (WORD)i;
		m_vertices[i].texCoord = Vector2::One;
	}

	//In each loop create the two traingles for the matching sub-square on each of the six faces
	int vert = 0;
	for (int i = -(m_size - 1) / 2; i<(m_size - 1) / 2; i++)
	{
		for (int j = -(m_size - 1) / 2; j<(m_size - 1) / 2; j++)
		{
			//Top
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

			//Back
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

			//Right
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

			//Front
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

			//Left
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
	
	//Change the BOIDs to be different colour shapes using a transform
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
		ConeTransform(Color(1.0f, 0.0f, 1.0f));
	}

	//Calculate the normals for the basic lighting in the base shader
	for (unsigned int i = 0; i<m_numPrims; i++)
	{
		WORD V1 = (WORD)(3 * i);
		WORD V2 = (WORD)(3 * i + 1);
		WORD V3 = (WORD)(3 * i + 2);

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

	BuildIB(GameData::p3d, indices);
	BuildVB(GameData::p3d, numVerts, m_vertices);

	delete[] m_vertices;
}

Boid::~Boid()
{

}


void Boid::Eat(){
	//Increase scale by 0.1 * weight
	//SetScale(GetFloatScale() + (m_weight * 0.1f));
	//Decrease max speed by 2.0 units
	max_speed -= 2.0f;
	//Increment weight
	m_weight++;
	//Set last kill time to now
	lastUpdateTickCount = GetTickCount64();
}
void Boid::Starve(){
	//Decrease scale by 0.1 * weight
	//SetScale(GetFloatScale() - (m_weight * 0.1f));
	//Increase max speed by 2.0 units
	max_speed += 2.0f;
	//Decrement weight
	m_weight--;
	//Set last kill time to now
	lastUpdateTickCount = GetTickCount64();
}

void Boid::Tick(GameData* GD)
{
	//If the simulation is running
	if (GD->GS == GS_PLAY_PLAY)
	{
		//If this BOID is not an obstacle
		if (m_type != 0)
		{
			//If the BOID has eaten
			if (m_weight > 1)
			{
				//If it has been a predetermined amount of time since the BOID ate/starved
				if (GetTickCount64() - lastUpdateTickCount > SimulationParameters::starvationTime)
				{
					Starve();
				}
			}
			//For safety always set direction y component to 0
			m_direction.y = 0.0f;
			//Normalize direction
			m_direction.Normalize();
			//Create a new speed based on weight
			float w_speed = m_speed;
			if (m_weight > 0)
			{
				w_speed *= 1.0f / m_weight;
			}
			//Move BOID in direction by new speed
			m_pos += m_direction * w_speed * GD->dt;
			//Point BOID in direction of travel
			m_yaw = atan2(m_direction.x, m_direction.z);
			//Slow down BOID to half max speed
			if (m_speed > max_speed / 2)
			{
				m_speed -= (m_speed - max_speed) * GD->dt;
			}
			else if (m_speed < max_speed / 2)
			{
				m_speed += GD->dt;
			}

		}
		VBGO::Tick(GD);
	}
}

void Boid::Draw(DrawData* DD)
{
	//Draw the BOID
	VBGO::Draw(DD);

	//If debug mode is on
	if (SimulationParameters::showDebug)
	{
		//Create 2 vertices
		//One at the object's position
		myVertex newVertex1;
		newVertex1.Pos = Vector3::Zero;
		newVertex1.Color = Color(1.0f, 1.0f, 1.0f, 1.0f);
		//One at the direction of travel
		myVertex newVertex2;
		newVertex2.Pos = m_direction * m_speed;
		newVertex2.Color = Color(1.0f, 1.0f, 1.0f, 1.0f);

		myVertex* lineVertices = new myVertex[2];

		lineVertices[0] = (newVertex1);
		lineVertices[1] = (newVertex2);

		//Build a line vertex buffer using these two points
		BuildLineVB(GameData::p3d, 2, lineVertices);

		//Set raster state
		ID3D11RasterizerState* useRasterS = m_pRasterState ? m_pRasterState : s_pRasterState;
		DD->pd3dImmediateContext->RSSetState(useRasterS);

		//Create a new world matrix based on a one scale and with no rotation and with translation set to the position of the BOID
		Matrix tempScale = Matrix::CreateScale(Vector3(1.0f, 1.0f, 1.0f));
		Matrix tempRot = Matrix::CreateFromYawPitchRoll(0.0f, 0.0f, 0.0f);
		Matrix tempTrans = Matrix::CreateTranslation(m_pos);
		Matrix newWorld = tempScale * tempRot * tempTrans;

		//Transpose new world matrix onto constant buffer
		s_pCB->world = newWorld.Transpose();
		//Transpose new rotation matrix onto constant buffer
		s_pCB->rot = m_rotMat.Transpose();

		//Set vertex buffer
		UINT stride = sizeof(myVertex);
		UINT offSet = 0;
		DD->pd3dImmediateContext->IASetVertexBuffers(0, 1, &m_LineVertexBuffer, &stride, &offSet);

		//Update the Constant Buffer in classes that inherit from this then push to the GPU here
		ID3D11Buffer* useCB = m_pConstantBuffer ? m_pConstantBuffer : s_pConstantBuffer;
		void* useCBData = m_pCB ? m_pCB : s_pCB;
		DD->pd3dImmediateContext->UpdateSubresource(useCB, 0, NULL, useCBData, 0, 0);
		DD->pd3dImmediateContext->VSSetConstantBuffers(0, 1, &useCB);
		DD->pd3dImmediateContext->PSSetConstantBuffers(0, 1, &useCB);

		//Set primitive type to linelist
		DD->pd3dImmediateContext->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_LINELIST);

		//Then finally draw
		DD->pd3dImmediateContext->DrawIndexed(2, 0, 0);//number here will need to change depending on the primative topology!

		//Cleanup line vertex buffer
		if (m_LineVertexBuffer)
		{
			m_LineVertexBuffer->Release();
			m_LineVertexBuffer = nullptr;
		}
	}
}

