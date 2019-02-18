#include "Camera.h"
#include <ios>

Camera::Camera(UINT windowHeight, UINT windowWidth) : _WindowHeight(windowHeight), _WindowWidth(windowWidth)
{
	rotationX = 0.05f;
	rotationY = 0.0f;
	rotationZ = 1.0f;
	
	forwardMoveSpeed = 0.0f;
	backwardMoveSpeed = 0.0f;
	leftTurnSpeed = 1000.f;
	rightTurnSpeed = 1000.0f;
	ascendingSpeed = 0.0f;
	descendingSpeed = 0.0f;
	lookUpSpeed = 100.0f;
	lookDownSpeed = 100.0f;

	Update();
}

void Camera::Movement()
{	 
	ForwardMovement();
	BackwardMovement();
	LeftTurn();
	RightTurn();
	Ascension();
	Descension();
	UpwardTurn();
	DownwardTurn();
}

XMFLOAT3 Normalize(const XMFLOAT3 & v)
{
	const float result = sqrtf(v.x * v.x + v.y * v.y + v.z * v.z);

	return XMFLOAT3(v.x / result, v.y / result, v.z / result);
}

void Camera::ForwardMovement()
{
	if (!GetAsyncKeyState(VK_SHIFT) && GetAsyncKeyState(VK_UP))
	{
		forwardMoveSpeed += 0.01f;

		if (forwardMoveSpeed > 0.5f)
			forwardMoveSpeed = 0.5f;
	}
	else
	{
		forwardMoveSpeed -= 0.01f;

		if (forwardMoveSpeed < 0.0f)
			forwardMoveSpeed = 0.0f;
	}

	const XMFLOAT3 result = Normalize(XMFLOAT3(atPos.x - worldPosition.x, atPos.y - worldPosition.y, atPos.z - worldPosition.z));

	worldPosition.x += result.x * forwardMoveSpeed;
	worldPosition.z += result.z * forwardMoveSpeed;
}

void Camera::BackwardMovement()
{
	if (!GetAsyncKeyState(VK_SHIFT) && GetAsyncKeyState(VK_DOWN))
	{
		backwardMoveSpeed += 0.01f;

		if (backwardMoveSpeed > 0.5f)
			backwardMoveSpeed = 0.5f;
	}
	else
	{
		backwardMoveSpeed -= 0.01f;

		if (backwardMoveSpeed < 0.0f)		
			backwardMoveSpeed = 0.0f;		
	}

	const XMFLOAT3 result = Normalize(XMFLOAT3(atPos.x - worldPosition.x, atPos.y - worldPosition.y, atPos.z - worldPosition.z));

	worldPosition.x -= result.x * backwardMoveSpeed;
	worldPosition.z -= result.z * backwardMoveSpeed;
}

void Camera::Ascension()
{
	if (GetAsyncKeyState(VK_SHIFT) && GetAsyncKeyState(VK_UP))
	{
		ascendingSpeed +=  0.1f;
		if (ascendingSpeed > 0.5f)
			ascendingSpeed = 0.5f;		
	}
	else
	{
		ascendingSpeed -= 0.02f;

		if (ascendingSpeed < 0.0f)		
			ascendingSpeed = 0.0f;		
	}

	worldPosition.y += ascendingSpeed;
}

void Camera::Descension()
{
	if (GetAsyncKeyState(VK_SHIFT) && GetAsyncKeyState(VK_DOWN))
	{
		descendingSpeed += 0.1f;
		if (descendingSpeed > 0.5f)
			descendingSpeed = 0.5f;		
	}
	else
	{
		descendingSpeed -= 0.02f;

		if (descendingSpeed < 0.0f)		
			descendingSpeed = 0.0f;		
	}

	worldPosition.y -= descendingSpeed;
}

void Camera::LeftTurn()
{
	if (GetAsyncKeyState(VK_LEFT))
	{
		atPos.x = worldPosition.x + sinf(rotationY) * leftTurnSpeed;
		atPos.z = worldPosition.z + cosf(rotationY) * leftTurnSpeed;

		// Updates Rotation
		rotationY -= 0.05f;
	}
}

void Camera::RightTurn()
{
	if (GetAsyncKeyState(VK_RIGHT))
	{ 
		atPos.x = worldPosition.x + sinf(rotationY) * rightTurnSpeed; 
		atPos.z = worldPosition.z + cosf(rotationY) * rightTurnSpeed;

		// Updates Rotation
		rotationY += 0.05f;
	}
}

void Camera::UpwardTurn()
{
	if (GetAsyncKeyState('W') & 0x8000)
	{
		rotationX -= 0.5f;

		atPos.y = worldPosition.y + sinf(rotationX) * lookUpSpeed;
	}
}

void Camera::DownwardTurn()
{
	if (GetAsyncKeyState('S') & 0x8000)
	{
		rotationX += .5f;
		
		atPos.y = worldPosition.y + sinf(rotationX) * lookDownSpeed;
	}

}

void Camera::Update()
{
	// Initialize the view matrix
	eye = XMVectorSet(worldPosition.x, worldPosition.y, worldPosition.z, 1.0f);
	at = XMVectorSet(atPos.x, atPos.y, atPos.z, 1.0f);
	up = XMVectorSet(0.0f, 1.0f, 0.0f, 1.0f);

	XMStoreFloat4x4(&view, XMMatrixLookAtLH(eye, at, up));

	// Initialize the projection matrix
	XMStoreFloat4x4(&projection, XMMatrixPerspectiveFovLH(XM_PIDIV2, _WindowWidth / FLOAT(_WindowHeight), 0.01f, 100.0f));

	Movement();
}


void Camera::SetUp(XMVECTOR _up)
{
	up = _up;
}

void Camera::SetWorldPosition(XMFLOAT3 eyePos)
{
	worldPosition = eyePos;
}

void Camera::SetAt(XMFLOAT3 at)
{
	atPos = at;
}

void Camera::SetViewMatrix(XMFLOAT4X4 _view)
{
	view = _view;
}

void Camera::SetProjectionMatrix(XMFLOAT4X4 _projection)
{
	projection = _projection;
}

XMFLOAT3 Camera::GetWorldPosition() const
{
	return worldPosition;
}

XMFLOAT4X4 Camera::GetViewMatrix() const
{
	return view;
}

XMFLOAT4X4 Camera::GetProjectionMatrix() const
{
	return projection;
}


XMVECTOR Camera::GetEye() const
{
	return eye;
}

XMVECTOR Camera::GetUp() const
{
	return up;
}

XMFLOAT3 Camera::GetAt() const
{
	return atPos;
}

void Camera::Release()
{

}