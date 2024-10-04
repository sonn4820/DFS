#pragma  once
#include "Engine/Core/Vertex_PCU.hpp"
#include "Engine/Math/Vec2.hpp"
#include "Engine/Math/Vec3.hpp"
#include "Engine/Math/Vec4.hpp"
#include "Engine/Math/IntVec2.hpp"
#include "Engine/Math/IntVec3.hpp"
#include "Engine/Math/AABB2.hpp"
#include "Engine/Math/AABB3.hpp"
#include "Engine/Math/OBB2.hpp"
#include "Engine/Math/OBB3.hpp"
#include "Engine/Math/Capsule2.hpp"
#include "Engine/Math/Capsule3.hpp"
#include "Engine/Math/LineSegment2.hpp"
#include "Engine/Math/LineSegment3.hpp"
#include "Engine/Math/FloatRange.hpp"
#include "Engine/Math/Plane3.hpp"
#include "Engine/Math/EulerAngles.hpp"
#include "Engine/Math/Quaternion.hpp"
#include "Engine/Math/Mat44.hpp"
#include "ThirdParty/SquirrelNoise/RawNoise.hpp"
#include "ThirdParty/SquirrelNoise/SmoothNoise.hpp"
#include <math.h>


// Forward Declaration and Const
constexpr float PI = 3.14159265359f;
struct Mat44;

enum class BilboardType
{
	NONE = -1,
	WORLD_UP_CAMERA_FACING,
	WORLD_UP_CAMERA_OPPOSING,
	FULL_CAMERA_FACING,
	FULL_CAMERA_OPPOSING,
	COUNT
};

//Angle Utilities
float ConvertDegreesToRadians(float degrees);
float ConvertRadiansToDegrees(float radians);
float CosDegrees(float degrees);
float SinDegrees(float degrees);
float Atan2Degrees(float y, float x);
float GetShortestAngularDispDegrees(float startDegrees, float endDegrees);
float GetTurnedTowardDegrees(float currentDegrees, float goalDegrees, float maxDeltaDegrees);
float GetAngleDegreesBetweenVectors2D(Vec2 const& a, Vec2 const& b);
float GetAngleDegreesBetweenVectorsXY3D(Vec3 const& a, Vec3 const& b);
float GetNormalizedAngle(float angle);

//Basic 2D and 3D Utilities
float GetDistance2D(Vec2 const& positionA, Vec2 const& positionB);
float GetDistanceSquared2D(Vec2 const& positionA, Vec2 const& positionB);
float GetDistance3D(Vec3 const& positionA, Vec3 const& positionB);
float GetDistanceSquared3D(Vec3 const& positionA, Vec3 const& positionB);
float GetDistanceXY3D(Vec3 const& positionA, Vec3 const& positionB);
float GetDistanceXYSquared3D(Vec3 const& positionA, Vec3 const& positionB);
int GetTaxicabDistance2D(IntVec2 const& pointA, IntVec2 const& pointB);
float GetProjectedLength2D(Vec2 const& vectorToProject, Vec2 const& vectorToProjectOnto);
Vec2 const GetProjectedOnto2D(Vec2 const& vectorToProject, Vec2 const& vectorToProjectOnto);

//Geometric Query Utilities
bool IsPointInsideDisc2D(Vec2 const& point, Vec2 const& discCenter, float discRadius);
bool IsPointInsideAABB2D(Vec2 const& point, AABB2 const& box);
bool IsPointInsideCapsule2D(Vec2 const& point, Capsule2 const& capsule);
bool IsPointInsideCapsule2D(Vec2 const& point, Vec2 const& boneStart, Vec2 const& boneEnd, float radius);
bool IsPointInsideOBB2D(Vec2 const& point, OBB2 const& oriendtedBox);
bool IsPointInsideOrientedSector2D(Vec2 const& point, Vec2 const& sectorTip, float sectorForwardDegrees, float sectorApertureDegrees, float sectorRadius);
bool IsPointInsideDirectedSector2D(Vec2 const& point, Vec2 const& sectorTip, Vec2 const& sectorForwardNormal, float sectorApertureDegrees, float sectorRadius);

bool DoDiscsOverlap2D(Vec2 const& centerA, float radiusA, Vec2 const& centerB, float radiusB);
bool DoAABBsOverlap2D(AABB2 const& boxA, AABB2 const& boxB);
bool DoCapsuleAndPlaneOverlap2D(Capsule2 const& capsule, Vec2 planeStart, Vec2 planeEnd);

bool PushCapsuleOutOfPoint2D(Vec2 point, Capsule2& capsule);
bool PushCapsuleOutOfPlane2D(Vec2 planeStart, Vec2 planeEnd, Capsule2& capsule);

bool IsPointInsideAABB3D(Vec3 const& point, AABB3 const& box);
bool IsPointInsideCapsule3D(Vec3 const& point, Capsule3 const& capsule);
bool IsPointInsideSphere3D(Vec3 const& point, Vec3 const& sphereCenter, float radius);
bool IsPointInsideZCylinder3D(Vec3 const& point, Vec2 const& cylCenterXY, FloatRange cylMinMaxZ, float radius);

bool DoAABBsOverlap3D(AABB3 const& boxA, AABB3 const& boxB);
bool DoAABBAndPlaneOverlap3D(AABB3 const& box, Plane3 const& plane);
bool DoSpheresOverlap3D(Vec3 const& centerA, float radiusA, Vec3 const& centerB, float radiusB);
bool DoSphereAndAABBOverlap3D(Vec3 const& sphereCenter, float sphereRadius, AABB3 const& box);
bool DoSphereAndPlaneOverlap3D(Vec3 const& center, float radius, Plane3 const& plane);
bool DoZCylindersOverlap3D(Vec2 const& cylinderAcenterXY, float cylinderARadius, FloatRange const& cylinderAMinMaxZ, Vec2 const& cylinderBcenterXY, float cylinderBRadius, FloatRange const& cylinderBMinMaxZ);
bool DoZCylinderAndAABBOverlap3D(Vec2 const& cylinderCenterXY, float cylinderRadius, FloatRange const& cylinderMinMaxZ, AABB3 const& box);
bool DoZCylinderAndSphereOverlap3D(Vec2 const& cylinderCenterXY, float cylinderRadius, FloatRange const& cylinderMinMaxZ, Vec3 const& sphereCenter, float sphereRadius);
bool DoZCylinderAndPlaneOverlap3D(Vec2 const& cylinderCenterXY, float cylinderRadius, FloatRange const& cylinderMinMaxZ, Plane3 const& plane);
bool DoOBBsOverlap3D(OBB3 const& boxA, OBB3 const& boxB);
bool DoOBBAndPlaneOverlap3D(OBB3 const& box, Plane3 const& plane);
bool DoOBBAndAABBOverlap3D(OBB3 const& obb, AABB3 const& aabb);
bool DoOBBAndSphereOverlap3D(OBB3 const& obb, Vec3 const& center, float radius);
bool DoOBBAndZCylinderOverlap3D(OBB3 const& obb, Vec2 const& cylinderCenterXY, float cylinderRadius, FloatRange const& cylinderMinMaxZ);
bool DoCapsuleAndAABBOverlap3D(Capsule3 const& capsule, AABB3 const& box);
bool DoCapsuleAndOBBOverlap3D(Capsule3 const& capsule, OBB3 const& obb);
bool DoCapsuleAndPlaneOverlap3D(Capsule3 const& capsule, Plane3 const& plane);
bool DoCapsuleAndSphereOverlap3D(Capsule3 const& capsule, Vec3 const& sphereCenter, float sphereRadius);
bool DoCapsulesOverlap3D(Capsule3 const& capsuleA, Capsule3 const& capsuleB);

Vec2 GetNearestPointOnDisc2D(Vec2 const& referencePosition, Vec2 const& discCenter, float discRadius);
Vec2 GetNearestPointOnAABB2D(Vec2 const& referencePosition, AABB2 const& box);
Vec2 GetNearestPointOnInfiniteLine2D(Vec2 const& referencePosition, LineSegment2 const& infiniteLine);
Vec2 GetNearestPointOnInfiniteLine2D(Vec2 const& referencePosition, Vec2 const& pointOnLine, Vec2 const& anotherPointOnLine);
Vec2 GetNearestPointOnLineSegment2D(Vec2 const& referencePosition, LineSegment2 const& lineSegment);
Vec2 GetNearestPointOnLineSegment2D(Vec2 const& referencePosition, Vec2 const& lineSegStart, Vec2 const& lineSegEnd);
Vec2 GetNearestPointOnCapsule2D(Vec2 const& referencePosition, Capsule2 const& capsule);
Vec2 GetNearestPointOnCapsule2D(Vec2 const& referencePosition, Vec2 const& boneStart, Vec2 const& boneEnd, float radius);
Vec2 GetNearestPointOnOBB2D(Vec2 const& referencePosition, OBB2 const& orientedBox);

Vec3 GetNearestPointOnAABB3D(Vec3 const& referencePosition, AABB3 const& box);
Vec3 GetNearestPointOnCapsule3D(Vec3 const& referencePosition, Capsule3 const& capsule);
Vec3 GetNearestPointOnOBB3D(Vec3 const& referencePosition, OBB3 const& box);
Vec3 GetNearestPointOnSphere3D(Vec3 const& referencePosition, Vec3 const& center, float radius);
Vec3 GetNearestPointOnZCynlinder3D(Vec3 const& referencePosition, Vec2 const& centerXY, FloatRange minMaxZ, float radius);
Vec3 GetNearestPointOnLineSegment3D(Vec3 start, Vec3 end, Vec3 point);
Vec3 GetNearestPointOnLineSegment3D(LineSegment3 line, Vec3 point);
std::vector<Vec3> GetNearestPointsBetweenLines3D(LineSegment3 const& lineA, LineSegment3 const& lineB);

bool PushDiscOutOfPoint2D(Vec2& mobileDiscCenter, float discRadius, Vec2 const& fixedPoint);
bool PushDiscOutOfDisc2D(Vec2& mobileDiscCenter, float mobileDiscRadius, Vec2 const& fixedDiscCenter, float fixedDiscRadius);
bool PushDiscsOutOfEachOther2D(Vec2& aCenter, float aRadius, Vec2& bCenter, float bRadius);
bool PushDiscOutOfAABB2D(Vec2& mobileDiscCenter, float discRadius, AABB2 const& fixedBox);

bool PushZCylinderOutOfPoint3D(Vec2& mobileCylXYCenter, FloatRange& cylMinMaxZ, float cylRadius, Vec3 const& fixedPoint);
bool PushZCylinderOutOfSphere3D(Vec2& mobileCylXYCenter, FloatRange& cylMinMaxZ, float cylRadius, Vec3 const& fixedDiscCenter, float fixedDiscRadius);
bool PushZCylindersOutOfZCylinders3D(Vec2& mobileCylCenter, FloatRange& mobileCylMinMaxZ, float mobileCylRadius, Vec2 const& fixedCylCenter, FloatRange const& fixedCylMinMaxZ, float fixedCylRadius);
bool PushZCylindersOutOfEachOther3D(Vec2& cylACenter, FloatRange& cylAMinMaxZ, float cylARadius, Vec2& cylBCenter, FloatRange& cylBMinMaxZ, float cylBRadius);
bool PushZCylinderOutOfAABB3D(Vec2& mobileCylXYCenter, FloatRange& cylMinMaxZ, float cylRadius, AABB3 const& fixedBox);

bool PushSphereOutOfSphere3D(Vec3& posA, float radiusA, Vec3& posB, float radiusB, bool isBStatic);
bool PushSphereOutOfZCylinder3D(Vec3& pos, float radius, Vec2 const& cylCenter, FloatRange const& cylMinMaxZ, float cylRadius);
bool PushSphereOutOfPlane3D(Vec3& center, float radius, Plane3 const& plane);
bool PushSphereOutOfAABB3(Vec3& center, float radius, AABB3 const& box);
bool PushSphereOutOfOBB3(Vec3& center, float radius, OBB3 const& obb);
bool PushSphereOutOfCapsule3(Vec3& center, float radius, Capsule3& capsule, bool isCapsuleStatic);
bool PushSphereOutOfPoint3D(Vec3& center, float radius, Vec3 const& point);

bool PushCapsuleOutOfPoint3D(Capsule3& capsule, Vec3 const& point, bool fixLength = true);
bool PushCapsuleOutOfPlane3D(Capsule3& capsule, Plane3 const& plane, bool fixLength = true);
bool PushCapsuleOutOfAABB3D(Capsule3& capsule, AABB3 const& box, bool fixLength = true);
bool PushCapsuleOutOfOBB3D(Capsule3& capsule, OBB3 const& box, bool fixLength = true);
bool PushCapsuleOutOfCapsule3D(Capsule3& capsuleA, Capsule3& capsuleB, bool isBStatic = true);
bool PushCapsuleOutOfSphere3D(Capsule3& capsule, Vec3& sCenter, float sRadius, bool isSphereStatic = true);

//Transform Utilities
void TransformPosition2D(Vec2& posToTransform, float uniformScale, float rotaitonDegrees, Vec2 const& translation);
void TransformPosition2D(Vec2& posToTransform, Vec2 const& iBasis, Vec2 const& jBasis, Vec2 const& translation);
void TransformPositionXY3D(Vec3& positionToTransform, float scaleXY, float zRotationDegrees, Vec2 const& translation);
void TransformPositionXY3D(Vec3& positionToTransform, Vec2 const& iBasis, Vec2 const& jBasis, Vec2 const& translation);

// Clamp and lerp
float Clamp(float value, float minValue, float maxValue);
float ClampZeroToOne(float value);
float Interpolate(float start, float end, float fraction);
Vec2 Interpolate(Vec2 start, Vec2 end, float zeroToOne);
Vec3 Interpolate(Vec3 start, Vec3 end, float zeroToOne);
Rgba8 Interpolate(Rgba8 start, Rgba8 end, float zeroToOne);
float GetFractionWithinRange(float value, float rangeStart, float rangeEnd);
float RangeMap(float inValue, float inStart, float inEnd, float outStart, float outEnd);
float RangeMapClamped(float inValue, float inStart, float inEnd, float outStart, float outEnd);
int RoundDownToInt(float value);

//Dot and Cross
float DotProduct2D(Vec2 const& a, Vec2 const& b);
float DotProduct3D(Vec3 const& a, Vec3 const& b);
float DotProduct4D(Vec4 const& a, Vec4 const& b);

float CrossProduct2D(Vec2 A, Vec2 B);
Vec3 CrossProduct3D(Vec3 A, Vec3 B);


//ByteConvertFloat
float NormalizeByte(unsigned char byte);
unsigned char DenormalizeByte(float f);

// Billboard
Mat44 GetBillboardMatrix(BilboardType type, Mat44 const& cameraMatrix, const Vec3& billboardPosition, const Vec2& billboardScale = Vec2(1.f, 1.f));

// Easing
float SmoothStart2(float t);
float SmoothStart3(float t);
float SmoothStart4(float t);
float SmoothStart5(float t);
float SmoothStart6(float t);

float SmoothStop2(float t);
float SmoothStop3(float t);
float SmoothStop4(float t);
float SmoothStop5(float t);
float SmoothStop6(float t);

float SmoothStep3(float t);
float SmoothStep5(float t);

float Hesitate3(float t);
float Hesitate5(float t);

// Compare

bool FloatEqual(float a, float compareTo, float tolerance);
float FloatMin(float a, float b);
float FloatMax(float a, float b);

// Bezier Curve
float ComputeCubicBezier1D(float t, float p0, float p1, float p2, float p3);
float ComputeQuinticBezier1D(float t, float p0, float p1, float p2, float p3, float p4, float p5);

// Physics
bool BounceDiscOffPoint(Vec2& discPosition, float discRadius, Vec2& discVelocity, float discElasticity, Vec2& pointPosition, float pointElasticity);
bool BounceDiscOffStaticDisc2D(Vec2& mobileDiscPosition, float mobileDiscRadius, Vec2& mobileDiscVelocity, float mobileDiscElasticity, const Vec2& staticDiscPosition, float staticDiscRadius, float staticDiscElasticity);
bool BounceDiscOffEachOther2D(Vec2& discAPosition, float discARadius, Vec2& discAVelocity, float discAElasticity, Vec2& discBPosition, float discBRadius, Vec2& discBVelocity, float discBElasticity);

bool BounceSphereOffPoint(Vec3& spherePosition, float sphereRadius, Vec3& sphereVelocity, float sphereElasticity, Vec3 pointPosition, float pointElasticity, float pointFriction = 0.f);
bool BounceSphereOffStaticSphere3D(Vec3& mobileSpherePosition, float mobileSphereRadius, Vec3& mobileSphereVelocity, float mobileSphereElasticity, const Vec3& staticSpherePosition, float staticSphereRadius, float staticSphereElasticity);
bool BounceSphereOffEachOther3D(Vec3& posA, Vec3& posB, float radiusA, float radiusB, Vec3& velA, Vec3& velB, float elasticiyA, float elasticiyB, float massA, float massB);

bool BounceCapsuleOffPoint(Capsule3 capsule, Vec3& startPointVel, Vec3& endPointVel, float elasticity, Vec3 pointPosition, float pointElasticity, float pointFriction = 0.f);