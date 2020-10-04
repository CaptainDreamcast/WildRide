#include "titlescreen.h"

#include <prism/blitz.h>

#include "gamescreen.h"
#include "storyscreen.h"
#include "tiles.h"
#include "coaster.h"
#include "gamescreen.h"
#include "stuckmann.h"


static struct {
	MugenSpriteFile mSprites;
	MugenSounds mSounds;
} gTitleScreenData;

static void loadTitleScreen() {
	gTitleScreenData.mSprites = loadMugenSpriteFileWithoutPalette("levels/title.sff");
	auto id = addMugenAnimation(createOneFrameMugenAnimationForSprite(1, 0), &gTitleScreenData.mSprites, Vector3D(0, 0, 1));
	addFadeIn(20, NULL, NULL);
	resetWaggonAmount();
	resetStuckmannAge();
	resetCheckpoint();
	setWrapperTitleScreen(getTitleScreen());
	streamMusicFile("tracks/3.ogg");
}


static void gotoGameScreen(void* tCaller) {
	(void)tCaller;
	setNewScreen(getGameScreen());
}

static void updateTitleScreen() {

	if (hasPressedAFlank() || hasPressedStartFlank()) {
		addFadeOut(30, gotoGameScreen, NULL);
	}
}

static Screen gTitleScreen;

Screen* getTitleScreen() {
	gTitleScreen = makeScreen(loadTitleScreen, updateTitleScreen);
	return &gTitleScreen;
};
