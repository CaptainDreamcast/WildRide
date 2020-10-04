#pragma once

#include <prism/actorhandler.h>
#include <prism/geometry.h>

ActorBlueprint getTileHandler();

Vector2DI getCoasterStartPosition();

Position2D getRealPositionFromTile(const Vector2DI& tTilePosition);
Vector2DI getTilePositionFromReal(const Position2D& tPosition);
int getCoasterPositionAndAngle(const Vector2DI& tTilePosition, double tOffsetInsideTile, Position2D& oPos, double& oAngle);
int isOutsideTiles(const Vector2D& tPosition);

Vector2DI getFollowupCoasterTilePosition(const Vector2DI& tTilePosition);

int addTileDestruction(const Vector2DI& tTilePosition);
int canCoasterLand(const Position2D& tPosition, double tAngle);
void testCoasterCollections(const Vector2DI& tTilePosition);
void resetCheckpoint();