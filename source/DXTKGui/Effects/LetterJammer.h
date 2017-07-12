#pragma once
#include "..\Controls\TextLabel.h"
#include <random>

class LetterJam {
public:

	int getWidth() {
		return (sourceRECT.right - sourceRECT.left) * scale.x;
	}
	Vector2 position;
	RECT sourceRECT;
	Color tint = Color(1, 1, 1, 1);
	float rotation = 0.0;
	Vector2 origin = Vector2(0, 0);
	Vector2 scale = Vector2(1, 1);
	SpriteEffects spriteEffects = SpriteEffects_None;
	float layerDepth = 1;

};

interface JammerEffect {
public:
	virtual void initialize(vector<LetterJam>& letterJams, LPVOID pvoid) = 0;
	/** Return true when effect is done. */
	virtual bool run(double deltaTime, vector<LetterJam>& letterJams) = 0;
};

/** Funky dragon to double helix, then back again */
class DragonJammer : public JammerEffect {
public:
	DragonJammer(int amplitude, float speed);
	~DragonJammer();
	/* PVOID must be a Vector2. */
	virtual void initialize(vector<LetterJam>& letterJams, LPVOID pvoid);
	virtual bool run(double deltaTime, vector<LetterJam>& letterJams) override;

private:
	Vector2 position;
	int amplitude;
	float speed;
	vector<double> times;
};

class PulsatingJammer : public JammerEffect {
public:
	PulsatingJammer(float amplitude = .125, float scaleOffset = 1,
		float speed = 1, int horizontalAdjustment = 2);
	~PulsatingJammer();
	virtual void initialize(vector<LetterJam>& letterJams, LPVOID pvoid);
	virtual bool run(double deltaTime, vector<LetterJam>& letterJams) override;

	Vector2 position;
	float amplitude;
	float scaleOffset;
	float speed;
	int horizontalAdjustment;
	vector<double> times;
};

class ColorJammer : public JammerEffect {
public:
	ColorJammer(float changeSpeed);
	~ColorJammer();
	virtual void initialize(vector<LetterJam>& letterJams, LPVOID pvoid) override;
	virtual bool run(double deltaTime, vector<LetterJam>& letterJams) override;

private:
	mt19937 rng;
	uniform_int_distribution<mt19937::result_type> randState;

	float speedChange;
	enum State {
		Red, Green, Blue, RedReverse, GreenReverse, BlueReverse
	};
	vector<State> states;
	vector<Color> startColors;
	vector<double> times;
};

/** Simulates an RPG damage effect. 
	int fallDistance, int fallSpeed, float fadeOutTime */
class RPGDamageJammer : public JammerEffect {
public:
	RPGDamageJammer(int fallDistance, int fallSpeed, float fadeOutTime);
	virtual void initialize(vector<LetterJam>& letterJams, LPVOID pvoid) override;
	virtual bool run(double deltaTime, vector<LetterJam>& letterJams) override;

private:
	bool jamDone;
	int basePosition;
	Color startTint;
	Color endTint;

	int fallDistance;
	int fallSpeed;
	float fadeOutTime;

	vector<double> times;
};

class LetterJammer : public TextLabel {
public:
	LetterJammer(GUIFactory* factory, shared_ptr<MouseController> mouseController,
		Vector2 position, wstring text, const pugi::char_t* font = "Default Font");
	virtual ~LetterJammer();

	virtual bool update(double deltaTime) override;
	virtual void draw(SpriteBatch* batch) override;

	void setEffect(unique_ptr<JammerEffect> effect);
	void clearEffects();
	virtual void setText(wstring text) override;

	bool isDone();
	void reset();
private:

	vector<unique_ptr<JammerEffect>> jammerFXs;
	vector<LetterJam> letterJams;
	ComPtr<ID3D11ShaderResourceView> textTexture;
	unique_ptr<GraphicsAsset> gfxAsset;

	bool done;
};