#include "BasketballCourt.hpp"
#include "Game/Player.hpp"
#include "Game/Ball.hpp"
#include "Game/Entity.hpp"
#include "Game/Prop.hpp"
#include "Game/Blocker.hpp"

BasketballCourt::BasketballCourt()
{
	m_gameFloor = Plane3(Vec3(0.f, 0.f, 1.f), 0.f);
	m_northWall = Plane3(Vec3(0.f, 1.f, 0.f), 50.f);
	m_southWall = Plane3(Vec3(0.f, -1.f, 0.f), 50.f);
	m_eastWall = Plane3(Vec3(1.f, 0.f, 0.f), 50.f);
	m_westWall = Plane3(Vec3(-1.f, 0.f, 0.f), 50.f);
}

BasketballCourt::~BasketballCourt()
{

}

void BasketballCourt::Startup()
{
	InitializeCourt();

	if (g_theGame->m_currentGameMode == CREATIVE)
	{
		m_player = CreatePlayer(Vec3(-3.f, -3.f, 1.f));
	}
	if (g_theGame->m_currentGameMode == TIMER)
	{
		m_player = CreatePlayer();
		RandomPlayerPosition();
		g_theGame->m_timeChallengeScore = 0;
	}
	if (g_theGame->m_currentGameMode == OBSTACLE)
	{
		m_player = CreatePlayer(Vec3(-7, -7.f, 1.f));
		SpawnRandomBlockers(g_theGame->m_obstacleChallengeLevel + 2);
	}
}

void BasketballCourt::Update(float deltaSeconds)
{
	for (size_t i = 0; i < m_entityList.size(); i++)
	{
		m_entityList[i]->Update(deltaSeconds);
	}
	for (size_t i = 0; i < m_ballList.size(); i++)
	{
		if (m_ballList[i])
		{
			m_ballList[i]->Update(deltaSeconds);
		}
	}

	if (g_theGame->m_currentGameMode == OBSTACLE)
	{
		for (size_t i = 0; i < m_blockerList.size(); i++)
		{
			m_blockerList[i]->Update(deltaSeconds);
		}
	}
}

void BasketballCourt::UpdatePhysics(float fixedDeltaSeconds)
{
	PlayerCollisionWithWorld();
	BounceBallsCourt();
	BounceBallsOffPlayer();
	BounceBallsOffBalls();

	m_player->UpdatePhysics(fixedDeltaSeconds);
	for (size_t i = 0; i < m_entityList.size(); i++)
	{
		m_entityList[i]->UpdatePhysics(fixedDeltaSeconds);
	}
	for (size_t i = 0; i < m_ballList.size(); i++)
	{
		if (m_ballList[i])
		{
			m_ballList[i]->UpdatePhysics(fixedDeltaSeconds);
			IsBallAScore(m_ballList[i], m_hoopA);
			IsBallAScore(m_ballList[i], m_hoopB);
		}
	}

	if (g_theGame->m_currentGameMode == OBSTACLE)
	{
		BounceBallsOffBlockers();
	}
}

void BasketballCourt::Render() const
{
	g_theRenderer->BeginCamera(*m_player->GetCamera());
	m_player->Render();
	for (size_t i = 0; i < m_entityList.size(); i++)
	{
		m_entityList[i]->Render();
	}
	for (size_t i = 0; i < m_ballList.size(); i++)
	{
		if (m_ballList[i])
		{
			m_ballList[i]->Render();
		}
	}
	if (g_theGame->m_currentGameMode == OBSTACLE)
	{
		for (size_t i = 0; i < m_blockerList.size(); i++)
		{
			m_blockerList[i]->Render();
		}
	}

	if (g_debugDrawing)
	{
		DrawGrid();
	}

	DrawCourt();
	g_theRenderer->EndCamera(*m_player->GetCamera());
}

void BasketballCourt::Shutdown()
{

}

Prop* BasketballCourt::CreateProp(bool isGravityEnabled /*= false*/, float mass /*= 1.f*/, float height /*= 1.f*/, float radius /*= 1.f*/, Vec3 position /*= Vec3::ZERO*/, EulerAngles orientation /*= EulerAngles()*/)
{
	Prop* newProp = new Prop(this);
	newProp->m_isGravityEnabled = isGravityEnabled;
	newProp->m_position = position;
	newProp->m_orientationDegrees = orientation;
	newProp->m_mass = mass;
	newProp->m_height = height;
	newProp->m_radius = radius;
	m_entityList.push_back(newProp);
	return newProp;
}

Ball* BasketballCourt::CreateBall(Vec3 position /*= Vec3::ZERO*/, EulerAngles orientation /*= EulerAngles()*/)
{
	Ball* newBall = new Ball(this);
	newBall->m_position = position;
	newBall->m_rotation = orientation;
	newBall->m_texture = g_theGame->m_ballTexture;

	for (size_t i = 0; i < m_ballList.size(); i++)
	{
		if (m_ballList[i] == nullptr)
		{
			m_ballList[i] = newBall;
			return newBall;
		}
	}

	m_ballList.push_back(newBall);
	return newBall;
}

Player* BasketballCourt::CreatePlayer(Vec3 position /*= Vec3::ZERO*/, EulerAngles orientation /*= EulerAngles()*/)
{
	Player* player = new Player(this);
	player->m_position = position;
	player->m_orientationDegrees = orientation;
	m_entityList.push_back(player);
	return player;
}

Blocker* BasketballCourt::CreateBlocker(BlockerType type, Vec3 position, float width, float minHeight /*= 0*/, float maxHeight /*= 10*/, float time)
{
	Blocker* blocker = new Blocker(this, type, position, width, minHeight, maxHeight, time);
	m_blockerList.push_back(blocker);
	return blocker;
}

GameRaycast3D BasketballCourt::RaycastVsBall(Vec3 startPos, Vec3 fwdNormal)
{
	GameRaycast3D result;
	float smallestDitance = FLT_MAX;
	for (size_t i = 0; i < m_ballList.size(); i++)
	{
		if (m_ballList[i])
		{
			RaycastResult3D ray = RaycastVsSphere3D(startPos, fwdNormal, RAY_CAST_LENGTH, m_ballList[i]->m_position, RAY_CAST_RADIUS);
			if (ray.m_didImpact)
			{
				if (ray.m_impactDist < smallestDitance)
				{
					result.m_didImpact = ray.m_didImpact;
					result.m_impactDist = ray.m_impactDist;
					result.m_impactNormal = ray.m_impactNormal;
					result.m_impactPos = ray.m_impactPos;
					result.m_hitBall = m_ballList[i];
				}
			}
		}
	}

	return result;
}

void BasketballCourt::PlayerCollisionWithWorld()
{
	Vec2 playerXY = Vec2(m_player->m_position.x, m_player->m_position.y);
	FloatRange playerZ = FloatRange(m_player->m_position.z, m_player->m_position.z + m_player->m_height);

	if (PushZCylinderOutOfPoint3D(playerXY, playerZ, m_player->m_radius, m_gameFloor.GetNearestPoint(m_player->m_position)))
	{
		m_player->m_position = Vec3(playerXY.x, playerXY.y, playerZ.m_min);
	}
	if (PushZCylinderOutOfPoint3D(playerXY, playerZ, m_player->m_radius, m_southWall.GetNearestPoint(m_player->m_position)))
	{
		m_player->m_position = Vec3(playerXY.x, playerXY.y, playerZ.m_min);
	}
	if (PushZCylinderOutOfPoint3D(playerXY, playerZ, m_player->m_radius, m_northWall.GetNearestPoint(m_player->m_position)))
	{
		m_player->m_position = Vec3(playerXY.x, playerXY.y, playerZ.m_min);
	}
	if (PushZCylinderOutOfPoint3D(playerXY, playerZ, m_player->m_radius, m_westWall.GetNearestPoint(m_player->m_position)))
	{
		m_player->m_position = Vec3(playerXY.x, playerXY.y, playerZ.m_min);
	}
	if (PushZCylinderOutOfPoint3D(playerXY, playerZ, m_player->m_radius, m_eastWall.GetNearestPoint(m_player->m_position)))
	{
		m_player->m_position = Vec3(playerXY.x, playerXY.y, playerZ.m_min);
	}

	if (g_theGame->m_currentGameMode == OBSTACLE)
	{
		if (PushZCylinderOutOfAABB3D(playerXY, playerZ, m_player->m_radius, AABB3(-5, -55, 0.5f, 5, 55, 10)))
		{
			m_player->m_position = Vec3(playerXY.x, playerXY.y, playerZ.m_min);
		}
		if (PushZCylinderOutOfAABB3D(playerXY, playerZ, m_player->m_radius, AABB3(-50, -25, 0.5f, 50, -25, 10)))
		{
			m_player->m_position = Vec3(playerXY.x, playerXY.y, playerZ.m_min);
		}
		if (PushZCylinderOutOfAABB3D(playerXY, playerZ, m_player->m_radius, AABB3(-50, 25, 0.5f, 50, 25, 10)))
		{
			m_player->m_position = Vec3(playerXY.x, playerXY.y, playerZ.m_min);
		}
	}
}

void BasketballCourt::DeleteAllBalls()
{
	for (size_t i = 0; i < m_ballList.size(); i++)
	{
		delete m_ballList[i];
		m_ballList[i] = nullptr;
	}

	m_ballList.clear();
}

void BasketballCourt::DeleteAllBallsThatPlayerNotHolding()
{
	for (size_t i = 0; i < m_ballList.size(); i++)
	{
		if (m_player->m_currentBall == m_ballList[i])
		{
			continue;
		}
		delete m_ballList[i];
		m_ballList[i] = nullptr;
	}
}

void BasketballCourt::DeleteAllGarbageBalls()
{
	for (size_t i = 0; i < m_ballList.size(); i++)
	{
		if (m_ballList[i]->m_isGarbage)
		{
			delete m_ballList[i];
			m_ballList[i] = nullptr;
		}
	}
}

void BasketballCourt::BounceBallsCourt()
{
	for (size_t i = 0; i < m_ballList.size(); i++)
	{
		if (m_ballList[i])
		{
			BounceBallOffFloor(m_ballList[i]);
			BounceBallOffHoop(m_ballList[i]);
		}
	}
}

void BasketballCourt::BounceBallsOffPlayer()
{
	for (size_t i = 0; i < m_ballList.size(); i++)
	{
		if (m_ballList[i])
		{
			Vec3 nearestPoint = GetNearestPointOnZCynlinder3D(m_ballList[i]->m_position, m_player->GetPositionXY(), m_player->GetHeightRange(), m_player->m_radius);
			if (!m_player->m_isHoldingBall && m_player->m_currentBall != m_ballList[i])
			{
				PushSphereOutOfZCylinder3D(m_ballList[i]->m_position, m_ballList[i]->m_radius, m_player->GetPositionXY(), m_player->GetHeightRange(), m_player->m_radius);
			}
		}
	}
}

void BasketballCourt::BounceBallsOffBalls()
{
	for (size_t i = 0; i < m_ballList.size(); i++)
	{
		for (size_t j = i + 1; j < m_ballList.size(); j++)
		{
			if (m_ballList[i] && m_ballList[j])
			{
				BounceBallsOffBall(m_ballList[i], m_ballList[j]);
			}
		}
	}
}

void BasketballCourt::BounceBallsOffBlockers()
{
	for (size_t i = 0; i < m_ballList.size(); i++)
	{
		if (m_ballList[i])
		{
			BounceBallOffBlocker(m_ballList[i]);
		}
	}
}

bool BasketballCourt::BounceBallsOffBall(Ball* a, Ball* b)
{
	if (!a || !b)
	{
		return false;
	}

	if (!PushSphereOutOfSphere3D(a->m_position, a->m_radius, b->m_position, b->m_radius, false))
	{
		return false;
	}

	Vec3 dist = b->m_position - a->m_position;

	float totalE = 0.9f * 0.9f;

	dist.Normalize();
	Vec3 contactPointA = a->m_position + dist * a->m_radius;
	float velALengthOnDis = DotProduct3D(dist, a->m_velocity);
	Vec3 velAX = dist * velALengthOnDis;
	Vec3 velAY = a->m_velocity - velAX;

	dist = dist * -1;
	Vec3 contactPointB = b->m_position + dist * b->m_radius;
	float velBLengthOnDis = DotProduct3D(dist, b->m_velocity);
	Vec3 velBX = dist * velBLengthOnDis;
	Vec3 velBY = b->m_velocity - velBX;

	float inverseTotalMass = 1.f / (a->m_mass + b->m_mass);

	Vec3 velA = Vec3(velAX * (a->m_mass - b->m_mass) * inverseTotalMass + velBX * (2.f * b->m_mass) * inverseTotalMass + velAY) * totalE;
	Vec3 velB = Vec3(velAX * (2.f * a->m_mass) * inverseTotalMass + velBX * (b->m_mass - a->m_mass) * inverseTotalMass + velBY) * totalE;

	a->m_velocity = (velA)+Vec3(0, 0, MODIFIED_GRAVITY_RATE) * g_theGame->m_fixedTimeStep * g_theGame->m_fixedTimeStep;
	b->m_velocity = (velB)+Vec3(0, 0, MODIFIED_GRAVITY_RATE) * g_theGame->m_fixedTimeStep * g_theGame->m_fixedTimeStep;

	a->AddForce(-b->m_velocity * 0.8f);
	b->AddForce(-a->m_velocity * 0.8f);

	if (a->m_angularVelocity.GetLengthSquared() < 0.05f)
	{
		a->AddImpulseTorque(contactPointA, -velB);
	}
	if (b->m_angularVelocity.GetLengthSquared() < 0.05f)
	{
		b->AddImpulseTorque(contactPointB, -velA);
	}

	return true;
}

void BasketballCourt::BounceBallOffFloor(Ball* ball)
{
	if (!ball)
	{
		return;
	}

	bool isHit = false;
	Vec3 pointOnFloor = m_gameFloor.GetNearestPoint(ball->m_position);
	if (BounceSphereOffPoint(ball->m_position, BALL_RADIUS, ball->m_velocity, 0.9f, pointOnFloor, 0.99f, GROUND_STATIC_FRICTION))
	{
		ball->AddImpulseTorque(pointOnFloor, -ball->m_velocity);
		isHit = true;
	}
	Vec3 pointOnNWall = m_northWall.GetNearestPoint(ball->m_position);
	if (BounceSphereOffPoint(ball->m_position, BALL_RADIUS, ball->m_velocity, 0.9f, pointOnNWall, 0.99f, GROUND_STATIC_FRICTION))
	{

		ball->AddImpulseTorque(pointOnNWall, -ball->m_velocity);
		isHit = true;
	}
	Vec3 pointOnSWall = m_southWall.GetNearestPoint(ball->m_position);
	if (BounceSphereOffPoint(ball->m_position, BALL_RADIUS, ball->m_velocity, 0.9f, pointOnSWall, 0.99f, GROUND_STATIC_FRICTION))
	{
		ball->AddImpulseTorque(pointOnSWall, -ball->m_velocity);
		isHit = true;
	}
	Vec3 pointOnEWall = m_eastWall.GetNearestPoint(ball->m_position);
	if (BounceSphereOffPoint(ball->m_position, BALL_RADIUS, ball->m_velocity, 0.9f, pointOnEWall, 0.99f, GROUND_STATIC_FRICTION))
	{
		ball->AddImpulseTorque(pointOnEWall, -ball->m_velocity);
		isHit = true;
	}
	Vec3 pointOnWWall = m_westWall.GetNearestPoint(ball->m_position);
	if (BounceSphereOffPoint(ball->m_position, BALL_RADIUS, ball->m_velocity, 0.9f, pointOnWWall, 0.99f, GROUND_STATIC_FRICTION))
	{
		ball->AddImpulseTorque(pointOnWWall, -ball->m_velocity);
		isHit = true;
	}
	if (isHit)
	{
		SonFormularForBallVsGroundCollisionResolve(ball);
		ball->AddImpulse(0.87f * CrossProduct3D(ball->m_angularVelocity, -ball->m_velocity));
		if (ball->m_velocity.z > BALL_RADIUS + 0.05f)
		{
			ball->PlaySound(g_theGame->m_ballBounceGroundSound);
		}
	}
}

void BasketballCourt::BounceBallOffHoop(Ball* ball)
{
	if (!ball)
	{
		return;
	}

	bool isHit = false;

	if (m_hoopCylA)
	{
		Vec3 pHoopACyl = GetNearestPointOnZCynlinder3D(ball->m_position, m_hoopCylA->GetPositionXY(), m_hoopCylA->GetHeightRange(), m_hoopCylA->m_radius);
		if (BounceSphereOffPoint(ball->m_position, ball->m_radius, ball->m_velocity, 0.9f, pHoopACyl, 0.6f, 0.1f))
		{
			ball->AddImpulseTorque(pHoopACyl, -ball->m_velocity);
			isHit = true;
		}
	}

	if (m_hoopCylB)
	{
		Vec3 pHoopBCyl = GetNearestPointOnZCynlinder3D(ball->m_position, m_hoopCylB->GetPositionXY(), m_hoopCylB->GetHeightRange(), m_hoopCylA->m_radius);
		if (BounceSphereOffPoint(ball->m_position, ball->m_radius, ball->m_velocity, 0.9f, pHoopBCyl, 0.6f, 0.1f))
		{
			ball->AddImpulseTorque(pHoopBCyl, -ball->m_velocity);
			isHit = true;
		}
	}

	if (m_hoopBoardA)
	{
		Vec3 pHoodBoardA = GetNearestPointOnAABB3D(ball->m_position, AABB3(m_hoopBoardA->m_position, m_hoopBoardA->m_height, 0.1f, m_hoopBoardA->m_radius));
		if (BounceSphereOffPoint(ball->m_position, ball->m_radius, ball->m_velocity, 0.9f, pHoodBoardA, 0.7f, 0.1f))
		{
			ball->AddImpulseTorque(pHoodBoardA, -ball->m_velocity);
			isHit = true;
		}
	}

	if (m_hoopBoardB)
	{
		Vec3 pHoodBoardB = GetNearestPointOnAABB3D(ball->m_position, AABB3(m_hoopBoardB->m_position, m_hoopBoardB->m_height, 0.1f, m_hoopBoardB->m_radius));
		if (BounceSphereOffPoint(ball->m_position, ball->m_radius, ball->m_velocity, 0.9f, pHoodBoardB, 0.7f, 0.1f))
		{
			ball->AddImpulseTorque(pHoodBoardB, -ball->m_velocity);
			isHit = true;
		}
	}

	if (m_hoopBasketA1)
	{
		Vec3 pHoodBasketA1 = GetNearestPointOnAABB3D(ball->m_position, AABB3(m_hoopBasketA1->m_position, m_hoopBasketA1->m_height, m_hoopBasketA1->m_radius, m_hoopBasketA1->m_height));
		if (BounceSphereOffPoint(ball->m_position, ball->m_radius, ball->m_velocity, 0.9f, pHoodBasketA1, 0.7f, 0.1f))
		{
			ball->AddImpulseTorque(pHoodBasketA1, -ball->m_velocity);
			isHit = true;
		}
	}

	if (m_hoopBasketA2)
	{
		Vec3 pHoodBasketA2 = GetNearestPointOnAABB3D(ball->m_position, AABB3(m_hoopBasketA2->m_position, m_hoopBasketA2->m_height, m_hoopBasketA2->m_height, m_hoopBasketA2->m_radius));
		if (BounceSphereOffPoint(ball->m_position, ball->m_radius, ball->m_velocity, 0.9f, pHoodBasketA2, 0.7f, 0.1f))
		{
			ball->AddImpulseTorque(pHoodBasketA2, -ball->m_velocity);
			isHit = true;
		}
	}

	if (m_hoopBasketA3)
	{
		Vec3 pHoodBasketA3 = GetNearestPointOnAABB3D(ball->m_position, AABB3(m_hoopBasketA3->m_position, m_hoopBasketA3->m_height, m_hoopBasketA3->m_radius, m_hoopBasketA3->m_height));
		if (BounceSphereOffPoint(ball->m_position, ball->m_radius, ball->m_velocity, 0.9f, pHoodBasketA3, 0.7f, 0.1f))
		{
			ball->AddImpulseTorque(pHoodBasketA3, -ball->m_velocity);
			isHit = true;
		}
	}

	if (m_hoopBasketB1)
	{
		Vec3 pHoodBasketB1 = GetNearestPointOnAABB3D(ball->m_position, AABB3(m_hoopBasketB1->m_position, m_hoopBasketB1->m_height, m_hoopBasketB1->m_radius, m_hoopBasketB1->m_height));
		if (BounceSphereOffPoint(ball->m_position, ball->m_radius, ball->m_velocity, 0.9f, pHoodBasketB1, 0.7f, 0.1f))
		{
			ball->AddImpulseTorque(pHoodBasketB1, -ball->m_velocity);
			isHit = true;
		}
	}

	if (m_hoopBasketB2)
	{
		Vec3 pHoodBasketB2 = GetNearestPointOnAABB3D(ball->m_position, AABB3(m_hoopBasketB2->m_position, m_hoopBasketB2->m_height, m_hoopBasketB2->m_height, m_hoopBasketB2->m_radius));
		if (BounceSphereOffPoint(ball->m_position, ball->m_radius, ball->m_velocity, 0.9f, pHoodBasketB2, 0.7f, 0.1f))
		{
			ball->AddImpulseTorque(pHoodBasketB2, -ball->m_velocity);
			isHit = true;
		}
	}

	if (m_hoopBasketB3)
	{
		Vec3 pHoodBasketB3 = GetNearestPointOnAABB3D(ball->m_position, AABB3(m_hoopBasketB3->m_position, m_hoopBasketB3->m_height, m_hoopBasketB3->m_radius, m_hoopBasketB3->m_height));
		if (BounceSphereOffPoint(ball->m_position, ball->m_radius, ball->m_velocity, 0.9f, pHoodBasketB3, 0.7f, 0.1f))
		{
			ball->AddImpulseTorque(pHoodBasketB3, -ball->m_velocity);
			isHit = true;
		}
	}


	if (isHit)
	{
		SonFormularForBallVsGroundCollisionResolve(ball);
		ball->PlaySound(g_theGame->m_ballBounceBackboardSound);
	}
}

void BasketballCourt::BounceBallOffBlocker(Ball* ball)
{
	for (size_t i = 0; i < m_blockerList.size(); i++)
	{
		if (BounceSphereOffPoint(ball->m_position, ball->m_radius, ball->m_velocity, 0.9f, m_blockerList[i]->GetNearestPoint(ball->m_position), 0.5f))
		{
			switch (m_blockerList[i]->m_type)
			{
			case BlockerType::STATIC_BLOCK:
				m_blockerList[i]->PlaySound(g_theGame->m_staticBlockerSound);
				break;
			case BlockerType::CONTINUOUS_BLOCK:
				m_blockerList[i]->PlaySound(g_theGame->m_continuousBlockerSound);
				break;
			case BlockerType::TIMER_BLOCK:
				m_blockerList[i]->PlaySound(g_theGame->m_timerBlockerSound);
				break;
			}
		}
	}
}

bool BasketballCourt::IsBallAScore(Ball* ball, Hoop* hoop)
{
	if (!ball || !hoop)
	{
		return false;
	}

	if (DoSphereAndAABBOverlap3D(ball->m_position, ball->m_radius, hoop->m_collider))
	{
		if (hoop->m_isColliding)
		{
			return false;
		}
		hoop->m_isColliding = true;
		if (ball->m_position.z > hoop->m_collider.GetCenter().z + 0.9f
			&& FloatRange(hoop->m_collider.m_mins.x, hoop->m_collider.m_maxs.x).IsOnRange(ball->m_position.x)
			&& FloatRange(hoop->m_collider.m_mins.y, hoop->m_collider.m_maxs.y).IsOnRange(ball->m_position.y))
		{
			g_theGame->PlaySound(g_theGame->m_scoreSound, true);
			g_theGame->ShakeCamera(15.f);
			if (g_theGame->m_currentGameMode == TIMER)
			{
				g_theGame->m_timeChallengeScore++;
				RandomPlayerPosition();
				DeleteAllBallsThatPlayerNotHolding();
			}
			if (g_theGame->m_currentGameMode == OBSTACLE)
			{
				g_theGame->m_obstacleChallengeLevel++;
				SpawnRandomBlockers(g_theGame->m_obstacleChallengeLevel + 2);
				DeleteAllBallsThatPlayerNotHolding();
			}
			return true;
		}
	}
	else
	{
		hoop->m_isColliding = false;
	}

	return false;
}

void BasketballCourt::SonFormularForBallVsGroundCollisionResolve(Ball* ball)
{
	// SON"S FORMULA - NOBEL PRIZE PLEASE
	// MomemtumToBeat = sqrtf(spring constant * friction * 2/ mass) * velocity
	Vec3 MomemtumToBeat = sqrtf(20.f * GROUND_STATIC_FRICTION * 2.f / ball->m_mass) * ball->m_velocity;
	if (MomemtumToBeat.GetLengthSquared() > ball->m_angularVelocity.GetLengthSquared() * ball->m_radius * ball->m_radius)
	{
		ball->m_angularVelocity = -ball->m_angularVelocity;
	}
	ball->AddImpulseTorque(-ball->m_angularVelocity * GROUND_STATIC_FRICTION);
	ball->AddImpulse(GROUND_STATIC_FRICTION * -ball->m_velocity);
}

void BasketballCourt::InitializeCourt()
{
	AddVertsForQuad3D(m_courtVertices, m_courtIndices, Vec3(-50, -50, 0), Vec3(50, -50, 0), Vec3(-50, 50, 0), Vec3(50, 50, 0));

	AddVertsForSkyBox(m_skyboxVertices, AABB3(-50.f, -50.f, -40.f, 50.f, 50.f, 60.f), Rgba8::COLOR_WHITE);

	AddVertsForQuad3D(m_netVertices, m_netIndices, Vec3(39.2f, -1.3f, 10.f), Vec3(42.2f, -1.3f, 10.f), Vec3(39.2f, -1.46f, 12.6f), Vec3(42.2f, -1.46f, 12.6f));
	AddVertsForQuad3D(m_netVertices, m_netIndices, Vec3(39.2f, -1.3f, 10.f), Vec3(39.2f, 1.3f, 10.f), Vec3(39.2f, -1.46f, 12.6f), Vec3(39.2f, 1.46f, 12.6f));
	AddVertsForQuad3D(m_netVertices, m_netIndices, Vec3(42.2f, -1.3f, 10.f), Vec3(42.2f, 1.3f, 10.f), Vec3(42.2f, -1.46f, 12.6f), Vec3(42.2f, 1.46f, 12.6f));
	AddVertsForQuad3D(m_netVertices, m_netIndices, Vec3(39.2f, 1.3f, 10.f), Vec3(42.2f, 1.3f, 10.f), Vec3(39.2f, 1.46f, 12.6f), Vec3(42.2f, 1.46f, 12.6f));

	m_hoopCylA = CreateProp(false, 100.f, 17.f, 0.7f, Vec3(43.3f, 0, 0));
	AddVertsForZCylinder3D(m_hoopCylA->m_vertexes, Vec2(0, 0), m_hoopCylA->GetHeightRange(), m_hoopCylA->m_radius, 32);
	m_hoopCylA->m_texture = g_theGame->m_hoopCylTexture;

	m_hoopBoardA = CreateProp(false, 100.f, 3.f, 6.f, Vec3(42.5f, 0.f, 15.f));
	AddVertsForAABB3D(m_hoopBoardA->m_vertexes, AABB3(Vec3(0, 0, 0), m_hoopBoardA->m_height, 0.1f, m_hoopBoardA->m_radius));
	m_hoopBoardA->m_texture = g_theGame->m_hoopBoardTexture;

	m_hoopBasketA1 = CreateProp(false, 100.f, 0.1f, 1.65f, Vec3(40.8f, 1.46f, 12.7f));
	AddVertsForAABB3D(m_hoopBasketA1->m_vertexes, AABB3(Vec3(0, 0, 0), m_hoopBasketA1->m_height, m_hoopBasketA1->m_radius, m_hoopBasketA1->m_height));
	m_hoopBasketA1->m_texture = g_theGame->m_hoopBasketTexture;

	m_hoopBasketA2 = CreateProp(false, 100.f, 0.1f, 1.57f, Vec3(39.2f, 0.f, 12.7f));
	AddVertsForAABB3D(m_hoopBasketA2->m_vertexes, AABB3(Vec3(0, 0, 0), m_hoopBasketA2->m_height, m_hoopBasketA2->m_height, m_hoopBasketA2->m_radius));
	m_hoopBasketA2->m_texture = g_theGame->m_hoopBasketTexture;

	m_hoopBasketA3 = CreateProp(false, 100.f, 0.1f, 1.65f, Vec3(40.8f, -1.46f, 12.7f));
	AddVertsForAABB3D(m_hoopBasketA3->m_vertexes, AABB3(Vec3(0, 0, 0), m_hoopBasketA3->m_height, m_hoopBasketA3->m_radius, m_hoopBasketA3->m_height));
	m_hoopBasketA3->m_texture = g_theGame->m_hoopBasketTexture;

	m_hoopA = new Hoop(AABB3(Vec3(40.8f, 0.f, 11.5f), 0.5f, 1.5f, 1.5f));

	if (g_theGame->m_currentGameMode == CREATIVE)
	{
		AddVertsForQuad3D(m_netVertices, m_netIndices, Vec3(-39.2f, -1.3f, 10.f), Vec3(-42.2f, -1.3f, 10.f), Vec3(-39.2f, -1.46f, 12.6f), Vec3(-42.2f, -1.46f, 12.6f));
		AddVertsForQuad3D(m_netVertices, m_netIndices, Vec3(-39.2f, -1.3f, 10.f), Vec3(-39.2f, 1.3f, 10.f), Vec3(-39.2f, -1.46f, 12.6f), Vec3(-39.2f, 1.46f, 12.6f));
		AddVertsForQuad3D(m_netVertices, m_netIndices, Vec3(-42.2f, -1.3f, 10.f), Vec3(-42.2f, 1.3f, 10.f), Vec3(-42.2f, -1.46f, 12.6f), Vec3(-42.2f, 1.46f, 12.6f));
		AddVertsForQuad3D(m_netVertices, m_netIndices, Vec3(-39.2f, 1.3f, 10.f), Vec3(-42.2f, 1.3f, 10.f), Vec3(-39.2f, 1.46f, 12.6f), Vec3(-42.2f, 1.46f, 12.6f));

		m_hoopCylB = CreateProp(false, 100.f, 17.f, 0.7f, Vec3(-43.3f, 0, 0));
		AddVertsForZCylinder3D(m_hoopCylB->m_vertexes, Vec2(0, 0), m_hoopCylB->GetHeightRange(), m_hoopCylB->m_radius, 32);
		m_hoopCylB->m_texture = g_theGame->m_hoopCylTexture;

		m_hoopBoardB = CreateProp(false, 100.f, 3.f, 6.f, Vec3(-42.5f, 0.f, 15.f));
		AddVertsForAABB3D(m_hoopBoardB->m_vertexes, AABB3(Vec3(0, 0, 0), m_hoopBoardB->m_height, 0.1f, m_hoopBoardB->m_radius));
		m_hoopBoardB->m_texture = g_theGame->m_hoopBoardTexture;

		m_hoopBasketB1 = CreateProp(false, 100.f, 0.1f, 1.65f, Vec3(-40.8f, 1.46f, 12.7f));
		AddVertsForAABB3D(m_hoopBasketB1->m_vertexes, AABB3(Vec3(0, 0, 0), m_hoopBasketB1->m_height, m_hoopBasketB1->m_radius, m_hoopBasketB1->m_height));
		m_hoopBasketB1->m_texture = g_theGame->m_hoopBasketTexture;

		m_hoopBasketB2 = CreateProp(false, 100.f, 0.1f, 1.57f, Vec3(-39.2f, 0.f, 12.7f));
		AddVertsForAABB3D(m_hoopBasketB2->m_vertexes, AABB3(Vec3(0, 0, 0), m_hoopBasketB2->m_height, m_hoopBasketB2->m_height, m_hoopBasketB2->m_radius));
		m_hoopBasketB2->m_texture = g_theGame->m_hoopBasketTexture;

		m_hoopBasketB3 = CreateProp(false, 100.f, 0.1f, 1.65f, Vec3(-40.8f, -1.46f, 12.7f));
		AddVertsForAABB3D(m_hoopBasketB3->m_vertexes, AABB3(Vec3(0, 0, 0), m_hoopBasketB3->m_height, m_hoopBasketB3->m_radius, m_hoopBasketB3->m_height));
		m_hoopBasketB3->m_texture = g_theGame->m_hoopBasketTexture;

		m_hoopB = new Hoop(AABB3(Vec3(-40.8f, 0.f, 11.5f), 0.5f, 1.5f, 1.5f));
	}
}

void BasketballCourt::DrawCourt() const
{
	g_theRenderer->BindShader(nullptr);
	g_theRenderer->SetModelConstants();
	g_theRenderer->SetDepthStencilMode(DepthMode::ENABLED);
	g_theRenderer->SetBlendMode(BlendMode::ALPHA);

	g_theRenderer->SetRasterizerMode(RasterizerMode::SOLID_CULL_BACK);
	g_theRenderer->BindTexture(g_theGame->m_courtTexture);
	g_theRenderer->DrawIndexedBuffer(m_courtVertices, m_courtIndices);

	g_theRenderer->BindTexture(g_theGame->m_skyboxTexture);
	g_theRenderer->DrawVertexArray(m_skyboxVertices.size(), m_skyboxVertices.data());

	g_theRenderer->SetRasterizerMode(RasterizerMode::SOLID_CULL_NONE);
	g_theRenderer->BindTexture(g_theGame->m_netTexture);
	g_theRenderer->DrawIndexedBuffer(m_netVertices, m_netIndices);
}

void BasketballCourt::DrawGrid() const
{
	DebugAddWorldBasis();
	// Drawing Grid

	g_theRenderer->BindTexture(nullptr);
	g_theRenderer->SetRasterizerMode(RasterizerMode::SOLID_CULL_BACK);

	std::vector<Vertex_PCU> gridVertexes;

	float smallSize = 0.02f;
	float medSize = 0.03f;
	float largeSize = 0.06f;

	for (int x = -50; x <= 50; x++)
	{
		AABB3 cube;
		Rgba8 color;

		if (x % 5 == 0)
		{
			cube = AABB3(-medSize + x, -50.f, -medSize, medSize + x, 50.f, medSize);
			color = Rgba8(0, 180, 0);
		}
		else
		{
			cube = AABB3(-smallSize + x, -50.f, -smallSize, smallSize + x, 50.f, smallSize);
			color = Rgba8::COLOR_GRAY;
		}
		if (x == 0)
		{
			cube = AABB3(-largeSize + x, -50.f, -largeSize, largeSize + x, 50.f, largeSize);
			color = Rgba8::COLOR_GREEN;
		}
		AddVertsForAABB3D(gridVertexes, cube, color);
	}
	for (int y = -50; y <= 50; y++)
	{
		AABB3 cube;
		Rgba8 color;

		if (y % 5 == 0)
		{
			cube = AABB3(-50.f, -medSize + y, -medSize, 50.f, medSize + y, medSize);
			color = Rgba8(180, 0, 0);
		}
		else
		{
			cube = AABB3(-50.f, -smallSize + y, -smallSize, 50.f, smallSize + y, smallSize);
			color = Rgba8::COLOR_GRAY;
		}
		if (y == 0)
		{
			cube = AABB3(-50.f, -largeSize + y, -largeSize, 50.f, largeSize + y, largeSize);
			color = Rgba8::COLOR_RED;
		}
		AddVertsForAABB3D(gridVertexes, cube, color);
	}

	g_theRenderer->SetModelConstants();
	g_theRenderer->DrawVertexArray((int)gridVertexes.size(), gridVertexes.data());
}

void BasketballCourt::RandomPlayerPosition()
{
	float x = g_theRNG->RollRandomFloatInRange(-20.f, 25.f);
	float y = g_theRNG->RollRandomFloatInRange(-25.f, 25.f);
	while ((Vec2(x, y) - m_player->GetPositionXY()).GetLengthSquared() < 25.f)
	{
		x = g_theRNG->RollRandomFloatInRange(-20.f, 25.f);
		y = g_theRNG->RollRandomFloatInRange(-25.f, 25.f);
	}
	m_player->m_position = Vec3(x, y, 1.f);
}

void BasketballCourt::SpawnRandomBlockers(int num)
{
	for (size_t i = 0; i < m_blockerList.size(); i++)
	{
		if (m_blockerList[i])
		{
			delete m_blockerList[i];
		}
	}
	m_blockerList.clear();

	for (size_t i = 0; i < num; i++)
	{
		BlockerType type = (BlockerType)g_theRNG->RollRandomIntInRange(0, (int)BlockerType::BlockerType_COUNT - 1);
		float xPos = g_theRNG->RollRandomFloatInRange(1.f, 21.f);
		float yPos = g_theRNG->RollRandomFloatInRange(-26.f, 26.f);
		float zPos = 0.f;
		float width = g_theRNG->RollRandomFloatInRange(3.f, 7.f);
		float minHeight = g_theRNG->RollRandomFloatInRange(2.f, 4.f);
		float maxHeight = g_theRNG->RollRandomFloatInRange(15.f, 25.f);
		if (type == BlockerType::STATIC_BLOCK)
		{
			maxHeight = g_theRNG->RollRandomFloatInRange(15.f, 20.f);
		}
		float timer = g_theRNG->RollRandomFloatInRange(0.5f, 3.);
		CreateBlocker(type, Vec3(xPos, yPos, zPos), width, minHeight, maxHeight, timer);
	}
}
