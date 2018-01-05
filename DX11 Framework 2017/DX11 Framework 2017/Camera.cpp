#include "Camera.h"

Camera::Camera(UINT windowHeight, UINT windowWidth)
{
	_WindowHeight = windowHeight;
	_WindowWidth = windowWidth;

	atPos = XMFLOAT3(0.0f, 0.0f, 1.0f);
	
	frameTimer = 0.0f;

	rotationX = 0.05f;
	rotationY = 0.0f;
	rotationZ = 1.0f;
	
	forwardMoveSpeed = 0.0f;
	backwardMoveSpeed = 0.0f;
	leftTurnSpeed = 0.0f;
	rightTurnSpeed = 0.0f;
	ascendingSpeed = 0.0f;
	descendingSpeed = 0.0f;
	lookUpSpeed = 0.0f;
	lookDownSpeed = 0.0f;
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

void Camera::ForwardMovement()
{
	float rads; // Radians

	if (GetAsyncKeyState(VK_UP))
	{
		forwardMoveSpeed += frameTimer * 0.001f;
		if (forwardMoveSpeed > frameTimer * 0.03f)
		{
			forwardMoveSpeed = frameTimer * 0.03f;
		}
	}
	else
	{
		forwardMoveSpeed -= frameTimer * 0.0007f;

		if (forwardMoveSpeed < 0.0f)
		{
			forwardMoveSpeed = 0.0f;
		}
	}

	rads = XMConvertToRadians(0.25f);

	posW.x += sinf(rads) * forwardMoveSpeed;
	posW.z += cosf(rads) * forwardMoveSpeed;
}

void Camera::BackwardMovement()
{
	float rads; // Radians
	if (GetAsyncKeyState(VK_DOWN))
	{
		backwardMoveSpeed += frameTimer * 0.001f;
		if (backwardMoveSpeed > frameTimer * 0.03f)
		{
			backwardMoveSpeed = frameTimer * 0.03f;
		}
	}
	else
	{
		backwardMoveSpeed -= frameTimer * 0.0007f;

		if (backwardMoveSpeed < 0.0f)
		{
			backwardMoveSpeed = 0.0f;
		}
	}

	rads = XMConvertToRadians(0.2f);

	//posW.x -= sinf(rads) * forwardMoveSpeed;
	//posW.z -= cosf(rads) * forwardMoveSpeed;

	posW.x -= sinf(rads) * backwardMoveSpeed;
	posW.z -= cosf(rads) * backwardMoveSpeed;
}

void Camera::Ascension()
{
	if (GetAsyncKeyState(VK_SHIFT))
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

	posW.y += 0.5f * ascendingSpeed;
}

void Camera::Descension()
{
	if (GetAsyncKeyState('L') & 0x8000)
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

	posW.y -= 0.25f * descendingSpeed;
}

void Camera::LeftTurn()
{
	if (GetAsyncKeyState('A') & 0x8000)
	{
		leftTurnSpeed += frameTimer * 0.01f;

		if (leftTurnSpeed > (frameTimer * 0.15f))
		{
			leftTurnSpeed = frameTimer * 0.15f;
		}
	}
	else
	{
		leftTurnSpeed -= frameTimer * 0.005f;

		if (leftTurnSpeed < 0.0f)
		{
			leftTurnSpeed = 0.0f;
		}
	}

	// Updates Rotation
	rotationY -= leftTurnSpeed;

	// Rotates betweeen 0 & 360 degrees
	if (rotationY < 0.0f)
	{
		rotationY += 360.0f;
	}

	//atPos.x -= sinf(XMConvertToRadians(rotationY)) * leftTurnSpeed;
	//atPos.z -= cosf(XMConvertToRadians(rotationY)) * leftTurnSpeed;
}

void Camera::RightTurn()
{
	if (GetAsyncKeyState('D') & 0x8000)
	{
		rightTurnSpeed += frameTimer * 0.01f;

		if (rightTurnSpeed > (frameTimer * 0.15f))
		{
			rightTurnSpeed = frameTimer * 0.15f;
		}
	}
	else
	{
		rightTurnSpeed -= frameTimer * 0.005f;

		if (rightTurnSpeed < 0.0f)
		{
			rightTurnSpeed = 0.0f;
		}
	}

	// Updates Rotation
	rotationY += rightTurnSpeed;

	// Rotates between 0 & 360 degrees
	if (rotationY > 360.0f)
	{
		rotationY -= 360.0f;
	}
	return;
	//atPos.x += sinf(XMConvertToRadians(rotationY)) * rightTurnSpeed;
	//atPos.z += cosf(XMConvertToRadians(rotationY)) * rightTurnSpeed;
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

		atPos.y += sinf(XMConvertToRadians(rotationX)) * lookUpSpeed;
		atPos.z += cosf(XMConvertToRadians(rotationX)) * lookUpSpeed;
	

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

		atPos.y -= sinf(XMConvertToRadians(rotationX)) * lookDownSpeed;
		atPos.z -= cosf(XMConvertToRadians(rotationX)) * lookDownSpeed;
	
}

void Camera::CreateCamera()
{
	XMVECTOR rotationQ = XMVectorSet(XMConvertToRadians(rotationX), XMConvertToRadians(rotationY), XMConvertToRadians(rotationZ), 1.0F);
	XMFLOAT4X4 _rotations;
	XMMATRIX rotationMatrix;
	float pitch, yaw, roll;

	// Initialize the view matrix
	Eye = XMVectorSet(posW.x, posW.y, posW.z, 1.0f);
	At = XMVectorSet(atPos.x, atPos.y, atPos.z, 1.0f);
	Up = XMVectorSet(0.0f, 1.0f, 0.0f, 1.0f);

	pitch = XMConvertToRadians(rotationX);
	yaw = XMConvertToRadians(rotationY);
	roll = XMConvertToRadians(rotationZ);

	XMVector3Rotate(At, rotationQ);

	XMMatrixRotationRollPitchYaw(pitch, yaw, roll);
	XMStoreFloat4x4(&_rotations, XMMatrixRotationX(rotationX) * XMMatrixRotationY(rotationY) * XMMatrixRotationZ(rotationZ));
	rotationMatrix = XMLoadFloat4x4(&_rotations);
	XMVector4Transform(At, rotationMatrix);
	XMVector4Transform(Up, rotationMatrix);

	XMStoreFloat4x4(&_view, XMMatrixLookAtLH(Eye, At, Up));

	// Initialize the projection matrix
	XMStoreFloat4x4(&_projection, XMMatrixPerspectiveFovLH(XM_PIDIV2, _WindowWidth / (FLOAT)_WindowHeight, 0.01f, 100.0f));
}

void Camera::UpdateFrameTimer(float time)
{
	frameTimer = time;
	return;
}

void Camera::SetUp(XMVECTOR up)
{
	Up = up;
}

void Camera::SetWorldPosition(XMFLOAT3 eyePos)
{
	posW = eyePos;
}

void Camera::SetAt(XMVECTOR at)
{
	At = at;
}

void Camera::SetViewMatrix(XMFLOAT4X4 view)
{
	_view = view;
}

void Camera::SetProjectionMatrix(XMFLOAT4X4 projection)
{
	_projection = projection;
}

void Camera::SetEye(XMVECTOR eye)
{
	Eye = eye;
}

XMFLOAT3 Camera::GetWorldPosition()
{
	return posW;
}

float Camera::GetEyePosX()
{
	return posW.x;
}

float Camera::GetEyePosY()
{
	return posW.y;
}

float Camera::GetEyePosZ()
{
	return posW.z;
}

XMFLOAT4X4 Camera::GetViewMatrix()
{
	return _view;
}

XMFLOAT4X4 Camera::GetProjectionMatrix()
{
	return _projection;
}


XMVECTOR Camera::GetEye()
{
	return Eye;
}

XMVECTOR Camera::GetUp()
{
	return Up;
}

XMVECTOR Camera::GetAt()
{
	return At;
}

void Camera::Release()
{

}