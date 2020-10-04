#include "stuckmann.h"

#include <prism/blitz.h>

#include "gamescreen.h"


static struct {
	int mSaveAge = 2;
} gStuckmannData;

class Stuckmann {
public:
	int mAge = 2;
	int mTextID;
	int mAgeLeftTextID;
	int mRealAgeTicks = 0;
	int mRealAge = 20;

	Stuckmann() {
		mAge = gStuckmannData.mSaveAge;
		mRealAge = 20;
		mTextID = addMugenTextMugenStyle("", Vector3D(230, 10, TEXT_Z), Vector3DI(1, 0, 1));
		setMugenTextTextBoxWidth(mTextID, 70);
		setMugenTextScale(mTextID, 0.5);
		mAgeLeftTextID = addMugenTextMugenStyle("", Vector3D(230, 20, TEXT_Z), Vector3DI(1, 0, 1));
		setMugenTextTextBoxWidth(mAgeLeftTextID, 70);
		setMugenTextScale(mAgeLeftTextID, 0.5);
		updateDisplayText();
		updateRealAgeText();
	}

	void updateDisplayText() {
		char text[1024];
		std::string ending;
		if ((mAge % 10) == 2 && (mAge % 100) != 12) {
			ending = "nd";
		}
		else if ((mAge % 10) == 3 && (mAge % 100) != 13) {
			ending = "rd";
		}
		else if ((mAge % 10) == 1 && (mAge % 100) != 11) {
			ending = "st";
		}
		else {
			ending = "th";
		}

		sprintf(text, "Stuckmann the %d%s", mAge, ending.c_str());
		changeMugenText(mTextID, text);
	}

	void updateRealAgeText() {
		char text[1024];
		if (mRealAge == 79) {
			sprintf(text, "Age %d                   (%d year left)", mRealAge, 80 - mRealAge);
		}
		else {
			sprintf(text, "Age %d                   (%d years left)", mRealAge, 80 - mRealAge);
		}
		changeMugenText(mAgeLeftTextID, text);
	}

	void updateRealAge() {
		mRealAgeTicks++;
		if (mRealAgeTicks >= 120) {
			mRealAge++;
			mRealAgeTicks = 0;
			updateRealAgeText();
		}
	}

	void update() {
		updateRealAge();
	}
};

EXPORT_ACTOR_CLASS(Stuckmann);

void saveStuckmannAge()
{
	gStuckmannData.mSaveAge = gStuckmann->mAge;
}

void resetStuckmannAge()
{
	gStuckmannData.mSaveAge = 2;
}

int getStuckmannAge()
{
	return gStuckmann->mRealAge;
}

void increaseStuckmannAge()
{
	gStuckmann->mAge++;
	gStuckmann->mRealAge = 20;
	gStuckmann->updateRealAgeText();
	gStuckmann->updateDisplayText();
}
