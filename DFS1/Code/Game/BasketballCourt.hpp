#pragma once
#include "Game/GameCommon.hpp"

class Entity;
class Player;
class Prop;
class Ball;
class Blocker;
enum class BlockerType;

struct GameRaycast3D : RaycastResult3D
{
	Ball* m_hitBall = nullptr;
};
struct Hoop
{
	Hoop(AABB3 collider) {m_collider = collider;}
	AABB3 m_collider;
	bool m_isColliding = false;
};
class BasketballCourt
{
public:

	BasketballCourt();
	~BasketballCourt();

	void Startup();
	void Update(float deltaSeconds);
	void UpdatePhysics(float fixedDeltaSeconds);
	void Render() const;
	void Shutdown();

	std::vector<Entity*> m_entityList;
	std::vector<Ball*> m_ballList;
	std::vector<Prop*> m_obstacleList;

	Player* m_player = nullptr;

	Plane3 m_gameFloor;
	Plane3 m_northWall;
	Plane3 m_southWall;
	Plane3 m_eastWall;
	Plane3 m_westWall;

	Prop* m_hoopCylA = nullptr;
	Prop* m_hoopCylB = nullptr;
	Prop* m_hoopBoardA = nullptr;
	Prop* m_hoopBoardB = nullptr;
	Prop* m_hoopBasketA1 = nullptr;
	Prop* m_hoopBasketA2 = nullptr;
	Prop* m_hoopBasketA3 = nullptr;
	Prop* m_hoopBasketB1 = nullptr;
	Prop* m_hoopBasketB2 = nullptr;
	Prop* m_hoopBasketB3 = nullptr;

	Hoop* m_hoopA = nullptr;
	Hoop* m_hoopB = nullptr;

	std::vector<Vertex_PCU> m_courtVertices;
	std::vector<unsigned int> m_courtIndices;

	std::vector<Vertex_PCU> m_netVertices;
	std::vector<unsigned int> m_netIndices;

	std::vector<Vertex_PCU> m_skyboxVertices;

	// OBSTACLE
	std::vector<Blocker*> m_blockerList;

	// Prop
	Prop* CreateProp(bool isGravityEnabled = false, float mass = 1.f, float height = 1.f, float radius = 1.f, Vec3 position = Vec3::ZERO, EulerAngles orientation = EulerAngles());
	Ball* CreateBall(Vec3 position = Vec3::ZERO, EulerAngles orientation = EulerAngles());
	Player* CreatePlayer(Vec3 position = Vec3::ZERO, EulerAngles orientation = EulerAngles());
	Blocker* CreateBlocker(BlockerType type, Vec3 position, float width, float minHeight = 0, float maxHeight = 10, float time = 2.f);

	// Ball
	GameRaycast3D RaycastVsBall(Vec3 startPos, Vec3 fwdNormal);
	void PlayerCollisionWithWorld();

	void DeleteAllBalls();
	void DeleteAllBallsThatPlayerNotHolding();
	void DeleteAllGarbageBalls();
	void BounceBallsCourt();
	void BounceBallsOffPlayer();
	void BounceBallsOffBalls();
	void BounceBallsOffBlockers();
	bool BounceBallsOffBall(Ball* a, Ball* b);
	void BounceBallOffFloor(Ball* ball);
	void BounceBallOffHoop(Ball* ball);
	void BounceBallOffBlocker(Ball* ball);
	bool IsBallAScore(Ball* ball, Hoop* hoop);

	void SonFormularForBallVsGroundCollisionResolve(Ball* ball);

	// Field
	void InitializeCourt();
	void DrawCourt() const;

	// DEBUG
	void DrawGrid() const;

	// PLAYER
	void RandomPlayerPosition();

	// OBSTACLE 

	void SpawnRandomBlockers(int num);
};