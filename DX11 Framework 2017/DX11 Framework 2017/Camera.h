#pragma once
#include <Windows.h>
#include <DirectXMath.h>
#include <d3d11_1.h>

using namespace DirectX;

class Camera
{
private:
	// Initialize the view matrix
	XMVECTOR Eye;
	XMVECTOR At;
	XMVECTOR Up;
	XMFLOAT4X4 _view;
	XMFLOAT4X4 _projection;

	// Camera position in world space
	XMFLOAT3 posW; 	
	XMFLOAT3 atPos;

	UINT _WindowHeight;
	UINT _WindowWidth;

	float forwardMoveSpeed, backwardMoveSpeed;
	float leftTurnSpeed, rightTurnSpeed;
	float ascendingSpeed, descendingSpeed;
	float lookUpSpeed, lookDownSpeed;
	float frameTimer;

	float rotationX, rotationY, rotationZ; 

public:
	Camera(UINT windowHeight, UINT windowWidth);
	void Release();

	void CreateCamera();
	void UpdateFrameTimer(float time);

	// Camera Movement method
	void CameraMovement();


	void ForwardMovement();
	void BackwardMovement();
	void Ascension();
	void Descension();
	void LeftTurn();
	void RightTurn();
	void UpwardTurn();
	void DownwardTurn();

	// Variables Getters & Setters 
	void SetWorldPosition(XMFLOAT3 eyePos);
	void SetEye(XMVECTOR eye);
	void SetAt(XMVECTOR at);
	void SetUp(XMVECTOR up);
	void SetViewMatrix(XMFLOAT4X4 view);
	void SetProjectionMatrix(XMFLOAT4X4 projection);
	XMVECTOR GetEye(), GetAt(), GetUp();
	XMFLOAT3 GetWorldPosition();
	float GetEyePosX(), GetEyePosY(), GetEyePosZ();
	XMFLOAT4X4 GetViewMatrix();
	XMFLOAT4X4 GetProjectionMatrix();
};