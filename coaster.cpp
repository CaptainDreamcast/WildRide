#include "coaster.h"

#include <prism/blitz.h>
#include <deque>

#include "tiles.h"
#include "gamescreen.h"
#include "stuckmann.h"
#include "finalboss.h"
#include "storyscreen.h"

static struct {
	int mWaggonCount = 0;

} gCoasterData;

class Coaster {
public:
	Vector2DI mTilePosition;
	double mTrackOffset;
	int mBlitzID;
	
	double mSpeed = 0.1;
	int mIsFlying = 0;

	Vector2D mFlyingVelocity;

	class WaggonPosition {
	public:
		Vector2D mPosition;
		double mAngle;
	};
	std::deque<WaggonPosition> mPastPositions;

	class Waggon {
	public:
		int mBlitzID;
		int mIndex;
	};
	std::vector<Waggon> mWaggons;

	Coaster() {
		mTrackOffset = 0.5;
		mTilePosition = getCoasterStartPosition();
		double angle = 0;
		Position2D pos = Position2D(0, 0);
		assert(getCoasterPositionAndAngle(mTilePosition, mTrackOffset, pos, angle));
		mBlitzID = addBlitzEntity(pos.xyz(COASTER_Z));
		addBlitzMugenAnimationComponent(mBlitzID, getGameSprites(), getGameAnimations(), 100);
		setBlitzEntityRotationZ(mBlitzID, angle);
		addBlitzPhysicsComponent(mBlitzID);

		mPastPositions.push_back(WaggonPosition{ pos, angle });
		for (int i = 0; i < gCoasterData.mWaggonCount; i++) {
			addWaggon();
		}
	}

	int mAirTime = 0;

	void startFlying() {
		mFlyingVelocity = vecRotateZ(Vector3D(1, 0, 0), (M_PI - getBlitzEntityRotationZ(mBlitzID))).xy() * mSpeed * 16;
		setBlitzPhysicsVelocity(mBlitzID, mFlyingVelocity.xyz(0));
		tryPlayMugenSound(getGameSounds(), 1, 2);
		mAirTime = 0;
		mIsFlying = 1;
	}

	void stopFlying() {
		mTilePosition = getTilePositionFromReal(getBlitzEntityPosition(mBlitzID).xy());
		mTrackOffset = 0.5;
		setBlitzPhysicsVelocity(mBlitzID, Vector3D(0, 0, 0));
		updateNonFlyingMovement();
		tryPlayMugenSound(getGameSounds(), 1, 1);
		mIsFlying = 0;
	}

	void updateNonFlyingMovement() {
		mTrackOffset += mSpeed;
		while (mTrackOffset >= 1.0) {
			mTilePosition = getFollowupCoasterTilePosition(mTilePosition);
			mTrackOffset -= 1.0;
		}

		double angle = 0;
		Position2D pos = Position2D(0, 0);
		const auto isOnTracks = getCoasterPositionAndAngle(mTilePosition, mTrackOffset, pos, angle);
		if (!isOnTracks) {
			startFlying();
			return;
		}
		setBlitzEntityPosition(mBlitzID, pos.xyz(COASTER_Z));
		setBlitzEntityRotationZ(mBlitzID, angle);
	}

	void updateSpeedup() {
		if (hasPressedLeft()) {
			mSpeed += 0.005;
		} else if (hasPressedRight()) {
			mSpeed -= 0.005;
			mSpeed = std::max(0.05, mSpeed);
		}
	}

	void updateTileDestruction() {
		if (hasPressedAFlank()) {
			tryPlayMugenSound(getGameSounds(), 1, 0);
			if (addTileDestruction(mTilePosition)) {
				startFlying();
			}
		}
	}

	void updateNonFlyingInput() {
		updateSpeedup();
		updateTileDestruction();
	}

	void updateNonFlying() {
		updateNonFlyingMovement();
		if (!mIsFlying) {
			testCoasterCollections(mTilePosition);
			updateNonFlyingInput();
		}
	}

	void updateMotion() {
		addBlitzPhysicsVelocity(mBlitzID, Vector3D(0, 0.02, 0));
	}

	double handleAngleWrap(double x) {
		x = std::fmod(x, M_PI * 2);
		while (x < 0) {
			x += M_PI * 2;
		}
		return x;
	}

	void updateFlyingRotation() {
		auto currentAngle = getBlitzEntityRotationZ(mBlitzID);
		currentAngle = handleAngleWrap(currentAngle);
		auto targetAngle = getAngleFromDirection(getBlitzPhysicsVelocity(mBlitzID));
		targetAngle = handleAngleWrap(targetAngle);
		//if (targetAngle > M_PI / 2 && targetAngle < M_PI * 3 / 2) targetAngle += M_PI;
		//targetAngle -= M_PI / 2;
		double t = clamp(mAirTime / 60.0, 0.0, 1.0);
		//while (targetAngle < currentAngle) targetAngle += 2 * M_PI;
		setBlitzEntityRotationZ(mBlitzID, targetAngle - (M_PI / 2) * t);
	}

	void updateLanding() {
		if (mAirTime > 10 && canCoasterLand(getBlitzEntityPosition(mBlitzID).xy(), getBlitzEntityRotationZ(mBlitzID))) {
			stopFlying();
		}
	}

	void updateFlying() {
		mAirTime++;
		updateMotion();
		updateFlyingRotation();
		updateLanding();
	}

	void updateCamera() {
		const auto posY = getBlitzEntityPositionY(mBlitzID);
		auto cameraPos = getBlitzCameraHandlerPosition();
		if (posY - cameraPos.y < 120) {
			const auto delta = 120 - (posY - cameraPos.y);
			setBlitzCameraHandlerPositionY(cameraPos.y - delta);
		}
		if (posY - cameraPos.y > 200) {
			const auto delta = (posY - cameraPos.y) - 200;
			setBlitzCameraHandlerPositionY(cameraPos.y + delta);
		}
	}

	void updateDeath() {
		if (isOutsideTiles(getBlitzEntityPosition(mBlitzID).xy()) || (getStuckmannAge() == 80)) {
			setNewScreen(getGameScreen());
		}

		if (!isOnDreamcast() && hasPressedKeyboardKeyFlank(KEYBOARD_R_PRISM)) {
			setNewScreen(getGameScreen());
		}

		if (isOnDreamcast() && hasPressedRFlank()) {
			setNewScreen(getGameScreen());
		}
	}

	void updateVictory() {
		const auto pos2D = getBlitzEntityPosition(mBlitzID).xy();
		if (vecLength(pos2D - getFinalBossPosition()) < 20) {
			setCurrentStoryDefinitionFile("OUTRO");
			setNewScreen(getStoryScreen());
		}
	}

	void updatePastPositions() {
		mPastPositions.push_back(WaggonPosition{ getBlitzEntityPosition(mBlitzID).xy(), getBlitzEntityRotationZ(mBlitzID) });
		while (mPastPositions.size() > 60) mPastPositions.pop_front();
	}

	void updateWaggons() {
		for (auto& w : mWaggons) {
			const auto posEntry = mPastPositions.at(std::max(0, int(mPastPositions.size() - 1) - 5 * (w.mIndex + 1)));
			setBlitzEntityPosition(w.mBlitzID, posEntry.mPosition.xyz(COASTER_Z));
			setBlitzEntityRotationZ(w.mBlitzID, posEntry.mAngle);
		}
	}

	void update() {
		if (mIsFlying) {
			updateFlying();
		}
		else {
			updateNonFlying();
		}
		updateCamera();
		updateDeath();
		updateVictory();
		updatePastPositions();
		updateWaggons();
	}


	void addWaggon() {
		Waggon w;
		w.mIndex = mWaggons.size();
		const auto posEntry = mPastPositions.at(std::max(0, int(mPastPositions.size() - 1) - 5 * (w.mIndex + 1)));
		w.mBlitzID = addBlitzEntity(posEntry.mPosition.xyz(COASTER_Z));
		addBlitzMugenAnimationComponent(w.mBlitzID, getGameSprites(), getGameAnimations(), 100);
		setBlitzEntityRotationZ(w.mBlitzID, posEntry.mAngle);
		mWaggons.push_back(w);
	}
};

EXPORT_ACTOR_CLASS(Coaster);

void addCoasterWaggon()
{
	gCoaster->addWaggon();
}

void saveWaggonAmount()
{
	gCoasterData.mWaggonCount = int(gCoaster->mWaggons.size());
}

void resetWaggonAmount()
{
	gCoasterData.mWaggonCount = 0;
}
