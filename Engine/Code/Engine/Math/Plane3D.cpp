#include "Engine/Math/AABB3.hpp"
#include "Engine/Math/MathUtils.hpp"

AABB3::AABB3(AABB3 const& copyfrom)
	: m_mins(copyfrom.m_mins), m_maxs(copyfrom.m_maxs)
{
}

AABB3::AABB3(float minX, float minY, float minZ, float maxX, float maxY, float maxZ)
	:m_mins(Vec3(minX, minY, minZ)), m_maxs(Vec3(maxX, maxY, maxZ))
{
}

AABB3::AABB3(Vec3 const& mins, Vec3 const& maxs)
	:m_mins(mins), m_maxs(maxs)
{
}

void AABB3::StretchToIncludePoint(Vec3 const& point)
{
	if (point.x > m_maxs.x) m_maxs = Vec3(point.x, m_maxs.y, m_maxs.z);
	if (point.x < m_mins.x)	m_mins = Vec3(point.x, m_mins.y, m_mins.z);
	if (point.y > m_maxs.y)	m_maxs = Vec3(m_maxs.x, point.y, m_maxs.z);
	if (point.y < m_mins.y)	m_mins = Vec3(m_mins.x, point.y, m_mins.z);
	if (point.z > m_maxs.z)	m_maxs = Vec3(m_maxs.x, m_maxs.y, point.z);
	if (point.z < m_mins.z)	m_mins = Vec3(m_mins.x, m_mins.y, point.z);

}
AABB3 AABB3::ZERO_TO_ONE = AABB3(0.f, 0.f, 0.f, 1.f, 1.f, 1.f);