#include "Model.h"
#include <assimp/Importer.hpp>
#include <assimp\scene.h>
#include <assimp\postprocess.h>

void Model::ProcessNode(aiNode * node, const aiScene * scene)
{
	for (UINT i = 0; i < node->mNumMeshes; i++)
	{
		aiMesh* mesh = scene->mMeshes[node->mMeshes[i]];
		Sleep(1);
		//meshes.push_back(this->processMesh(mesh, scene));
	}

	for (UINT i = 0; i < node->mNumChildren; i++)
	{
		this->ProcessNode(node->mChildren[i], scene);
	}
}

bool Model::Initialize(ID3D11Device * device, ID3D11DeviceContext * deviceContext, ID3D11ShaderResourceView * texture, ConstantBuffer<CB_VS_vertexshader>& cb_vs_vertexshader)
{
	this->device = device;
	this->deviceContext = deviceContext;
	this->texture = texture;
	this->cb_vs_vertexshader = &cb_vs_vertexshader;
	/*using namespace Assimp;
	Importer importer;
	const aiScene * pScene = importer.ReadFile("Data\\Objects\\Nanosuit\\nanosuit.obj", aiProcess_Triangulate |
												aiProcess_ConvertToLeftHanded);
	if (pScene != nullptr)
	{
		this->ProcessNode(pScene->mRootNode, pScene);
	}*/

	try
	{
		XMFLOAT3 positionsArray[] =
		{
			XMFLOAT3(-0.5f,  -0.5f, -0.5f), //FRONT Bottom Left   - [0]
			XMFLOAT3(-0.5f,   0.5f, -0.5f), //FRONT Top Left      - [1]
			XMFLOAT3(0.5f,   0.5f, -0.5f), //FRONT Top Right     - [2]
			XMFLOAT3(0.5f,  -0.5f, -0.5f), //FRONT Bottom Right   - [3]
			XMFLOAT3(-0.5f,  -0.5f, 0.5f), //BACK Bottom Left   - [4]
			XMFLOAT3(-0.5f,   0.5f, 0.5f), //BACK Top Left      - [5]
			XMFLOAT3(0.5f,   0.5f, 0.5f), //BACK Top Right     - [6]
			XMFLOAT3(0.5f,  -0.5f, 0.5f ), //BACK Bottom Right   - [7]
		};
		XMFLOAT2 texcoordsArray[] =
		{
			XMFLOAT2( 0.0f, 1.0f), //FRONT Bottom Left   - [0]
			XMFLOAT2( 0.0f, 0.0f), //FRONT Top Left      - [1]
			XMFLOAT2( 1.0f, 0.0f), //FRONT Top Right     - [2]
			XMFLOAT2( 1.0f, 1.0f), //FRONT Bottom Right   - [3]
			XMFLOAT2( 0.0f, 1.0f), //BACK Bottom Left   - [4]
			XMFLOAT2( 0.0f, 0.0f), //BACK Top Left      - [5]
			XMFLOAT2( 1.0f, 0.0f), //BACK Top Right     - [6]
			XMFLOAT2( 1.0f, 1.0f), //BACK Bottom Right   - [7]
		};
		XMFLOAT3 normalsArray[] =
		{
			XMFLOAT3(0,0,0),
			XMFLOAT3(0,0,0),
			XMFLOAT3(0,0,0),
			XMFLOAT3(0,0,0),
			XMFLOAT3(0,0,0),
			XMFLOAT3(0,0,0),
			XMFLOAT3(0,0,0),
			XMFLOAT3(0,0,0),
		};
		
		//Load Vertex Data
		HRESULT hr = this->positions.Initialize(this->device, positionsArray, ARRAYSIZE(positionsArray));
		COM_ERROR_IF_FAILED(hr, "Failed to initialize positions vertex buffer.");

		hr = this->texcoords.Initialize(this->device, texcoordsArray, ARRAYSIZE(texcoordsArray));
		COM_ERROR_IF_FAILED(hr, "Failed to initialize text coords vertex buffer.");

		hr = this->normals.Initialize(this->device, normalsArray, ARRAYSIZE(normalsArray));
		COM_ERROR_IF_FAILED(hr, "Failed to initialize normals vertex buffer.");


		DWORD indices[] =
		{
			0, 1, 2, //FRONT
			0, 2, 3, //FRONT
			4, 7, 6, //BACK 
			4, 6, 5, //BACK
			3, 2, 6, //RIGHT SIDE
			3, 6, 7, //RIGHT SIDE
			4, 5, 1, //LEFT SIDE
			4, 1, 0, //LEFT SIDE
			1, 5, 6, //TOP
			1, 6, 2, //TOP
			0, 3, 7, //BOTTOM
			0, 7, 4, //BOTTOM
		};

		//Load Index Data
		hr = this->indexBuffer.Initialize(this->device, indices, ARRAYSIZE(indices));
		COM_ERROR_IF_FAILED(hr, "Failed to initialize index buffer.");
	}
	catch (COMException & exception)
	{
		ErrorLogger::Log(exception);
		return false;
	}

	this->SetPosition(0.0f, 0.0f, 0.0f);
	this->SetRotation(0.0f, 0.0f, 0.0f);
	this->UpdateWorldMatrix();
	return true;
}

void Model::SetTexture(ID3D11ShaderResourceView * texture)
{
	this->texture = texture;
}

void Model::Draw(const XMMATRIX & viewProjectionMatrix)
{
	//Update Constant buffer with WVP Matrix
	this->cb_vs_vertexshader->data.mat = this->worldMatrix * viewProjectionMatrix; //Calculate World-View-Projection Matrix
	this->cb_vs_vertexshader->data.mat = XMMatrixTranspose(this->cb_vs_vertexshader->data.mat);
	this->cb_vs_vertexshader->ApplyChanges();
	this->deviceContext->VSSetConstantBuffers(0, 1, this->cb_vs_vertexshader->GetAddressOf());

	this->deviceContext->PSSetShaderResources(0, 1, &this->texture); //Set Texture
	this->deviceContext->IASetIndexBuffer(this->indexBuffer.Get(), DXGI_FORMAT::DXGI_FORMAT_R32_UINT, 0); 
	UINT offset = 0;
	this->deviceContext->IASetVertexBuffers(0, 1, this->positions.GetAddressOf(), this->positions.StridePtr(), &offset);
	this->deviceContext->IASetVertexBuffers(1, 1, this->texcoords.GetAddressOf(), this->texcoords.StridePtr(), &offset);
	this->deviceContext->IASetVertexBuffers(2, 1, this->normals.GetAddressOf(), this->normals.StridePtr(), &offset);

	this->deviceContext->DrawIndexed(this->indexBuffer.BufferSize(), 0, 0); //Draw
}

void Model::UpdateWorldMatrix()
{
	this->worldMatrix = XMMatrixRotationRollPitchYaw(this->rot.x, this->rot.y, this->rot.z) * XMMatrixTranslation(this->pos.x, this->pos.y, this->pos.z);
	XMMATRIX vecRotationMatrix = XMMatrixRotationRollPitchYaw(0.0f, this->rot.y, 0.0f);
	this->vec_forward = XMVector3TransformCoord(this->DEFAULT_FORWARD_VECTOR, vecRotationMatrix);
	this->vec_backward = XMVector3TransformCoord(this->DEFAULT_BACKWARD_VECTOR, vecRotationMatrix);
	this->vec_left = XMVector3TransformCoord(this->DEFAULT_LEFT_VECTOR, vecRotationMatrix);
	this->vec_right = XMVector3TransformCoord(this->DEFAULT_RIGHT_VECTOR, vecRotationMatrix);
}

const XMVECTOR & Model::GetPositionVector() const
{
	return this->posVector;
}

const XMFLOAT3 & Model::GetPositionFloat3() const
{
	return this->pos;
}

const XMVECTOR & Model::GetRotationVector() const
{
	return this->rotVector;
}

const XMFLOAT3 & Model::GetRotationFloat3() const
{
	return this->rot;
}

void Model::SetPosition(const XMVECTOR & pos)
{
	XMStoreFloat3(&this->pos, pos);
	this->posVector = pos;
	this->UpdateWorldMatrix();
}

void Model::SetPosition(const XMFLOAT3 & pos)
{
	this->pos = pos;
	this->posVector = XMLoadFloat3(&this->pos);
	this->UpdateWorldMatrix();
}

void Model::SetPosition(float x, float y, float z)
{
	this->pos = XMFLOAT3(x, y, z);
	this->posVector = XMLoadFloat3(&this->pos);
	this->UpdateWorldMatrix();
}

void Model::AdjustPosition(const XMVECTOR & pos)
{
	this->posVector += pos;
	XMStoreFloat3(&this->pos, this->posVector);
	this->UpdateWorldMatrix();
}

void Model::AdjustPosition(const XMFLOAT3 & pos)
{
	this->pos.x += pos.y;
	this->pos.y += pos.y;
	this->pos.z += pos.z;
	this->posVector = XMLoadFloat3(&this->pos);
	this->UpdateWorldMatrix();
}

void Model::AdjustPosition(float x, float y, float z)
{
	this->pos.x += x;
	this->pos.y += y;
	this->pos.z += z;
	this->posVector = XMLoadFloat3(&this->pos);
	this->UpdateWorldMatrix();
}

void Model::SetRotation(const XMVECTOR & rot)
{
	this->rotVector = rot;
	XMStoreFloat3(&this->rot, rot);
	this->UpdateWorldMatrix();
}

void Model::SetRotation(const XMFLOAT3 & rot)
{
	this->rot = rot;
	this->rotVector = XMLoadFloat3(&this->rot);
	this->UpdateWorldMatrix();
}

void Model::SetRotation(float x, float y, float z)
{
	this->rot = XMFLOAT3(x, y, z);
	this->rotVector = XMLoadFloat3(&this->rot);
	this->UpdateWorldMatrix();
}

void Model::AdjustRotation(const XMVECTOR & rot)
{
	this->rotVector += rot;
	XMStoreFloat3(&this->rot, this->rotVector);
	this->UpdateWorldMatrix();
}

void Model::AdjustRotation(const XMFLOAT3 & rot)
{
	this->rot.x += rot.x;
	this->rot.y += rot.y;
	this->rot.z += rot.z;
	this->rotVector = XMLoadFloat3(&this->rot);
	this->UpdateWorldMatrix();
}

void Model::AdjustRotation(float x, float y, float z)
{
	this->rot.x += x;
	this->rot.y += y;
	this->rot.z += z;
	this->rotVector = XMLoadFloat3(&this->rot);
	this->UpdateWorldMatrix();
}

void Model::SetLookAtPos(XMFLOAT3 lookAtPos)
{
	//Verify that look at pos is not the same as cam pos. They cannot be the same as that wouldn't make sense and would result in undefined behavior.
	if (lookAtPos.x == this->pos.x && lookAtPos.y == this->pos.y && lookAtPos.z == this->pos.z)
		return;

	lookAtPos.x = this->pos.x - lookAtPos.x;
	lookAtPos.y = this->pos.y - lookAtPos.y;
	lookAtPos.z = this->pos.z - lookAtPos.z;

	float pitch = 0.0f;
	if (lookAtPos.y != 0.0f)
	{
		const float distance = sqrt(lookAtPos.x * lookAtPos.x + lookAtPos.z * lookAtPos.z);
		pitch = atan(lookAtPos.y / distance);
	}

	float yaw = 0.0f;
	if (lookAtPos.x != 0.0f)
	{
		yaw = atan(lookAtPos.x / lookAtPos.z);
	}
	if (lookAtPos.z > 0)
		yaw += XM_PI;

	this->SetRotation(pitch, yaw, 0.0f);
}

const XMVECTOR & Model::GetForwardVector()
{
	return this->vec_forward;
}

const XMVECTOR & Model::GetRightVector()
{
	return this->vec_right;
}

const XMVECTOR & Model::GetBackwardVector()
{
	return this->vec_backward;
}

const XMVECTOR & Model::GetLeftVector()
{
	return this->vec_left;
}