#pragma once
#include <Windows.h>
#include <DirectXMath.h>
#include <d3d11_1.h>

using namespace DirectX;

class Camera
{
private:
	// Initialize the view matrix
	XMVECTOR position;
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
	const float lookVerticalSpeed;

	float rotationX, rotationY; 


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
	Camera(const XMFLOAT3 &worldPos, const UINT &windowHeight, const UINT &windowWidth);
	void Release();

	// Camera Movement method
	void Movement();
	void Update();

	// Variables Getters & Setters 
	void SetWorldPosition(const XMFLOAT3& eyePos);
	void SetAt(const XMFLOAT3& at);
	void SetUp(const XMVECTOR& _up);
	void SetViewMatrix(const XMFLOAT4X4& view);
	void SetProjectionMatrix(const XMFLOAT4X4& _projection);

	XMVECTOR GetEye() const, GetUp() const;
	XMFLOAT3 GetAt() const;
	XMFLOAT3 GetWorldPosition() const;
	XMFLOAT4X4 GetViewMatrix() const;
	XMFLOAT4X4 GetProjectionMatrix() const;

	XMFLOAT3 Subtract(const XMFLOAT3 &left, const XMFLOAT3 &right) const
	{
		return XMFLOAT3(left.x - right.x, left.y - right.y, left.z - right.z);
	}
};