#include <vector>
#include <map>
#include <sstream>
#include <cmath>
#include <list>
#include <iostream>

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
	//Accessors
	const bool& isPressed() const
	{
		return pressed;
	}

	const bool& isHovered() const
	{
		if (this->buttonState == button_states::BTN_HOVER) return true;
		return false;
	}


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

	//Check click
	void updateEvent(const Event& event, const Vector2f mousePos)
	{
		if (event.type == Event::MouseButtonPressed)
		{
			if (event.mouseButton.button == Mouse::Left && this->shape.getGlobalBounds().contains(mousePos)) pressed = true;
		}
	}

	//Functions
	void update(const Vector2f& mousePos)
	{
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

class ChatDialog
{
	RectangleShape shape;
	Text text;
	Sprite sprite;
	Texture texture;
	Button* choice1;
	Button* choice2;
	list<ChatDialog*>* chat;
	int* dialoglog;

public:
	const bool& haveButton() const
	{
		if (this->choice1) return true;
		else return false;
	}

	ChatDialog(wstring text, string fileloc)
	{
		this->chat = NULL;
		this->dialoglog = NULL;
		this->choice1 = NULL;
		this->choice2 = NULL;
		this->shape.setSize(Vector2f(1900, 200));
		this->text.setCharacterSize(30);
		this->shape.setOrigin(1900 / 2.f, 200 / 2.f);

		this->text.setFont(font);
		this->text.setString(text);
		this->shape.setFillColor(Color::White);
		this->text.setFillColor(Color::Black);
		this->shape.setOutlineThickness(3.f);
		this->shape.setOutlineColor(Color::Black);
		this->shape.setPosition(960, 980);
		this->text.setPosition(30, 900);
		this->texture.loadFromFile(fileloc);
		this->sprite.setTexture(this->texture);
		this->sprite.setPosition(10, 330);
	}

	ChatDialog(wstring text)
	{
		this->chat = NULL;
		this->dialoglog = NULL;
		this->choice1 = NULL;
		this->choice2 = NULL;
		this->shape.setSize(Vector2f(1900, 200));
		this->text.setCharacterSize(30);
		this->shape.setOrigin(1900 / 2.f, 200 / 2.f);

		this->text.setFont(font);
		this->text.setString(text);
		this->shape.setFillColor(Color::White);
		this->text.setFillColor(Color::Black);
		this->shape.setOutlineThickness(3.f);
		this->shape.setOutlineColor(Color::Black);
		this->shape.setPosition(960, 980);
		this->text.setPosition(30, 900);

	}

	ChatDialog(wstring text, string choice1, string choice2, int* dialoglog, list<ChatDialog*>* chat)
		:dialoglog(dialoglog), chat(chat)
	{
		this->choice1 = new Button(835, 500, 350, 80, choice1, 50,
			Color(70, 70, 70, 200), Color(255, 255, 255, 255), Color(20, 20, 20, 50),
			Color(70, 70, 70, 100), Color(150, 150, 150, 150), Color(20, 20, 20, 100));
		this->choice2 = new Button(835, 600, 350, 80, choice2, 50,
			Color(70, 70, 70, 200), Color(255, 255, 255, 255), Color(20, 20, 20, 50),
			Color(70, 70, 70, 100), Color(150, 150, 150, 150), Color(20, 20, 20, 100));
		this->shape.setSize(Vector2f(1900, 200));
		this->text.setCharacterSize(30);
		this->shape.setOrigin(1900 / 2.f, 200 / 2.f);

		this->text.setFont(font);
		this->text.setString(text);
		this->shape.setFillColor(Color::White);
		this->text.setFillColor(Color::Black);
		this->shape.setOutlineThickness(3.f);
		this->shape.setOutlineColor(Color::Black);
		this->shape.setPosition(960, 980);
		this->text.setPosition(30, 900);
	}

	~ChatDialog()
	{
		delete choice1;
		delete choice2;
	}

	void updateEvent(const Event& event, const Vector2f& mousePos)
	{
		if (this->choice1 != NULL)
		{
			this->choice1->updateEvent(event, mousePos);
		}
		if (this->choice2 != NULL)
		{
			this->choice2->updateEvent(event, mousePos);
		}
	}

	void update(const Vector2f& mousePos)
	{
		if (choice1 != NULL) {
			if (this->choice1->isPressed())
			{
				*this->dialoglog = 2;
				this->chat->pop_front();
			}
			this->choice1->update(mousePos);
		}
		if (choice2 != NULL) {
			if (this->choice2->isPressed())
			{
				*this->dialoglog = 1;
				this->chat->pop_front();
			}
			this->choice2->update(mousePos);
		}

	}

	void render(RenderTarget* target)
	{
		target->draw(this->sprite);
		target->draw(this->shape);
		target->draw(this->text);
		if (this->choice1 != NULL) this->choice1->render(*target);
		if (this->choice2 != NULL) this->choice2->render(*target);
	}

};

class ShowText
{
	Text text;
	float time;

public:
	const bool& isEnd() const
	{
		if (this->time > 2.f)
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
	//Accessors
	const FloatRect& getGlobalBounds() const
	{
		return this->hitbox.getGlobalBounds();
	}

	const RectangleShape& getHitbox() const
	{
		return this->hitbox;
	}

	const bool& intersects(const FloatRect& frect) const
	{
		return this->hitbox.getGlobalBounds().intersects(frect);
	}

	//Constructor / Destructor
	HitboxComponent(Sprite& sprite, float offset_x, float offset_y, float width, float height)
		:sprite(sprite), offsetX(offset_x), offsetY(offset_y)
	{
		this->hitbox.setPosition(this->sprite.getPosition().x + offset_x, this->sprite.getPosition().y + offset_y);
		this->hitbox.setSize(Vector2f(width, height));
		this->hitbox.setFillColor(Color::White);
	}

	//Functions
	void update()
	{
		this->hitbox.setPosition(this->sprite.getPosition().x + this->offsetX, this->sprite.getPosition().y + this->offsetY);
	}

	void render(RenderTarget& target)
	{

	}
};

enum class movementState { IDLE, MOVING_UP, MOVING_LEFT, MOVING_DOWN, MOVING_RIGHT };

class MovementComponent
{
	//Variables
	Sprite& sprite;
	Vector2f velocity;
	movementState state;

	float maxVelocity;

public:
	//Accessors
	const movementState& getMovementState() const
	{
		return this->state;
	}

	const Vector2f& getVelocity() const
	{
		return this->velocity;
	}

	const int& getState() const
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

	//Constructor / Destructor
	MovementComponent(Sprite& sprite, float maxVelocity)
		: sprite(sprite), velocity(Vector2f(0, 0)), maxVelocity(maxVelocity), state(movementState::IDLE)
	{

	}

	//Functions
	void move(const float dir_x, const float dir_y, const float& dt)
	{
		this->velocity = Vector2f(dir_x * dt, dir_y * dt);
	}

	void update()
	{
		this->sprite.move(this->velocity.x * this->maxVelocity, this->velocity.y * this->maxVelocity);
		if (this->velocity.y < 0)
			this->state = movementState::MOVING_UP;
		else if (this->velocity.x < 0)
			this->state = movementState::MOVING_LEFT;
		else if (this->velocity.y > 0)
			this->state = movementState::MOVING_DOWN;
		else if (this->velocity.x > 0)
			this->state = movementState::MOVING_RIGHT;
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
		vector<IntRect> rect;

		float animationTimer;
		float timer;
		int width;
		int height;
		bool end;

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

		void reset()
		{
			this->timer = 0.f;
		}
	};
	//Variables
	Sprite& sprite;
	Texture& textureSheet;
	map<string, Animation* > animations;

	//Constructor / Destructor
	AnimationComponent(Sprite& sprite, Texture& texture_sheet)
		:sprite(sprite), textureSheet(texture_sheet)
	{

	}

	~AnimationComponent()
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
	//Accessors
	virtual const Vector2f& getPosition() const
	{
		return Vector2f();
	}

	virtual const FloatRect& getGlobalBounds() const
	{
		return FloatRect();
	}

	//Constructor / Destructor
	Entity()
	{
		this->animationComponent = NULL;

		this->lvl = 0;
		this->hp = 0;
		this->max_hp = 0;
		this->att = 0;
		this->def = 0;
		this->time = 0.f;
	}

	virtual ~Entity()
	{
		delete this->animationComponent;
	}

	//Component functions
	void setTexture(Texture& texture)
	{
		this->texture = texture;
	}

	//Functions
	virtual void createAnimationComponent(Texture& texture_sheet)
	{

	}

	virtual void showTextPushBack(int dmg)
	{
		this->showText.push_back(new ShowText(Vector2f(this->getPosition().x + this->getGlobalBounds().width / 2, this->getPosition().y), dmg));
	}

	virtual void attack(Entity& opp) {
		opp.beAttacked(att + rand() % 10);
	}

	virtual void beAttacked(int oppatk) {
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
		for (auto& i : this->showText)
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
		this->showText.push_back(new ShowText(Vector2f(this->getGlobalBounds().left + this->getGlobalBounds().width / 2, this->getGlobalBounds().top - 50.f), dmg));
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
	TargetCursor()
	{
		this->texture.loadFromFile("Images/targetcursor.png");
		this->sprite.setTexture(this->texture);
		this->sprite.setPosition(0.f, 0.f);
	}

	void setPosition(Enemy* enemy)
	{
		this->sprite.setPosition(enemy->getPosition().x + enemy->getGlobalBounds().width / 2 - this->sprite.getGlobalBounds().width / 2, enemy->getPosition().y - 150.f);
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
	const Vector2f& getVelocity() const
	{
		return this->movementComponent->getVelocity();
	}

	const movementState& getMovementState() const
	{
		return this->movementComponent->getMovementState();
	}

	const FloatRect& getSpriteGlobalBounds() const
	{
		if (this->mode) return this->battleSprite.getGlobalBounds();
		else
			return this->openWorldSprite.getGlobalBounds();
	}

	const FloatRect& getHitboxGlobalBounds() const
	{
		return this->hitboxComponent->getGlobalBounds();
	}

	const Vector2f& getPosition() const
	{
		if (this->mode) return this->battleSprite.getPosition();
		else
			return this->openWorldSprite.getPosition();
	}

	const bool& isdead() const
	{
		if (this->hp <= 0) return true;
		return false;
	}

	const bool& isRendered() const
	{
		return this->rendered;
	}

	const bool& intersects(const FloatRect& frect) const
	{
		return this->hitboxComponent->intersects(frect);
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
		this->movementComponent = new MovementComponent(this->openWorldSprite, 1000.f);

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
	void beAttacked(int oppatk) {
		int dmg = 0;
		if (oppatk > this->def) {
			dmg = oppatk - this->def;
		}
		this->hp -= dmg;
		if (this->hp <= 0) this->hp = 0;
		this->showTextPushBack(dmg);
	}

	void showTextPushBack(int dmg, Color color = Color::Red)
	{
		this->showText.push_back(new ShowText(Vector2f(this->getSpriteGlobalBounds().left + this->getSpriteGlobalBounds().width / 2.f, this->getSpriteGlobalBounds().top), dmg, color));
	}

	void move(const float dir_x, const float dir_y, const float& dt)
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

	bool useSkill1(vector<Enemy*>& enemies)
	{
		if (this->stamina >= 30)
		{
			this->stamina -= 30;
			for (auto& i : enemies)
			{
				i->beAttacked(att / 2);
			}
			return true;
		}
		else
		{
			this->showText.push_back(new ShowText(this->getPosition(), "Mp is not enough"));
			return false;
		}
		return false;
	}

	bool useSkill2(Enemy*& enemy)
	{
		if (this->stamina >= 30)
		{
			this->stamina -= 30;
			enemy->beAttacked(att * 2);
			return true;
		}
		else
		{
			this->showText.push_back(new ShowText(this->getPosition(), "Mp is not enough"));
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
		if (currentState == "Battle") this->mode = true;
		else this->mode = false;

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
	Player& player;
	RectangleShape shape;

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
	bool update(const float& dt)
	{
		if (this->shape.getGlobalBounds().intersects(this->player.getHitboxGlobalBounds()))
		{
			if (this->player.getVelocity().y < 0)
			{
				this->player.setPosition(this->player.getPosition().x, this->shape.getGlobalBounds().top + this->shape.getGlobalBounds().height);
			}
			if (this->player.getVelocity().x < 0)
			{
				this->player.setPosition(this->shape.getGlobalBounds().left + this->shape.getGlobalBounds().width, this->player.getPosition().y);
			}
			if (this->player.getVelocity().y > 0)
			{
				this->player.setPosition(this->player.getPosition().x, this->shape.getGlobalBounds().top - this->player.getHitboxGlobalBounds().height);
			}
			if (this->player.getVelocity().x > 0)
			{
				this->player.setPosition(this->shape.getGlobalBounds().left - this->player.getHitboxGlobalBounds().width, this->player.getPosition().y);
			}
			return false;
		}
		return true;
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
	Vector2i mousePosScreen;
	Vector2i mousePosWindow;
	Vector2f mousePosView;

	RenderWindow* window;
	map<string, State*>* states;

	bool quit;

public:
	//Accessors
	const bool& getQuit() const
	{
		return this->quit;
	}

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



class ImageButton
{
	Texture texture;
	Sprite sprite;

	bool pressed;

public:
	const bool& isPressed() const
	{
		return this->pressed;
	}

	ImageButton(float x, float y, string textureLocation)
	{
		this->pressed = false;
		this->texture.loadFromFile(textureLocation);
		this->sprite.setTexture(this->texture);
		this->sprite.setPosition(x, y);
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

	//button
	map<string, ImageButton*> Mainbuttons;
	map<string, Button*> Itembuttons, Skillbuttons;
	map<string, Button*> detailButtons;

	//entity player&ene
	Sprite skillDetail;
	Sprite itemDetail;
	vector<Enemy*> enemy;
	Player* player;
	Enemy* target;
	TargetCursor* targetCursor;
	ShowText* clear;

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
		if (this->battleStage == 1)
			this->bgtexture.loadFromFile("Images/battle_scene/map1.png");
		else if (this->battleStage == 2)
			this->bgtexture.loadFromFile("Images/battle_scene/map2.png");
		else if (this->battleStage == 3)
			this->bgtexture.loadFromFile("Images/battle_scene/map3.png");

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

		this->Skillbuttons["SKILL1"] = new Button(200, 600, 420, 100, "เขี้ยวจันทรา ทะลวง สวรรค์", 50, Color::White, Color::White, Color::White,
			Color(70, 70, 70, 200), Color(150, 150, 150, 255), Color(20, 20, 20, 200));
		this->Skillbuttons["SKILL2"] = new Button(200, 700, 420, 100, "เบียวโดไคจินเซ็นไซ", 50, Color::White, Color::White, Color::White,
			Color(70, 70, 70, 200), Color(150, 150, 150, 255), Color(20, 20, 20, 200));

		this->detailButtons["SkillDetail"] = new Button(1650, 850, 200, 100, "รายละเอียดสกิล", 30, Color::White, Color::White, Color::White,
			Color(70, 70, 70, 200), Color(150, 150, 150, 255), Color(20, 20, 20, 200));
		this->detailButtons["ItemDetail"] = new Button(1650, 950, 200, 100, "รายละเอียดไอเทม", 30, Color::White, Color::White, Color::White,
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
		else
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

	~BattleState()
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

	void enemyTurn()
	{
		for (auto& i : enemy)
		{
			i->attack(*this->player);
		}
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
				if (this->player->useSkill1(this->enemy))
					this->isPlayerturn = false;
			}

			if (this->Skillbuttons["SKILL2"]->isPressed() && this->clear == nullptr)
			{
				if (this->player->useSkill2(this->target))
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
			this->states->erase(this->states->find("Map1_1"), this->states->end());
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
	Texture shadowTexture;
	Sprite sprite;
	Sprite shadowSprite;
	CollisionBox* collisionBox;
	Player* player;

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
			this->shadowTexture.loadFromFile("Images/map/tree_type_1_shadow.png");
			this->shadowSprite.setTexture(this->shadowTexture);
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
			this->collisionBox = new CollisionBox(*this->player, positionX, positionY, 105.f, 75.f);
		}
		else if (this->name == "Rock3")
		{
			this->texture.loadFromFile("Images/map/rock_type_3.png");
			this->sprite.setTexture(this->texture);
			this->collisionBox = new CollisionBox(*this->player, positionX, positionY, 425.f, 320.f);
		}
		else if (this->name == "House1")
		{
			this->texture.loadFromFile("Images/map/house1.png");
			this->sprite.setTexture(this->texture);
			this->collisionBox = new CollisionBox(*this->player, positionX + 15.f, positionY + 190.f, 350.f, 130.f);
		}
		else if (this->name == "House2")
		{
			this->texture.loadFromFile("Images/map/house2.png");
			this->sprite.setTexture(this->texture);
			this->collisionBox = new CollisionBox(*this->player, positionX + 15.f, positionY + 190.f, 350.f, 130.f);
		}
		else if (this->name == "House3")
		{
			this->texture.loadFromFile("Images/map/house3.png");
			this->sprite.setTexture(this->texture);
			this->collisionBox = new CollisionBox(*this->player, positionX, positionY + 120.f, 540.f, 140.f);
		}
		else if (this->name == "Fence1")
		{
			this->texture.loadFromFile("Images/map/fence1.png");
			this->sprite.setTexture(this->texture);
			this->collisionBox = new CollisionBox(*this->player, positionX, positionY, 21.f, 122.f);
		}
		else if (this->name == "Fence2")
		{
			this->texture.loadFromFile("Images/map/fence2.png");
			this->sprite.setTexture(this->texture);
			this->collisionBox = new CollisionBox(*this->player, positionX, positionY, 75.f, 74.f);
		}
		else if (this->name == "BossOni")
		{
			this->texture.loadFromFile("Images/monster_on_map/map1/boss_oni.png");
			this->sprite.setTexture(this->texture);
			this->collisionBox = new CollisionBox(*this->player, positionX, positionY, 250.f, 250.f);
		}
		else if (this->name == "Hitotsume")
		{
			this->texture.loadFromFile("Images/monster_on_map/map1/hitotsume.png");
			this->sprite.setTexture(this->texture);
			this->collisionBox = new CollisionBox(*this->player, positionX, positionY, 250.f, 250.f);
		}
		else if (this->name == "Karakasa")
		{
			this->texture.loadFromFile("Images/monster_on_map/map1/karakasa.png");
			this->sprite.setTexture(this->texture);
			this->collisionBox = new CollisionBox(*this->player, positionX, positionY, 125.f, 125.f);
		}
		else if (this->name == "Amikiri")
		{
			this->texture.loadFromFile("Images/monster_on_map/map2/amikiri.png");
			this->sprite.setTexture(this->texture);
			this->collisionBox = new CollisionBox(*this->player, positionX, positionY, 250.f, 250.f);
		}
		else if (this->name == "BossUmibozu")
		{
			this->texture.loadFromFile("Images/monster_on_map/map2/boss_umibozu.png");
			this->sprite.setTexture(this->texture);
			this->collisionBox = new CollisionBox(*this->player, positionX, positionY, 250.f, 250.f);
		}
		else if (this->name == "Kappa")
		{
			this->texture.loadFromFile("Images/monster_on_map/map2/kappa.png");
			this->sprite.setTexture(this->texture);
			this->collisionBox = new CollisionBox(*this->player, positionX, positionY, 250.f, 250.f);
		}
		else if (this->name == "BossGashadokuro")
		{
			this->texture.loadFromFile("Images/monster_on_map/map3/boss_gashadokuro.png");
			this->sprite.setTexture(this->texture);
			this->collisionBox = new CollisionBox(*this->player, positionX, positionY, 250.f, 250.f);
		}
		else if (this->name == "Skeleton")
		{
			this->texture.loadFromFile("Images/monster_on_map/map3/skeleton.png");
			this->sprite.setTexture(this->texture);
			this->collisionBox = new CollisionBox(*this->player, positionX, positionY, 250.f, 250.f);
		}
		else if (this->name == "Wanyudo")
		{
			this->texture.loadFromFile("Images/monster_on_map/map3/wanyudo.png");
			this->sprite.setTexture(this->texture);
			this->collisionBox = new CollisionBox(*this->player, positionX, positionY, 250.f, 250.f);
		}
		else
		{
			this->texture.loadFromFile("Images/map/tree_type_1.png");
			this->sprite.setTexture(this->texture);
			this->collisionBox = new CollisionBox(*this->player, positionX + 65.f, positionY + 220.f, 45.f, 40.f);
		}
		this->sprite.setPosition(positionX, positionY);
		this->shadowSprite.setPosition(positionX, positionY);
	}

	~Object()
	{
		delete this->collisionBox;
	}

	virtual bool update(const float& dt)
	{

		if (!this->collisionBox->update(dt)) return false;
	}

	virtual void render(RenderTarget& target)
	{
		this->collisionBox->render(target);
		if (this->player->getHitboxGlobalBounds().top < this->collisionBox->getShape().getGlobalBounds().top && !this->player->isRendered())
			this->player->render(target);
		target.draw(this->sprite);
		target.draw(this->shadowSprite);
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
	Texture backgroundTexture;
	RectangleShape background;
	Sprite environment;
	vector<Object*> objects;
	vector<CollisionBox*> collisions;
	string textline;
	list<ChatDialog*> chat;
	Player* player;

	int& dialogchat;
	int gameStage;
	int dialog;

public:
	//Initilizer functions

	//Constructor / Destructor
	GameState(RenderWindow* window, map<string, State*>* states, Player* player, int& dialogchat, int gameStage = 11)
		: State(window, states), player(player), gameStage(gameStage), dialogchat(dialogchat)
	{
		this->dialog = 0;
		this->dialogchat = 0;
		if (this->gameStage == 11)
		{
			this->backgroundTexture.loadFromFile("Images/map/map1/map1_1.png");

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

			this->objects.push_back(new Object("Shadow1", this->player, 704.f, -172.f));
			this->objects.push_back(new Object("Shadow1", this->player, 948.f, -5.f));
			this->objects.push_back(new Object("Shadow1", this->player, 927.f, -163.f));
			this->objects.push_back(new Object("Shadow1", this->player, 487.f, -179.f));
			this->objects.push_back(new Object("Shadow1", this->player, 200.f, -164.f));
			this->objects.push_back(new Object("Shadow1", this->player, 633.f, 760.f));
			this->objects.push_back(new Object("Shadow1", this->player, 1567.f, 19.f));
			this->objects.push_back(new Object("Shadow1", this->player, 185.f, 83.f));
			this->objects.push_back(new Object("Shadow1", this->player, 375.f, 95.f));
			this->objects.push_back(new Object("Shadow1", this->player, 17.f, 342.f));
			this->objects.push_back(new Object("Shadow1", this->player, 224.f, 699.f));
			this->objects.push_back(new Object("Shadow1", this->player, 1221.f, 150.f));
			this->objects.push_back(new Object("Shadow1", this->player, 1521.f, 694.f));
			this->objects.push_back(new Object("Shadow1", this->player, 1719.f, 608.f));

			this->chat.push_back(new ChatDialog(L"ชายหนุ่มได้ออกเดินทางตามหาต้นตอของคำสาปที่ทำให้หญิงผู้ที่เป็นที่รักต้องทนทุกข์ทรมาร จนมาถึงป่าแห่งหนึ่งที่เต็มไปด้วยกลิ่นอายอันชั่วร้าย"));
			this->chat.push_back(new ChatDialog(L"หยุดก่อนเจ้าหนุ่ม เจ้ามีธุระอะไรในป่าแห่งนี้กัน", "Images/character/gurad.png"));
			this->chat.push_back(new ChatDialog(L" ", "พูดความจริง", "โกหกว่ามาตามหาคน", &dialogchat, &this->chat));
			this->dialog = 1;
		}
		else if (this->gameStage == 12)
		{
			this->backgroundTexture.loadFromFile("Images/map/map1/map1_2.png");

			this->collisions.push_back(new CollisionBox(*this->player, 1434.f, 584.f, 1.f, 358.f));
			this->collisions.push_back(new CollisionBox(*this->player, 1634.f, 589.f, 1.f, 353.f));
			this->collisions.push_back(new CollisionBox(*this->player, 610.f, 0.f, 1.f, 310.f));

			this->objects.push_back(new Object("Rock", this->player, 1341.f, 821.f));
			this->objects.push_back(new Object("Rock", this->player, 761.f, 504.f));
			this->objects.push_back(new Object("Rock", this->player, 1074.f, 681.f));
			this->objects.push_back(new Object("Rock", this->player, 1110.f, 395.f));
			this->objects.push_back(new Object("Rock", this->player, 524.f, 34.f));
			this->objects.push_back(new Object("Rock", this->player, 968.f, 614.f));
			this->objects.push_back(new Object("Rock2", this->player, 866.f, 536.f));
			this->objects.push_back(new Object("Rock2", this->player, 1713.f, 489.f));
			this->objects.push_back(new Object("Rock2", this->player, 1649.f, 805.f));
			this->objects.push_back(new Object("Rock2", this->player, 1820.f, 723.f));
			this->objects.push_back(new Object("Rock2", this->player, 531.f, 78.f));
			this->objects.push_back(new Object("Rock2", this->player, 531.f, 146.f));
			this->objects.push_back(new Object("Rock2", this->player, 541.f, 203.f));
			this->objects.push_back(new Object("Rock2", this->player, 1276.f, 483.f));
			this->objects.push_back(new Object("Rock2", this->player, 1167.f, 408.f));
			this->objects.push_back(new Object("Rock2", this->player, 884.f, 239.f));
			this->objects.push_back(new Object("Rock2", this->player, 768.f, 176.f));
			this->objects.push_back(new Object("Rock2", this->player, 683.f, 100.f));
			this->objects.push_back(new Object("Rock2", this->player, 1196.f, 741.f));
			this->objects.push_back(new Object("Rock2", this->player, 661.f, 424.f));
			this->objects.push_back(new Object("Tree1", this->player, 524.f, 73.f));
			this->objects.push_back(new Object("Tree1", this->player, 1317.f, -161.f));
			this->objects.push_back(new Object("Tree1", this->player, 1555.f, -161.f));
			this->objects.push_back(new Object("Tree1", this->player, 0.f, -145.f));
			this->objects.push_back(new Object("Tree1", this->player, 296.f, -118.f));
			this->objects.push_back(new Object("Tree1", this->player, 659.f, 249.f));
			this->objects.push_back(new Object("Tree1", this->player, 866.f, 344.f));
			this->objects.push_back(new Object("Tree1", this->player, 1073.f, 468.f));
			this->objects.push_back(new Object("Tree1", this->player, -43.f, 789.f));
			this->objects.push_back(new Object("Tree1", this->player, 509.f, 562.f));
			this->objects.push_back(new Object("Tree1", this->player, 1799.f, -145.f));
			this->objects.push_back(new Object("Tree1", this->player, 1685.f, 540.f));
			this->objects.push_back(new Object("Tree2", this->player, -28.f, 578.f));
			this->objects.push_back(new Object("Tree2", this->player, 1261.f, 572.f));
			this->objects.push_back(new Object("Tree2", this->player, 603.f, -145.f));
			this->objects.push_back(new Object("Tree2", this->player, 669.f, -40.f));
			this->objects.push_back(new Object("Tree2", this->player, 160.f, -75.f));
			this->objects.push_back(new Object("Tree2", this->player, 1761.f, 256.f));
			this->objects.push_back(new Object("Tree2", this->player, 768.f, 28.f));
			this->objects.push_back(new Object("Tree2", this->player, 768.f, 313.f));
			this->objects.push_back(new Object("Tree2", this->player, 994.f, 411.f));
			this->objects.push_back(new Object("Tree2", this->player, 1734.f, 821.f));
			this->objects.push_back(new Object("Tree2", this->player, 1597.f, 330.f));
			this->objects.push_back(new Object("Tree2", this->player, 889.f, 75.f));
			this->objects.push_back(new Object("Tree2", this->player, 1001.f, 146.f));
			this->objects.push_back(new Object("Tree2", this->player, 1170.f, 265.f));
			this->objects.push_back(new Object("Tree2", this->player, 1305.f, 319.f));
			this->objects.push_back(new Object("Tree2", this->player, 594.f, 184.f));
			this->objects.push_back(new Object("Tree3", this->player, 1873.f, 469.f));
			this->objects.push_back(new Object("Tree3", this->player, 638.f, 380.f));
			this->objects.push_back(new Object("Tree3", this->player, 866.f, 0.f));
			this->objects.push_back(new Object("Tree3", this->player, 1046.f, 1.f));
			this->objects.push_back(new Object("Tree3", this->player, 1225.f, 15.f));
			this->objects.push_back(new Object("Tree3", this->player, 712.f, 853.f));
			this->objects.push_back(new Object("Tree3", this->player, 1010.f, 330.f));
		}
		else if (this->gameStage == 13)
		{
			this->backgroundTexture.loadFromFile("Images/map/map1/map1_3.png");

			this->objects.push_back(new Object("Rock", this->player, 738.f, 730.f));
			this->objects.push_back(new Object("Rock", this->player, 1165.f, 210.f));
			this->objects.push_back(new Object("Rock", this->player, 199.f, 169.f));
			this->objects.push_back(new Object("Rock", this->player, 584.f, 982.f));
			this->objects.push_back(new Object("Rock", this->player, 1740.f, 675.f));
			this->objects.push_back(new Object("Rock", this->player, 221.f, 962.f));
			this->objects.push_back(new Object("Rock", this->player, 1580.f, 190.f));
			this->objects.push_back(new Object("Tree1", this->player, 327.f, 767.f));
			this->objects.push_back(new Object("Tree1", this->player, 94.f, 148.f));
			this->objects.push_back(new Object("Tree1", this->player, 1230.f, 210.f));
			this->objects.push_back(new Object("Tree1", this->player, 1812.f, 666.f));
			this->objects.push_back(new Object("Tree1", this->player, 829.f, 783.f));
			this->objects.push_back(new Object("Tree1", this->player, 199.f, 439.f));
			this->objects.push_back(new Object("Tree1", this->player, 965.f, -65.f));
			this->objects.push_back(new Object("Tree1", this->player, 1267.f, 524.f));
			this->objects.push_back(new Object("Tree1", this->player, 453.f, 0.f));
			this->objects.push_back(new Object("Tree1", this->player, 1669.f, 194.f));
			this->objects.push_back(new Object("Tree1", this->player, 362.f, 492.f));
			this->objects.push_back(new Object("Tree1", this->player, 1097.f, 783.f));
			this->objects.push_back(new Object("Tree1", this->player, 47.f, 508.f));
			this->objects.push_back(new Object("Tree1", this->player, 1689.f, 749.f));
			this->objects.push_back(new Object("Tree2", this->player, -18.f, -88.f));
			this->objects.push_back(new Object("Tree2", this->player, -18.f, 355.f));
			this->objects.push_back(new Object("Tree2", this->player, 979.f, 355.f));
			this->objects.push_back(new Object("Tree2", this->player, 547.f, 540.f));
			this->objects.push_back(new Object("Tree2", this->player, 1609.f, -112.f));
			this->objects.push_back(new Object("Tree2", this->player, 1798.f, 471.f));
			this->objects.push_back(new Object("Tree2", this->player, 62.f, 723.f));
			this->objects.push_back(new Object("Tree2", this->player, 1032.f, 566.f));
			this->objects.push_back(new Object("Tree2", this->player, 1418.f, 147.f));
			this->objects.push_back(new Object("Tree2", this->player, 627.f, 18.f));
			this->objects.push_back(new Object("Tree2", this->player, 1339.f, 799.f));
			this->objects.push_back(new Object("Tree2", this->player, 1530.f, 566.f));
			this->objects.push_back(new Object("Tree3", this->player, 1173.f, 457.f));
			this->objects.push_back(new Object("Tree3", this->player, 1324.f, 118.f));
			this->objects.push_back(new Object("Tree3", this->player, 282.f, 801.f));
			this->objects.push_back(new Object("Tree3", this->player, 224.f, 81.f));
			this->objects.push_back(new Object("Tree3", this->player, 1462.f, 675.f));
			this->objects.push_back(new Object("Tree3", this->player, 1623.f, 443.f));
			this->objects.push_back(new Object("Tree3", this->player, 793.f, 79.f));
			this->objects.push_back(new Object("Tree3", this->player, 996.f, 769.f));
			this->objects.push_back(new Object("Tree3", this->player, 58.f, 1023.f));
		}
		else if (this->gameStage == 21)
		{
			this->backgroundTexture.loadFromFile("Images/map/map2/map2_1.png");

			this->collisions.push_back(new CollisionBox(*this->player, 490.f, 815.f, 1430.f, 1.f));

			this->objects.push_back(new Object("Rock", this->player, 806.f, 704.f));
			this->objects.push_back(new Object("Rock", this->player, 1376.f, 119.f));
			this->objects.push_back(new Object("Rock", this->player, 917.f, 99.f));
			this->objects.push_back(new Object("Rock", this->player, 180.f, 429.f));
			this->objects.push_back(new Object("Rock2", this->player, 375.f, 763.f));
			this->objects.push_back(new Object("Rock2", this->player, 613.f, 293.f));
			this->objects.push_back(new Object("Rock2", this->player, 1595.f, 220.f));
			this->objects.push_back(new Object("Rock2", this->player, 159.f, 0.f));
			this->objects.push_back(new Object("Rock3", this->player, -20.f, 763.f));
			this->objects.push_back(new Object("Rock3", this->player, 927.f, 270.f));
			this->objects.push_back(new Object("Tree1", this->player, -20.f, 39.f));
			this->objects.push_back(new Object("Tree1", this->player, -29.f, 472.f));
			this->objects.push_back(new Object("Tree1", this->player, 720.f, 37.f));
			this->objects.push_back(new Object("Tree1", this->player, 335.f, -145.f));
			this->objects.push_back(new Object("Tree1", this->player, 335.f, 220.f));
			this->objects.push_back(new Object("Tree1", this->player, 562.f, -172.f));
			this->objects.push_back(new Object("Tree2", this->player, 0.f, 257.f));
			this->objects.push_back(new Object("Tree2", this->player, 403.f, 445.f));
			this->objects.push_back(new Object("Tree2", this->player, 1003.f, -139.f));
		}
		else if (this->gameStage == 22)
		{
			this->backgroundTexture.loadFromFile("Images/map/map2/map2_2.png");

			this->collisions.push_back(new CollisionBox(*this->player, 1700.f, 320.f, 1.f, 500.f));
			this->collisions.push_back(new CollisionBox(*this->player, 1660.f, 840.f, 50.f, 50.f));

			this->objects.push_back(new Object("Rock", this->player, 509.f, 743.f));
			this->objects.push_back(new Object("Rock", this->player, 1231.f, 406.f));
			this->objects.push_back(new Object("Rock", this->player, 1059.f, 794.f));
			this->objects.push_back(new Object("Rock", this->player, 151.f, 390.f));
			this->objects.push_back(new Object("Rock2", this->player, 1531.f, 814.f));
			this->objects.push_back(new Object("Rock2", this->player, 1210.f, 8.f));
			this->objects.push_back(new Object("Rock2", this->player, 1563.f, 294.f));
			this->objects.push_back(new Object("Rock2", this->player, 429.f, 374.f));
			this->objects.push_back(new Object("Rock2", this->player, 886.f, 612.f));
			this->objects.push_back(new Object("Rock2", this->player, 195.f, 648.f));
			this->objects.push_back(new Object("Rock3", this->player, 1102.f, 91.f));
			this->objects.push_back(new Object("Rock3", this->player, 1102.f, 763.f));
			this->objects.push_back(new Object("Tree1", this->player, 20.f, -127.f));
			this->objects.push_back(new Object("Tree1", this->player, 368.f, -109.f));
			this->objects.push_back(new Object("Tree1", this->player, 114.f, 763.f));
			this->objects.push_back(new Object("Tree1", this->player, 536.f, 776.f));
			this->objects.push_back(new Object("Tree1", this->player, 845.f, 705.f));
			this->objects.push_back(new Object("Tree1", this->player, 650.f, -109.f));
			this->objects.push_back(new Object("Tree1", this->player, 524.f, 76.f));
			this->objects.push_back(new Object("Tree1", this->player, 208.f, 37.f));
			this->objects.push_back(new Object("Tree1", this->player, 335.f, 705.f));
			this->objects.push_back(new Object("Tree2", this->player, -22.f, 758.f));
			this->objects.push_back(new Object("Tree2", this->player, 35.f, 92.f));
			this->objects.push_back(new Object("Tree2", this->player, 766.f, 115.f));
			this->objects.push_back(new Object("Tree2", this->player, 665.f, 685.f));
			this->objects.push_back(new Object("Tree2", this->player, 429.f, 814.f));
			this->objects.push_back(new Object("Tree3", this->player, 429.f, 280.f));
			this->objects.push_back(new Object("Tree3", this->player, 317.f, 996.f));
			this->objects.push_back(new Object("Tree3", this->player, 190.f, 323.f));
			this->objects.push_back(new Object("Tree3", this->player, 820.f, 1010.f));
			this->objects.push_back(new Object("Tree3", this->player, 744.f, 280.f));
			this->objects.push_back(new Object("Tree3", this->player, 299.f, 859.f));
			this->objects.push_back(new Object("Tree3", this->player, 17.f, 266.f));
		}
		else if (this->gameStage == 23)
		{
			this->backgroundTexture.loadFromFile("Images/map/map2/map2_3.png");

			this->collisions.push_back(new CollisionBox(*this->player, 1310.f, 870.f, 1.f, 210.f));
			this->collisions.push_back(new CollisionBox(*this->player, 1310.f, 870.f, 610.f, 1.f));

			this->objects.push_back(new Object("Rock", this->player, 200.f, 967.f));
			this->objects.push_back(new Object("Rock", this->player, 376.f, 728.f));
			this->objects.push_back(new Object("Rock", this->player, 672.f, 360.f));
			this->objects.push_back(new Object("Rock2", this->player, 1150.f, 866.f));
			this->objects.push_back(new Object("Rock2", this->player, 1177.f, 987.f));
			this->objects.push_back(new Object("Rock2", this->player, 269.f, 416.f));
			this->objects.push_back(new Object("Rock2", this->player, 1550.f, 202.f));
			this->objects.push_back(new Object("Rock2", this->player, 1592.f, 769.f));
			this->objects.push_back(new Object("Tree1", this->player, 113.f, -16.f));
			this->objects.push_back(new Object("Tree1", this->player, -39.f, 276.f));
			this->objects.push_back(new Object("Tree1", this->player, -39.f, 769.f));
			this->objects.push_back(new Object("Tree1", this->player, -11.f, 490.f));
			this->objects.push_back(new Object("Tree1", this->player, 343.f, 131.f));
			this->objects.push_back(new Object("Tree1", this->player, 646.f, -105.f));
			this->objects.push_back(new Object("Tree1", this->player, 1010.f, 12.f));
			this->objects.push_back(new Object("Tree1", this->player, 1336.f, -118.f));
			this->objects.push_back(new Object("Tree1", this->player, 1699.f, -32.f));
			this->objects.push_back(new Object("Tree2", this->player, 0.f, -73.f));
			this->objects.push_back(new Object("Tree2", this->player, 97.f, 230.f));
			this->objects.push_back(new Object("Tree2", this->player, 487.f, 0.f));
			this->objects.push_back(new Object("Tree2", this->player, 18.f, 680.f));
			this->objects.push_back(new Object("Tree2", this->player, 243.f, -102.f));
			this->objects.push_back(new Object("Tree2", this->player, 832.f, -73.f));
			this->objects.push_back(new Object("Tree2", this->player, 1177.f, -102.f));
			this->objects.push_back(new Object("Tree2", this->player, -104.f, 421.f));
			this->objects.push_back(new Object("Tree2", this->player, 1498.f, -129.f));
			this->objects.push_back(new Object("Fence1", this->player, 1475.f, 608.f));
			this->objects.push_back(new Object("Fence1", this->player, 1531.f, 608.f));
			this->objects.push_back(new Object("Fence1", this->player, 1587.f, 608.f));
			this->objects.push_back(new Object("Fence1", this->player, 1643.f, 608.f));
			this->objects.push_back(new Object("Fence1", this->player, 1699.f, 608.f));
			this->objects.push_back(new Object("Fence1", this->player, 1755.f, 608.f));
			this->objects.push_back(new Object("Fence1", this->player, 1811.f, 608.f));
			this->objects.push_back(new Object("Fence1", this->player, 1475.f, 416.f));
			this->objects.push_back(new Object("Fence1", this->player, 1531.f, 416.f));
			this->objects.push_back(new Object("Fence1", this->player, 1587.f, 416.f));
			this->objects.push_back(new Object("Fence1", this->player, 1643.f, 416.f));
			this->objects.push_back(new Object("Fence1", this->player, 1699.f, 416.f));
			this->objects.push_back(new Object("Fence1", this->player, 1755.f, 416.f));
			this->objects.push_back(new Object("Fence1", this->player, 1811.f, 416.f));
		}
		else if (this->gameStage == 31)
		{
			this->backgroundTexture.loadFromFile("Images/map/map3/map3_1.png");

			this->collisions.push_back(new CollisionBox(*this->player, 10.f, 870.f, 50.f, 50.f));
			this->collisions.push_back(new CollisionBox(*this->player, 70.f, 885.f, 50.f, 50.f));
			this->collisions.push_back(new CollisionBox(*this->player, 130.f, 895.f, 50.f, 50.f));
			this->collisions.push_back(new CollisionBox(*this->player, 190.f, 910.f, 50.f, 50.f));
			this->collisions.push_back(new CollisionBox(*this->player, 250.f, 925.f, 50.f, 50.f));
			this->collisions.push_back(new CollisionBox(*this->player, 310.f, 940.f, 50.f, 50.f));
			this->collisions.push_back(new CollisionBox(*this->player, 370.f, 950.f, 50.f, 50.f));
			this->collisions.push_back(new CollisionBox(*this->player, 430.f, 965.f, 1490.f, 1.f));

			this->objects.push_back(new Object("Rock", this->player, 191.f, 69.f));
			this->objects.push_back(new Object("Rock", this->player, 1428.f, 90.f));
			this->objects.push_back(new Object("Rock", this->player, 1821.f, 280.f));
			this->objects.push_back(new Object("Rock", this->player, 6322.f, 868.f));
			this->objects.push_back(new Object("Rock2", this->player, 1272.f, 868.f));
			this->objects.push_back(new Object("Rock2", this->player, 75.f, 688.f));
			this->objects.push_back(new Object("Rock2", this->player, 1714.f, 852.f));
			this->objects.push_back(new Object("Rock2", this->player, 1116.f, 652.f));
			this->objects.push_back(new Object("Tree1", this->player, 296.f, -90.f));
			this->objects.push_back(new Object("Tree1", this->player, 16.f, -90.f));
			this->objects.push_back(new Object("Tree1", this->player, 1514.f, -126.f));
			this->objects.push_back(new Object("Tree2", this->player, 595.f, -69.f));
			this->objects.push_back(new Object("Tree2", this->player, 1192.f, -84.f));
			this->objects.push_back(new Object("House1", this->player, 18.f, 117.f));
			this->objects.push_back(new Object("House1", this->player, 409.f, 117.f));
			this->objects.push_back(new Object("House2", this->player, 246.f, 540.f));
			this->objects.push_back(new Object("House2", this->player, 744.f, 540.f));
			this->objects.push_back(new Object("House3", this->player, 1229.f, 567.f));
			this->objects.push_back(new Object("House3", this->player, 1229.f, 144.f));
			this->objects.push_back(new Object("Fence1", this->player, 800.f, 0.f));
			this->objects.push_back(new Object("Fence1", this->player, 800.f, 55.f));
			this->objects.push_back(new Object("Fence1", this->player, 800.f, 110.f));
			this->objects.push_back(new Object("Fence1", this->player, 800.f, 165.f));
			this->objects.push_back(new Object("Fence1", this->player, 800.f, 220.f));
			this->objects.push_back(new Object("Fence1", this->player, 800.f, 275.f));
			this->objects.push_back(new Object("Fence1", this->player, 800.f, 330.f));
			this->objects.push_back(new Object("Fence1", this->player, 1157.f, 0.f));
			this->objects.push_back(new Object("Fence1", this->player, 1157.f, 55.f));
			this->objects.push_back(new Object("Fence1", this->player, 1157.f, 110.f));
			this->objects.push_back(new Object("Fence1", this->player, 1157.f, 165.f));
			this->objects.push_back(new Object("Fence1", this->player, 1157.f, 220.f));
			this->objects.push_back(new Object("Fence1", this->player, 1157.f, 275.f));
			this->objects.push_back(new Object("Fence1", this->player, 1157.f, 330.f));
			this->objects.push_back(new Object("Fence2", this->player, 0.f, 776.f));
			this->objects.push_back(new Object("Fence2", this->player, 56.f, 776.f));
			this->objects.push_back(new Object("Fence2", this->player, 112.f, 776.f));
			this->objects.push_back(new Object("Fence2", this->player, 168.f, 776.f));
			this->objects.push_back(new Object("Fence2", this->player, 1776.f, 776.f));
			this->objects.push_back(new Object("Fence2", this->player, 1832.f, 776.f));
		}
		else if (this->gameStage == 32)
		{
			this->backgroundTexture.loadFromFile("Images/map/map3/map3_2.png");

			this->collisions.push_back(new CollisionBox(*this->player, 10.f, 960.f, 50.f, 50.f));
			this->collisions.push_back(new CollisionBox(*this->player, 70.f, 980.f, 50.f, 50.f));
			this->collisions.push_back(new CollisionBox(*this->player, 130.f, 1000.f, 50.f, 50.f));
			this->collisions.push_back(new CollisionBox(*this->player, 190.f, 1025.f, 15.f, 50.f));
			this->collisions.push_back(new CollisionBox(*this->player, 215.f, 1035.f, 1705.f, 1.f));

			this->objects.push_back(new Object("House1", this->player, 28.f, 99.f));
			this->objects.push_back(new Object("House1", this->player, 436.f, 99.f));
			this->objects.push_back(new Object("House1", this->player, 724.f, -168.f));
			this->objects.push_back(new Object("House2", this->player, 28.f, 578.f));
			this->objects.push_back(new Object("House2", this->player, 436.f, 578.f));
			this->objects.push_back(new Object("House3", this->player, 1017.f, 178.f));
			this->objects.push_back(new Object("House3", this->player, 1597.f, 357.f));
			this->objects.push_back(new Object("House3", this->player, 1017.f, 604.f));
			this->objects.push_back(new Object("Fence2", this->player, 295.f, 920.f));
			this->objects.push_back(new Object("Fence2", this->player, 351.f, 920.f));
			this->objects.push_back(new Object("Fence2", this->player, 407.f, 920.f));
			this->objects.push_back(new Object("Fence2", this->player, 463.f, 920.f));
			this->objects.push_back(new Object("Fence2", this->player, 239.f, 920.f));
			this->objects.push_back(new Object("Fence2", this->player, 183.f, 920.f));
			this->objects.push_back(new Object("Fence2", this->player, 519.f, 920.f));
			this->objects.push_back(new Object("Fence2", this->player, 575.f, 920.f));
			this->objects.push_back(new Object("Fence2", this->player, 631.f, 920.f));
			this->objects.push_back(new Object("Fence2", this->player, 687.f, 920.f));
			this->objects.push_back(new Object("Fence2", this->player, 1041.f, 920.f));
			this->objects.push_back(new Object("Fence2", this->player, 1097.f, 920.f));
			this->objects.push_back(new Object("Fence2", this->player, 1153.f, 920.f));
			this->objects.push_back(new Object("Fence2", this->player, 1209.f, 920.f));
			this->objects.push_back(new Object("Fence2", this->player, 1265.f, 920.f));
			this->objects.push_back(new Object("Fence2", this->player, 1321.f, 920.f));
			this->objects.push_back(new Object("Fence2", this->player, 1377.f, 920.f));
			this->objects.push_back(new Object("Fence2", this->player, 1433.f, 920.f));
			this->objects.push_back(new Object("Fence2", this->player, 1489.f, 920.f));
			this->objects.push_back(new Object("Fence2", this->player, 1545.f, 920.f));
			this->objects.push_back(new Object("Fence2", this->player, 1601.f, 920.f));
			this->objects.push_back(new Object("Fence2", this->player, 1657.f, 920.f));
			this->objects.push_back(new Object("Fence2", this->player, 1713.f, 920.f));
			this->objects.push_back(new Object("Fence2", this->player, 1769.f, 920.f));
			this->objects.push_back(new Object("Fence2", this->player, 1825.f, 920.f));
			this->objects.push_back(new Object("Tree1", this->player, 1657.f, -46.f));
			this->objects.push_back(new Object("Tree1", this->player, 277.f, -145.f));
			this->objects.push_back(new Object("Tree1", this->player, 1694.f, 578.f));
			this->objects.push_back(new Object("Tree1", this->player, 1195.f, -168.f));
			this->objects.push_back(new Object("Tree2", this->player, -51.f, -81.f));
			this->objects.push_back(new Object("Tree2", this->player, 1461.f, -81.f));
			this->objects.push_back(new Object("Tree2", this->player, 1503.f, 661.f));
			this->objects.push_back(new Object("Tree2", this->player, 482.f, -145.f));
			this->objects.push_back(new Object("Rock", this->player, 787.f, 992.f));
			this->objects.push_back(new Object("Rock", this->player, 1590.f, 316.f));
			this->objects.push_back(new Object("Rock", this->player, 154.f, 49.f));
			this->objects.push_back(new Object("Rock", this->player, 1814.f, 244.f));
			this->objects.push_back(new Object("Rock", this->player, 1105.f, 18.f));
		}
		else if (this->gameStage == 33)
		{
			this->backgroundTexture.loadFromFile("Images/map/map3/map3_3.png");

			this->objects.push_back(new Object("House3", this->player, 1559.f, 24.f));
			this->objects.push_back(new Object("House3", this->player, 1559.f, 382.f));
			this->objects.push_back(new Object("House3", this->player, -213.f, 24.f));
			this->objects.push_back(new Object("House3", this->player, -213.f, 382.f));
			this->objects.push_back(new Object("Fence1", this->player, 800.f, 747.f));
			this->objects.push_back(new Object("Fence1", this->player, 800.f, 803.f));
			this->objects.push_back(new Object("Fence1", this->player, 800.f, 859.f));
			this->objects.push_back(new Object("Fence1", this->player, 800.f, 914.f));
			this->objects.push_back(new Object("Fence1", this->player, 800.f, 970.f));
			this->objects.push_back(new Object("Fence1", this->player, 1157.f, 747.f));
			this->objects.push_back(new Object("Fence1", this->player, 1157.f, 803.f));
			this->objects.push_back(new Object("Fence1", this->player, 1157.f, 859.f));
			this->objects.push_back(new Object("Fence1", this->player, 1157.f, 914.f));
			this->objects.push_back(new Object("Fence1", this->player, 1157.f, 970.f));
			this->objects.push_back(new Object("Fence2", this->player, 741.f, 747.f));
			this->objects.push_back(new Object("Fence2", this->player, 684.f, 747.f));
			this->objects.push_back(new Object("Fence2", this->player, 627.f, 747.f));
			this->objects.push_back(new Object("Fence2", this->player, 570.f, 747.f));
			this->objects.push_back(new Object("Fence2", this->player, 513.f, 747.f));
			this->objects.push_back(new Object("Fence2", this->player, 456.f, 747.f));
			this->objects.push_back(new Object("Fence2", this->player, 399.f, 747.f));
			this->objects.push_back(new Object("Fence2", this->player, 342.f, 747.f));
			this->objects.push_back(new Object("Fence2", this->player, 1155.f, 747.f));
			this->objects.push_back(new Object("Fence2", this->player, 1212.f, 747.f));
			this->objects.push_back(new Object("Fence2", this->player, 1269.f, 747.f));
			this->objects.push_back(new Object("Fence2", this->player, 1326.f, 747.f));
			this->objects.push_back(new Object("Fence2", this->player, 1440.f, 747.f));
			this->objects.push_back(new Object("Fence2", this->player, 1497.f, 747.f));
			this->objects.push_back(new Object("Tree1", this->player, 97.f, 698.f));
			this->objects.push_back(new Object("Tree1", this->player, 1252.f, 698.f));
			this->objects.push_back(new Object("Tree1", this->player, 1572.f, 658.f));
			this->objects.push_back(new Object("Tree1", this->player, 1732.f, 789.f));
			this->objects.push_back(new Object("Tree1", this->player, 476.f, 743.f));
			this->objects.push_back(new Object("Tree2", this->player, -62.f, 665.f));
			this->objects.push_back(new Object("Tree2", this->player, 258.f, 807.f));
			this->objects.push_back(new Object("Tree2", this->player, 1421.f, 819.f));
			this->objects.push_back(new Object("Rock", this->player, 655.f, 937.f));
			this->objects.push_back(new Object("Rock", this->player, 1201.f, 904.f));
			this->objects.push_back(new Object("Rock", this->player, 11.f, 989.f));
			this->objects.push_back(new Object("Rock", this->player, 245.f, 698.f));
			this->objects.push_back(new Object("Rock", this->player, 1782.f, 727.f));
		}

		sortObject(objects);

		this->background.setTexture(&this->backgroundTexture);
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

		if (!this->chat.empty()) {
			this->player->move(0.f, 0.f, dt);
		}

		if (Keyboard::isKeyPressed(Keyboard::B))
		{
			this->states->insert_or_assign("Battle", new BattleState(this->window, this->states, this->player, 1));
			currentState = "Battle";
		}

		if (Keyboard::isKeyPressed(Keyboard::Escape))
		{
			currentState = "MainMenu";
		}
	}

	void updateEvent(const Event& event)
	{
		if (!this->chat.empty())
			this->chat.front()->updateEvent(event, this->mousePosView);
		if (event.type == Event::MouseButtonPressed)
		{
			if (event.mouseButton.button == Mouse::Left) {
				if (!this->chat.empty() && !this->chat.front()->haveButton())
					this->chat.pop_front();
			}
		}
	}

	void update(const float& dt)
	{
		this->updateMousePositions();
		this->updateInput(dt);

		this->player->update(dt);

		for (auto& i : objects)
		{
			if (!i->update(dt)) break;
		}

		for (auto& i : collisions)
		{
			if (!i->update(dt)) break;
		}

		if (!this->chat.empty())
			this->chat.front()->update(this->mousePosView);

		// Map details
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

			//Chatdialog

			if (this->dialogchat == 2 && this->dialog == 1) {
				this->chat.push_back(new ChatDialog(L"ข้ามาตามหาวิธีแก้คำสาปให้คนรัก", "Images/character/MC.png"));
				this->chat.push_back(new ChatDialog(L"คำสาปงั้นเหรอ? ข้าไม่รู้หรอกว่าในป่าแห่งนี้มันจะมีวิธีถอนคำสาปที่เจ้าต้องการรึเปล่าแต่ข้าขอแนะนำเจ้าอย่าง เจ้าอย่าได้เข้าไปเลย เอาชีวิตมาเสี่ยงเสียเปล่าๆ", "Images/character/gurad.png"));
				this->chat.push_back(new ChatDialog(L"ทำไมละ?", "Images/character/MC.png"));
				this->chat.push_back(new ChatDialog(L"เมื่อไม่กี่วันก่อนจู่ๆก็มีพวกโยไคปรากฏตัวที่หมู่บ้านที่อยู่ในป่าแห่งนี้และไล่ฆ่าทุกคนในหมู่บ้าน จนตอนนี้ก็ลามมาถึงป่าแห่งนี้แล้วแต่โชคยังดีที่ทางเข้าป่าแห่งนี้มีอาคมที่ช่วยไล่พวกโยไคเอาไว้อยู่", "Images/character/gurad.png"));
				this->chat.push_back(new ChatDialog(L"แต่ถึงอย่างนั้นข้าก็จะเข้าไป", "Images/character/MC.png"));
				this->chat.push_back(new ChatDialog(L"ทำไมเจ้าถึงได้ดื้อดึงที่จะเข้าไปถึงขนาดนั้นกัน?", "Images/character/gurad.png"));
				this->chat.push_back(new ChatDialog(L"เพราะถ้าข้าไม่รีบจัดการเกี่ยวกับคำสาปละก็ นางคง....", "Images/character/MC.png"));
				this->chat.push_back(new ChatDialog(L"เฮ้อ...ก็ได้ข้าจะปล่อยให้เจ้าเข้าไปแต่ขอบอกเอาไว้ก่อนนะ ถ้าเจ้าตายหรือเป็นอะไรจะไม่มีใครเข้าไปช่วยเจ้าเด็ดขาดเข้าใจมั้ย?", "Images/character/gurad.png"));

				this->dialog = 2;
			}

			if (this->dialogchat == 1 && this->dialog == 1) {
				this->chat.push_back(new ChatDialog(L"ข้ามาตามหาคนรู้จักที่อยู่ในหมู่บ้านข้างหน้านี้", "Images/character/MC.png"));
				this->chat.push_back(new ChatDialog(L"อะไรนะเจ้ามาตามหาคนงั้นเหรอ? งั้นก็ขอบอกเลยว่าโชคร้ายหน่อย", "Images/character/gurad.png"));
				this->chat.push_back(new ChatDialog(L"เกิดเรื่องอะไรขึ้น", "Images/character/MC.png"));
				this->chat.push_back(new ChatDialog(L"เมื่อไม่กี่วันก่อนจู่ๆก็มีพวกโยไคปรากฏตัวที่หมู่บ้านที่อยู่ในป่าแห่งนี้และไล่ฆ่าทุกคนในหมู่บ้าน จนตอนนี้ก็ลามมาถึงป่าแห่งนี้แล้วแต่โชคยังดีที่ทางเข้าป่าแห่งนี้มีอาคมที่ช่วยไล่พวกโยไคเอาไว้อยู่", "Images/character/gurad.png"));
				this->chat.push_back(new ChatDialog(L"ถ้าอย่างนั้นขอข้าเข้าไปดูเพื่อความแน่ใจจะได้มั้ย", "Images/character/MC.png"));
				this->chat.push_back(new ChatDialog(L"ไม่ได้ถึงเจ้าจะเข้าไปแต่ก็ทำอะไรไม่ได้หรอก", "Images/character/gurad.png"));
				this->chat.push_back(new ChatDialog(L"ยังงั้นเหรอ? อะมีโยไคอยู่ข้างหลังเจ้าน่ะ", "Images/character/MC.png"));
				this->chat.push_back(new ChatDialog(L"ฮะ! อยู่ไหนๆ....เฮ้อ ไม่เห็นมีเลยโล่งอกไปที....ทำไมเจ้า เอะ หายไปไหนแล้ว", "Images/character/gurad.png"));

				this->dialog = 2;
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
			if (this->player->getHitboxGlobalBounds().left + this->player->getHitboxGlobalBounds().width > this->window->getView().getSize().x &&
				!(this->player->getHitboxGlobalBounds().top > 500.f && this->player->getHitboxGlobalBounds().top + this->player->getHitboxGlobalBounds().height < 650.f))
			{
				this->player->setPosition(this->window->getView().getSize().x - this->player->getHitboxGlobalBounds().width, this->player->getPosition().y);
			}
			else if (this->player->getHitboxGlobalBounds().left > this->window->getView().getSize().x)
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
			if (this->player->getHitboxGlobalBounds().left + this->player->getHitboxGlobalBounds().width > this->window->getView().getSize().x &&
				!(this->player->getHitboxGlobalBounds().top > 30.f && this->player->getHitboxGlobalBounds().top + this->player->getHitboxGlobalBounds().height < 1080.f))
			{
				this->player->setPosition(this->window->getView().getSize().x - this->player->getHitboxGlobalBounds().width, this->player->getPosition().y);
			}
			else if (this->player->getHitboxGlobalBounds().left > this->window->getView().getSize().x)
			{
				this->player->setPosition(0.f, this->player->getPosition().y + 265.f);
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
			if (this->player->getHitboxGlobalBounds().left + this->player->getHitboxGlobalBounds().left < 0.f)
			{
				this->player->setPosition(this->window->getView().getSize().x - this->player->getHitboxGlobalBounds().width, this->player->getPosition().y - 265.f);
				openWorldState = "Map2_1";
				currentState = "Map2_1";
			}

			//DOWN
			if (this->player->getHitboxGlobalBounds().top + this->player->getHitboxGlobalBounds().height > this->window->getView().getSize().y)
			{
				this->player->setPosition(this->player->getPosition().x, this->window->getView().getSize().y - this->player->getHitboxGlobalBounds().height);
			}

			//UP
			if (this->player->getHitboxGlobalBounds().top < 0.f &&
				!(this->player->getHitboxGlobalBounds().left > 955.f && this->player->getHitboxGlobalBounds().left + this->player->getHitboxGlobalBounds().width < 1090.f))
			{
				this->player->setPosition(this->player->getPosition().x, 0.f);
			}
			else if (this->player->getHitboxGlobalBounds().top + this->player->getHitboxGlobalBounds().height < 0.f)
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
			if (this->player->getHitboxGlobalBounds().top + this->player->getHitboxGlobalBounds().height > this->window->getView().getSize().y &&
				!(this->player->getHitboxGlobalBounds().left > 955.f && this->player->getHitboxGlobalBounds().left + this->player->getHitboxGlobalBounds().width < 1090.f))
			{
				this->player->setPosition(this->player->getPosition().x, this->window->getView().getSize().y - this->player->getHitboxGlobalBounds().height);
			}
			else if (this->player->getHitboxGlobalBounds().top > this->window->getView().getSize().y)
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

			//Chatdialog
			if (this->player->intersects(FloatRect(150.f, 500.f, 100.f, 100.f)) && this->dialog == 0)
			{
				if (this->dialogchat == 2) {
					this->chat.push_back(new ChatDialog(L"ความรู้สึกแบบนี้มันอะไรกัน ราวกับเคยพบเจอมาก่อนเลย", "Images/character/MC.png"));
					this->dialog = 3;
				}
				if (this->dialogchat == 1) {
					this->chat.push_back(new ChatDialog(L"ความรู้สึกแบบนี้ไม่ผิดแน่ต้องเป็นมันแน่นอน", "Images/character/MC.png"));

					this->dialog = 3;
				}
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

			if (this->player->intersects(FloatRect(750.f, 423.f, 100.f, 100.f)) && this->dialog == 0)
			{
				if (this->dialogchat == 2) {
					this->chat.push_back(new ChatDialog(L"หัวกะโหลกขนาดใหญ่นั้นมัน หรือว่า?", "Images/character/MC.png"));
					this->chat.push_back(new ChatDialog(L"ดูเหมือนเจ้าจะรู้ตัวแล้วสินะ ไม่คิดเลยว่าเจ้าจะลืมกลิ่นอายของข้าไป", "Images/character/gyuki.png"));
					this->chat.push_back(new ChatDialog(L"ไม่คิดเลยว่าจะเป็นเจ้า กิวคิ เจ้าเองสินะที่ร่ายคำสาปใส่นาง", "Images/character/MC.png"));
					this->chat.push_back(new ChatDialog(L"ถูกต้อง ข้าเองแหละ ที่เป็นผู้ร่ายคำสาป แต่ก็เพื่อล่อให้เจ้าออกมายังไงละ ข้าจะได้ล้างแค้นในสิ่งที่เจ้าทำกับข้าเอาไว้", "Images/character/gyuki.png"));
					this->chat.push_back(new ChatDialog(L"งั้นเหรอถ้าอย่างนั้น เรามาจบเรื่องกัน!", "Images/character/MC.png"));
					this->dialog = 3;
				}
				if (this->dialogchat == 1) {
					this->chat.push_back(new ChatDialog(L"หัวกะโหลกขนาดใหญ่นี่ กาชาโดคุโระ สินะ กิวคิ", "Images/character/MC.png"));
					this->chat.push_back(new ChatDialog(L"ไม่คิดเลยว่าเจ้าจะรู้ว่าเป็นข้า ตั้งแต่เมื่อไหร่กัน", "Images/character/gyuki.png"));
					this->chat.push_back(new ChatDialog(L"ตั้งแต่ตอนแรกที่เข้ามาในป่าแล้ว เขตแดนแบบนี้ไม่มีใครอื่นนอกจากเจ้าที่ทำได้หรอก กิวคิ", "Images/character/MC.png"));
					this->chat.push_back(new ChatDialog(L"ฮึ งั้นก็ดีไหนๆเจ้าก็รู้ตั้งแต่แรกแล้ว เรามาจบเรื่องของเราตั้งแต่เมื่อร้อยปีก่อนเลยดีกว่า", "Images/character/gyuki.png"));
					this->chat.push_back(new ChatDialog(L"ครั้งนั้นข้ายังอ่อนประสบการณ์จนไม่สามารถจัดการเจ้าได้ แต่ครั้งนี้แหละ ", "Images/character/MC.png"));
					this->chat.push_back(new ChatDialog(L"ถ้าอย่างนั้นก็เข้ามาเลย อาเบะโนะ เซย์เมย์!", "Images/character/gyuki.png"));

					this->dialog = 3;
				}
			}
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
};

class MainMenuState : public State
{
	//Variables
	Texture backgroundTexture;
	RectangleShape background;
	map<string, Button*> buttons;
	Player* player;

	int dialog;

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
		this->dialog = 0;
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
		if (this->player != NULL && !this->player->isdead())
			if (this->buttons["CONTINUE_STATE"]->isPressed())
			{
				currentState = openWorldState;
			}

		if (this->buttons["START_STATE"]->isPressed())
		{
			if (this->player != NULL)
			{
				delete this->player;
				this->player = NULL;
			}

			this->player = new Player();
			this->states->insert_or_assign("Map1_1", new GameState(this->window, this->states, this->player, this->dialog, 11));
			this->states->insert_or_assign("Map1_2", new GameState(this->window, this->states, this->player, this->dialog, 12));
			this->states->insert_or_assign("Map1_3", new GameState(this->window, this->states, this->player, this->dialog, 13));
			this->states->insert_or_assign("Map2_1", new GameState(this->window, this->states, this->player, this->dialog, 21));
			this->states->insert_or_assign("Map2_2", new GameState(this->window, this->states, this->player, this->dialog, 22));
			this->states->insert_or_assign("Map2_3", new GameState(this->window, this->states, this->player, this->dialog, 23));
			this->states->insert_or_assign("Map3_1", new GameState(this->window, this->states, this->player, this->dialog, 31));
			this->states->insert_or_assign("Map3_2", new GameState(this->window, this->states, this->player, this->dialog, 32));
			this->states->insert_or_assign("Map3_3", new GameState(this->window, this->states, this->player, this->dialog, 33));
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
	/*enemyTextures["battleKarakasa"].loadFromFile("Images/on_battle_stage/monmap1_karakasa.png");
	enemyTextures["battleHitosume"].loadFromFile("Images/on_battle_stage/monmap1_boss_oni.png");
	enemyTextures["battleBossOni"].loadFromFile("Images/on_battle_stage/monmap1_karakasa.png");
	enemyTextures["battleKappa"].loadFromFile("Images/on_battle_stage/monmap2_kappa.png");
	enemyTextures["battleAmikiri"].loadFromFile("Images/on_battle_stage/monmap2_amikiri.png");
	enemyTextures["battleBossUmibozu"].loadFromFile("Images/on_battle_stage/monmap2_boss_umibozu.png");
	enemyTextures["battleSkeleton"].loadFromFile("Images/on_battle_stage/monmap3_skeleton.png");
	enemyTextures["battleWanyudo"].loadFromFile("Images/on_battle_stage/monmap3_wanyudo.png");*/

	bool fullscreen = true;
	unsigned antialiasing_level = 5;
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
