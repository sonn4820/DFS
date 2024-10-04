#pragma once
#include "Engine/Math/Vec3.hpp"
#include "Engine/Math/AABB3.hpp"
#include "Engine/Math/LineSegment3.hpp"
struct Capsule3
{
public:
	Vec3 m_start;
	Vec3 m_end;
	float m_radius;

public:

	~Capsule3() = default;
	Capsule3() = default;
	Capsule3(Capsule3 const& copyfrom);
	explicit Capsule3(Vec3 start, Vec3 end, float radius);

	Vec3 GetAxis() const;
	Vec3 GetAxisNormal() const;
	float GetAxisLength() const;
	float GetAxisHalfLength() const;
	Vec3 GetNearestPoint(Vec3 const point) const;
	bool IsPointInside(Vec3 const point) const;
	AABB3 GetBound() const;
	float GetBoneLength() const;
	void FixLength();
	LineSegment3 GetBone() const;

private:
	float m_length;
};