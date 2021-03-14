#include <vector>
#include <map>
#include <sstream>
#include <cmath>
#include <list>

#include "SFML/Graphics.hpp"
#include "SFML/System.hpp"
#include "SFML/Window.hpp"
#include "SFML/Audio.hpp"
#include "SFML/Network.hpp"

using namespace std;
using namespace sf;

string currentState = "MainMenu";
string openWorldState = "Map1_1";
string battleState = "Battle";
Font font;
map<string, Texture> enemyTextures;

class ChatDialog
{
	RectangleShape shape;
	Text text;

public:
	ChatDialog(wstring text)
	{
		this->shape.setSize(Vector2f(1900, 200));
		this->text.setCharacterSize(30);
		this->shape.setOrigin(1900 / 2.f, 200 / 2.f);

		this->text.setFont(font);
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

class ShowText
{
	Text text;
	float time;

public:
	const bool isEnd() const
	{
		if (time > 2.f)
			return true;
		return false;
	}

	ShowText(Vector2f position, int damage, Color color = Color::Red)
	{
		this->text.setFont(font);
		this->text.setFillColor(color);
		this->text.setOutlineColor(Color::Black);
		this->text.setOutlineThickness(3.f);
		this->text.setCharacterSize(40);
		this->text.setString(to_string(damage));
		this->text.setOrigin(this->text.getGlobalBounds().width / 2, this->text.getGlobalBounds().height / 2);
		this->text.setPosition(position);
		this->time = 0;
	}

	ShowText(Vector2f position, string text, int size = 40, Color color = Color::Red)
	{
		this->text.setFont(font);
		this->text.setFillColor(color);
		this->text.setOutlineColor(Color::Black);
		this->text.setOutlineThickness(3.f);
		this->text.setCharacterSize(size);
		this->text.setString(text);
		this->text.setOrigin(this->text.getGlobalBounds().width / 2, this->text.getCharacterSize() * 0.75f);
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
		this->hitbox.setFillColor(Color::White);
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
	Texture texture;
	string name;
	vector<ShowText*> showText;
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
	virtual const Vector2f& getPosition() const
	{
		return Vector2f();
	}

	virtual const FloatRect& getGlobalBounds() const
	{
		return FloatRect();
	}

	//Component functions

	void setTexture(Texture& texture)
	{
		this->texture = texture;
	}

	virtual void createAnimationComponent(Texture& texture_sheet)
	{

	}

	//Functions
	virtual void showTextPushBack(int dmg)
	{
		showText.push_back(new ShowText(Vector2f(this->getPosition().x + this->getGlobalBounds().width / 2, this->getPosition().y), dmg));
	}

	void attack(Entity& opp) {
		opp.beAttacked(att + rand() % 10);
	}

	void beAttacked(int oppatk) {
		int dmg = 0;
		if (oppatk > this->def) {
			dmg = oppatk - this->def;
		}
		this->hp -= dmg;
		if (this->hp <= 0) this->hp = 0;
		this->showTextPushBack(dmg);
	}

	virtual void showTextUpdate(const float& dt)
	{
		for (auto i = this->showText.begin(); i < this->showText.end(); i++)
		{
			(*i)->update(dt);
			if ((*i)->isEnd()) { this->showText.erase(i); i--; }
		}
	}

	virtual void update(const float& dt)
	{

	}

	virtual void showTextRender(RenderTarget& target)
	{
		for (auto& i : showText)
		{
			i->render(target);
		}
	}

	virtual void render(RenderTarget& target)
	{

	}
};

class Enemy : public Entity
{
	Sprite sprite;
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
		if (this->hp <= 0) return true;
		return false;
	}

	Enemy(string name, float x, float y)
	{
		this->pressed = false;
		this->name = name;
		if (this->name == "Karakasa")
		{
			this->texture.loadFromFile("Images/on_battle_stage/monmap1_karakasa.png");
			this->hp = 600;
			this->max_hp = 600;
			this->att = 80;
			this->def = 5;
			this->lvl = 1;
			this->exp = 500;

			this->droprate = 30;
			this->dropnum = 1;
		}
		else if (this->name == "Hitotsume")
		{
			this->texture.loadFromFile("Images/on_battle_stage/monmap1_hitotsume.png");
			this->hp = 600;
			this->max_hp = 600;
			this->att = 80;
			this->def = 10;
			this->lvl = 1;
			this->exp = 500;

			this->droprate = 30;
			this->dropnum = 1;
		}
		else if (this->name == "Boss Oni")
		{
			this->texture.loadFromFile("Images/on_battle_stage/monmap1_boss_oni.png");
			this->hp = 1000;
			this->max_hp = 1000;
			this->att = 150;
			this->def = 20;
			this->lvl = 1;
			this->exp = 500;

			this->droprate = 30;
			this->dropnum = 1;
		}
		else if (this->name == "Amikiri")
		{
			this->texture.loadFromFile("Images/on_battle_stage/monmap2_amikiri.png");
			this->hp = 750;
			this->max_hp = 750;
			this->att = 100;
			this->def = 15;
			this->lvl = 1;
			this->exp = 500;

			this->droprate = 30;
			this->dropnum = 1;
		}
		else if (this->name == "Kappa")
		{
			this->texture.loadFromFile("Images/on_battle_stage/monmap2_kappa.png");
			this->hp = 700;
			this->max_hp = 700;
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
			this->hp = 2000;
			this->max_hp = 2000;
			this->att = 200;
			this->def = 30;
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
			this->att = 300;
			this->def = 5;
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
		this->sprite.setPosition(x, y);
		this->hpBar = new Bar(this->sprite.getPosition().x, this->sprite.getPosition().y - 10.f, this->sprite.getGlobalBounds().width, 10);

	}

	~Enemy()
	{
		delete hpBar;
	}

	void showTextPushBack(int dmg)
	{
		showText.push_back(new ShowText(Vector2f(this->getPosition().x + this->getGlobalBounds().width / 2, this->getPosition().y - 50.f), dmg));
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
		this->showTextUpdate(dt);
		this->pressed = false;
	}

	void render(RenderTarget& target)
	{
		this->hpBar->render(target);
		this->showTextRender(target);
		target.draw(this->sprite);
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

	//Item
	int elixirs;
	int sacredWater;

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

	const FloatRect& getSpriteGlobalBounds() const
	{
		if (this->mode) return this->battleSprite.getGlobalBounds();
		else
			return this->openWorldSprite.getGlobalBounds();
	}

	const bool& isRendered() const
	{
		return this->rendered;
	}

	const Vector2f& getPosition() const
	{
		if (this->mode) return this->battleSprite.getPosition();
		else
			return this->openWorldSprite.getPosition();
	}

	const FloatRect& getHitboxGlobalBounds() const
	{
		return this->hitboxComponent->getGlobalBounds();
	}

	const bool& isdead() const
	{
		if (this->hp <= 0) return true;
		return false;
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
		this->setOrigin(40.f, 85.f);
		this->setPosition(20.f, 145.f);

		this->hitboxComponent = new HitboxComponent(this->openWorldSprite, 0.f, 0.f, 45.f, 30.f);
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

		this->hpText.setFont(font);
		this->spText.setFont(font);
		this->attText.setFont(font);
		this->defText.setFont(font);
		this->lvlText.setFont(font);
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

		this->elixirs = 3;
		this->sacredWater = 3;
	}

	virtual ~Player()
	{
		delete this->hpBar;
		delete this->staminaBar;
	}

	//Component functions

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

	//Functions
	void showTextPushBack(int dmg, Color color = Color::Red)
	{
		showText.push_back(new ShowText(Vector2f(this->getPosition().x + this->getSpriteGlobalBounds().width / 2.f, this->getPosition().y), dmg, color));
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
		this->max_exp += lvl * 300;
		this->lvl++;
		this->showText.push_back(new ShowText(this->getPosition(), "Level Up!!!!"));
		this->max_hp += 30;
		this->att += 10;
		this->def += 5;
		this->max_stamina += 10;
		this->hp = max_hp;
		this->stamina = max_stamina;
	}

	void expUp(int exp)
	{
		this->current_exp += exp;
	}

	bool useSkill1(vector<Enemy*>& enemies, vector<ShowText*>& showDamages)
	{
		if (stamina >= 30)
		{
			stamina -= 30;
			for (auto& i : enemies)
			{
				i->beAttacked(att / 2);
			}
			return true;
		}
		else
		{
			showText.push_back(new ShowText(this->getPosition(), "Mp is not enough"));
			return false;
		}
		return false;
	}

	bool useSkill2(Enemy*& enemy, vector<ShowText*>& showDamages)
	{
		if (stamina >= 30)
		{
			stamina -= 30;
			enemy->beAttacked(att * 2);
			return true;
		}
		else
		{
			showText.push_back(new ShowText(this->getPosition(), "Mp is not enough"));
			return false;
		}
	}

	void useItem(string name)
	{
		if (name == "Elixirs")
		{
			if (this->elixirs > 0)
			{
				this->elixirs--;
				int heal = this->max_hp * 3 / 4;
				if (heal + this->hp > this->max_hp) heal = this->max_hp - this->hp;
				this->hp += heal;
				this->showTextPushBack(heal, Color::Green);
			}
		}
		else if (name == "Sacred water")
		{
			if (this->sacredWater > 0)
			{
				this->sacredWater--;
				int heal = this->max_stamina * 3 / 4;
				if (heal + this->stamina > this->max_stamina) heal = this->max_stamina - this->stamina;
				this->stamina += heal;
				this->showTextPushBack(heal, Color::Blue);
			}
		}
	}

	void openWorldUpdate(const float& dt)
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
		this->staminaBar->update(this->stamina, this->max_stamina);
		this->hpText.setString("HP: " + to_string(this->hp));
		this->spText.setString("MP: " + to_string(this->stamina));
		this->attText.setString("ATK: " + to_string(this->att));
		this->defText.setString("DEF: " + to_string(this->def));
		this->lvlText.setString("LV: " + to_string(this->lvl));
	}

	void update(const float& dt)
	{
		if (!mode) this->openWorldUpdate(dt);
		else this->battleUpdate(dt);
		if (this->current_exp >= this->max_exp)
			this->levelUp();
		this->showTextUpdate(dt);
	}

	void openWorldRender(RenderTarget& target)
	{
		this->hitboxComponent->render(target);
		target.draw(this->openWorldSprite);
		target.draw(this->light);
		this->rendered = true;
	}

	void battleRender(RenderTarget& target)
	{
		target.draw(this->battleSprite);
		this->hpBar->render(target);
		this->staminaBar->render(target);
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
		this->showTextRender(target);
	}
};

class CollisionBox
{
	//Variables
	RectangleShape shape;
	Player& player;

public:
	//Accessors
	const RectangleShape& getShape() const
	{
		return this->shape;
	}

	//Constructor / Destructor
	CollisionBox(Player& player, float x, float y, float width = 1.f, float height = 1.f)
		:player(player)
	{

		this->shape.setSize(Vector2f(width, height));
		this->shape.setPosition(x, y);
	}

	~CollisionBox()
	{

	}

	//Functions
	void update(const float& dt)
	{
		if (this->shape.getGlobalBounds().intersects(this->player.getHitboxGlobalBounds()))
		{
			if (this->player.getMovementState() == movementState::MOVING_UP)
			{
				this->player.setPosition(this->player.getPosition().x, this->shape.getGlobalBounds().top + this->shape.getGlobalBounds().height + 1.f);
			}
			if (this->player.getMovementState() == movementState::MOVING_LEFT)
			{
				this->player.setPosition(this->shape.getGlobalBounds().left + this->shape.getGlobalBounds().width + 1.f, this->player.getPosition().y);
			}
			if (this->player.getMovementState() == movementState::MOVING_DOWN)
			{
				this->player.setPosition(this->player.getPosition().x, this->shape.getGlobalBounds().top - this->player.getHitboxGlobalBounds().height - 1.f);
			}
			if (this->player.getMovementState() == movementState::MOVING_RIGHT)
			{
				this->player.setPosition(this->shape.getGlobalBounds().left - this->player.getHitboxGlobalBounds().width - 1.f, this->player.getPosition().y);
			}
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
	bool quit;

	Vector2i mousePosScreen;
	Vector2i mousePosWindow;
	Vector2f mousePosView;

	//Resources
	map<string, Texture> textures;

public:

	//Constructor / Destructor
	State(RenderWindow* window, map<string, State*>* states)
	{
		this->window = window;
		this->states = states;
		this->quit = false;
	}

	virtual ~State()
	{

	}

	//Accessors
	const bool& getQuit() const
	{
		return this->quit;
	}

	//Functions
	virtual void endState()
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
	Text text;

	Color textIdleColor;
	Color textHoverColor;
	Color textActiveColor;

	Color idleColor;
	Color hoverColor;
	Color activeColor;

	bool pressed;

public:

	Button(float x, float y, float width, float height, string text, unsigned character_size, Color text_idle_color, Color text_hover_color, Color text_active_color,
		Color idle_color, Color hover_color, Color active_color)
	{
		this->buttonState = button_states::BTN_IDLE;

		this->shape.setPosition(Vector2f(x, y));
		this->shape.setSize(Vector2f(width, height));
		this->shape.setFillColor(idle_color);

		this->text.setFont(font);
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

	const bool isHovered() const
	{
		if (this->buttonState == button_states::BTN_HOVER) return true;
		return false;
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

		this->pressed = false;

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
	RectangleShape background;
	Texture bgtexture;
	Texture skillDetailTexture;
	Texture itemDetailTexture;

	//target cursor
	Entity* targetentity;

	//button
	map<string, ImageButton*> Mainbuttons;
	map<string, Button*> Itembuttons, Skillbuttons;
	map<string, Button*> detailButtons;

	//entity player&ene
	Player* player;
	Sprite skillDetail;
	Sprite itemDetail;
	Enemy* target;
	TargetCursor* targetCursor;
	ShowText* clear;
	vector<Enemy*> enemy;
	vector<ShowText*> showDamages;

	//item window
	bool isItemWindowActive;
	bool isSkillWindowActive;
	bool isPlayerturn;

	float time;

	int stage;
	int battleStage;

public:
	void initBackground()
	{
		if(this->battleStage == 1)
			this->bgtexture.loadFromFile("Images/battle_scene/map1.png");
		else if(this->battleStage == 2)
			this->bgtexture.loadFromFile("Images/battle_scene/map2.png");

		this->background.setSize(Vector2f(this->window->getView().getSize()));
		this->background.setTexture(&this->bgtexture);
	}

	void initButtons()
	{
		this->Mainbuttons["Attack"] = new ImageButton(45, 837, "Images/Buttons/ButtonAttack.png");
		this->Mainbuttons["Skill"] = new ImageButton(289, 837, "Images/Buttons/ButtonSkill.png");
		this->Mainbuttons["Items"] = new ImageButton(531, 837, "Images/Buttons/ButtonItems.png");
		this->Mainbuttons["Flee"] = new ImageButton(777, 837, "Images/Buttons/ButtonFlee.png");

		this->Itembuttons["ITEM1"] = new Button(530, 600, 220, 100, "Elixirs", 50, Color::White, Color::White, Color::White,
			Color(70, 70, 70, 200), Color(150, 150, 150, 255), Color(20, 20, 20, 200));
		this->Itembuttons["ITEM2"] = new Button(530, 700, 220, 100, "Sacred water", 50, Color::White, Color::White, Color::White,
			Color(70, 70, 70, 200), Color(150, 150, 150, 255), Color(20, 20, 20, 200));

		this->Skillbuttons["SKILL1"] = new Button(200, 600, 420, 100, "����Ǩѹ��� ���ǧ ���ä�", 50, Color::White, Color::White, Color::White,
			Color(70, 70, 70, 200), Color(150, 150, 150, 255), Color(20, 20, 20, 200));
		this->Skillbuttons["SKILL2"] = new Button(200, 700, 420, 100, "�����䤨Թ���", 50, Color::White, Color::White, Color::White,
			Color(70, 70, 70, 200), Color(150, 150, 150, 255), Color(20, 20, 20, 200));

		this->detailButtons["SkillDetail"] = new Button(1650, 850, 200, 100, "��������´ʡ��", 30, Color::White, Color::White, Color::White,
			Color(70, 70, 70, 200), Color(150, 150, 150, 255), Color(20, 20, 20, 200));
		this->detailButtons["ItemDetail"] = new Button(1650, 950, 200, 100, "��������´����", 30, Color::White, Color::White, Color::White,
			Color(70, 70, 70, 200), Color(150, 150, 150, 255), Color(20, 20, 20, 200));
	}

	virtual bool initEnemy()
	{
		if (this->battleStage == 1)
			if (this->stage == 1)
			{
				this->enemy.push_back(new Enemy("Karakasa", 400, 200));
				this->enemy.push_back(new Enemy("Karakasa", 600, 400));
				this->enemy.push_back(new Enemy("Karakasa", 300, 500));
				return true;
			}
			else if (this->stage == 2)
			{
				this->enemy.push_back(new Enemy("Hitotsume", 400, 200));
				this->enemy.push_back(new Enemy("Hitotsume", 150, 350));
				return true;
			}
			else if (this->stage == 3)
			{
				this->enemy.push_back(new Enemy("Boss Oni", 300, 200));
				return true;
			}
			else
			{
				return false;
			}
		else if (this->battleStage == 2)
			if (this->stage == 1)
			{
				this->enemy.push_back(new Enemy("Kappa", 400, 200));
				this->enemy.push_back(new Enemy("Kappa", 600, 400));
				this->enemy.push_back(new Enemy("Kappa", 300, 500));
				return true;
			}
			else if (this->stage == 2)
			{
				this->enemy.push_back(new Enemy("Amikiri", 400, 200));
				this->enemy.push_back(new Enemy("Amikiri", 150, 350));
				return true;
			}
			else if (this->stage == 3)
			{
				this->enemy.push_back(new Enemy("Boss Umibozu", 300, 200));
				return true;
			}
			else
			{
				return false;
			}
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
		for (auto it = this->Skillbuttons.begin(); it != this->Skillbuttons.end(); ++it)
		{
			delete it->second;
		}
		for (auto it = this->detailButtons.begin(); it != this->detailButtons.end(); ++it)
		{
			delete it->second;
		}
	}

	//Constructor / Destructor
	BattleState(RenderWindow* window, map<string, State*>* states, Player* player, int battleStage = 1)
		: State(window, states), player(player), battleStage(battleStage)
	{
		this->time = 0;
		this->stage = 1;
		this->initBackground();
		this->initButtons();
		this->initEnemy();
		this->target = enemy[0];
		this->isItemWindowActive = false;
		this->isSkillWindowActive = false;
		this->isPlayerturn = true;
		this->targetCursor = new TargetCursor();
		this->skillDetailTexture.loadFromFile("Images/skill window.png");
		this->itemDetailTexture.loadFromFile("Images/ItemwindowV2.png");
		this->skillDetail.setColor(Color::Transparent);
		this->itemDetail.setColor(Color::Transparent);
		this->skillDetail.setTexture(this->skillDetailTexture);
		this->itemDetail.setTexture(this->itemDetailTexture);
	}

	virtual ~BattleState()
	{
		this->deleteButtons();
		for (auto& it : enemy)
		{
			delete it;
		}
		delete targetCursor;
		delete clear;
	}

	//Functions
	void endState(int x = 0)
	{
		if (!this->clear && x == 1)
			this->clear = new ShowText(Vector2f(960.f, 540.f), "Stage Clear", 300, Color::White);
		else if (!this->clear && x == 0)
			this->clear = new ShowText(Vector2f(960.f, 540.f), "Run!!!", 250, Color::White);
	}

	void updateButtons()
	{
		if (this->isPlayerturn && this->target)
		{
			//Attack
			if (this->Mainbuttons["Attack"]->isPressed() && this->clear == nullptr)
			{
				this->player->attack(*this->target);
				this->isPlayerturn = false;
			}

			//Skill
			if (this->Mainbuttons["Skill"]->isPressed() && !this->isItemWindowActive && this->clear == nullptr)
			{
				if (!this->isSkillWindowActive)
					this->isSkillWindowActive = true;
				else this->isSkillWindowActive = false;
			}

			if (this->Skillbuttons["SKILL1"]->isPressed() && this->clear == nullptr)
			{
				if (this->player->useSkill1(this->enemy, this->showDamages))
					this->isPlayerturn = false;
			}

			if (this->Skillbuttons["SKILL2"]->isPressed() && this->clear == nullptr)
			{
				if (this->player->useSkill2(this->target, this->showDamages))
					this->isPlayerturn = false;
			}

			//Item 
			if (this->Mainbuttons["Items"]->isPressed() && !this->isSkillWindowActive && this->clear == nullptr)
			{
				if (!this->isItemWindowActive)
					this->isItemWindowActive = true;
				else this->isItemWindowActive = false;
			}
			if (this->Itembuttons["ITEM1"]->isPressed() && this->clear == nullptr)
			{
				this->player->useItem("Elixirs");
			}
			if (this->Itembuttons["ITEM2"]->isPressed() && this->clear == nullptr)
			{
				this->player->useItem("Sacred water");
			}

			//Quit the game
			if (this->Mainbuttons["Flee"]->isPressed())
			{
				this->endState();
			}
		}

		/*Updates all the buttons in the state and handles their functionlaity.*/
		for (auto& it : Mainbuttons)
		{
			it.second->update();
		}

		for (auto& it : detailButtons)
		{
			it.second->update(this->mousePosView);
		}

		if (this->isItemWindowActive)
			for (auto& it : Itembuttons)
			{
				it.second->update(this->mousePosView);
			}

		if (this->isSkillWindowActive)
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

		if (this->isItemWindowActive)
			for (auto& it : Itembuttons)
			{
				it.second->updateEvent(event, this->mousePosView);
			}

		if (this->isSkillWindowActive)
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
		for (auto& i : enemy)
		{
			i->attack(*this->player);
		}

	}

	void updateTarget()
	{
		for (auto& i : enemy)
		{
			if (i->isPressed() && !this->isSkillWindowActive && !this->isItemWindowActive)
			{
				this->target = i;
			}
		}
	}

	void update(const float& dt)
	{
		this->skillDetail.setPosition(this->mousePosView.x - this->skillDetail.getGlobalBounds().width,
			this->mousePosView.y - this->skillDetail.getGlobalBounds().height);
		this->itemDetail.setPosition(this->mousePosView.x - this->itemDetail.getGlobalBounds().width,
			this->mousePosView.y - this->itemDetail.getGlobalBounds().height);

		if (this->detailButtons["SkillDetail"]->isHovered())
			this->skillDetail.setColor(Color::White);
		else this->skillDetail.setColor(Color::Transparent);

		if (this->detailButtons["ItemDetail"]->isHovered())
			this->itemDetail.setColor(Color::White);
		else this->itemDetail.setColor(Color::Transparent);

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
				this->target = NULL;
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

		if (!this->isPlayerturn)
			this->enemyTurn();

		if (this->enemy.empty())
		{
			this->target = NULL;
			this->stage++;
			if (!this->initEnemy()) this->endState(1);
		}

		if (!this->target && !this->enemy.empty())
			this->target = *this->enemy.begin();

		if (this->target)
			this->targetCursor->setPosition(this->target);

		this->isPlayerturn = true;

		if (this->clear)
		{
			this->clear->update(dt);
			if (this->clear->isEnd())
			{
				currentState = openWorldState; // back to game state
				this->player->setMode(false);
				this->quit = true;
			}
		}

		if (this->player->isdead())
		{
			currentState = "MainMenu";
			this->states->erase(this->states->find("Map1_1"),this->states->end());
		}

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

		if (this->isSkillWindowActive)
			for (auto& it : Skillbuttons)
			{
				it.second->render(*target);
			}

		for (auto& it : detailButtons)
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

		//enemy target cursor
		if (this->target)
			this->targetCursor->render(*target);

		//render player zone
		this->player->render(*target);

		//render enemy zone
		for (auto& i : enemy)
			i->render(*target);

		//render damage to enemy

		//render button
		this->renderButtons(target);

		if (this->clear)
			this->clear->render(*target);

		target->draw(this->skillDetail);
		target->draw(this->itemDetail);

		//show mouse position zone
		Text mouseText;
		mouseText.setPosition(this->mousePosView.x, this->mousePosView.y - 20);
		mouseText.setFont(font);
		mouseText.setCharacterSize(20);
		std::stringstream ss;
		ss << this->mousePosView.x << " " << this->mousePosView.y;
		mouseText.setString(ss.str());

		target->draw(mouseText);
	}
};

class Object
{
protected:
	string name;
	Texture texture;
	CollisionBox* collisionBox;
	Player* player;
	Sprite sprite;

public:
	const float& getObjectPosition() const
	{
		return this->collisionBox->getShape().getGlobalBounds().top;
	}

	Object(string name, Player* player, float positionX, float positionY)
		:player(player), name(name)
	{
		this->collisionBox = NULL;
		if (this->name == "Rock")
		{
			this->texture.loadFromFile("Images/map/rock_type_1.png");
			this->sprite.setTexture(this->texture);
			this->collisionBox = new CollisionBox(*this->player, positionX + 17.5f, positionY, 60.f, 40.f);
		}
		else if (this->name == "Tree1")
		{
			this->texture.loadFromFile("Images/map/tree_type_1.png");
			this->sprite.setTexture(this->texture);
			this->collisionBox = new CollisionBox(*this->player, positionX + 70.f, positionY + 255.f, 55.f, 40.f);
		}
		else if (this->name == "Tree2")
		{
			this->texture.loadFromFile("Images/map/tree_type_2.png");
			this->sprite.setTexture(this->texture);
			this->collisionBox = new CollisionBox(*this->player, positionX + 65.f, positionY + 220.f, 45.f, 40.f);
		}
		else if (this->name == "Tree3")
		{
			this->texture.loadFromFile("Images/map/tree_type_3.png");
			this->sprite.setTexture(this->texture);
			this->collisionBox = new CollisionBox(*this->player, positionX, positionY, 37.f, 30.f);
		}
		else if (this->name == "Rock2")
		{
			this->texture.loadFromFile("Images/map/rock_type_2.png");
			this->sprite.setTexture(this->texture);
		}
		else if (this->name == "Rock3")
		{
			this->texture.loadFromFile("Images/map/rock_type_3.png");
			this->sprite.setTexture(this->texture);
		}
		else
		{
			this->texture.loadFromFile("Images/map/tree_type_1.png");
			this->sprite.setTexture(this->texture);
			this->collisionBox = new CollisionBox(*this->player, positionX + 65.f, positionY + 220.f, 45.f, 40.f);
		}
		this->sprite.setPosition(positionX, positionY);
	}

	virtual void update(const float& dt)
	{
		this->collisionBox->update(dt);
	}

	virtual void render(RenderTarget& target)
	{
		this->collisionBox->render(target);
		if (this->player->getHitboxGlobalBounds().top < this->collisionBox->getShape().getGlobalBounds().top && !this->player->isRendered())
			this->player->render(target);
		target.draw(this->sprite);
	}
};

void sortObject(vector<Object*>& objects)
{
	for (auto& i : objects)
	{
		for (auto& j : objects)
		{
			if (j->getObjectPosition() > i->getObjectPosition())
			{
				Object* temp = j;
				j = i;
				i = temp;
			}
		}
	}
}

class GameState : public State
{
	//Variables
	Player* player;
	Texture backgroundTexture;
	/*Texture environmentTexture;*/
	RectangleShape background;
	Sprite environment;
	vector<Object*> objects;
	vector<CollisionBox*> collisions;
	string textline;
	list<ChatDialog*> chat;

	int gameStage;

public:
	//Initilizer functions

	//Constructor / Destructor
	GameState(RenderWindow* window, map<string, State*>* states, Player* player, int gameStage = 11)
		: State(window, states), player(player), gameStage(gameStage)
	{
		if (this->gameStage == 11)
		{
			this->backgroundTexture.loadFromFile("Images/map/map1/map1_1.png");
			/*this->environmentTexture.loadFromFile("Images/map/map1/map1_1obj.png");*/
			this->objects.push_back(new Object("Tree1", this->player, 704.f, -172.f));
			this->objects.push_back(new Object("Tree1", this->player, 948.f, -5.f));
			this->objects.push_back(new Object("Tree1", this->player, 927.f, -163.f));
			this->objects.push_back(new Object("Tree1", this->player, 487.f, -179.f));
			this->objects.push_back(new Object("Tree1", this->player, 200.f, -164.f));
			this->objects.push_back(new Object("Tree1", this->player, 633.f, 760.f));
			this->objects.push_back(new Object("Tree1", this->player, 1567.f, 19.f));
			this->objects.push_back(new Object("Tree1", this->player, 185.f, 83.f));
			this->objects.push_back(new Object("Tree1", this->player, 375.f, 95.f));
			this->objects.push_back(new Object("Tree1", this->player, 17.f, 342.f));
			this->objects.push_back(new Object("Tree1", this->player, 224.f, 699.f));
			this->objects.push_back(new Object("Tree1", this->player, 1221.f, 150.f));
			this->objects.push_back(new Object("Tree1", this->player, 1521.f, 694.f));
			this->objects.push_back(new Object("Tree1", this->player, 1719.f, 608.f));
			this->objects.push_back(new Object("Tree2", this->player, 657.f, 111.f));
			this->objects.push_back(new Object("Tree2", this->player, 1252.f, -89.f));
			this->objects.push_back(new Object("Tree2", this->player, 30.f, -124.f));
			this->objects.push_back(new Object("Tree2", this->player, 47.f, 149.f));
			this->objects.push_back(new Object("Tree2", this->player, 958.f, 189.f));
			this->objects.push_back(new Object("Tree2", this->player, 1765.f, 219.f));
			this->objects.push_back(new Object("Tree2", this->player, 963.f, 484.f));
			this->objects.push_back(new Object("Tree2", this->player, 22.f, 631.f));
			this->objects.push_back(new Object("Tree2", this->player, 970.f, 637.f));
			this->objects.push_back(new Object("Tree2", this->player, 476.f, 700.f));
			this->objects.push_back(new Object("Tree2", this->player, 1154.f, 785.f));
			this->objects.push_back(new Object("Tree2", this->player, 970.f, 791.f));
			this->objects.push_back(new Object("Tree2", this->player, 1364.f, 799.f));
			this->objects.push_back(new Object("Tree3", this->player, 1151.f, 402.f));
			this->objects.push_back(new Object("Tree3", this->player, 1161.f, 109.f));
			this->objects.push_back(new Object("Tree3", this->player, 1061.f, 269.f));
			this->objects.push_back(new Object("Tree3", this->player, 1607.f, 416.f));
			this->objects.push_back(new Object("Tree3", this->player, 349.f, 335.f));
			this->objects.push_back(new Object("Tree3", this->player, 22.f, 326.f));
			this->objects.push_back(new Object("Tree3", this->player, 77.f, 984.f));
			this->objects.push_back(new Object("Tree3", this->player, 422.f, 1014.f));
			this->objects.push_back(new Object("Tree3", this->player, 1654.f, 1030.f));
			this->objects.push_back(new Object("Tree3", this->player, 1830.f, 945.f));
			this->objects.push_back(new Object("Tree3", this->player, 1838.f, 96.f));
			this->objects.push_back(new Object("Rock", this->player, 1802.f, 1011.f));
			this->objects.push_back(new Object("Rock", this->player, 1536.f, 1031.f));
			this->objects.push_back(new Object("Rock", this->player, 1687.f, 348.f));
			this->objects.push_back(new Object("Rock", this->player, 1131.f, 235.f));
			this->objects.push_back(new Object("Rock", this->player, 657.f, 90.f));
			this->objects.push_back(new Object("Rock", this->player, 393.f, 91.f));
			this->objects.push_back(new Object("Rock", this->player, 309.f, 379.f));
			this->objects.push_back(new Object("Rock", this->player, 147.f, 841.f));
			this->objects.push_back(new Object("Rock", this->player, 1516.f, 41.f));
			this->chat.push_back(new ChatDialog(L"ฮัลโหล"));
			this->chat.push_back(new ChatDialog(L"เทสๆ"));
		}
		else if (this->gameStage == 12)
		{
			this->backgroundTexture.loadFromFile("Images/map/map1/map1_2.png");
			/*this->environmentTexture.loadFromFile("Images/map/map1/map1_1obj.png");*/
			this->collisions.push_back(new CollisionBox(*this->player, 1434.f, 584.f, 1.f, 358.f));
			this->collisions.push_back(new CollisionBox(*this->player, 1634.f, 589.f, 1.f, 353.f));
			this->collisions.push_back(new CollisionBox(*this->player, 610.f, 0.f, 1.f, 295.f));
		}
		else if (this->gameStage == 13)
		{
			this->backgroundTexture.loadFromFile("Images/map/map1/map1_3.png");
		}
		else if (this->gameStage == 21)
		{
			this->backgroundTexture.loadFromFile("Images/map/map2/map2_1.png");
		}
		else if (this->gameStage == 22)
		{
			this->backgroundTexture.loadFromFile("Images/map/map2/map2_2.png");
		}
		else if (this->gameStage == 23)
		{
			this->backgroundTexture.loadFromFile("Images/map/map2/map2_3.png");
		}
		else if (this->gameStage == 31)
		{
			this->backgroundTexture.loadFromFile("Images/map/map3/map3_1.png");
		}
		else if (this->gameStage == 32)
		{
			this->backgroundTexture.loadFromFile("Images/map/map3/map3_2.png");
		}
		else if (this->gameStage == 33)
		{
			this->backgroundTexture.loadFromFile("Images/map/map3/map3_3.png");
		}


		this->background.setTexture(&this->backgroundTexture);
		/*this->environment.setTexture(environmentTexture);*/
		sortObject(objects);
		this->background.setSize(Vector2f(this->window->getView().getSize()));
	}

	virtual ~GameState()
	{
		for (auto& i : this->objects)
			delete i;

		for (auto& i : this->collisions)
			delete i;
	}

	//Functions
	void updateInput(const float& dt)
	{
		//Update player input
		if (this->chat.empty()) {
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
		}

		if (Keyboard::isKeyPressed(Keyboard::B))
		{
			this->states->insert_or_assign("Battle", new BattleState(this->window, this->states, this->player, 2));
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
		if (this->gameStage == 11)
		{
			//RIGHT
			if (this->player->getHitboxGlobalBounds().left + this->player->getHitboxGlobalBounds().width > this->window->getView().getSize().x &&
				!(this->player->getHitboxGlobalBounds().top > 480.f && this->player->getHitboxGlobalBounds().top + this->player->getHitboxGlobalBounds().height < 656.f))
			{
				this->player->setPosition(this->window->getView().getSize().x - this->player->getHitboxGlobalBounds().width, this->player->getPosition().y);
			}
			else if (this->player->getHitboxGlobalBounds().left > this->window->getView().getSize().x)
			{
				this->player->setPosition(0.f, this->player->getPosition().y);
				openWorldState = "Map1_2";
				currentState = "Map1_2";
			}

			//LEFT
			if (this->player->getHitboxGlobalBounds().left < 0.f)
			{
				this->player->setPosition(0.f, this->player->getPosition().y);
			}

			//DOWN
			if (this->player->getHitboxGlobalBounds().top + this->player->getHitboxGlobalBounds().height > this->window->getView().getSize().y &&
				!(this->player->getHitboxGlobalBounds().left > 835.f && this->player->getHitboxGlobalBounds().left + this->player->getHitboxGlobalBounds().width < 982.f))
			{
				this->player->setPosition(this->player->getPosition().x, this->window->getView().getSize().y - this->player->getHitboxGlobalBounds().height);
			}
			else if (this->player->getHitboxGlobalBounds().top > this->window->getView().getSize().y)
			{
				this->player->setPosition(this->player->getPosition().x, 0.f);
				openWorldState = "Map1_3";
				currentState = "Map1_3";
			}

			//UP
			if (this->player->getHitboxGlobalBounds().top < 0.f)
			{
				this->player->setPosition(this->player->getPosition().x, 0.f);
			}
		}
		else if (this->gameStage == 12)
		{
			//RIGHT
			if (this->player->getHitboxGlobalBounds().left + this->player->getHitboxGlobalBounds().width > this->window->getView().getSize().x)
			{
				this->player->setPosition(this->window->getView().getSize().x - this->player->getHitboxGlobalBounds().width, this->player->getPosition().y);
			}

			//LEFT
			if (this->player->getHitboxGlobalBounds().left < 0.f &&
				!(this->player->getHitboxGlobalBounds().top > 480.f && this->player->getHitboxGlobalBounds().top + this->player->getHitboxGlobalBounds().height < 656.f))
			{
				this->player->setPosition(0.f, this->player->getPosition().y);
			}
			else if (this->player->getHitboxGlobalBounds().left + this->player->getHitboxGlobalBounds().width < 0.f)
			{
				this->player->setPosition(this->window->getView().getSize().x - this->player->getHitboxGlobalBounds().width, this->player->getPosition().y);
				openWorldState = "Map1_1";
				currentState = "Map1_1";
			}

			//DOWN
			if (this->player->getHitboxGlobalBounds().top + this->player->getHitboxGlobalBounds().height > this->window->getView().getSize().y)
			{
				this->player->setPosition(this->player->getPosition().x, this->window->getView().getSize().y - this->player->getHitboxGlobalBounds().height);
			}

			//UP
			if (this->player->getHitboxGlobalBounds().top < 0.f)
			{
				this->player->setPosition(this->player->getPosition().x, 0.f);
			}
		}
		else if (this->gameStage == 13)
		{
			//RIGHT
			if (this->player->getHitboxGlobalBounds().left > this->window->getView().getSize().x)
			{
				this->player->setPosition(0.f, this->player->getPosition().y);
				openWorldState = "Map2_1";
				currentState = "Map2_1";
			}

			//LEFT
			if (this->player->getHitboxGlobalBounds().left < 0.f)
			{
				this->player->setPosition(0.f, this->player->getPosition().y);
			}

			//DOWN
			if (this->player->getHitboxGlobalBounds().top + this->player->getHitboxGlobalBounds().height > this->window->getView().getSize().y)
			{
				this->player->setPosition(this->player->getPosition().x, this->window->getView().getSize().y - this->player->getHitboxGlobalBounds().height);
			}

			//UP
			if (this->player->getHitboxGlobalBounds().top < 0.f &&
				!(this->player->getHitboxGlobalBounds().left > 835.f && this->player->getHitboxGlobalBounds().left + this->player->getHitboxGlobalBounds().width < 982.f))
			{
				this->player->setPosition(this->player->getPosition().x, 0.f);
			}
			else if (this->player->getHitboxGlobalBounds().top + this->player->getHitboxGlobalBounds().height < 0.f)
			{
				this->player->setPosition(this->player->getPosition().x, this->window->getView().getSize().y - this->player->getHitboxGlobalBounds().height);
				openWorldState = "Map1_1";
				currentState = "Map1_1";
			}
		}
		else if (this->gameStage == 21)
		{
			//RIGHT
			if (this->player->getHitboxGlobalBounds().left > this->window->getView().getSize().x)
			{
				this->player->setPosition(0.f, this->player->getPosition().y);
				openWorldState = "Map2_2";
				currentState = "Map2_2";
			}

			//LEFT
			if (this->player->getHitboxGlobalBounds().left + this->player->getHitboxGlobalBounds().left < 0.f)
			{
				this->player->setPosition(this->window->getView().getSize().x - this->player->getHitboxGlobalBounds().width, this->player->getPosition().y);
				openWorldState = "Map1_3";
				currentState = "Map1_3";
			}

			//DOWN
			if (this->player->getHitboxGlobalBounds().top + this->player->getHitboxGlobalBounds().height > this->window->getView().getSize().y)
			{
				this->player->setPosition(this->player->getPosition().x, this->window->getView().getSize().y - this->player->getHitboxGlobalBounds().height);
			}

			//UP
			if (this->player->getHitboxGlobalBounds().top < 0.f)
			{
				this->player->setPosition(this->player->getPosition().x, 0.f);
			}
		}
		else if (this->gameStage == 22)
		{
			//RIGHT
			if (this->player->getHitboxGlobalBounds().left + this->player->getHitboxGlobalBounds().width > this->window->getView().getSize().x)
			{
				this->player->setPosition(this->window->getView().getSize().x - this->player->getHitboxGlobalBounds().width, this->player->getPosition().y);
			}

			//LEFT
			if (this->player->getHitboxGlobalBounds().left + this->player->getHitboxGlobalBounds().width < 0.f)
			{
				this->player->setPosition(this->window->getView().getSize().x - this->player->getHitboxGlobalBounds().width, this->player->getPosition().y);
				openWorldState = "Map2_1";
				currentState = "Map2_1";
			}

			//DOWN
			if (this->player->getHitboxGlobalBounds().top + this->player->getHitboxGlobalBounds().height > this->window->getView().getSize().y)
			{
				this->player->setPosition(this->player->getPosition().x, this->window->getView().getSize().y - this->player->getHitboxGlobalBounds().height);
			}

			//UP
			if (this->player->getHitboxGlobalBounds().top + this->player->getHitboxGlobalBounds().height < 0.f)
			{
				this->player->setPosition(this->player->getPosition().x, this->window->getView().getSize().y - this->player->getHitboxGlobalBounds().height);
				openWorldState = "Map2_3";
				currentState = "Map2_3";
			}
		}
		else if (this->gameStage == 23)
		{
			//RIGHT
			if (this->player->getHitboxGlobalBounds().left > this->window->getView().getSize().x)
			{
				this->player->setPosition(0.f, this->player->getPosition().y);
				openWorldState = "Map3_1";
				currentState = "Map3_1";
			}

			//LEFT
			if (this->player->getHitboxGlobalBounds().left < 0.f)
			{
				this->player->setPosition(0.f, this->player->getPosition().y);
			}

			//DOWN
			if (this->player->getHitboxGlobalBounds().top > this->window->getView().getSize().y)
			{
				this->player->setPosition(this->player->getPosition().x, 0.f);
				openWorldState = "Map2_2";
				currentState = "Map2_2";
			}

			//UP
			if (this->player->getHitboxGlobalBounds().top < 0.f)
			{
				this->player->setPosition(this->player->getPosition().x, 0.f);
			}
		}
		else if (this->gameStage == 31)
		{
			//RIGHT
			if (this->player->getHitboxGlobalBounds().left > this->window->getView().getSize().x)
			{
				this->player->setPosition(0.f, this->player->getPosition().y);
				openWorldState = "Map3_2";
				currentState = "Map3_2";
			}

			//LEFT
			if (this->player->getHitboxGlobalBounds().left + this->player->getHitboxGlobalBounds().width < 0.f)
			{
				this->player->setPosition(this->window->getView().getSize().x - this->player->getHitboxGlobalBounds().width, this->player->getPosition().y);
				openWorldState = "Map2_3";
				currentState = "Map2_3";
			}

			//DOWN
			if (this->player->getHitboxGlobalBounds().top + this->player->getHitboxGlobalBounds().height > this->window->getView().getSize().y)
			{
				this->player->setPosition(this->player->getPosition().x, this->window->getView().getSize().y - this->player->getHitboxGlobalBounds().height);
			}

			//UP
			if (this->player->getHitboxGlobalBounds().top + this->player->getHitboxGlobalBounds().height < 0.f)
			{
				this->player->setPosition(this->player->getPosition().x, this->window->getView().getSize().y - this->player->getHitboxGlobalBounds().height);
				openWorldState = "Map3_3";
				currentState = "Map3_3";
			}
		}
		else if (this->gameStage == 32)
		{
			//RIGHT
			if (this->player->getHitboxGlobalBounds().left + this->player->getHitboxGlobalBounds().width > this->window->getView().getSize().x)
			{
				this->player->setPosition(this->window->getView().getSize().x - this->player->getHitboxGlobalBounds().width, this->player->getPosition().y);
			}

			//LEFT
			if (this->player->getHitboxGlobalBounds().left + this->player->getHitboxGlobalBounds().width < 0.f)
			{
				this->player->setPosition(this->window->getView().getSize().x - this->player->getHitboxGlobalBounds().width, this->player->getPosition().y);
				openWorldState = "Map3_1";
				currentState = "Map3_1";
			}

			//DOWN
			if (this->player->getHitboxGlobalBounds().top + this->player->getHitboxGlobalBounds().height > this->window->getView().getSize().y)
			{
				this->player->setPosition(this->player->getPosition().x, this->window->getView().getSize().y - this->player->getHitboxGlobalBounds().height);
			}

			//UP
			if (this->player->getHitboxGlobalBounds().top < 0.f)
			{
				this->player->setPosition(this->player->getPosition().x, 0.f);
			}
		}
		else if (this->gameStage == 33)
		{
			//RIGHT
			if (this->player->getHitboxGlobalBounds().left + this->player->getHitboxGlobalBounds().width > this->window->getView().getSize().x)
			{
				this->player->setPosition(this->window->getView().getSize().x - this->player->getHitboxGlobalBounds().width, this->player->getPosition().y);
			}

			//LEFT
			if (this->player->getHitboxGlobalBounds().left < 0.f)
			{
				this->player->setPosition(0.f, this->player->getPosition().y);
			}

			//DOWN
			if (this->player->getHitboxGlobalBounds().top > this->window->getView().getSize().y)
			{
				this->player->setPosition(this->player->getPosition().x, 0.f);
				openWorldState = "Map3_1";
				currentState = "Map3_1";
			}

			//UP
			if (this->player->getHitboxGlobalBounds().top < 0.f)
			{
				this->player->setPosition(this->player->getPosition().x, 0.f);
			}
		}

		this->player->update(dt);

		for (auto& i : objects)
		{
			i->update(dt);
		}

		for (auto& i : collisions)
		{
			i->update(dt);
		}


	}

	void render(RenderTarget* target = NULL)
	{
		if (!target)
			target = this->window;

		target->draw(background);
		//target->draw(environment);
		for (auto& i : objects)
		{
			i->render(*target);
		}
		for (auto& i : collisions)
		{
			i->render(*target);
		}

		if (!this->player->isRendered()) this->player->render(*target);

		if (!this->chat.empty())
			this->chat.front()->render(target);

		Text mouseText;
		mouseText.setPosition(this->mousePosView.x, this->mousePosView.y - 20);
		mouseText.setFont(font);
		mouseText.setCharacterSize(20);
		stringstream ss;
		ss << this->mousePosView.x << " " << this->mousePosView.y;
		mouseText.setString(ss.str());
		target->draw(mouseText);
	}

	void updateEvent(const Event& event)
	{
		if (event.type == Event::MouseButtonPressed)
		{
			if (event.mouseButton.button == Mouse::Left) {
				if (!this->chat.empty())
					this->chat.pop_front();
			}
		}

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
		this->buttons["CONTINUE_STATE"] = new Button(835, 500, 250, 50, "เล่นต่อ", 50,
			Color(70, 70, 70, 200), Color(250, 250, 250, 250), Color(20, 20, 20, 50),
			Color(70, 70, 70, 0), Color(150, 150, 150, 0), Color(20, 20, 20, 0));

		this->buttons["START_STATE"] = new Button(835, 600, 250, 50, "เริ่มเกมใหม่", 50,
			Color(70, 70, 70, 200), Color(250, 250, 250, 250), Color(20, 20, 20, 50),
			Color(70, 70, 70, 0), Color(150, 150, 150, 0), Color(20, 20, 20, 0));

		this->buttons["EXIT_STATE"] = new Button(835, 700, 250, 50, "ออกจากเกม", 50,
			Color(70, 70, 70, 200), Color(250, 250, 250, 250), Color(20, 20, 20, 50),
			Color(100, 100, 100, 0), Color(150, 150, 150, 0), Color(20, 20, 20, 0));
	}

	//Constructor / Destructor
	MainMenuState(RenderWindow* window, map<string, State*>* states)
		: State(window, states), player(player)
	{
		this->initBackground();
		this->initButtons();
	}

	~MainMenuState()
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
		if (this->player != nullptr && !this->player->isdead())
			if (this->buttons["CONTINUE_STATE"]->isPressed())
			{
				currentState = openWorldState;
			}

		if (this->buttons["START_STATE"]->isPressed())
		{
			if (this->player != nullptr)
			{
				delete this->player;
				this->player = NULL;
			}

			this->player = new Player();
			this->states->emplace("Map1_1", new GameState(this->window, this->states, this->player, 11));
			this->states->emplace("Map1_2", new GameState(this->window, this->states, this->player, 12));
			this->states->emplace("Map1_3", new GameState(this->window, this->states, this->player, 13));
			this->states->emplace("Map2_1", new GameState(this->window, this->states, this->player, 21));
			this->states->emplace("Map2_2", new GameState(this->window, this->states, this->player, 22));
			this->states->emplace("Map2_3", new GameState(this->window, this->states, this->player, 23));
			this->states->emplace("Map3_1", new GameState(this->window, this->states, this->player, 31));
			this->states->emplace("Map3_2", new GameState(this->window, this->states, this->player, 32));
			this->states->emplace("Map3_3", new GameState(this->window, this->states, this->player, 33));
			openWorldState = "Map1_1";
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
		mouseText.setFont(font);
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
	font.loadFromFile("Fonts/2005_iannnnnJPG.ttf");
	enemyTextures["battleKarakasa"].loadFromFile("Images/on_battle_stage/monmap1_karakasa.png");
	enemyTextures["battleHitosume"].loadFromFile("Images/on_battle_stage/monmap1_boss_oni.png");
	enemyTextures["battleBossOni"].loadFromFile("Images/on_battle_stage/monmap1_karakasa.png");
	enemyTextures["battleKappa"].loadFromFile("Images/on_battle_stage/monmap2_kappa.png");
	enemyTextures["battleAmikiri"].loadFromFile("Images/on_battle_stage/monmap2_amikiri.png");
	enemyTextures["battleBossUmibozu"].loadFromFile("Images/on_battle_stage/monmap2_boss_umibozu.png");
	enemyTextures["battleSkeleton"].loadFromFile("Images/on_battle_stage/monmap3_skeleton.png");
	enemyTextures["battleWanyudo"].loadFromFile("Images/on_battle_stage/monmap3_wanyudo.png");

	bool fullscreen = true;
	unsigned antialiasing_level = 0;
	float speed = 10;
	float dt = 0;

	Event event;
	Clock dtClock;

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
	states["MainMenu"] = new MainMenuState(window, &states);

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

	for (auto& i : states)
		delete i.second;

	delete window;

	return 0;

}
