#include "Camera.h"
#include <ios>

template<class X>
float Dot(const X &v, const X &u)
{
	return (v.x * u.x) + (v.y * u.y) + (v.z * u.z);
}

template<class X>
float Magnitude(const X &v)
{
	return (v.x * v.x) + (v.y * v.y) + (v.z * v.z);
}

template<class X>
X Normalize(const X & v)
{
	const float result = Magnitude(v);

	return X(v.x / result, v.y / result, v.z / result);
}

Camera::Camera(const XMFLOAT3 &worldPos, const UINT &windowHeight, const UINT &windowWidth) : worldPosition(worldPos), _WindowHeight(windowHeight), _WindowWidth(windowWidth), lookVerticalSpeed(100.f)
{
	atPos = XMFLOAT3(0.0f, 0.0f, 1.0f);

	position = XMVectorSet(worldPos.x, worldPos.y, worldPos.z, 1);
	at = XMVectorSet(atPos.x, atPos.y, atPos.z, 1.0f);
	up = XMVectorSet(0.0f, 1.0f, 0.0f, 1.0f);

	rotationX = 0.05f;
	rotationY = 0.05f;
	
	forwardMoveSpeed = 0.0f;
	backwardMoveSpeed = 0.0f;
	leftTurnSpeed = 10.f;
	rightTurnSpeed = 10.0f;
	ascendingSpeed = 0.0f;
	descendingSpeed = 0.0f;
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

	worldPosition.x -= result.x * forwardMoveSpeed;
	worldPosition.z -= result.z * forwardMoveSpeed;
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
		// atPos.x = worldPosition.x + sinf(rotationY) * leftTurnSpeed;
		// atPos.z = worldPosition.z + cosf(rotationY) * leftTurnSpeed;

		// Updates Rotation
		rotationY -= 0.05f;
	}
}

void Camera::RightTurn()
{
	if (GetAsyncKeyState(VK_RIGHT))
	{ 

		// Updates Rotation
		rotationY += 0.05f;
	}
}

void Camera::UpwardTurn()
{
	if (GetAsyncKeyState('W') & 0x8000)
	{
		rotationX -= rotationX > -1.35 ? 0.05f : .0f;
	}
}

void Camera::DownwardTurn()
{
	if (GetAsyncKeyState('S') & 0x8000)
	{

		rotationX += rotationX < 1.35 ? .05f : 0;
	}

}

void Camera::Update()
{
	Movement();

	auto forwardVector = XMVectorSet(0, 0, 1, 0);

	auto rotationMatrix = XMMatrixRotationRollPitchYaw(rotationX, rotationY, 0);
	auto target = XMVector3TransformCoord(forwardVector, rotationMatrix);
	target = XMVector3Normalize(target);

	XMMATRIX yMatrix = XMMatrixRotationY(rotationY);

	auto camHorizontal = XMVector3TransformCoord(XMVectorSet(worldPosition.x, worldPosition.y, worldPosition.z, 0), yMatrix);
	XMVECTOR camUp = XMVECTOR();
	camUp = XMVector3TransformCoord(camUp, yMatrix);
	auto forward = XMVector3TransformCoord(forwardVector, yMatrix);

	position += leftTurnSpeed * camHorizontal;
	position += forwardMoveSpeed * forwardVector;
	
	leftTurnSpeed = 0;
	backwardMoveSpeed = 0;

	at = position + target;
	
	// Initialize the view matrix
	XMStoreFloat4x4(&view, XMMatrixLookAtLH(position, at, up));

	// Initialize the projection matrix
	XMStoreFloat4x4(&projection, XMMatrixPerspectiveFovLH(XM_PIDIV2, _WindowWidth / FLOAT(_WindowHeight), 0.01f, 100.0f));
}


void Camera::SetUp(const XMVECTOR& _up)
{
	up = _up;
}

void Camera::SetWorldPosition(const XMFLOAT3& eyePos)
{
	worldPosition = eyePos;
}

void Camera::SetAt(const XMFLOAT3& at)
{
	atPos = at;
}

void Camera::SetViewMatrix(const XMFLOAT4X4& _view)
{
	view = _view;
}

void Camera::SetProjectionMatrix(const XMFLOAT4X4& _projection)
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
	return position;
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