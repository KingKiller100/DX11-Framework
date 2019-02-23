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

	UINT _WindowHeight;
	UINT _WindowWidth;

	float forwardMoveSpeed, backwardMoveSpeed;
	float ascendingSpeed, descendingSpeed;
	float rotationX, rotationY; 
	float turnSpeed;

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
	Camera(const UINT &windowHeight, const UINT &windowWidth);
	void Release();

	// Camera Movement method
	void Movement();
	void Update();
	void RecalculateCamera();

	// Variables Getters & Setters 
	void SetWorldPosition(const XMFLOAT3& eyePos);
	void SetAt(const XMFLOAT3& atPos);
	void SetViewMatrix(const XMFLOAT4X4& view);
	void SetProjectionMatrix(const XMFLOAT4X4& _projection);

	XMVECTOR GetEye() const, GetUp() const;
	XMFLOAT3 GetAt() const;
	XMFLOAT3 GetWorldPosition() const;
	XMFLOAT4X4 GetViewMatrix() const;
	XMFLOAT4X4 GetProjectionMatrix() const;
};