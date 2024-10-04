#pragma once
#include "Engine/Math/Vec3.hpp"
#include "Engine/Math/Vec2.hpp"
struct AABB3
{

public:
	Vec3 m_mins;
	Vec3 m_maxs;

public:

	~AABB3() = default;
	AABB3() = default;
	AABB3(AABB3 const& copyfrom);
	AABB3(Vec3 center, float height, float widthX, float widthY);
	explicit AABB3(float minX, float minY, float minZ, float maxX, float maxY, float maxZ);
	explicit AABB3(Vec3 const& mins, Vec3 const& maxs);
	void StretchToIncludePoint(Vec3 const& point);
	void GetCornerPoints(Vec3* out_fourCornerWorldPositions) const;
	Vec3 GetCenter() const;
	Vec2 GetCenterXY() const;
	Vec3 GetNearestEdgePosition(Vec3 const& point) const;
	Vec3 GetDimension() const;
	Vec3 GetHalfDimension() const;


 	bool IsPointInside(Vec3 const& point) const;
	Vec3 const GetNearestPoint(Vec3 const& referencePosition) const;
// 
// 	void Translate(Vec3 const& translationToApply);
// 	void SetCenter(Vec3 const& newCenter);
// 	void SetDimensions(Vec3 const& newDimensions);
// 	void StretchToIncludePoint(Vec3 const& point); //does minimal stretching required

	static AABB3 ZERO_TO_ONE;
};