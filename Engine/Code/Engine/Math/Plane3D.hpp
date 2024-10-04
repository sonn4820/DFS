#pragma once
#include "Engine/Math/Vec3.hpp"
struct AABB3
{

public:
	Vec3 m_mins;
	Vec3 m_maxs;

public:

	~AABB3() = default;
	AABB3() = default;
	AABB3(AABB3 const& copyfrom);
	explicit AABB3(float minX, float minY, float minZ, float maxX, float maxY, float maxZ);
	explicit AABB3(Vec3 const& mins, Vec3 const& maxs);
	void StretchToIncludePoint(Vec3 const& point);

// 	bool IsPointInside(Vec3 const& point) const;
// 	Vec3 const GetCenter() const;
// 	Vec3 const GetDimensions() const;
// 	Vec3 const GetNearestPoint(Vec3 const& referencePosition) const;
// 	Vec3 const GetPointAtUV(Vec3 const& uv) const; //(0,0) min, (1,1) max 
// 	Vec3 const GetUVForPoint(Vec3 const& point) const; // (.5,.5) at center, u or v outside [0,1] extrapolated
// 
// 	void Translate(Vec3 const& translationToApply);
// 	void SetCenter(Vec3 const& newCenter);
// 	void SetDimensions(Vec3 const& newDimensions);
// 	void StretchToIncludePoint(Vec3 const& point); //does minimal stretching required

	static AABB3 ZERO_TO_ONE;
};