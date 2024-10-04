#pragma once
#include "Engine/Math/Vec3.hpp"
struct OBB3
{

public:
	Vec3 m_center;
	Vec3 m_iBasisNormal;
	Vec3 m_jBasisNormal;
	Vec3 m_halfDimensions;

public:

	~OBB3() = default;
	OBB3() = default;
	OBB3(OBB3 const& copyfrom);
	explicit OBB3(Vec3 center, Vec3 iBasisNormal, Vec3 jBasisNormal, Vec3 halfDimensions);

	bool IsPointInside(Vec3 const& point) const;
	Vec3 const GetNearestPoint(Vec3 const& referencePosition) const;
	void GetCornerPoints(Vec3* out_fourCornerWorldPositions) const;
};