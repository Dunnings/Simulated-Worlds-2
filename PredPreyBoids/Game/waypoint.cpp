#include "waypoint.h"
#include "drawdata.h"

Waypoint::Waypoint(){

}

Waypoint::~Waypoint(){

}

void Waypoint::Tick(GameData* GD)
{ 
	VBGO::Tick(GD);
}
void Waypoint::Draw(DrawData* DD)
{ 
	if (SimulationParameters::showDebug)
	{
		VBGO::Draw(DD);

		int sides = 100;
		float Angle = (360.0f / sides) * (XM_PI/180);

		Vector3 vec(areaOfInfluence * cos(Angle), 0.0f, areaOfInfluence * sin(Angle));
		Vector3 rot;
		int x = 0;

		Color c;
		if (myType == waypointType::start){
			c = Color(0.0f, 1.0f, 0.0f);
		}
		else if (myType == waypointType::outpost){
			c = Color(0.0f, 0.0f, 1.0f);
		}
		else if (myType == waypointType::finish){
			c = Color(1.0f, 0.0f, 0.0f);
		}

		for (unsigned short i = 0; i <= sides; ++i)
		{
			rot = Vector3(areaOfInfluence * cos(Angle * (i+1)), 0.0f, areaOfInfluence * sin(Angle * (i+1)));
			myVertex newVertex1;
			myVertex newVertex2;
			newVertex1.Pos = vec;
			newVertex1.Color = c;
			++x;
			newVertex2.Pos = rot;
			newVertex2.Color = c;
			++x;
			vec = rot;
			lineVertices.push_back(newVertex1);
			lineVertices.push_back(newVertex2);
		}

		myVertex* vertexArray = new myVertex[x];
		int i = 0;

		for (vector<myVertex>::iterator it = lineVertices.begin(); it != lineVertices.end(); ++it){
			vertexArray[i] = (*it);
			++i;
		}

		//Build a line vertex buffer using these two points
		BuildLineVB(GameData::p3d, x, vertexArray);

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
		DD->pd3dImmediateContext->DrawIndexed(x, 0, 0);//number here will need to change depending on the primative topology!

		//Cleanup line vertex buffer
		if (m_LineVertexBuffer)
		{
			m_LineVertexBuffer->Release();
			m_LineVertexBuffer = nullptr;
		}
		lineVertices.clear();
	}
}


void Waypoint::initialize()
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
		indices[i] = i;
		m_vertices[i].texCoord = Vector2::One;
	}

	Color c = Color(1.0f, 1.0f, 1.0f);
	if (typeToAffect == 1){
		c = Color(0.0f, 0.0f, 1.0f);
	}
	else if (myType == 2){
		c = Color(0.0f, 1.0f, 0.0f);
	}
	else if (myType == 3){
		c = Color(1.0f, 0.0f, 0.0f);
	}
	else if (myType == 4){
		c = Color(0.0f, 1.0f, 1.0f);
	}
	else if (myType == 5){
		c = Color(1.0f, 0.0f, 1.0f);
	}

	//In each loop create the two traingles for the matching sub-square on each of the six faces
	int vert = 0;
	for (int i = -(m_size - 1) / 2; i<(m_size - 1) / 2; i++)
	{
		for (int j = -(m_size - 1) / 2; j<(m_size - 1) / 2; j++)
		{
			//Top
			m_vertices[vert].Color = c;
			m_vertices[vert++].Pos = Vector3((float)i, 0.5f * (float)(m_size - 1), (float)j);
			m_vertices[vert].Color = c;
			m_vertices[vert++].Pos = Vector3((float)i, 0.5f * (float)(m_size - 1), (float)(j + 1));
			m_vertices[vert].Color = c;
			m_vertices[vert++].Pos = Vector3((float)(i + 1), 0.5f * (float)(m_size - 1), (float)j);

			m_vertices[vert].Color = c;
			m_vertices[vert++].Pos = Vector3((float)(i + 1), 0.5f * (float)(m_size - 1), (float)j);
			m_vertices[vert].Color = c;
			m_vertices[vert++].Pos = Vector3((float)i, 0.5f * (float)(m_size - 1), (float)(j + 1));
			m_vertices[vert].Color = c;
			m_vertices[vert++].Pos = Vector3((float)(i + 1), 0.5f * (float)(m_size - 1), (float)(j + 1));

			//Back
			m_vertices[vert].Color = c;
			m_vertices[vert++].Pos = Vector3((float)i, (float)j, 0.5f * (float)(m_size - 1));
			m_vertices[vert].Color = c;
			m_vertices[vert++].Pos = Vector3((float)(i + 1), (float)j, 0.5f * (float)(m_size - 1));
			m_vertices[vert].Color = c;
			m_vertices[vert++].Pos = Vector3((float)i, (float)(j + 1), 0.5f * (float)(m_size - 1));

			m_vertices[vert].Color = c;
			m_vertices[vert++].Pos = Vector3((float)(i + 1), (float)j, 0.5f * (float)(m_size - 1));
			m_vertices[vert].Color = c;
			m_vertices[vert++].Pos = Vector3((float)(i + 1), (float)(j + 1), 0.5f * (float)(m_size - 1));
			m_vertices[vert].Color = c;
			m_vertices[vert++].Pos = Vector3((float)i, (float)(j + 1), 0.5f * (float)(m_size - 1));

			//Right
			m_vertices[vert].Color = c;
			m_vertices[vert++].Pos = Vector3(0.5f * (float)(m_size - 1), (float)i, (float)j);
			m_vertices[vert].Color = c;
			m_vertices[vert++].Pos = Vector3(0.5f * (float)(m_size - 1), (float)(i + 1), (float)j);
			m_vertices[vert].Color = c;
			m_vertices[vert++].Pos = Vector3(0.5f * (float)(m_size - 1), (float)i, (float)(j + 1));

			m_vertices[vert].Color = c;
			m_vertices[vert++].Pos = Vector3(0.5f * (float)(m_size - 1), (float)(i + 1), (float)j);
			m_vertices[vert].Color = c;
			m_vertices[vert++].Pos = Vector3(0.5f * (float)(m_size - 1), (float)(i + 1), (float)(j + 1));
			m_vertices[vert].Color = c;
			m_vertices[vert++].Pos = Vector3(0.5f * (float)(m_size - 1), (float)i, (float)(j + 1));

			//Bottom
			m_vertices[vert].Color = c;
			m_vertices[vert++].Pos = Vector3((float)j, -0.5f * (float)(m_size - 1), (float)i);
			m_vertices[vert].Color = c;
			m_vertices[vert++].Pos = Vector3((float)(j + 1), -0.5f * (float)(m_size - 1), (float)i);
			m_vertices[vert].Color = c;
			m_vertices[vert++].Pos = Vector3((float)j, -0.5f * (float)(m_size - 1), (float)(i + 1));

			m_vertices[vert].Color = c;
			m_vertices[vert++].Pos = Vector3((float)j, -0.5f * (float)(m_size - 1), (float)(i + 1));
			m_vertices[vert].Color = c;
			m_vertices[vert++].Pos = Vector3((float)(j + 1), -0.5f * (float)(m_size - 1), (float)i);
			m_vertices[vert].Color = c;
			m_vertices[vert++].Pos = Vector3((float)(j + 1), -0.5f * (float)(m_size - 1), (float)(i + 1));

			//Front
			m_vertices[vert].Color = c;
			m_vertices[vert++].Pos = Vector3((float)j, (float)i, -0.5f * (float)(m_size - 1));
			m_vertices[vert].Color = c;
			m_vertices[vert++].Pos = Vector3((float)j, (float)(i + 1), -0.5f * (float)(m_size - 1));
			m_vertices[vert].Color = c;
			m_vertices[vert++].Pos = Vector3((float)(j + 1), (float)i, -0.5f * (float)(m_size - 1));

			m_vertices[vert].Color = c;
			m_vertices[vert++].Pos = Vector3((float)j, (float)(i + 1), -0.5f * (float)(m_size - 1));
			m_vertices[vert].Color = c;
			m_vertices[vert++].Pos = Vector3((float)(j + 1), (float)(i + 1), -0.5f * (float)(m_size - 1));
			m_vertices[vert].Color = c;
			m_vertices[vert++].Pos = Vector3((float)(j + 1), (float)i, -0.5f * (float)(m_size - 1));

			//Left
			m_vertices[vert].Color = c;
			m_vertices[vert++].Pos = Vector3(-0.5f * (float)(m_size - 1), (float)j, (float)i);
			m_vertices[vert].Color = c;
			m_vertices[vert++].Pos = Vector3(-0.5f * (float)(m_size - 1), (float)j, (float)(i + 1));
			m_vertices[vert].Color = c;
			m_vertices[vert++].Pos = Vector3(-0.5f * (float)(m_size - 1), (float)(j + 1), (float)i);

			m_vertices[vert].Color = c;
			m_vertices[vert++].Pos = Vector3(-0.5f * (float)(m_size - 1), (float)j, (float)(i + 1));
			m_vertices[vert].Color = c;
			m_vertices[vert++].Pos = Vector3(-0.5f * (float)(m_size - 1), (float)(j + 1), (float)(i + 1));
			m_vertices[vert].Color = c;
			m_vertices[vert++].Pos = Vector3(-0.5f * (float)(m_size - 1), (float)(j + 1), (float)i);
		}
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

	BuildIB(GameData::p3d, indices);
	BuildVB(GameData::p3d, numVerts, m_vertices);

	delete[] m_vertices;
}
