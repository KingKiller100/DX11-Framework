#pragma once

struct Vector2
{
	float x, y;

	Vector2()
	{
		x = 0.0f;
		y = 0.0f;
	}

	Vector2(float i, float j)
	{
		x = i;
		y = j;
	}

	// Bool Equals Operator Overload
	bool operator==(const Vector2& other) const
	{
		return (this->x == other.x && this->y == other.y);
	}
	// Bool Not Equals Operator Overload
	bool operator!=(const Vector2& other) const
	{
		return !(this->x == other.x && this->y == other.y);
	}
	// Addition Operator Overload
	Vector2 operator+(const Vector2& right) const
	{
		return Vector2(this->x + right.x, this->y + right.y);
	}
	// Subtraction Operator Overload
	Vector2 operator-(const Vector2& right) const
	{
		return Vector2(this->x - right.x, this->y - right.y);
	}
	// Float Multiplication Operator Overload
	Vector2 operator*(const float& other) const
	{
		return Vector2(this->x *other, this->y * other);
	}

};

struct Vector3
{
	float x, y, z;

	Vector3()
	{
		x = 0.0f;
		y = 0.0f;
		z = 0.0f;
	}

	Vector3(float i, float j, float k)
	{
		x = i;
		y = j;
		z = k;
	}

	// Bool Equals Operator Overload
	bool operator==(const Vector3& other) const
	{
		return (this->x == other.x && this->y == other.y && this->z == other.z);
	}
	// Bool Not Equals Operator Overload
	bool operator!=(const Vector3& other) const
	{
		return !(this->x == other.x && this->y == other.y && this->z == other.z);
	}
	// Addition Operator Overload
	Vector3 operator+(const Vector3& right) const
	{
		return Vector3(this->x + right.x, this->y + right.y, this->z + right.z);
	}
	// Subtraction Operator Overload
	Vector3 operator-(const Vector3& right) const
	{
		return Vector3(this->x - right.x, this->y - right.y, this->z - right.z);
	}
	// Float Multiplication Operator Overload
	Vector3 operator*(const float& other) const
	{
		return Vector3(this->x *other, this->y * other, this->z - other);
	}
};