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
	if (m_type == BOID_SPHERE)
	{
		SphereTransform(Color(1.0f, 1.0f, 1.0f));
	}
	if (m_type == BOID_RED_SPHERE)
	{
		SphereTransform(Color(1.0f, 0.0f, 0.0f));
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
	//Decrease scale by 0.05x
	m_scale.x -= (m_weight * 0.05);
	m_scale.y -= (m_weight * 0.05);
	m_scale.z -= (m_weight * 0.05);
	//Increase max speed by 2.0 units
	max_speed += 2.0f;
	//Decrement weight
	m_weight--;
	//Set last kill time to now
	lastKillTickCount = GetTickCount64();
}

void Boid::Tick(GameData* GD)
{
	//If this BOID is not an obstacle
	if (m_type != BOID_OBSTACLE){
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
	}
	VBGO::Tick(GD);
}

void Boid::Draw(DrawData* DD)
{
	VBGO::Draw(DD);
}

