#include "LetterJammer.h"



LetterJammer::LetterJammer(GUIFactory* factory,
	shared_ptr<MouseController> mouseController, Vector2 position,
	wstring text, const pugi::char_t* font)
	: TextLabel(factory, mouseController, position, text, font, true) {

	setText(text);
}


LetterJammer::~LetterJammer() {
	letterJams.clear();
	jammerFXs.clear();
}


bool LetterJammer::update(double deltaTime) {

	if (isHoverable) {
		if (hitArea->contains(mouse->getPosition())) {
			isHover = true;
			if (!isPressed) {
				if (!hasBeenSetHover) {
					onHover();
					setToHoverState();
					hasBeenSetHover = true;
					hasBeenSetUnpressed = false;
				}
			}
		} else
			isHover = false;

		if (isPressed && !mouse->leftButton()) {
			isClicked = true;
			isPressed = false;
			onClick();
			setToUnpressedState();
			hasBeenSetUnpressed = false;
			hasBeenSetHover = false;
		} else {
			isClicked = false;
			if (!isHover) {
				if (!hasBeenSetUnpressed) {
					isPressed = false;
					setToUnpressedState();
					hasBeenSetUnpressed = true;
					hasBeenSetHover = false;
				}
			} else if (mouse->pressed()) {
				isPressed = true;
				setToSelectedState();
				hasBeenSetUnpressed = false;
				hasBeenSetHover = false;
			}
		}
	}

	done = true;
	for (const auto& fx : jammerFXs)
		if (!fx->run(deltaTime, letterJams))
			done = false;

	if (refreshTexture) {
		if (label.size() > 0) {
			gfxAsset = texturize();
			textTexture = gfxAsset->getTexture();
		}
		refreshTexture = false;
		return true;
	}

	return false;
}

void LetterJammer::draw(SpriteBatch* batch) {

	for (LetterJam letter : letterJams) {
		batch->Draw(textTexture.Get(), letter.position, &letter.sourceRECT,
			letter.tint, letter.rotation, letter.origin, letter.scale,
			letter.spriteEffects, letter.layerDepth);
	}
}

void LetterJammer::setEffect(unique_ptr<JammerEffect> effect) {

	if (letterJams.size() > 0)
		effect->initialize(letterJams, (PVOID) &position);
	jammerFXs.push_back(move(effect));
}

void LetterJammer::clearEffects() {
	jammerFXs.clear();
}

void LetterJammer::setText(wstring text) {
	TextLabel::setText(text);

	done = false;
	letterJams.clear();
	// cut up texture
	for (int i = 0; i < label.size(); ++i) {
		wstring measureString = label.substr(0, i + 1);
		Vector2 stringMeasurement = font->measureString(measureString.c_str());


		RECT rect;
		rect.left = ((Vector2) font->measureString(label.substr(0, i).c_str())).x + 1;
		rect.top = 0; // if multiline text allowed something will have to be done about this
		rect.right = stringMeasurement.x;
		rect.bottom = ceil(stringMeasurement.y);

		Vector2 pos = position;
		pos.x += rect.left;
		LetterJam jam;
		jam.position = pos;
		jam.sourceRECT = rect;
		letterJams.push_back(jam);
	}

	for (const auto& fx : jammerFXs)
		fx->initialize(letterJams, (PVOID) &position);
}

bool LetterJammer::isDone() {
	return done;
}

void LetterJammer::reset() {
	done = false;
	if (label.size() > 0) {
		gfxAsset = texturize();
		textTexture = gfxAsset->getTexture();
	}
	refreshTexture = false;
	for (const auto& fx : jammerFXs)
		fx->initialize(letterJams, (PVOID) &position);
}



DragonJammer::DragonJammer(int amp, float spd) {
	amplitude = amp;
	speed = spd;
}

DragonJammer::~DragonJammer() {
	times.clear();
}

void DragonJammer::initialize(vector<LetterJam>& jams, LPVOID pvoid) {

	times.clear();

	Vector2* pos = (Vector2*) (pvoid);
	if (pos)
		position = *pos;
	else
		position = Vector2::Zero;

	for (int i = 0; i < jams.size(); ++i) {
		times.push_back(0);
	}
}

bool DragonJammer::run(double deltaTime, vector<LetterJam>& letterJams) {
	for (int i = 0; i < letterJams.size(); ++i) {
		times[i] += deltaTime*i;
		letterJams[i].position.y = position.y + amplitude * sin(speed * times[i]);
	}
	return false;
}



PulsatingJammer::PulsatingJammer(float amp, float scl, float spd, int horz) {
	amplitude = amp;
	scaleOffset = scl;
	speed = spd;
	if (horz == 0)
		horizontalAdjustment = 1;
	else
		horizontalAdjustment = horz;
}

PulsatingJammer::~PulsatingJammer() {
	times.clear();
}


void PulsatingJammer::initialize(vector<LetterJam>& jams, LPVOID pvoid) {

	times.clear();

	Vector2* pos = (Vector2*) (pvoid);
	if (pos)
		position = *pos;
	else
		position = Vector2::Zero;

	float min = 1.5;
	float max = 2;
	mt19937 rng;
	rng.seed(random_device{}());
	uniform_real_distribution<float> rand(min, max);
	uniform_real_distribution<float> randTime(-1, 1);

	int nudgeBy = 0;
	for (int i = 0; i < jams.size(); ++i) {
		float num = rand(rng);
		jams[i].position.x += nudgeBy / horizontalAdjustment;
		int oldWidth = jams[i].getWidth();
		jams[i].scale = Vector2(num, num);
		int newWidth = jams[i].getWidth();
		nudgeBy += newWidth - oldWidth;
		times.push_back(randTime(rng));
	}
}


bool PulsatingJammer::run(double deltaTime, vector<LetterJam>& jams) {

	int nudgeBy = 0;
	for (int i = 0; i < jams.size(); ++i) {
		times[i] += deltaTime;

		jams[i].position.x += nudgeBy / horizontalAdjustment;
		int oldWidth = jams[i].getWidth();
		float scl = scaleOffset + amplitude*sin(speed * times[i]);
		jams[i].scale = Vector2(scl, scl);
		int newWidth = jams[i].getWidth();
		nudgeBy += newWidth - oldWidth;
	}
	return false;
}


ColorJammer::ColorJammer(float sped) {
	speedChange = sped;
}

ColorJammer::~ColorJammer() {
	times.clear();
	startColors.clear();
	states.clear();
}

void ColorJammer::initialize(vector<LetterJam>& jams, LPVOID pvoid) {

	times.clear();
	states.clear();
	startColors.clear();

	int min = State::Red;
	int max = State::BlueReverse;
	rng.seed(random_device{}());
	randState = uniform_int_distribution<mt19937::result_type>(min, max);
	uniform_real_distribution<float> randColor(0, 1);

	for (int i = 0; i < jams.size(); ++i) {
		State newState = State(randState(rng));
		Color newColor = Color(randColor(rng), randColor(rng), randColor(rng), 1);
		states.push_back(newState);
		startColors.push_back(newColor);
		times.push_back(0);
	}
}

bool ColorJammer::run(double deltaTime, vector<LetterJam>& jams) {

	for (int i = 0; i < jams.size(); ++i) {
		times[i] += deltaTime;
		Color endColor = jams[i].tint;
		switch (states[i]) {
			case Red:
				endColor.R(1);
				break;
			case RedReverse:
				endColor.R(0);
				break;
			case Green:
				endColor.G(1);
				break;
			case GreenReverse:
				endColor.G(0);
				break;
			case Blue:
				endColor.B(1);
				break;
			case BlueReverse:
				endColor.B(0);
				break;
		}
		jams[i].tint = Color::Lerp(startColors[i], endColor,
			times[i] / speedChange);
		if (times[i] >= speedChange) {
			times[i] = 0;
			State newState = State(randState(rng));
			while (newState == states[i])
				newState = State(randState(rng));
			states[i] = newState;
			startColors[i] = jams[i].tint;
		}
	}
	return false;
}



RPGDamageJammer::RPGDamageJammer(int fallD, int speed, float fade) {
	fallDistance = fallD;
	fallSpeed = speed;
	fadeOutTime = fade;
}

const double START_DELAY = .125;
void RPGDamageJammer::initialize(vector<LetterJam>& jams, LPVOID pvoid) {

	times.clear();

	basePosition = jams[0].position.y;
	startTint = jams[0].tint;
	endTint = startTint;
	endTint.w = 0;
	double timer = 0;
	for (int i = 0; i < jams.size(); ++i) {
		times.push_back(timer);
		timer -= START_DELAY;
		jams[i].tint.w = 0;
	}
}

const double FADE_TIME = 1.0;
bool RPGDamageJammer::run(double deltaTime, vector<LetterJam>& jams) {


	for (int i = 0; i < jams.size(); ++i) {
		times[i] += deltaTime;
		if (times[i] > 0) {

			if (times[i] >= FADE_TIME * 2) {
				jams[i].tint = Color::Lerp(startTint, endTint,
					times[i] / (FADE_TIME + fadeOutTime));
				if (i == jams.size() - 1 && jams[jams.size() - 1].tint.w < 0)
					return true;
			} else if (times[i] >= FADE_TIME) {
				jams[i].position.y = basePosition;
			} else {
				jams[i].tint.w = 1;
				jams[i].position.y = (basePosition - fallDistance)
					+ (fallDistance / times[i])*sin(-fallSpeed * times[i]);
			}
		}
	}
	return jams.size() == 0;
}