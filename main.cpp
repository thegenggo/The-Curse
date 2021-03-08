#include <vector>
#include <stack>
#include <map>
#include <iostream>
#include <sstream>
#include <functional>
#include<iterator>


#include "SFML/Graphics.hpp"
#include "SFML/System.hpp"
#include "SFML/Window.hpp"
#include "SFML/Audio.hpp"
#include "SFML/Network.hpp"

using namespace std;
using namespace sf;

string currentState = "MainMenu";
string openWorldState = "Game";
string battleState = "Battle";

class ChatDialog
{
	RectangleShape shape;
	Text text;
	Font font;

public:
	ChatDialog(wstring text)
	{
		this->shape.setSize(Vector2f(1900, 200));
		this->text.setCharacterSize(30);
		this->shape.setOrigin(1900 / 2.f, 200 / 2.f);
		this->font.loadFromFile("Fonts/2005_iannnnnJPG.ttf");
		this->text.setFont(this->font);
		this->text.setString(text);
		this->shape.setFillColor(Color(70, 70, 70, 200));
		this->text.setFillColor(Color::White);
		this->shape.setPosition(960, 980);
		this->text.setPosition(30, 900);
	}

	~ChatDialog()
	{

	}

	void update()
	{

	}

	void render(RenderTarget* target)
	{
		target->draw(this->shape);
		target->draw(this->text);
	}
};

class ShowDamage
{
	Text text;
	Font font;
	float time;

public:
	const bool isEnd() const
	{
		if (time > 2.f)
			return true;
		return false;
	}

	ShowDamage(Vector2f position, int damage)
	{
		this->font.loadFromFile("Fonts/2005_iannnnnJPG.ttf");
		this->text.setFont(this->font);
		this->text.setFillColor(Color::Red);
		this->text.setCharacterSize(40);
		this->text.setString(to_string(damage));
		this->text.setPosition(position);
		this->time = 0;
	}

	ShowDamage(Vector2f position, string text)
	{
		this->font.loadFromFile("Fonts/2005_iannnnnJPG.ttf");
		this->text.setFont(this->font);
		this->text.setFillColor(Color::Red);
		this->text.setCharacterSize(40);
		this->text.setString(text);
		this->text.setPosition(position);
		this->time = 0;
	}

	void update(const float& dt)
	{
		this->time += dt;
		this->text.move(0.f, -1.f);
	}

	void render(RenderTarget& target)
	{
		target.draw(this->text);
	}
};

class Bar
{
	RectangleShape maxBar;
	RectangleShape currentBar;

public:
	Bar(float x, float y, float width, float height, Color color = Color::Green)
	{
		this->maxBar.setSize(Vector2f(width, height));
		this->maxBar.setFillColor(Color::Red);
		this->maxBar.setOutlineThickness(5.f);
		this->maxBar.setOutlineColor(Color::Black);
		this->maxBar.setPosition(x, y);
		this->currentBar.setSize(Vector2f(width, height));
		this->currentBar.setFillColor(color);
		this->currentBar.setPosition(x, y);
	}

	void update(int current, int max)
	{
		this->currentBar.setSize(Vector2f(((float)current / max) * maxBar.getSize().x, 10));
	}

	void render(RenderTarget& target)
	{
		target.draw(this->maxBar);
		target.draw(this->currentBar);
	}
};

class HitboxComponent
{
	//Variables
	Sprite& sprite;
	RectangleShape hitbox;
	float offsetX;
	float offsetY;

public:
	//Constructor / Destructor
	HitboxComponent(Sprite& sprite, float offset_x, float offset_y, float width, float height)
		:sprite(sprite), offsetX(offset_x), offsetY(offset_y)
	{
		this->hitbox.setPosition(this->sprite.getPosition().x + offset_x, this->sprite.getPosition().y + offset_y);
		this->hitbox.setSize(Vector2f(width, height));
		this->hitbox.setFillColor(Color::Transparent);
		this->hitbox.setOutlineThickness(1.f);
		this->hitbox.setOutlineColor(Color::Black);
	}

	//Accessors
	const FloatRect& getGlobalBounds() const
	{
		return this->hitbox.getGlobalBounds();
	}

	const RectangleShape& getHitbox() const
	{
		return this->hitbox;
	}

	//Functions
	const bool& checkIntersect(const FloatRect& frect) const
	{
		return this->hitbox.getGlobalBounds().intersects(frect);
	}

	void update()
	{
		this->hitbox.setPosition(this->sprite.getPosition().x + this->offsetX, this->sprite.getPosition().y + this->offsetY);
	}

	void render(RenderTarget& target)
	{
		target.draw(this->hitbox);
	}
};

enum class movementState { IDLE, MOVING_UP, MOVING_LEFT, MOVING_DOWN, MOVING_RIGHT };

class MovementComponent
{
	//Variables
	Sprite& sprite;
	Vector2f velocity;

	float maxVelocity;
	movementState state;

public:
	//Constructor / Destructor
	MovementComponent(Sprite& sprite, float maxVelocity)
		: sprite(sprite), velocity(Vector2f(0, 0)), maxVelocity(maxVelocity), state(movementState::IDLE)
	{

	}

	//Accessors
	const movementState getMovementState() const
	{
		return this->state;
	}

	const Vector2f& getVelocity() const
	{
		return this->velocity;
	}

	const int getState() const
	{
		if (this->velocity.x == 0.f && this->velocity.y == 0.f)
			switch (state)
			{
			case movementState::MOVING_UP:
				return 1;
				break;
			case movementState::MOVING_LEFT:
				return 2;
				break;
			case movementState::MOVING_DOWN:
				return 3;
				break;
			case movementState::MOVING_RIGHT:
				return 4;
				break;
			default:
				break;
			}
		else if (this->velocity.y < 0)
			return 5;
		else if (this->velocity.x < 0)
			return 6;
		else if (this->velocity.y > 0)
			return 7;
		else if (this->velocity.x > 0)
			return 8;

		return 0;
	}

	//Functions
	void update()
	{
		this->sprite.move(velocity);
		if (this->velocity.y < 0)
			this->state = movementState::MOVING_UP;
		else if (this->velocity.x < 0)
			this->state = movementState::MOVING_LEFT;
		else if (this->velocity.y > 0)
			this->state = movementState::MOVING_DOWN;
		else if (this->velocity.x > 0)
			this->state = movementState::MOVING_RIGHT;
	}

	void move(const float dir_x, const float dir_y, const float& dt)
	{
		this->velocity = Vector2f(dir_x * maxVelocity * dt, dir_y * maxVelocity * dt);
	}
};

class AnimationComponent
{
public:
	class Animation
	{
		//Variables
		Sprite& sprite;
		Texture& textureSheet;
		float animationTimer;
		float timer;
		int width;
		int height;
		bool end;
		vector<IntRect> rect;

	public:

		Animation(Sprite& sprite, Texture& textureSheet, float animation_timer, int start_frame_x, int start_frame_y, int frames_x, int frames_y, int width, int height)
			: sprite(sprite), textureSheet(textureSheet), animationTimer(animation_timer / (frames_x + 1 - start_frame_x)), width(width), height(height)
		{
			this->end = false;
			this->timer = 0.f;
			for (int i = 0; i < frames_x - start_frame_x + 1; i++)
				rect.push_back(IntRect(start_frame_x * width + i * width, start_frame_y * height, width, height));
			this->sprite.setTexture(this->textureSheet);
			this->sprite.setTextureRect(IntRect(start_frame_x * width, start_frame_y * height, width, height));
		}

		//Functions
		void play(const float& dt)
		{
			//Update timer
			this->timer += dt / this->animationTimer;
			size_t n = rect.size();
			if (this->timer >= n) {
				this->timer -= n; this->end = 1;
			}
			if (n > 0) sprite.setTextureRect(rect[int(this->timer)]);
		}

		//Accessors
		bool isEnd()
		{
			return this->end;
		}

		void reset()
		{
			this->timer = 0.f;
		}
	};
	//Variables
	map<string, Animation* > animations;
	Sprite& sprite;
	Texture& textureSheet;

	//Constructor / Destructor
	AnimationComponent(Sprite& sprite, Texture& texture_sheet)
		:sprite(sprite), textureSheet(texture_sheet)
	{

	}

	virtual ~AnimationComponent()
	{
		for (auto& i : this->animations)
		{
			delete i.second;
		}
	}

	//Functions
	void addAnimation(string key, float animation_timer, int start_frame_x, int start_frame_y, int frames_x, int frames_y, int width, int height)
	{
		this->animations[key] = new Animation(this->sprite, this->textureSheet, animation_timer, start_frame_x, start_frame_y, frames_x, frames_y, width, height);
	}

	void deleteAnimation(const string key)
	{
		this->animations.erase(key);
	}

	void play(const string key, const float& dt)
	{
		this->animations[key]->play(dt);
	}

	bool isEnd(const string key)
	{
		return this->animations[key]->isEnd();
	}

	void reset()
	{
		for (auto& i : this->animations)
		{
			i.second->reset();
		}
	}
};

class Entity
{
protected:
	Sprite sprite;
	Texture texture;
	string name;
	Text textDamage;
	Font font;
	int lvl;
	int hp;
	int max_hp;
	int att;
	int def;

	float time;

	AnimationComponent* animationComponent;
public:
	//Variables

	//Initilizer functions
	virtual void initVariables()
	{
		this->font.loadFromFile("Fonts/2005_iannnnnJPG.ttf");
		this->textDamage.setCharacterSize(40);
		this->textDamage.setFillColor(Color::Red);
		this->textDamage.setFont(font);
		this->time = 0.f;
	}

	//Constructor / Destructor
	Entity()
	{
		this->initVariables();
		this->createAnimationComponent(this->texture);
	}

	virtual ~Entity()
	{
		delete this->animationComponent;
	}

	//Accessors
	const FloatRect& getGlobalBounds() const
	{
		return sprite.getGlobalBounds();
	}

	virtual const Vector2f& getPosition() const
	{
		return this->sprite.getPosition();
	}
	//Component functions

	void setTexture(Texture& texture)
	{
		this->texture = texture;
		this->sprite.setTexture(texture);
	}

	virtual void createAnimationComponent(Texture& texture_sheet)
	{
		this->animationComponent = new AnimationComponent(this->sprite, texture_sheet);
	}

	//Functions
	int attack(Entity& opp) {
		return opp.beAttacked(att + rand() % 10);
	}

	int beAttacked(int oppatk) {
		int dmg = 0;
		if (oppatk > this->def) {
			dmg = oppatk - this->def;
		}
		this->hp -= dmg;
		if (this->hp <= 0) this->hp = 0;
		return dmg;
	}

	void showDamage(const float& dt)
	{
		if (textDamage.getString() != "\0")
		{
			time += dt;
			if (time >= 1.f)
			{
				textDamage.setString("\0");
				time = 0;
			}
		}
	}

	virtual void update(const float& dt)
	{

	}

	virtual void render(RenderTarget& target)
	{
		target.draw(this->sprite);
	}
};

class Enemy : public Entity
{
	Bar* hpBar;

	int droprate;
	int dropnum;
	int exp;

	bool pressed;

public:
	const FloatRect& getGlobalBounds() const
	{
		return this->sprite.getGlobalBounds();
	}

	const Vector2f& getPosition() const
	{
		return this->sprite.getPosition();
	}

	const int& getExp() const
	{
		return this->exp;
	}

	const bool& isPressed() const
	{
		return pressed;
	}

	const bool& isdead() const
	{
		if (hp <= 0) return true;
		return false;
	}

	Enemy(string name, float x, float y)
	{
		this->pressed = false;
		this->name = name;
		if (this->name == "Karakasa")
		{
			this->texture.loadFromFile("Images/on_battle_stage/monmap1_karakasa.png");
			this->hp = 400;
			this->max_hp = 400;
			this->att = 150;
			this->def = 10;
			this->lvl = 1;
			this->exp = 500;

			this->droprate = 30;
			this->dropnum = 1;
		}
		else if (this->name == "Hitotsume")
		{
			this->texture.loadFromFile("Images/on_battle_stage/monmap1_hitotsume.png");
			this->hp = 400;
			this->max_hp = 400;
			this->att = 150;
			this->def = 10;
			this->lvl = 1;
			this->exp = 500;

			this->droprate = 30;
			this->dropnum = 1;
		}
		else if (this->name == "Boss Oni")
		{
			this->texture.loadFromFile("Images/on_battle_stage/monmap1_hitotsume.png");
			this->hp = 400;
			this->max_hp = 400;
			this->att = 150;
			this->def = 10;
			this->lvl = 1;
			this->exp = 500;

			this->droprate = 30;
			this->dropnum = 1;
		}
		else if (this->name == "Amikiri")
		{
			this->texture.loadFromFile("Images/on_battle_stage/monmap2_amikiri.png");
			this->hp = 400;
			this->max_hp = 400;
			this->att = 150;
			this->def = 10;
			this->lvl = 1;
			this->exp = 500;

			this->droprate = 30;
			this->dropnum = 1;
		}
		else if (this->name == "Kappa")
		{
			this->texture.loadFromFile("Images/on_battle_stage/monmap2_kappa.png");
			this->hp = 400;
			this->max_hp = 400;
			this->att = 150;
			this->def = 10;
			this->lvl = 1;
			this->exp = 500;

			this->droprate = 30;
			this->dropnum = 1;
		}
		else if (this->name == "Boss Umibozu")
		{
			this->texture.loadFromFile("Images/on_battle_stage/monmap2_boss_umibozu.png");
			this->hp = 400;
			this->max_hp = 400;
			this->att = 150;
			this->def = 10;
			this->lvl = 1;
			this->exp = 500;

			this->droprate = 30;
			this->dropnum = 1;
		}
		else if (this->name == "Skeleton")
		{
			this->texture.loadFromFile("Images/on_battle_stage/monmap3_skeleton.png");
			this->hp = 400;
			this->max_hp = 400;
			this->att = 150;
			this->def = 10;
			this->lvl = 1;
			this->exp = 500;

			this->droprate = 30;
			this->dropnum = 1;
		}
		else if (this->name == "Wanyudo")
		{
			this->texture.loadFromFile("Images/on_battle_stage/monmap3_wanyudo.png");
			this->hp = 400;
			this->max_hp = 400;
			this->att = 150;
			this->def = 10;
			this->lvl = 1;
			this->exp = 500;

			this->droprate = 30;
			this->dropnum = 1;
		}
		else
		{
			this->texture.loadFromFile("Images/on_battle_stage/monmap3_skeleton.png");
			this->hp = 400;
			this->max_hp = 400;
			this->att = 150;
			this->def = 10;
			this->lvl = 1;
			this->exp = 500;

			this->droprate = 30;
			this->dropnum = 1;
		}
		this->sprite.setTexture(this->texture);
		this->textDamage.setPosition(x + this->sprite.getGlobalBounds().width / 2, y - 50);
		this->sprite.setPosition(x, y);
		this->hpBar = new Bar(sprite.getPosition().x, sprite.getPosition().y - 10.f, sprite.getGlobalBounds().width, 10);

	}

	~Enemy()
	{
		delete hpBar;
	}

	void updateEvent(const Event& event, const Vector2f mousePos)
	{
		if (event.type == Event::MouseButtonPressed)
		{
			if (event.mouseButton.button == Mouse::Left && this->sprite.getGlobalBounds().contains(mousePos)) pressed = true;
		}
	}

	void update(const float& dt)
	{
		this->hpBar->update(this->hp, this->max_hp);
		this->showDamage(dt);
		this->pressed = false;
	}

	void render(RenderTarget& target)
	{
		this->hpBar->render(target);
		target.draw(this->sprite);
		target.draw(this->textDamage);
	}
};

class TargetCursor
{
	Sprite sprite;
	Texture texture;

public:
	void setPosition(Enemy* enemy)
	{
		this->sprite.setPosition(enemy->getPosition().x + enemy->getGlobalBounds().width / 2 - this->sprite.getGlobalBounds().width / 2, enemy->getPosition().y - 150.f);
	}

	TargetCursor()
	{
		this->texture.loadFromFile("Images/targetcursor.png");
		this->sprite.setTexture(this->texture);
		this->sprite.setPosition(0.f, 0.f);
	}

	void render(RenderTarget& target)
	{
		target.draw(this->sprite);
	}
};

class Player : public Entity
{
	//Variables
	Sprite openWorldSprite;
	Sprite battleSprite;
	Sprite light;
	Texture lightTexture;
	Texture openWorldTexture;
	Texture battleTexture;

	Bar* hpBar;
	Bar* staminaBar;
	bool mode; // 0 = OpenWorld, 1 = Battle
	bool rendered;

	//Stat
	int stamina;
	int max_stamina;
	int current_exp;
	int max_exp;

	//Text
	Text hpText;
	Text spText;
	Text attText;
	Text defText;
	Text lvlText;

	HitboxComponent* hitboxComponent;
	MovementComponent* movementComponent;
	AnimationComponent* battleAnimationComponent;
	AnimationComponent* openWorldanimationComponent;

public:
	//Accessors
	const movementState getMovementState() const
	{
		return this->movementComponent->getMovementState();
	}

	const RectangleShape& getHitbox() const
	{
		return this->hitboxComponent->getHitbox();
	}

	const FloatRect& getSpriteGlobalBounds() const
	{
		return this->openWorldSprite.getGlobalBounds();
	}

	const Sprite& getSprite() const
	{
		return this->openWorldSprite;
	}

	const bool& isRendered() const
	{
		return this->rendered;
	}

	//Constructor / Destructor
	Player()
	{
		this->hitboxComponent = NULL;
		this->movementComponent = NULL;
		this->openWorldanimationComponent = NULL;

		this->openWorldTexture.loadFromFile("Images/Sprites/Player/test.png");
		this->battleTexture.loadFromFile("Images/Sprites/Player/on_battle.png");

		this->battleSprite.setPosition(1400.f, 280.f);
		this->setOrigin(40.f, 10.f);
		this->setPosition(20.f, 145.f);

		this->hitboxComponent = new HitboxComponent(this->openWorldSprite, 0.f, 75.f, 45.f, 30.f);
		this->openWorldanimationComponent = new AnimationComponent(this->openWorldSprite, this->openWorldTexture);
		this->battleAnimationComponent = new AnimationComponent(this->battleSprite, this->battleTexture);
		this->movementComponent = new MovementComponent(this->openWorldSprite, 300.f);

		this->lightTexture.loadFromFile("Images/light.png");
		this->light.setTexture(lightTexture);
		this->light.setOrigin(this->light.getGlobalBounds().width / 2, this->light.getGlobalBounds().height / 2);
		this->light.setColor(Color::Transparent);

		this->openWorldanimationComponent->addAnimation("IDLE", 0.5f, 0, 0, 0, 0, 125, 125);
		this->openWorldanimationComponent->addAnimation("IDLE_UP", 0.5f, 1, 1, 1, 1, 125, 125);
		this->openWorldanimationComponent->addAnimation("IDLE_LEFT", 0.5f, 1, 2, 1, 2, 125, 125);
		this->openWorldanimationComponent->addAnimation("IDLE_DOWN", 0.5f, 1, 3, 1, 3, 125, 125);
		this->openWorldanimationComponent->addAnimation("IDLE_RIGHT", 0.5f, 1, 4, 1, 4, 125, 125);
		this->openWorldanimationComponent->addAnimation("UP", 0.5f, 0, 1, 3, 1, 125, 125);
		this->openWorldanimationComponent->addAnimation("LEFT", 0.5f, 0, 2, 3, 2, 125, 125);
		this->openWorldanimationComponent->addAnimation("DOWN", 0.5f, 0, 3, 3, 3, 125, 125);
		this->openWorldanimationComponent->addAnimation("RIGHT", 0.5f, 0, 4, 3, 4, 125, 125);
		this->battleAnimationComponent->addAnimation("IDLE", 0.5f, 0, 0, 0, 0, 240, 320);

		this->hp = 1000;
		this->max_hp = 1000;
		this->att = 150;
		this->def = 10;
		this->stamina = 100;
		this->max_stamina = 100;
		this->lvl = 1;
		this->current_exp = 0;
		this->max_exp = 500;

		this->mode = 0;

		this->hpBar = new Bar(1070, 880, 400, 10);
		this->staminaBar = new Bar(1070, 930, 200, 10, Color::Yellow);

		this->textDamage.setPosition(this->battleSprite.getPosition().x + 125, this->battleSprite.getPosition().y - 50);

		this->hpText.setFont(this->font);
		this->spText.setFont(this->font);
		this->attText.setFont(this->font);
		this->defText.setFont(this->font);
		this->lvlText.setFont(this->font);
		this->hpText.setCharacterSize(40);
		this->spText.setCharacterSize(40);
		this->attText.setCharacterSize(40);
		this->defText.setCharacterSize(40);
		this->lvlText.setCharacterSize(40);
		this->hpText.setPosition(1070, 830);
		this->spText.setPosition(1070, 880);
		this->attText.setPosition(1500, 880);
		this->defText.setPosition(1500, 930);
		this->lvlText.setPosition(1500, 830);
		this->hpText.setFillColor(Color::Black);
		this->spText.setFillColor(Color::Black);
		this->attText.setFillColor(Color::Black);
		this->defText.setFillColor(Color::Black);
		this->lvlText.setFillColor(Color::Black);

		this->rendered = false;
	}

	virtual ~Player()
	{
		delete this->hpBar;
		delete this->staminaBar;
	}

	//Component functions
	const FloatRect& getGlobalBounds() const
	{
		return this->hitboxComponent->getGlobalBounds();
	}

	const Vector2f& getPosition() const
	{
		if (this->mode) this->battleSprite.getPosition();
		else
			return this->openWorldSprite.getPosition();
	}

	virtual void setScale(const float x, const float y)
	{
		this->openWorldSprite.setScale(x, y);
	}

	virtual void setOrigin(const float x, const float y)
	{
		this->openWorldSprite.setOrigin(x, y);
	}

	virtual void setPosition(const float x, const float y)
	{
		this->openWorldSprite.setPosition(x, y);
	}

	void setMode(bool x)
	{
		this->mode = x;
	}

	void setLight(bool x)
	{
		if (x)
			this->light.setColor(Color::Black);
		else
			this->light.setColor(Color::Transparent);
	}

	virtual void move(const float dir_x, const float dir_y, const float& dt)
	{
		if (this->movementComponent)
		{
			this->movementComponent->move(dir_x, dir_y, dt); //Sets velocity
		}
	}

	void levelUp()
	{
		this->current_exp -= max_exp;
		this->lvl++;
		this->max_hp += 30;
		this->att += 10;
		this->def += 5;
		this->max_stamina += 10;
		this->hp = max_hp;
		this->stamina = max_stamina;
	}

	void expUp(int exp)
	{
		current_exp += exp;
	}

	bool useSkill1(vector<Enemy*>& enemies, vector<ShowDamage*>& showDamages)
	{
		if (stamina >= 30)
		{
			stamina -= 30;
			for (auto i : enemies)
			{
				showDamages.push_back(new ShowDamage(Vector2f(i->getPosition().x + 30, i->getPosition().y - 40), i->beAttacked(att / 2)));
			}
			return true;
		}
		else
		{
			showDamages.push_back(new ShowDamage(Vector2f(this->getPosition().x + 30, this->getPosition().y - 40), "Mp is not enough"));
			return false;
		}
		return false;
	}

	bool useSkill2(Enemy*& enemy, vector<ShowDamage*>& showDamages)
	{
		if (stamina >= 30)
		{
			stamina -= 30;
			showDamages.push_back(new ShowDamage(Vector2f(enemy->getPosition().x + 30, enemy->getPosition().y - 40), enemy->beAttacked(att * 3 / 2)));
			return true;
		}
		else
		{
			showDamages.push_back(new ShowDamage(Vector2f(this->getPosition().x + 30, this->getPosition().y - 40), "Mp is not enough"));
			return false;
		}
	}

	void openUpdate(const float& dt)
	{
		this->movementComponent->update();

		switch (this->movementComponent->getState())
		{
		case 1:
			this->openWorldanimationComponent->play("IDLE_UP", dt);
			this->openWorldanimationComponent->reset();
			break;
		case 2:
			this->openWorldanimationComponent->play("IDLE_LEFT", dt);
			this->openWorldanimationComponent->reset();
			break;
		case 3:
			this->openWorldanimationComponent->play("IDLE_DOWN", dt);
			this->openWorldanimationComponent->reset();
			break;
		case 4:
			this->openWorldanimationComponent->play("IDLE_RIGHT", dt);
			this->openWorldanimationComponent->reset();
			break;
		case 5:
			this->openWorldanimationComponent->play("UP", dt);
			break;
		case 6:
			this->openWorldanimationComponent->play("LEFT", dt);
			break;
		case 7:
			this->openWorldanimationComponent->play("DOWN", dt);
			break;
		case 8:
			this->openWorldanimationComponent->play("RIGHT", dt);
			break;
		default:
			this->openWorldanimationComponent->play("IDLE", dt);
			break;
		}

		this->hitboxComponent->update();
		this->light.setPosition(this->openWorldSprite.getPosition().x + this->getGlobalBounds().width / 2, this->openWorldSprite.getPosition().y + this->getGlobalBounds().height / 2);

		this->rendered = false;
	}

	void battleUpdate(const float& dt)
	{
		this->battleAnimationComponent->play("IDLE", dt);
		this->hpBar->update(this->hp, this->max_hp);
		this->showDamage(dt);
		this->hpText.setString("HP: " + to_string(this->hp));
		this->spText.setString("MP: " + to_string(this->stamina));
		this->attText.setString("ATK: " + to_string(this->att));
		this->defText.setString("DEF: " + to_string(this->def));
		this->lvlText.setString("LV: " + to_string(this->lvl));
	}

	void update(const float& dt)
	{
		if (!mode) this->openUpdate(dt);
		else this->battleUpdate(dt);
		if (this->current_exp >= this->max_exp)
			this->levelUp();
	}

	void openWorldRender(RenderTarget& target)
	{
		target.draw(this->openWorldSprite);
		target.draw(this->light);
		this->rendered = true;
	}

	void battleRender(RenderTarget& target)
	{
		target.draw(this->battleSprite);
		this->hpBar->render(target);
		this->staminaBar->render(target);
		target.draw(this->textDamage);
		target.draw(this->hpText);
		target.draw(this->spText);
		target.draw(this->attText);
		target.draw(this->defText);
		target.draw(this->lvlText);
	}

	void render(RenderTarget& target)
	{
		if (!mode) this->openWorldRender(target);
		else this->battleRender(target);
	}
};

class CollisionBox
{
	//Variables
	RectangleShape shape;
	Player& player;
	RectangleShape up;
	RectangleShape left;
	RectangleShape down;
	RectangleShape right;

public:
	//Accessors
	const RectangleShape& getShape() const
	{
		return this->shape;
	}

	//Constructor / Destructor
	CollisionBox(Player& player, float x, float y, float width, float height)
		:player(player)
	{
		this->shape.setPosition(x, y);
		this->shape.setSize(Vector2f(width, height));
		this->shape.setFillColor(Color::Black);
		this->shape.setOutlineThickness(1.f);
		this->shape.setOutlineColor(Color::Black);
		this->up.setFillColor(Color::Red);
		this->up.setPosition(x, y);
		this->up.setSize(Vector2f(width, 1.f));
		this->left.setFillColor(Color::Blue);
		this->left.setPosition(x, y);
		this->left.setSize(Vector2f(1.f, height));
		this->down.setFillColor(Color::Green);
		this->down.setPosition(x, y + height);
		this->down.setSize(Vector2f(width, 1.f));
		this->right.setFillColor(Color::Magenta);
		this->right.setPosition(x + width, y);
		this->right.setSize(Vector2f(1.f, height));
	}

	//Functions
	void update()
	{
		if (this->player.getHitbox().getGlobalBounds().intersects(this->up.getGlobalBounds()) && this->player.getMovementState() == movementState::MOVING_DOWN)
		{
			this->player.setPosition(this->player.getPosition().x, this->shape.getGlobalBounds().top - this->player.getHitbox().getGlobalBounds().height - 75.f);
		}
		else if (this->player.getHitbox().getGlobalBounds().intersects(this->left.getGlobalBounds()) && this->player.getMovementState() == movementState::MOVING_RIGHT)
		{
			this->player.setPosition(this->shape.getGlobalBounds().left - this->player.getHitbox().getGlobalBounds().width, this->player.getPosition().y);
		}
		else if (this->player.getHitbox().getGlobalBounds().intersects(this->down.getGlobalBounds()) && this->player.getMovementState() == movementState::MOVING_UP)
		{
			this->player.setPosition(this->player.getPosition().x, this->shape.getGlobalBounds().top + this->shape.getGlobalBounds().height - 75.f);
		}
		else if (this->player.getHitbox().getGlobalBounds().intersects(this->right.getGlobalBounds()) && this->player.getMovementState() == movementState::MOVING_LEFT)
		{
			this->player.setPosition(this->shape.getGlobalBounds().left + this->shape.getGlobalBounds().width, this->player.getPosition().y);
		}
	}

	void render(RenderTarget& target)
	{
		target.draw(this->shape);
	}
};

class State
{
protected:
	//Variables
	RenderWindow* window;
	map<string, State*>* states;
	map<string, int> keybinds;
	Font font;
	bool quit;

	Vector2i mousePosScreen;
	Vector2i mousePosWindow;
	Vector2f mousePosView;

	//Resources
	map<string, Texture> textures;

	void initFonts()
	{
		if (!this->font.loadFromFile("Fonts/2005_iannnnnJPG.ttf"))
			throw("ERROR::MAINMENUSTATE::COULD NOT LOAD FONT");
	}

public:

	//Constructor / Destructor
	State(RenderWindow* window, map<string, State*>* states)
	{
		this->window = window;
		this->states = states;
		this->quit = false;
	}

	//Accessors
	const bool& getQuit() const
	{
		return this->quit;
	}

	//Functions
	void endState()
	{
		this->quit = true;
	}

	virtual void updateMousePositions()
	{
		this->mousePosScreen = Mouse::getPosition();
		this->mousePosWindow = Mouse::getPosition(*this->window);
		this->mousePosView = this->window->mapPixelToCoords(Mouse::getPosition(*this->window));
	}

	virtual void updateInput(const float& dt)
	{

	}

	virtual void updateEvent(const Event& event)
	{

	}

	virtual void update(const float& dt)
	{

	}

	virtual void render(RenderTarget* target = NULL)
	{

	}
};

enum class button_states { BTN_IDLE, BTN_HOVER, BTN_ACTIVE };

class Button
{
	button_states buttonState;

	RectangleShape shape;
	Font* font;
	Text text;

	Color textIdleColor;
	Color textHoverColor;
	Color textActiveColor;

	Color idleColor;
	Color hoverColor;
	Color activeColor;

	bool pressed;

public:

	Button(float x, float y, float width, float height, Font* font, string text, unsigned character_size, Color text_idle_color, Color text_hover_color, Color text_active_color,
		Color idle_color, Color hover_color, Color active_color)
	{
		this->buttonState = button_states::BTN_IDLE;

		this->shape.setPosition(Vector2f(x, y));
		this->shape.setSize(Vector2f(width, height));
		this->shape.setFillColor(idle_color);

		this->font = font;
		this->text.setFont(*this->font);
		this->text.setString(text);
		this->text.setFillColor(text_idle_color);
		this->text.setCharacterSize(character_size);
		this->text.setPosition(
			this->shape.getPosition().x + this->shape.getGlobalBounds().width / 2.f - this->text.getGlobalBounds().width / 2.f,
			this->shape.getPosition().y + this->shape.getGlobalBounds().height / 2.f - this->text.getCharacterSize() / 2.f);

		this->textIdleColor = text_idle_color;
		this->textHoverColor = text_hover_color;
		this->textActiveColor = text_active_color;

		this->idleColor = idle_color;
		this->hoverColor = hover_color;
		this->activeColor = active_color;

		this->pressed = false;
	}

	//Accessors
	const bool isPressed() const
	{
		return pressed;
	}

	//check click
	void updateEvent(const Event& event, const Vector2f mousePos)
	{
		if (event.type == Event::MouseButtonPressed)
		{
			if (event.mouseButton.button == Mouse::Left && this->shape.getGlobalBounds().contains(mousePos)) pressed = true;
		}
	}

	//Functions
	void update(const Vector2f mousePos)
	{
		//Update the booleans for hover and pressed

		//Idle
		this->buttonState = button_states::BTN_IDLE;

		//Hover
		if (this->shape.getGlobalBounds().contains(mousePos))
		{
			this->buttonState = button_states::BTN_HOVER;

			if (Mouse::isButtonPressed(Mouse::Left))
			{
				this->buttonState = button_states::BTN_ACTIVE;
			}
		}

		pressed = false;

		switch (this->buttonState)
		{
		case button_states::BTN_IDLE:
			this->shape.setFillColor(this->idleColor);
			this->text.setFillColor(this->textIdleColor);
			break;

		case button_states::BTN_HOVER:
			this->shape.setFillColor(this->hoverColor);
			this->text.setFillColor(this->textHoverColor);
			break;

		case button_states::BTN_ACTIVE:
			this->shape.setFillColor(this->activeColor);
			this->text.setFillColor(this->textActiveColor);
			break;

		default:
			this->shape.setFillColor(Color::Red);
			this->text.setFillColor(Color::Blue);
			break;
		}
	}

	void render(RenderTarget& target)
	{
		target.draw(this->shape);
		target.draw(this->text);
	}
};

class ImageButton
{
	Texture texture;
	Sprite sprite;
	bool pressed;

public:
	ImageButton(float x, float y, string textureLocation)
	{
		this->pressed = false;
		this->texture.loadFromFile(textureLocation);
		this->sprite.setTexture(this->texture);
		this->sprite.setPosition(x, y);
	}

	bool isPressed()
	{
		return pressed;
	}

	void updateEvent(const Event& event, const Vector2f mousePos)
	{
		if (event.type == Event::MouseButtonPressed)
		{
			if (event.mouseButton.button == Mouse::Left && this->sprite.getGlobalBounds().contains(mousePos)) this->pressed = true;
		}
	}

	void update()
	{
		this->pressed = false;
	}

	void render(RenderTarget& target)
	{
		target.draw(this->sprite);
	}
};

class BattleState : public State
{
	Texture bgtexture;
	RectangleShape background;

	//target cursor
	Entity* targetentity;

	//button
	map<string, ImageButton*> Mainbuttons;
	map<string, Button*> Itembuttons, Skillbuttons;

	//entity player&ene
	Player* player;
	vector<Enemy*> enemy;
	Enemy* target;
	TargetCursor* targetCursor;
	vector<ShowDamage*> showDamages;

	//item window
	bool isItemWindowActive;
	bool isSkillWindowActive;
	bool isPlayerturn;

	float time;

	int maxRound;
	int stage;

public:
	//variables

	//round
	/*int round = 0;
	int roundlimit;
	bool isroundclear = false;*/

	//stage 
	/*bool  isstageclear = false;
	int stagelevel;
	bool isbossstage;*/

	//turn 
	/*bool isPlayerturn;
	bool isEnemystop;*/


	//iniilizer function

	//set default target
	/*void initVariables(int round, int lvl, bool isboss)
	{
		this->isPlayerturn = true;
		this->roundlimit = round;
		this->stagelevel = lvl;
		this->isbossstage = isboss;
	}*/

	void initBackground()
	{
		if (!this->bgtexture.loadFromFile("Images/battle_scene/map1.png"))
		{
			throw "ERROR::MAIN_MENU_STATE::FAILED_TO_LOAD_BACKGROUND_TEXTURE";
		}

		this->background.setSize(Vector2f(this->window->getView().getSize()));
		this->background.setTexture(&this->bgtexture);
	}

	void initButtons()
	{
		this->Mainbuttons["Attack"] = new ImageButton(45, 837, "Images/Buttons/ButtonAttack.png");
		this->Mainbuttons["Skill"] = new ImageButton(289, 837, "Images/Buttons/ButtonSkill.png");
		this->Mainbuttons["Items"] = new ImageButton(531, 837, "Images/Buttons/ButtonItems.png");
		this->Mainbuttons["Flee"] = new ImageButton(777, 837, "Images/Buttons/ButtonFlee.png");

		this->Itembuttons["ITEM1"] = new Button(560, 400, 150, 100, &this->font, "Item 1", 50, Color::White, Color::White, Color::White,
			Color(70, 70, 70, 200), Color(150, 150, 150, 255), Color(20, 20, 20, 200));
		this->Itembuttons["ITEM2"] = new Button(560, 500, 150, 100, &this->font, "Item 2", 50, Color::White, Color::White, Color::White,
			Color(70, 70, 70, 200), Color(150, 150, 150, 255), Color(20, 20, 20, 200));
		this->Itembuttons["ITEM3"] = new Button(560, 600, 150, 100, &this->font, "Item 3", 50, Color::White, Color::White, Color::White,
			Color(70, 70, 70, 200), Color(150, 150, 150, 255), Color(20, 20, 20, 200));
		this->Itembuttons["ITEM4"] = new Button(560, 700, 150, 100, &this->font, "Item 4", 50, Color::White, Color::White, Color::White,
			Color(70, 70, 70, 200), Color(150, 150, 150, 255), Color(20, 20, 20, 200));

		this->Skillbuttons["SKILL1"] = new Button(320, 600, 150, 100, &this->font, "Skill 1", 50, Color::White, Color::White, Color::White,
			Color(70, 70, 70, 200), Color(150, 150, 150, 255), Color(20, 20, 20, 200));
		this->Skillbuttons["SKILL2"] = new Button(320, 700, 150, 100, &this->font, "Skill 2", 50, Color::White, Color::White, Color::White,
			Color(70, 70, 70, 200), Color(150, 150, 150, 255), Color(20, 20, 20, 200));
	}

	virtual void initEnemy()
	{
		this->enemy.push_back(new Enemy("Karakasa", 400, 200));
		this->enemy.push_back(new Enemy("Karakasa", 600, 400));
		this->enemy.push_back(new Enemy("Karakasa", 300, 500));
	}

	void deleteButtons()
	{
		for (auto it = this->Mainbuttons.begin(); it != this->Mainbuttons.end(); ++it)
		{
			delete it->second;
		}
		for (auto it = this->Itembuttons.begin(); it != this->Itembuttons.end(); ++it)
		{
			delete it->second;
		}
		for (auto it = this->Skillbuttons.begin(); it != this->Itembuttons.end(); ++it)
		{
			delete it->second;
		}
	}

	//Constructor / Destructor
	BattleState(RenderWindow* window, map<string, State*>* states, Player* player, int Maxround = 3, int Stagelevel = 1, bool Isbossstage = false)
		: State(window, states), player(player), maxRound(Maxround)
	{
		/*this->initVariables(Maxround, Stagelevel, Isbossstage);*/
		this->initBackground();
		this->initFonts();
		this->initButtons();
		this->isItemWindowActive = false;
		this->isSkillWindowActive = false;
		this->isPlayerturn = true;
		this->initEnemy();
		this->target = enemy[0];
		this->targetCursor = new TargetCursor();
		this->time = 0;
	}

	virtual ~BattleState()
	{
		this->deleteButtons();
		for (auto& it : enemy)
		{
			delete it;
		}
		delete targetCursor;
	}

	//Functions
	void updateButtons()
	{
		if (isPlayerturn)
		{
			//Attack
			if (this->Mainbuttons["Attack"]->isPressed())
			{
				this->showDamages.push_back(new ShowDamage(Vector2f(this->target->getPosition().x + 30, this->target->getPosition().y - 40), this->player->attack(*this->target)));
				this->isPlayerturn = false;
			}

			//Skill
			if (this->Mainbuttons["Skill"]->isPressed())
			{
				if (!this->isSkillWindowActive)
					this->isSkillWindowActive = true;
				else this->isSkillWindowActive = false;
			}

			if (this->Skillbuttons["SKILL1"]->isPressed())
			{
				if (this->player->useSkill1(this->enemy, this->showDamages))
					this->isPlayerturn = false;
			}

			if (this->Skillbuttons["SKILL2"]->isPressed())
			{
				if (this->player->useSkill2(this->target, this->showDamages))
					this->isPlayerturn = false;
			}

			//Item 
			if (this->Mainbuttons["Items"]->isPressed())
			{
				if (!this->isItemWindowActive)
					this->isItemWindowActive = true;
				else this->isItemWindowActive = false;
			}

			//Quit the game
			if (this->Mainbuttons["Flee"]->isPressed())
			{
				currentState = openWorldState; // back to game state
				this->player->setMode(false);
			}
		}
		/*Updates all the buttons in the state and handles their functionlaity.*/
		for (auto& it : Mainbuttons)
		{
			it.second->update();
		}

		if (isItemWindowActive)
			for (auto& it : Itembuttons)
			{
				it.second->update(this->mousePosView);
			}

		if (isSkillWindowActive)
			for (auto& it : Skillbuttons)
			{
				it.second->update(this->mousePosView);
			}
	}

	void updateEvent(const Event& event)
	{
		for (auto& i : Mainbuttons)
		{
			i.second->updateEvent(event, this->mousePosView);
		}

		if (isItemWindowActive)
			for (auto& it : Itembuttons)
			{
				it.second->updateEvent(event, this->mousePosView);
			}

		if (isSkillWindowActive)
			for (auto& it : Skillbuttons)
			{
				it.second->updateEvent(event, this->mousePosView);
			}

		for (auto& i : enemy)
		{
			i->updateEvent(event, this->mousePosView);
		}
	}

	void enemyTurn()
	{
		int dmg = 0;
		for (auto& i : enemy)
		{
			dmg += i->attack(*this->player);
		}
		if (!enemy.empty())
			this->showDamages.push_back(new ShowDamage(Vector2f(this->player->getPosition().x + 150, this->player->getPosition().y - 20), dmg));
	}

	void updateTarget()
	{
		for (auto& i : enemy)
		{
			if (i->isPressed() && !isSkillWindowActive && !isItemWindowActive)
			{
				this->target = i;
			}
		}
	}

	void update(const float& dt)
	{
		//enviornment update zone
		this->updateButtons();
		this->updateMousePositions();
		this->updateTarget();
		//player update zone
		this->updateInput(dt);

		this->player->update(dt);

		//enemy update zome
		for (auto& i : enemy)
		{
			i->update(dt);
		}

		for (auto it = this->enemy.begin(); it != this->enemy.end(); it++)
		{
			if ((*it)->isdead())
			{
				this->player->expUp((*it)->getExp());
				this->enemy.erase(it);
				it--;
			}
		}

		for (auto it = this->showDamages.begin(); it != this->showDamages.end(); it++)
		{
			if ((*it)->isEnd())
			{
				this->showDamages.erase(it);
				it--;
			}
		}

		for (auto& i : showDamages)
		{
			i->update(dt);
		}

		if (!isPlayerturn)
			this->enemyTurn();

		if (enemy.empty())
		{
			this->enemy.push_back(new Enemy("Hitotsume", 350, 100));
			this->enemy.push_back(new Enemy("Hitotsume", 200, 350));
		}

		if (this->target->isdead()) this->target = *this->enemy.begin();
		if (this->target)
			this->targetCursor->setPosition(this->target);

		this->isPlayerturn = true;
		time = 0;

		//update texture
	}

	void renderButtons(RenderTarget* target = NULL)
	{
		for (auto& it : Mainbuttons)
		{
			it.second->render(*target);
		}

		if (this->isItemWindowActive)
			for (auto& it : Itembuttons)
			{
				it.second->render(*target);
			}

		if (isSkillWindowActive)
			for (auto& it : Skillbuttons)
			{
				it.second->render(*target);
			}
	}

	void render(RenderTarget* target = NULL)
	{
		if (!target)
			target = this->window;

		//update environment zone
		target->draw(this->background);

		//render player zone
		this->player->render(*target);
		//render enemy zone
		for (auto& i : enemy)
			i->render(*target);

		//enemy target cursor
		this->targetCursor->render(*target);

		//render damage to enemy
		for (auto& i : showDamages)
			i->render(*target);

		//render button
		this->renderButtons(target);

		//show mouse position zone
		Text mouseText;
		mouseText.setPosition(this->mousePosView.x, this->mousePosView.y - 20);
		mouseText.setFont(this->font);
		mouseText.setCharacterSize(20);
		std::stringstream ss;
		ss << this->mousePosView.x << " " << this->mousePosView.y;
		mouseText.setString(ss.str());

		target->draw(mouseText);
	}
};

class Object
{
	Texture texture;
	CollisionBox* collisionBox;
	Player* player;
	Sprite sprite;

public:
	Object(Player* player, float positionX, float positionY)
		:player(player)
	{
		this->texture.loadFromFile("Images/map/map1/Tree.png");
		this->sprite.setTexture(this->texture);
		this->sprite.setPosition(positionX, positionY);
		this->collisionBox = new CollisionBox(*this->player, positionX + 65.f, positionY + 220.f, 45.f, 40.f);
	}

	virtual void update()
	{
		this->collisionBox->update();
	}

	virtual void render(RenderTarget& target)
	{
		if (this->player->getGlobalBounds().top < this->collisionBox->getShape().getGlobalBounds().top + this->collisionBox->getShape().getGlobalBounds().height / 2
			&& !this->player->isRendered())
			this->player->render(target);
		target.draw(this->sprite);
	}
};

class GameState : public State
{
	//Variables
	Player* player;
	Texture backgroundTexture;
	Texture environmentTexture;
	RectangleShape background;
	Sprite environment;
	vector<Object*> objects;

public:
	//Initilizer functions

	//Constructor / Destructor
	GameState(RenderWindow* window, map<string, State*>* states, Player* player)
		: State(window, states), player(player)
	{
		this->backgroundTexture.loadFromFile("Images/map/map1/map1_1.png");
		this->environmentTexture.loadFromFile("Images/map/map1/map1_1obj.png");
		this->background.setTexture(&this->backgroundTexture);
		this->background.setSize(Vector2f(this->window->getView().getSize()));
		this->environment.setTexture(environmentTexture);
		this->objects.push_back(new Object(this->player, 656.f, 110.f));
		this->objects.push_back(new Object(this->player, 958.f, 187.f));
		this->initFonts();
	}

	virtual ~GameState()
	{
		delete this->player;
		this->objects.clear();
	}

	//Functions
	void updateInput(const float& dt)
	{
		//Update player input
		if (Keyboard::isKeyPressed(Keyboard::W))
			this->player->move(0.f, -1.f, dt);
		else if (Keyboard::isKeyPressed(Keyboard::A))
			this->player->move(-1.f, 0.f, dt);
		else if (Keyboard::isKeyPressed(Keyboard::S))
			this->player->move(0.f, 1.f, dt);
		else if (Keyboard::isKeyPressed(Keyboard::D))
			this->player->move(1.f, 0.f, dt);
		else
			this->player->move(0.f, 0.f, dt);

		if (Keyboard::isKeyPressed(Keyboard::B))
		{
			this->states->insert_or_assign("Battle", new BattleState(this->window, this->states, this->player));
			currentState = "Battle";
			this->player->setMode(true);
		}

		if (Keyboard::isKeyPressed(Keyboard::Escape))
		{
			currentState = "MainMenu";
		}
	}

	void update(const float& dt)
	{
		this->updateMousePositions();
		this->updateInput(dt);

		if (this->player->getGlobalBounds().left + this->player->getGlobalBounds().width > this->window->getView().getSize().x + 5.f)
		{
			this->player->setPosition(5, this->player->getPosition().y);
			openWorldState = "Game2";
			currentState = "Game2";
		}

		if (this->player->getGlobalBounds().left < -5.f)
		{
			this->player->setPosition(0, this->player->getPosition().y);
			openWorldState = "Game";
			currentState = "Game";
		}

		if (this->player->getGlobalBounds().top + this->player->getGlobalBounds().height > this->window->getView().getSize().y)
		{
			this->player->setPosition(this->player->getPosition().x, this->window->getView().getSize().y - this->player->getGlobalBounds().height);
		}

		if (this->player->getGlobalBounds().top < 0)
		{
			this->player->setPosition(this->player->getPosition().x, 0);
		}

		this->player->update(dt);

		for (auto& i : objects)
		{
			i->update();
		}
	}

	void render(RenderTarget* target = NULL)
	{
		if (!target)
			target = this->window;

		target->draw(background);
		target->draw(environment);
		for (auto& i : objects)
		{
			i->render(*target);
		}

		if (!this->player->isRendered()) this->player->render(*target);

		Text mouseText;
		mouseText.setPosition(this->mousePosView.x, this->mousePosView.y - 20);
		mouseText.setFont(this->font);
		mouseText.setCharacterSize(20);
		stringstream ss;
		ss << this->mousePosView.x << " " << this->mousePosView.y;
		mouseText.setString(ss.str());
		target->draw(mouseText);
	}
};

class MainMenuState : public State
{
	//Variables
	Player* player;
	Texture backgroundTexture;
	RectangleShape background;

	map<string, Button*> buttons;

public:
	//Initializer functions
	void initBackground()
	{
		if (!this->backgroundTexture.loadFromFile("Images/Backgrounds/mainmanu.jpg"))
			throw "ERROR::MAIN_MENU_STATE::FAILED_TO_LOAD_BACKGROUND_TEXTURE";

		this->background.setSize(Vector2f(this->window->getView().getSize()));
		this->background.setTexture(&this->backgroundTexture);
	}

	void initButtons()
	{
		this->buttons["GAME_STATE"] = new Button(835, 500, 250, 50, &this->font, "เข้าสู่เกม", 50,
			Color(70, 70, 70, 200), Color(250, 250, 250, 250), Color(20, 20, 20, 50),
			Color(70, 70, 70, 0), Color(150, 150, 150, 0), Color(20, 20, 20, 0));

		this->buttons["BATTLE_STATE"] = new Button(835, 600, 250, 50, &this->font, "ฝึกต่อสู้", 50,
			Color(70, 70, 70, 200), Color(250, 250, 250, 250), Color(20, 20, 20, 50),
			Color(70, 70, 70, 0), Color(150, 150, 150, 0), Color(20, 20, 20, 0));

		this->buttons["EXIT_STATE"] = new Button(835, 700, 250, 50, &this->font, "ออกจากเกม", 50,
			Color(70, 70, 70, 200), Color(250, 250, 250, 250), Color(20, 20, 20, 50),
			Color(100, 100, 100, 0), Color(150, 150, 150, 0), Color(20, 20, 20, 0));
	}

	//Constructor / Destructor
	MainMenuState(RenderWindow* window, map<string, State*>* states, Player* player)
		: State(window, states), player(player)
	{
		this->initBackground();
		this->initFonts();
		this->initButtons();
	}

	virtual ~MainMenuState()
	{
		for (auto it = this->buttons.begin(); it != this->buttons.end(); ++it)
		{
			delete it->second;
		}
	}

	//Functions
	void updateButtons()
	{
		//New game
		if (this->buttons["GAME_STATE"]->isPressed())
		{
			this->states->emplace("Game", new GameState(this->window, this->states, this->player));
			currentState = openWorldState;
		}

		//Quit the game
		if (this->buttons["EXIT_STATE"]->isPressed())
		{
			this->window->close();
		}

		/*Updates all the buttons in the state and handles their functionlaity.*/
		for (auto& it : this->buttons)
		{
			it.second->update(this->mousePosView);
		}
	}

	void updateEvent(const Event& event)
	{
		for (auto i : buttons)
		{
			i.second->updateEvent(event, this->mousePosView);
		}
	}

	void update(const float& dt)
	{
		this->updateMousePositions();
		this->updateInput(dt);

		this->updateButtons();
	}

	void renderButtons(RenderTarget& target)
	{
		for (auto it : this->buttons)
		{
			it.second->render(target);
		}
	}

	void render(RenderTarget* target = NULL)
	{
		if (!target)
			target = this->window;

		target->draw(this->background);

		Text mouseText;
		mouseText.setPosition(this->mousePosView.x, this->mousePosView.y - 20);
		mouseText.setFont(this->font);
		mouseText.setCharacterSize(20);
		stringstream ss;
		ss << this->mousePosView.x << " " << this->mousePosView.y;
		mouseText.setString(ss.str());
		target->draw(mouseText);

		this->renderButtons(*target);
	}
};

int main()
{
	bool fullscreen = true;
	unsigned antialiasing_level = 0;
	float speed = 10;
	float dt = 0;

	Event event;
	Clock dtClock;
	Player* player;
	player = new Player();

	ContextSettings windowSettings;
	windowSettings.antialiasingLevel = antialiasing_level;

	RenderWindow* window;
	if (fullscreen)
		window = new RenderWindow(VideoMode::getDesktopMode(), "The Curse", Style::Fullscreen, windowSettings);
	else
		window = new RenderWindow(VideoMode(1280, 720), "The Curse", Style::Default, windowSettings);

	window->setView(View(FloatRect(0, 0, 1920, 1080)));
	window->setVerticalSyncEnabled(true);

	map<string, State*> states;
	states["MainMenu"] = new MainMenuState(window, &states, player);
	states.insert(pair<string, State*>("Game2", new GameState(window, &states, player)));
	states.insert(pair<string, State*>("Game", new GameState(window, &states, player)));

	while (window->isOpen())
	{
		//Update
		dt = dtClock.restart().asSeconds();

		while (window->pollEvent(event))
		{
			if (event.type == Event::Closed)
				window->close();
			states[currentState]->updateEvent(event);
		}

		if (!states.empty())
		{
			states[currentState]->update(dt);

			if (states[currentState]->getQuit())
			{
				states[currentState]->endState();
				delete states[currentState];
				states.erase(states.find(currentState));
			}
		}
		else
		{
			window->close();
		}

		//Render
		window->clear();

		if (!states.empty())
		{
			states[currentState]->render();
		}

		window->display();
	}

	delete window;

	while (!states.empty())
	{
		states.clear();
	}

	return 0;
}
