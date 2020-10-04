#pragma once

#include <prism/actorhandler.h>
#include <prism/geometry.h>

ActorBlueprint getFinalBoss();

void setFinalBossPosition(const Vector2DI& tTilePosition);
const Position2D& getFinalBossPosition();