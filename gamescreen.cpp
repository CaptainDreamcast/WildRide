#include "gamescreen.h"

#include <prism/blitz.h>

#include "tiles.h"
#include "coaster.h"
#include "stuckmann.h"
#include "finalboss.h"

class GameScreen {
public:
	static MugenSpriteFile mSprites;
	static MugenAnimations mAnimations;
	static MugenSounds mSounds;
	
	MugenAnimationHandlerElement* mAnimationID;
	int mParkEntity;

	GameScreen() {
		mSprites = loadMugenSpriteFileWithoutPalette("levels/game.sff");
		mAnimations = loadMugenAnimationFile("levels/game.air");
		mSounds = loadMugenSoundFile("levels/game.snd");

		mAnimationID = addMugenAnimation(getMugenAnimation(getGameAnimations(), 401), getGameSprites(), Vector3D(0, 0, 1));
		setMugenAnimationTransparency(mAnimationID, 0.5);
		mParkEntity = addBlitzEntity(getRealPositionFromTile(Vector2DI(10, 127)).xyz(2));
		addBlitzMugenAnimationComponent(mParkEntity, getGameSprites(), getGameAnimations(), 402);
		setBlitzMugenAnimationBaseDrawScale(mParkEntity, 4);

		instantiateActor(getTileHandler());
		instantiateActor(getCoaster());
		instantiateActor(getStuckmann());
		instantiateActor(getFinalBoss());
		streamMusicFile("tracks/4.ogg");
	}

	void update() {
	
	}
};

MugenSpriteFile GameScreen::mSprites;
MugenAnimations GameScreen::mAnimations;
MugenSounds GameScreen::mSounds;

EXPORT_SCREEN_CLASS(GameScreen);

MugenSpriteFile* getGameSprites()
{
	return &GameScreen::mSprites;
}

MugenAnimations* getGameAnimations()
{
	return &GameScreen::mAnimations;
}

MugenSounds * getGameSounds()
{
	return &GameScreen::mSounds;
}
