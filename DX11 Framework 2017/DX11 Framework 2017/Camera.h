#pragma once
#include <Windows.h>
#include <DirectXMath.h>
#include <d3d11_1.h>

using namespace DirectX;

class Camera
{
private:
	// Initialize the view matrix
	XMVECTOR eye;
	XMVECTOR at;
	XMVECTOR up;
	XMFLOAT4X4 view;
	XMFLOAT4X4 projection;

	// Camera position in world space
	XMFLOAT3 worldPosition; 	
	XMFLOAT3 atPos;

	UINT _WindowHeight;
	UINT _WindowWidth;

	float forwardMoveSpeed, backwardMoveSpeed;
	float leftTurnSpeed, rightTurnSpeed;
	float ascendingSpeed, descendingSpeed;
	float lookUpSpeed, lookDownSpeed;

	float rotationX, rotationY, rotationZ; 


private:

	void ForwardMovement();
	void BackwardMovement();
	void Ascension();
	void Descension();
	void LeftTurn();
	void RightTurn();
	void UpwardTurn();
	void DownwardTurn();

public:
	Camera(UINT windowHeight, UINT windowWidth);
	void Release();

	// Camera Movement method
	void Movement();
	void Update();

	// Variables Getters & Setters 
	void SetWorldPosition(XMFLOAT3 eyePos);
	void SetAt(XMFLOAT3 at);
	void SetUp(XMVECTOR _up);
	void SetViewMatrix(XMFLOAT4X4 view);
	void SetProjectionMatrix(XMFLOAT4X4 _projection);

	XMVECTOR GetEye() const, GetUp() const;
	XMFLOAT3 GetAt() const;
	XMFLOAT3 GetWorldPosition() const;
	XMFLOAT4X4 GetViewMatrix() const;
	XMFLOAT4X4 GetProjectionMatrix() const;
};