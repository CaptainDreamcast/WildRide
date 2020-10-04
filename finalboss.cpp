#include "finalboss.h"

#include <prism/blitz.h>

#include "gamescreen.h"
#include "tiles.h"

static struct {
	Vector2D mPosition;

} gFinalBossData;

class FinalBoss {
public:
	int mBlitzID;

	FinalBoss() {
		mBlitzID = addBlitzEntity(gFinalBossData.mPosition.xyz(BOSS_Z));
		addBlitzMugenAnimationComponent(mBlitzID, getGameSprites(), getGameAnimations(), 300);
	}

	void update() {
		
	}
};

EXPORT_ACTOR_CLASS(FinalBoss);

void setFinalBossPosition(const Vector2DI& tTilePosition)
{
	gFinalBossData.mPosition = getRealPositionFromTile(tTilePosition);
}

const Position2D & getFinalBossPosition()
{
	return gFinalBossData.mPosition;
}
