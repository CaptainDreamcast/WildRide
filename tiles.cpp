#include "tiles.h"

#include <prism/blitz.h>

#include "gamescreen.h"
#include "coaster.h"
#include "stuckmann.h"
#include "finalboss.h"

static struct {
	int mHasCheckPoint = 0;
	Vector2DI mCheckPoint;

} gTileHandlerData;

class TileHandler {
public:

	class Tile {
	public:

		int mTileID;
		int mBlitzID;
		int mDamage = 0;
	};

	class Collectable {
	public:
		int mIsActive = 1;
		int mBlitzID;
		Vector2DI mPosition;
	};

	int mTileWidth;
	int mTileHeight;
	double mCameraZoom;
	Vector2DI mStartPosition;

	std::vector<std::vector<Tile>> mTiles;
	std::vector<Collectable> mCheckPoints;
	std::vector<Collectable> mWaggons;

	void loadTilesFromFile(const std::string& tFile) {
		auto buffer = fileToBuffer(tFile.c_str());
		auto p = getBufferPointer(buffer);
		mTileWidth = readIntegerFromTextStreamBufferPointer(&p);
		mTileHeight = readIntegerFromTextStreamBufferPointer(&p);

		mTiles = std::vector<std::vector<Tile>>(mTileHeight, std::vector<Tile>(mTileWidth));
		for (int j = 0; j < mTileHeight; j++) {
			for (int i = 0; i < mTileWidth; i++) {
				const auto value = readIntegerFromTextStreamBufferPointer(&p);
				const auto firstFlag = value / 10;
				if (firstFlag == 1) {
					mStartPosition.x = i;
					mStartPosition.y = j;
				}
				else if (firstFlag == 2) {
					Collectable c;
					c.mPosition = Vector2DI(i, j);
					c.mBlitzID = addBlitzEntity(Position(16 * i + 8, 16 * j + 15, CHECKPOINT_Z));
					addBlitzMugenAnimationComponent(c.mBlitzID, getGameSprites(), getGameAnimations(), 200);
					mCheckPoints.push_back(c);
				}
				else if (firstFlag == 3) {
					Collectable c;
					c.mPosition = Vector2DI(i, j);
					c.mBlitzID = addBlitzEntity(Position(16 * i + 8, 16 * j + 15, CHECKPOINT_Z));
					addBlitzMugenAnimationComponent(c.mBlitzID, getGameSprites(), getGameAnimations(), 100);
					mWaggons.push_back(c);
				}
				else if (firstFlag == 4) {
					setFinalBossPosition(Vector2DI(i, j));
				}
				mTiles[j][i].mTileID = value % 10;
				if (mTiles[j][i].mTileID) {
					mTiles[j][i].mBlitzID = addBlitzEntity(Position(16 * i, 16 * j, TILE_Z));
					addBlitzMugenAnimationComponentStatic(mTiles[j][i].mBlitzID, getGameSprites(), 1, mTiles[j][i].mTileID);
				}
			}
		}

		if (gTileHandlerData.mHasCheckPoint) {
			mStartPosition = gTileHandlerData.mCheckPoint;
		}

		mCameraZoom = readFloatFromTextStreamBufferPointer(&p);
		setBlitzCameraPositionBasedOnCenterPoint(Vector3D(160, getRealPositionFromTile(mStartPosition).y, 0));
		setBlitzCameraHandlerScale2D(mCameraZoom);
		setBlitzCameraHandlerEffectPositionOffset(Position2D(160, 120));
	}

	TileHandler() {
		loadTilesFromFile("levels/level1.def");
	}

	void update() {}
};

EXPORT_ACTOR_CLASS(TileHandler);

Vector2DI getCoasterStartPosition()
{
	return gTileHandler->mStartPosition;
}

Position2D getRealPositionFromTile(const Vector2DI& tTilePosition)
{
	return Position2D(tTilePosition.x * 16, tTilePosition.y * 16);
}

Vector2DI getTilePositionFromReal(const Position2D& tPosition)
{
	return Vector2DI(int(tPosition.x / 16), int(tPosition.y / 16));
}

int getCoasterPositionAndAngle(const Vector2DI& tTilePosition, double t, Position2D& oPos, double& oAngle)
{
	oAngle = 0.0;
	const auto baseOffset = getRealPositionFromTile(tTilePosition);
	if (tTilePosition.x < 0 || tTilePosition.y < 0 || tTilePosition.x >= gTileHandler->mTileWidth || tTilePosition.y >= gTileHandler->mTileHeight) return 0;
	const auto type = gTileHandler->mTiles[tTilePosition.y][tTilePosition.x].mTileID;

	if (type == 1) {
		oAngle = M_PI * 3 / 2 + (1 - t) * M_PI / 2;
		oPos = baseOffset + Position2D((1.0 - t) * 15, (1.0 - t) * 15);
		return 1;
	}
	else if (type == 2) {
		oAngle = 0;
		oPos = baseOffset + Position2D((1.0 - t) * 15, 15);
		return 1;
	}
	else if (type == 3) {
		oAngle = (1 - t) * M_PI / 2;
		oPos = baseOffset + Position2D((1.0 - t) * 15, t * 15);
		return 1;
	}
	else if (type == 4) {
		oAngle = M_PI / 2;
		oPos = baseOffset + Position2D(15, t * 15);
		return 1;
	}
	else if (type == 5) {
		oAngle = M_PI / 2 + (1 - t) * M_PI / 2;
		oPos = baseOffset + Position2D(t * 15, t * 15);
		return 1;
	}
	else if (type == 6) {
		oAngle = M_PI;
		oPos = baseOffset + Position2D(t * 15, 0);
		return 1;
	}
	else if (type == 7) {
		oAngle = M_PI + (1 - t) * M_PI / 2;
		oPos = baseOffset + Position2D(t * 15, (1.0 - t) * 15);
		return 1;
	}
	else if (type == 8) {
		oAngle = M_PI * 3 /2;
		oPos = baseOffset + Position2D(0, (1.0 - t) * 15);
		return 1;
	}

	return 0;
}

int isOutsideTiles(const Vector2D & tPosition)
{
	const auto tilePosition = getTilePositionFromReal(tPosition);
	return (tilePosition.x < 0 || tilePosition.y < 0 || tilePosition.x >= gTileHandler->mTileWidth || tilePosition.y >= gTileHandler->mTileHeight);
}

Vector2DI getFollowupCoasterTilePosition(const Vector2DI & tTilePosition)
{
	if (tTilePosition.x < 0 || tTilePosition.y < 0 || tTilePosition.x >= gTileHandler->mTileWidth || tTilePosition.y >= gTileHandler->mTileHeight) return tTilePosition;
	const auto type = gTileHandler->mTiles[tTilePosition.y][tTilePosition.x].mTileID;

	if (type == 1) {
		return Vector2DI(tTilePosition.x, tTilePosition.y - 1);
	}
	else if (type == 2) {
		return Vector2DI(tTilePosition.x - 1, tTilePosition.y);
	}
	else if (type == 3) {
		return Vector2DI(tTilePosition.x - 1, tTilePosition.y);
	}
	else if (type == 4) {
		return Vector2DI(tTilePosition.x, tTilePosition.y + 1);
	}
	else if (type == 5) {
		return Vector2DI(tTilePosition.x, tTilePosition.y + 1);
	}
	else if (type == 6) {
		return Vector2DI(tTilePosition.x + 1, tTilePosition.y);
	}
	else if (type == 7) {
		return Vector2DI(tTilePosition.x + 1, tTilePosition.y);
	}
	else if (type == 8) {
		return Vector2DI(tTilePosition.x, tTilePosition.y - 1);
	}

	return tTilePosition;
}

int addTileDestruction(const Vector2DI & tTilePosition)
{
	if (tTilePosition.x < 0 || tTilePosition.y < 0 || tTilePosition.x >= gTileHandler->mTileWidth || tTilePosition.y >= gTileHandler->mTileHeight) return 1;
	if (!gTileHandler->mTiles[tTilePosition.y][tTilePosition.x].mTileID) return 1;

	gTileHandler->mTiles[tTilePosition.y][tTilePosition.x].mDamage++;
	if (gTileHandler->mTiles[tTilePosition.y][tTilePosition.x].mDamage == 3) {
		gTileHandler->mTiles[tTilePosition.y][tTilePosition.x].mTileID = 0;
		removeBlitzEntity(gTileHandler->mTiles[tTilePosition.y][tTilePosition.x].mBlitzID);
		return 1;
	}

	const auto colorFactor = 1.0 - (gTileHandler->mTiles[tTilePosition.y][tTilePosition.x].mDamage / 3.0);
	setBlitzMugenAnimationColor(gTileHandler->mTiles[tTilePosition.y][tTilePosition.x].mBlitzID, 1.0, colorFactor, colorFactor);
	return 0;
}

double handleAngleWrap(double x) {
	x = std::fmod(x, M_PI * 2);
	while (x < 0) {
		x += M_PI * 2;
	}
	return x;
}

int canCoasterLand(const Position2D& tPosition, double tAngle)
{
	const auto tilePosition = getTilePositionFromReal(tPosition);
	if (tilePosition.x < 0 || tilePosition.y < 0 || tilePosition.x >= gTileHandler->mTileWidth || tilePosition.y >= gTileHandler->mTileHeight) return 0;
	const auto type = gTileHandler->mTiles[tilePosition.y][tilePosition.x].mTileID;
	if (!type) return 0;

	tAngle = handleAngleWrap(tAngle);
	if (type == 1) {
		return (tAngle > 0) && (tAngle < (M_PI / 2));
	}
	else if (type == 2) {
		return (tAngle < M_PI / 3) || (tAngle > (M_PI * 2) - (M_PI / 3));
	}
	else if (type == 3) {
		return (tAngle > (M_PI / 2)) && (tAngle < (M_PI));
	}
	else if (type == 4) {
		return (tAngle > (M_PI / 2) - (M_PI / 3)) && (tAngle < (M_PI / 2) + (M_PI / 3));
	}
	else if (type == 5) {
		return (tAngle > M_PI) && (tAngle < (M_PI * 3 / 2));
	}
	else if (type == 6) {
		return (tAngle > (M_PI) - (M_PI / 3)) && (tAngle < (M_PI) + (M_PI / 3));
	}
	else if (type == 7) {
		return (tAngle > (M_PI * 3 / 2)) && (tAngle < (M_PI * 2));
	}
	else if (type == 8) {
		return (tAngle > (M_PI * 3 / 2) - (M_PI / 3)) && (tAngle < (M_PI * 3 / 2) + (M_PI / 3));
	}

	return 0;
}

void testCoasterCollections(const Vector2DI & tTilePosition)
{
	for (auto& c : gTileHandler->mCheckPoints) {
		if (!c.mIsActive) continue;
		if (c.mPosition == tTilePosition) {
			removeBlitzEntity(c.mBlitzID);
			c.mIsActive = 0;
			saveWaggonAmount();
			saveStuckmannAge();
			gTileHandlerData.mCheckPoint = tTilePosition;
			gTileHandlerData.mHasCheckPoint = 1;
		}
	}

	for (auto& w : gTileHandler->mWaggons) {
		if (!w.mIsActive) continue;
		if (w.mPosition == tTilePosition) {
			removeBlitzEntity(w.mBlitzID);
			w.mIsActive = 0;
			addCoasterWaggon();
			increaseStuckmannAge();
		}
	}
}

void resetCheckpoint()
{
	gTileHandlerData.mHasCheckPoint = 0;
}
