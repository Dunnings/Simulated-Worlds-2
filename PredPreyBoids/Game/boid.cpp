#include "boid.h"
#include <Windows.h>
#include "drawdata.h"
#define _USE_MATH_DEFINES
#include <math.h>


Boid::Boid(Waypoint* _finish, Waypoint* _outpost)
{
	finish = _finish;
	outpost = _outpost;
	lastBreedTickCount = GetTickCount64();
	lastUpdateTickCount = GetTickCount64();
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
	
	//Change the boids to be different colour shapes using a transform
	if (m_type->id == 0)
	{
		SphereTransform(Color(1.0f, 1.0f, 1.0f));
	}
	else if (m_type->id == 1)
	{
		ConeTransform(Color(0.0f, 0.0f, 1.0f));
	}
	else if (m_type->id == 2)
	{
		ConeTransform(Color(0.0f, 1.0f, 0.0f));
	}
	else if (m_type->id == 3)
	{
		ConeTransform(Color(1.0f, 0.0f, 0.0f));
	}
	else if (m_type->id == 4)
	{
		ConeTransform(Color(0.0f, 1.0f, 1.0f));
	}
	else if (m_type->id == 5)
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

void Boid::Breed()
{
	lastBreedTickCount = GetTickCount64();
}

void Boid::Tick(GameData* GD)
{
	//If the simulation is running
	if (GD->GS == GS_PLAY_PLAY)
	{
		//If this boid is not an obstacle
		if (m_type->id != 0)
		{
			//If the boid has eaten
			if (m_weight > 1)
			{
				//If it has been a predetermined amount of time since the boid ate/starved
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
			//Move boid in direction by new speed
			m_pos += m_direction * w_speed * GD->dt;
			//Point boid in direction of travel
			m_yaw = atan2(m_direction.x, m_direction.z);
			//Slow down boid to half max speed
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
	//Draw the boid
	VBGO::Draw(DD);

	//If debug mode is on
	if (SimulationParameters::showDebugForces || SimulationParameters::showDebugSight)
	{
		//Integer to keep count of vertices
		int vertexCount = 0;
		if (SimulationParameters::showDebugForces){
			//Create 2 vertices
			//One at the object's position
			myVertex newVertex1;
			newVertex1.Pos = Vector3::Zero;
			newVertex1.Color = Color(1.0f, 1.0f, 1.0f, 1.0f);
			//One at the direction of travel
			myVertex newVertex2;
			newVertex2.Pos = m_direction * m_speed;
			newVertex2.Color = Color(1.0f, 1.0f, 1.0f, 1.0f);

			lineVertices.push_back(newVertex1);
			vertexCount++;
			lineVertices.push_back(newVertex2);
			vertexCount++;
		}
		if (SimulationParameters::showDebugSight){
			//Define how many sides this "circle" has
			int sides = 50;
			//The angle between each vertex is defined here
			float Angle = (360.0f / sides) * (XM_PI / 180);
			//Initialize first vertex position 
			Vector3 first(m_sight * cos(Angle), 0.0f, m_sight * sin(Angle));
			//Initialize second vertex position
			Vector3 second;
			//Define a color to use for every vertex based on the type of waypoint
			Color c = Color(0.5f, 1.0f, 1.0f);

			//Loop through for each side
			for (unsigned short i = 0; i <= sides; ++i)
			{
				//Set the second vertex to be the next point around the circle
				second = Vector3(m_sight * cos(Angle * (i + 1)), 0.0f, m_sight * sin(Angle * (i + 1)));
				//Define two vertices
				myVertex newVertex1;
				myVertex newVertex2;
				//Assign the first one the first position and the preset colour
				newVertex1.Pos = first;
				newVertex1.Color = c;
				//Increment vertex count
				++vertexCount;
				//Assign the second one the second position and the preset colour
				newVertex2.Pos = second;
				newVertex2.Color = c;
				//Increment vertex count
				++vertexCount;
				//Now to move onto the next two vertices set first to second
				first = second;
				//Add the two new vertices to lineVertices vector
				lineVertices.push_back(newVertex1);
				lineVertices.push_back(newVertex2);
			}
		}

		//Code to convert from vertex to array, which is needed to be passed in as a void pointer to Build vertex buffer
		myVertex* vertexArray = new myVertex[vertexCount];
		int i = 0;
		for (vector<myVertex>::iterator it = lineVertices.begin(); it != lineVertices.end(); ++it){
			vertexArray[i] = (*it);
			++i;
		}

		//Build a line vertex buffer using these two points
		BuildLineVB(GameData::p3d, vertexCount, vertexArray);

		//Set raster state
		ID3D11RasterizerState* useRasterS = m_pRasterState ? m_pRasterState : s_pRasterState;
		DD->pd3dImmediateContext->RSSetState(useRasterS);

		//Create a new world matrix based on a one scale and with no rotation and with translation set to the position of the boid
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
		DD->pd3dImmediateContext->DrawIndexed(vertexCount, 0, 0);//number here will need to change depending on the primative topology!

		//Cleanup line vertex buffer
		if (m_LineVertexBuffer)
		{
			m_LineVertexBuffer->Release();
			m_LineVertexBuffer = nullptr;
		}
		lineVertices.clear();
	}
}

