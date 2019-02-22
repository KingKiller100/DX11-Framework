#include "Camera.h"
#include <ios>

Camera::Camera(const XMFLOAT3 &worldPos, const UINT &windowHeight, const UINT &windowWidth) : worldPosition(worldPos), _WindowHeight(windowHeight), _WindowWidth(windowWidth)
{
	position = XMVectorSet(worldPos.x, worldPos.y, worldPos.z, 1);
	at = XMVectorSet(0.0f, 0.0f, 1.0f, 1.0f);
	up = XMVectorSet(0.0f, 1.0f, 0.0f, 1.0f);

	rotationX = 0.05f;
	rotationY = 0.05f;
	
	turnSpeed = 10.f;
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
		forwardMoveSpeed += forwardMoveSpeed < 0.5f ? 0.01f : 0;
	else
		forwardMoveSpeed = forwardMoveSpeed > 0.0f ? forwardMoveSpeed - 0.01f : 0.0f;
}

void Camera::BackwardMovement()
{
	if (!GetAsyncKeyState(VK_SHIFT) && GetAsyncKeyState(VK_DOWN))
		backwardMoveSpeed += backwardMoveSpeed < 0.5f ? 0.01f : 0;
	else
		backwardMoveSpeed = backwardMoveSpeed > 0.0f ? backwardMoveSpeed - 0.01f : 0.0f;
}

void Camera::Ascension()
{
	if (GetAsyncKeyState(VK_SHIFT) && GetAsyncKeyState(VK_UP))
		ascendingSpeed += ascendingSpeed < 0.5f ? 0.01f : 0;
	else
		ascendingSpeed = ascendingSpeed > 0.0f ? ascendingSpeed - 0.02f : 0.0f;	
}

void Camera::Descension()
{
	if (GetAsyncKeyState(VK_SHIFT) && GetAsyncKeyState(VK_DOWN))
		descendingSpeed += descendingSpeed < 0.5f ? 0.01f : 0;
	else
		descendingSpeed = descendingSpeed > 0.0f ? descendingSpeed - 0.02f : 0.0f;
}

void Camera::LeftTurn()
{
	rotationY -= GetAsyncKeyState('A') & 0x8000 ? 0.05f : .0f;
}

void Camera::RightTurn()
{
	rotationY += GetAsyncKeyState('D') & 0x8000 ? 0.05f : .0f;
}

void Camera::UpwardTurn()
{
	if (GetAsyncKeyState('W') & 0x8000)	
		rotationX -= rotationX > -1.35 ? 0.05f : .0f;	
}

void Camera::DownwardTurn()
{
	if (GetAsyncKeyState('S') & 0x8000)	
		rotationX += rotationX < 1.35 ? .05f : 0;	
}

void Camera::Update()
{
	Movement();

	RecalculateCamera();

	// Initialize the view matrix
	XMStoreFloat4x4(&view, XMMatrixLookAtLH(position, at, up));

	// Initialize the projection matrix
	XMStoreFloat4x4(&projection, XMMatrixPerspectiveFovLH(XM_PIDIV2, _WindowWidth / FLOAT(_WindowHeight), 0.01f, 100.0f));
}

void Camera::RecalculateCamera()
{
	const auto forwardVector = XMVectorSet(0, 0, 1, 0);
	const auto horizonalVector = XMVectorSet(1, 0, 0, 0);
	const auto verticalVector = XMVectorSet(0, 1, 0, 0);

	const auto rotationMatrix = XMMatrixRotationRollPitchYaw(rotationX, rotationY, 0);
	auto target = XMVector3TransformCoord(forwardVector, rotationMatrix);
	target = XMVector3Normalize(target);

	const auto camHorizontal = XMVector3TransformCoord(horizonalVector, rotationMatrix);
	const auto camForward = XMVector3TransformCoord(forwardVector, rotationMatrix);

	position += turnSpeed * camHorizontal;

	position += forwardMoveSpeed * camForward;
	position -= backwardMoveSpeed * camForward;

	position += ascendingSpeed * verticalVector;
	position -= descendingSpeed * verticalVector;

	turnSpeed = 0;

	at = position + target;	
}

void Camera::SetWorldPosition(const XMFLOAT3& eyePos)
{
	position = XMVectorSet(eyePos.x, eyePos.y, eyePos.z, 1);
}

void Camera::SetAt(const XMFLOAT3& atPos)
{
	at = XMVectorSet(atPos.x, atPos.y, atPos.z, 0);
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