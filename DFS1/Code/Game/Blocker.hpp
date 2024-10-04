#pragma once
#include "Game/GameCommon.hpp"
#include "Game/Entity.hpp"

enum class BlockerType
{
	STATIC_BLOCK,
	CONTINUOUS_BLOCK,
	TIMER_BLOCK,
	BlockerType_COUNT
};

class Blocker: public Entity
{
public:
	Blocker(BasketballCourt* map, BlockerType type, Vec3 pos, float width, float minHeight = 0, float maxHeight = 10, float time = 2.f);
	virtual ~Blocker();
	
	virtual void Update(float deltaSeconds) override;
	virtual void Render() const override;
	Vec3 GetNearestPoint(Vec3 const point);

	void PlaySound(SoundID sound);
public:
	float m_minHeight = 0;
	float m_maxHeight = 10;
	int m_sign = 1;

	BlockerType m_type = BlockerType::STATIC_BLOCK;
	Timer* m_timer = nullptr;
	Rgba8						m_color = Rgba8::COLOR_WHITE;
	Texture*					m_texture = nullptr;
};