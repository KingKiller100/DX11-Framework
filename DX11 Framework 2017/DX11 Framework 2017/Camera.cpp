#include "Camera.h"
#include <ios>

Camera::Camera(UINT windowHeight, UINT windowWidth) : _WindowHeight(windowHeight), _WindowWidth(windowWidth)
{
	frameTimer = 0.0f;

	rotationX = 0.05f;
	rotationY = 0.0f;
	rotationZ = 1.0f;
	
	forwardMoveSpeed = 0.0f;
	backwardMoveSpeed = 0.0f;
	leftTurnSpeed = 1000.f;
	rightTurnSpeed = 1000.0f;
	ascendingSpeed = 0.0f;
	descendingSpeed = 0.0f;
	lookUpSpeed = 0.0f;
	lookDownSpeed = 0.0f;

	atPos = XMFLOAT3(worldPosition.x + sinf(rotationY), -5.0f, worldPosition.z + cosf(rotationY));

	CameraMovement();
}

void Camera::CameraMovement()
{	 
	ForwardMovement();
	BackwardMovement();
	LeftTurn();
	RightTurn();
	Ascension();
	Descension();
	UpwardTurn();
	DownwardTurn();

	// Updates Camera
	CreateCamera();
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
		forwardMoveSpeed += frameTimer * 0.001f;
		if (forwardMoveSpeed > frameTimer * 0.03f)
		{
			forwardMoveSpeed = frameTimer * 0.03f;
		}

		const XMFLOAT3 result = Normalize(XMFLOAT3(atPos.x - worldPosition.x, atPos.y - worldPosition.y, atPos.z - worldPosition.z));

		worldPosition.x += result.x * forwardMoveSpeed;
		worldPosition.z += result.z * forwardMoveSpeed;
	}
	else
	{
		forwardMoveSpeed -= frameTimer * 0.0007f;

		if (forwardMoveSpeed < 0.0f)
		{
			forwardMoveSpeed = 0.0f;
		}
	}
}

void Camera::BackwardMovement()
{	
	if (!GetAsyncKeyState(VK_SHIFT) && GetAsyncKeyState(VK_DOWN))
	{
		backwardMoveSpeed += frameTimer * 0.001f;
		if (backwardMoveSpeed > frameTimer * 0.03f)
		{
			backwardMoveSpeed = frameTimer * 0.03f;
		}

		const XMFLOAT3 result = Normalize(XMFLOAT3(atPos.x - worldPosition.x, atPos.y - worldPosition.y, atPos.z - worldPosition.z));

		worldPosition.x -= result.x * backwardMoveSpeed;
		worldPosition.z -= result.z * backwardMoveSpeed;
	}
	else
	{
		backwardMoveSpeed -= frameTimer * 0.0007f;

		if (backwardMoveSpeed < 0.0f)
		{
			backwardMoveSpeed = 0.0f;
		}
	}

}

void Camera::Ascension()
{
	if (GetAsyncKeyState(VK_SHIFT) && GetAsyncKeyState(VK_UP))
	{
		ascendingSpeed += frameTimer * 0.003f;
		if (ascendingSpeed > (frameTimer * 0.3f))
		{
			ascendingSpeed = frameTimer * 0.3f;
		}
	}
	else
	{
		ascendingSpeed -= frameTimer * 0.002f;
		if (ascendingSpeed < 0.0f)
		{
			ascendingSpeed = 0.0f;
		}
	}

	worldPosition.y += 0.5f * ascendingSpeed;
}

void Camera::Descension()
{
	if (GetAsyncKeyState(VK_SHIFT) && GetAsyncKeyState(VK_DOWN))
	{
		descendingSpeed += frameTimer * 0.003f;
		if (descendingSpeed > (frameTimer * 0.3f))
		{
			descendingSpeed = frameTimer * 0.3f;
		}
	}
	else
	{
		descendingSpeed -= frameTimer * 0.002f;
		if (descendingSpeed < 0.0f)
		{
			descendingSpeed = 0.0f;
		}
	}

	worldPosition.y -= 0.25f * descendingSpeed;
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
		atPos.x = sinf(rotationY) * rightTurnSpeed; 
		atPos.z = cosf(rotationY) * rightTurnSpeed;

		// Updates Rotation
		rotationY += 0.05f;
	}

}

void Camera::UpwardTurn()
{
	if (GetAsyncKeyState('W') & 0x8000)
	{
		lookUpSpeed += frameTimer * 0.01f;

		if (lookUpSpeed > (frameTimer * 0.15f))
		{
			lookUpSpeed = frameTimer * 0.15f;
		}
	}
	else
	{
		lookUpSpeed -= frameTimer * 0.005f;

		if (lookUpSpeed < 0.0f)
		{
			lookUpSpeed = 0.0f;
		}
	}

	rotationX -= lookUpSpeed;

	if (rotationX > 90.0f)
	{
		rotationX = 90.0f;
	}

	atPos.y += sinf(XMConvertToRadians(0.75f)) * lookUpSpeed;
	atPos.z += cosf(XMConvertToRadians(0.75f)) * lookUpSpeed;
}

void Camera::DownwardTurn()
{
	if (GetAsyncKeyState('S') & 0x8000)
	{
		lookDownSpeed += frameTimer * 0.01f;

		if (lookDownSpeed > (frameTimer * 0.15f))
		{
			lookDownSpeed = frameTimer * 0.15f;
		}
	}
	else
	{
		lookDownSpeed -= frameTimer * 0.005f;

		if (lookDownSpeed < 0.0f)
		{
			lookDownSpeed = 0.0f;
		}
	}

	rotationX += lookDownSpeed;

	if (rotationX < -90.0f)
	{
		rotationX = -90.0f;
	}

	atPos.y -= sinf(XMConvertToRadians(0.75f)) * lookDownSpeed;
	atPos.z -= cosf(XMConvertToRadians(0.75f)) * lookDownSpeed;
}

void Camera::CreateCamera()
{
	const XMVECTOR rotationQ = XMVectorSet(XMConvertToRadians(rotationX), XMConvertToRadians(rotationY), XMConvertToRadians(rotationZ), 1.0f);
	XMFLOAT4X4 _rotations;

	// Initialize the view matrix
	eye = XMVectorSet(worldPosition.x, worldPosition.y, worldPosition.z, 1.0f);
	at = XMVectorSet(atPos.x, atPos.y, atPos.z, 1.0f);
	up = XMVectorSet(0.0f, 1.0f, 0.0f, 1.0f);

	const float pitch = XMConvertToRadians(rotationX);
	const float yaw = XMConvertToRadians(rotationY);
	const float roll = XMConvertToRadians(rotationZ);

	XMVector3Rotate(at, rotationQ);

	XMMatrixRotationRollPitchYaw(pitch, yaw, roll);
	XMStoreFloat4x4(&_rotations, XMMatrixRotationX(rotationX) * XMMatrixRotationY(rotationY) * XMMatrixRotationZ(rotationZ));

	const XMMATRIX rotationMatrix = XMLoadFloat4x4(&_rotations);

	XMVector4Transform(at, rotationMatrix);
	XMVector4Transform(up, rotationMatrix);

	XMStoreFloat4x4(&view, XMMatrixLookAtLH(eye, at, up));

	// Initialize the projection matrix
	XMStoreFloat4x4(&projection, XMMatrixPerspectiveFovLH(XM_PIDIV2, _WindowWidth / FLOAT(_WindowHeight), 0.01f, 100.0f));
}

void Camera::UpdateFrameTimer(float time)
{
	frameTimer += time;	
}

void Camera::SetUp(XMVECTOR up)
{
	up = up;
}

void Camera::SetWorldPosition(XMFLOAT3 eyePos)
{
	worldPosition = eyePos;
}

void Camera::SetAt(XMFLOAT3 at)
{
	atPos = at;
}

void Camera::SetViewMatrix(XMFLOAT4X4 view)
{
	view = view;
}

void Camera::SetProjectionMatrix(XMFLOAT4X4 projection)
{
	projection = projection;
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