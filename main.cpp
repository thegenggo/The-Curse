#include <vector>
#include <stack>
#include <map>
#include <iostream>
#include <sstream>
#include <functional>


#include "SFML/Graphics.hpp"
#include "SFML/System.hpp"
#include "SFML/Window.hpp"
#include "SFML/Audio.hpp"
#include "SFML/Network.hpp"

using namespace std;
using namespace sf;

class Status
{
public:

	//variables
	bool isdead = false;
	float hp = 200;
	float max_hp = 200;
	float att = 110;
	float def = 10;
	float stamina = 100;
	float max_stamina = 100;
	int lvl = 1;
	int current_exp = 0;
	int max_exp = 900;

	//enemy only
	int droprate = 30;
	int dropnum = 1;
	//damage show
	double damageshowtime = 0;
	Text Damageshow;
	bool isdamageshow = false;

	//item
	int itemnum[4] = { 0 };


	//function
	void ItemDrop(Status* recieve, int droprate, int dropnum)
	{
		int chance = rand() % 100 + 1;
		if (chance <= droprate)
		{
			for (int i = 0; i < dropnum; i++)
			{
				int ind = rand() % 4;
				recieve->itemnum[ind]++;
			}
		}
	}
	void isDie()
	{
		if (this->hp <= 0)
		{
			this->isdead = true;
		}
	}
	void UpdatePlayerlevel()
	{
		while (this->current_exp >= max_exp) {
			current_exp -= max_exp;
			this->lvl++;
			max_exp = max_exp + 30 * (this->lvl - 1);
			this->hp = this->hp + 30 * (this->lvl - 1);
			this->max_hp = this->hp;
			this->stamina = this->stamina + 20 * (this->lvl - 1);
			this->max_stamina = this->stamina;
			this->att = this->att + 10 * (this->lvl - 1);
			this->def = this->def + 5 * (this->lvl - 1);
			if (current_exp < max_exp)
				cout << "lvl up!!! " << this->lvl;
		}
	}


	void InitEnemystatus(int stagelevel)
	{
		this->hp = 100.f * stagelevel;
		this->att = 30.f * stagelevel;
		this->def = 10.f * stagelevel;
		this->droprate = 30;
		this->dropnum = 1;
	}


	void attacking(Status* target) {
		int critrate = rand() % 100 + 1;
		float damage;

		if (critrate <= 10) damage = this->att * 2.f - target->def;  //crit
		else damage = this->att - target->def;              //normal

		if (damage <= 0) damage = 0;   //def>atk

		target->hp -= damage;
	}

};

class HitboxComponent
{
public:
	//Variables
	Sprite& sprite;
	RectangleShape hitbox;
	float offsetX;
	float offsetY;

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

	virtual ~HitboxComponent()
	{

	}

	//Accessors
	const FloatRect& getGlobalBounds() const
	{
		return this->hitbox.getGlobalBounds();
	}

	//Functions
	bool checkIntersect(const FloatRect& frect)
	{
		return this->hitbox.getGlobalBounds().intersects(frect);
	}

	void update()
	{
		this->hitbox.setPosition(this->sprite.getPosition().x + this->offsetX, this->sprite.getPosition().y + this->offsetY);
	}

	void render(RenderTarget& target)
	{
		//target.draw(this->hitbox);
	}
};

enum class movementState { IDLE = 0, MOVING_UP, MOVING_LEFT, MOVING_DOWN, MOVING_RIGHT };

class MovementComponent
{
public:
	//Variables
	Sprite& sprite;
	Vector2f velocity;

	float maxVelocity;
	movementState state;

	//Initilizer functions

	//Constructor / Destructor
	MovementComponent(Sprite& sprite, float maxVelocity)
		: sprite(sprite), velocity(Vector2f(0, 0)), maxVelocity(maxVelocity), state(movementState::IDLE)
	{

	}

	virtual ~MovementComponent()
	{

	}

	//Accessors
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
	public:
		//Variables
		Sprite& sprite;
		Texture& textureSheet;
		float animationTimer;
		float timer;
		int width;
		int height;
		bool end;
		vector<IntRect> rect;

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
public:
	//Variables
	Sprite sprite;
	Texture texture;

	HitboxComponent* hitboxComponent;
	MovementComponent* movementComponent;
	AnimationComponent* animationComponent;

	bool isPressthenRelease[2];

	//Initilizer functions
	virtual void initVariables()
	{
		this->hitboxComponent = NULL;
		this->movementComponent = NULL;
		this->isPressthenRelease[0] = false;
		this->isPressthenRelease[1] = false;
	}

	//Constructor / Destructor
	Entity()
	{
		this->initVariables();
		this->createHitboxComponent(this->sprite, 0.f, 0.f, 45.f, 105.f);
		this->createMovementComponent(300.f);
		this->createAnimationComponent(this->texture);
	}

	virtual ~Entity()
	{
		delete this->hitboxComponent;
		delete this->movementComponent;
		delete this->animationComponent;
	}

	//Accessors
	const FloatRect& getGlobalBounds() const
	{
		return this->hitboxComponent->getGlobalBounds();
	}

	const Vector2f& getPosition() const
	{
		return this->sprite.getPosition();
	}

	const bool IsMousePress()
	{
		if (Mouse::isButtonPressed(Mouse::Left))
		{
			return true;
		}
		else return false;
	}

	const bool IsMouseRelease()
	{
		if (!(Mouse::isButtonPressed(Mouse::Left)))
		{
			return true;
		}
		else return false;
	}

	const bool isClickat(const Vector2f mousePos)
	{
		if (this->sprite.getGlobalBounds().contains(mousePos)) {
			if (this->IsMousePress())
			{
				isPressthenRelease[0] = true;
				isPressthenRelease[1] = false;
			}
			if (this->IsMouseRelease())isPressthenRelease[1] = true;

			if (isPressthenRelease[0] and isPressthenRelease[1])
			{

				isPressthenRelease[0] = false;
				isPressthenRelease[1] = false;

				return true;
			}
		}
		return false;
	}

	//Component functions

	void setTexture(Texture& texture)
	{
		this->texture = texture;
		this->sprite.setTexture(texture);
	}

	void createHitboxComponent(Sprite& sprite, float offset_x, float offset_y, float width, float height)
	{
		this->hitboxComponent = new HitboxComponent(sprite, offset_x, offset_y, width, height);
	}

	void createMovementComponent(const float velocity)
	{
		this->movementComponent = new MovementComponent(this->sprite, velocity);
	}

	void createAnimationComponent(Texture& texture_sheet)
	{
		this->animationComponent = new AnimationComponent(this->sprite, texture_sheet);
	}

	//Functions
	void setCursorPosition(const Entity &target)
	{
		this->sprite.setPosition(
			target.getPosition().x + target.getGlobalBounds().width / 2.f,
			target.getPosition().y - target.getGlobalBounds().height);
	}

	virtual void setScale(const float x, const float y)
	{
		this->sprite.setScale(x, y);
	}

	virtual void setOrigin(const float x, const float y)
	{
		this->sprite.setOrigin(x, y);
	}

	virtual void setPosition(const float x, const float y)
	{
		this->sprite.setPosition(x, y);
	}

	virtual void move(const float dir_x, const float dir_y, const float& dt)
	{
		if (this->movementComponent)
		{
			this->movementComponent->move(dir_x, dir_y, dt); //Sets velocity
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

class Player : public Entity
{
public:
	//Variables
	Sprite light;
	Texture lightTexture;

	//Initilizer functions
	void initVariables()
	{

	}

	void initComponents()
	{

	}

	//Constructor / Destructor
	Player(float x, float y, Texture& texture_sheet)
	{
		this->initVariables();

		this->setOrigin(40.f, 10.f);
		this->setPosition(x, y);

		this->createHitboxComponent(this->sprite, 0.f, 0.f, 45.f, 105.f);
		this->createMovementComponent(300.f);
		this->createAnimationComponent(texture_sheet);

		this->lightTexture.loadFromFile("Images/light.png");
		this->light.setTexture(lightTexture);
		this->light.setOrigin(this->light.getGlobalBounds().width / 2, this->light.getGlobalBounds().height / 2);
		this->light.setColor(Color::Transparent);

		this->animationComponent->addAnimation("IDLE", 0.5f, 0, 0, 0, 0, 125, 125);
		this->animationComponent->addAnimation("IDLE_UP", 0.5f, 1, 1, 1, 1, 125, 125);
		this->animationComponent->addAnimation("IDLE_LEFT", 0.5f, 1, 2, 1, 2, 125, 125);
		this->animationComponent->addAnimation("IDLE_DOWN", 0.5f, 1, 3, 1, 3, 125, 125);
		this->animationComponent->addAnimation("IDLE_RIGHT", 0.5f, 1, 4, 1, 4, 125, 125);
		this->animationComponent->addAnimation("UP", 0.5f, 0, 1, 3, 1, 125, 125);
		this->animationComponent->addAnimation("LEFT", 0.5f, 0, 2, 3, 2, 125, 125);
		this->animationComponent->addAnimation("DOWN", 0.5f, 0, 3, 3, 3, 125, 125);
		this->animationComponent->addAnimation("RIGHT", 0.5f, 0, 4, 3, 4, 125, 125);
	}

	virtual ~Player()
	{

	}

	//Component functions
	void setLight(bool x)
	{
		if (x)
			this->light.setColor(Color::Black);
		else
			this->light.setColor(Color::Transparent);
	}

	virtual void update(const float& dt)
	{
		this->movementComponent->update();
		
		switch (this->movementComponent->getState())
		{
		case 1:
			this->animationComponent->play("IDLE_UP", dt);
			this->animationComponent->reset();
			break;
		case 2:
			this->animationComponent->play("IDLE_LEFT", dt);
			this->animationComponent->reset();
			break;
		case 3:
			this->animationComponent->play("IDLE_DOWN", dt);
			this->animationComponent->reset();
			break;
		case 4:
			this->animationComponent->play("IDLE_RIGHT", dt);
			this->animationComponent->reset();
			break;
		case 5:
			this->animationComponent->play("UP", dt);
			break;
		case 6:
			this->animationComponent->play("LEFT", dt);
			break;
		case 7:
			this->animationComponent->play("DOWN", dt);
			break;
		case 8:
			this->animationComponent->play("RIGHT", dt);
			break;
		default:
			this->animationComponent->play("IDLE", dt);
			break;
		}

		this->hitboxComponent->update();
		this->light.setPosition(this->sprite.getPosition().x + this->getGlobalBounds().width / 2, this->sprite.getPosition().y + this->getGlobalBounds().height / 2);
	}


	virtual void render(RenderTarget& target)
	{
		target.draw(this->sprite);
		target.draw(this->light);
	}
};

class CollosionBox
{
public:
	//Variables
	RectangleShape shape;
	RectangleShape& playerShape;
	Player& player;
	RectangleShape up;
	RectangleShape left;
	RectangleShape down;
	RectangleShape right;

	//Constructor / Destructor
	CollosionBox(Player& player, float x, float y, float width, float height)
		:player(player), playerShape(player.hitboxComponent->hitbox)
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

	virtual ~CollosionBox()
	{

	}

	//Functions
	void update()
	{
		if (this->playerShape.getGlobalBounds().intersects(this->up.getGlobalBounds()) && this->player.movementComponent->state == movementState::MOVING_DOWN)
		{
			this->player.setPosition(this->player.getPosition().x, this->shape.getGlobalBounds().top - this->playerShape.getGlobalBounds().height);
		}
		else if (this->playerShape.getGlobalBounds().intersects(this->left.getGlobalBounds()) && this->player.movementComponent->state == movementState::MOVING_RIGHT)
		{
			this->player.setPosition(this->shape.getGlobalBounds().left - this->playerShape.getGlobalBounds().width, this->player.getPosition().y);
		}
		else if (this->playerShape.getGlobalBounds().intersects(this->down.getGlobalBounds()) && this->player.movementComponent->state == movementState::MOVING_UP)
		{
			this->player.setPosition(this->player.getPosition().x, this->shape.getGlobalBounds().top + this->shape.getGlobalBounds().height);
		}
		else if (this->playerShape.getGlobalBounds().intersects(this->right.getGlobalBounds()) && this->player.movementComponent->state == movementState::MOVING_LEFT)
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
public:
	//Variables
	RenderWindow* window;
	vector<State*>* states;
	map<string, int>* supportedKeys;
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

	//Constructor / Destructor
	State(RenderWindow* window, vector<State*>* states)
	{
		this->window = window;
		this->supportedKeys = supportedKeys;
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
	virtual void endStateUpdate()
	{

	}

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

	virtual void update(const float& dt)
	{

	}

	virtual void render(RenderTarget* target = NULL)
	{

	}
};

namespace temp {

	Status* TempStatus = new Status;
}

enum class button_states { BTN_IDLE, BTN_HOVER, BTN_ACTIVE };

class Button
{
public:
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

	bool isPressthenRelease[3] = { false,false,false };

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
	}

	virtual ~Button()
	{

	}

	//Accessors
	const bool isPressed() const
	{
		if (this->buttonState == button_states::BTN_ACTIVE)
			return true;

		return false;
	}
	//check click
	const bool IsMousePress() const
	{
		if (Mouse::isButtonPressed(Mouse::Left))
		{
			return true;
		}
		else return false;
	}

	const bool IsMouseRelease() const
	{
		if (!(Mouse::isButtonPressed(Mouse::Left)))
		{
			return true;
		}
		else return false;
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


			if (this->IsMousePress())
			{
				isPressthenRelease[0] = true;
				isPressthenRelease[1] = false;
			}
			if (this->IsMouseRelease())isPressthenRelease[1] = true;

			if (isPressthenRelease[0] and isPressthenRelease[1])
			{
				this->buttonState = button_states::BTN_ACTIVE;
				isPressthenRelease[0] = false;
				isPressthenRelease[1] = false;
			}
		}
		else {
			isPressthenRelease[0] = false;
			isPressthenRelease[1] = false;
		}

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

class BattleState : public State, Status {

public:
	//variables
	Texture bgtexture;
	RectangleShape background;
	Font font;

	//status
	Status playerstatus, * enemystatus, * ptplayerstatus;
	Status* targetstatus;
	int deadeneycount = 0;
	//target cursor
	Entity targetCursor, * targetentity;
	Texture targetCursortex;


	//button
	map<string, Button*> Mainbuttons, Itembuttons, * currentbutton;

	//entity player&ene
	Player* player;
	Entity* enemy;
	int enemynum = 3;     //enemy number
	Texture playertexture, enemytexture[3];
	float enemyposition[3][2] = { {1400.f,200.f},{1400.f,400.f},{1400.f,600.f} };

	//round
	int round = 0;
	int roundlimit;
	bool isroundclear = false;

	//stage 
	bool  isstageclear = false;
	int stagelevel;
	bool isbossstage;

	//turn 
	bool isPlayerturn;
	bool isEnemystop;

	//item window
	RectangleShape Itemwindow;
	Texture ItemwindowTex;
	bool isItemwindowActive;



	//iniilizer function

	void initItemwindow()
	{
		if (!this->ItemwindowTex.loadFromFile("Images/Itemwindow.jpg"))
		{
			throw "ERROR::MAIN_MENU_STATE::FAILED_TO_LOAD_BACKGROUND_TEXTURE";
		}

		this->Itemwindow.setSize(Vector2f(300.f, 700.f));
		this->Itemwindow.setTexture(&this->ItemwindowTex);
		this->Itemwindow.setPosition(Vector2f(700.f, 100.f));

		this->isItemwindowActive = false;
	}
	void initPlayer()
	{
		//turn
		this->isPlayerturn = true;
		this->isEnemystop = false;

		//load player status

		this->playerstatus = *temp::TempStatus;
		ptplayerstatus = &playerstatus;


		if (!this->playertexture.loadFromFile("Images/Sprites/Player/test.png"))
		{
			throw "ERROR::MAIN_MENU_STATE::FAILED_TO_LOAD_BACKGROUND_TEXTURE";
		}

		this->player = new Player(200.f, 200.f, this->playertexture);
	}

	void initEnemy()
	{
		this->enemy = new Entity[enemynum];
		this->enemystatus = new Status[enemynum];
		this->addEnemy();
	}
	void changeEnemy()
	{
		for (int i = 0; i < 3; i++)this->enemyposition[i][0] -= 10;   //change location for test
		delete[] this->enemy;
		delete[] this->enemystatus;

		this->enemystatus = new Status[enemynum];
		this->enemy = new Entity[enemynum];
	}

	void addEnemy()
	{

		for (int i = 0; i < enemynum; i++)
		{
			if (!this->enemytexture[i].loadFromFile("Images/monster_on_map/map1/hitotsume.png"))
			{
				throw "ERROR::MAIN_MENU_STATE::FAILED_TO_LOAD_BACKGROUND_TEXTURE";
			}

			this->enemy[i].setPosition(this->enemyposition[i][0], this->enemyposition[i][1]);
			this->enemy[i].setTexture(this->enemytexture[i]);

			//init enemystatus
			this->enemystatus[i].InitEnemystatus(this->stagelevel);
			//init enemy damage taken 
			this->enemystatus[i].Damageshow.setPosition(this->enemyposition[i][0] - 20, this->enemyposition[i][1] - 20);
			this->enemystatus[i].Damageshow.setFont(this->font);
			this->enemystatus[i].Damageshow.setString("");
			this->enemystatus[i].Damageshow.setFillColor(Color::Red);
			this->enemystatus[i].Damageshow.setCharacterSize(40);


		}

		//set default target
		this->targetstatus = &enemystatus[0];
		this->targetentity = &enemy[0];
		//target curser
		if (!this->targetCursortex.loadFromFile("Images/targetcursor.png"))
		{
			throw "ERROR::MAIN_MENU_STATE::FAILED_TO_LOAD_BACKGROUND_TEXTURE";
		}
		this->targetCursor.setTexture(this->targetCursortex);
		this->targetCursor.setCursorPosition(*this->targetentity);
	}

	void initVariables(int round, int lvl, bool isboss)
	{
		this->roundlimit = round;
		this->stagelevel = lvl;
		this->isbossstage = isboss;
	}
	void initBackground()
	{

		if (!this->bgtexture.loadFromFile("Images/battle_scene/map1.png"))
		{
			throw "ERROR::MAIN_MENU_STATE::FAILED_TO_LOAD_BACKGROUND_TEXTURE";
		}

		this->background.setSize(Vector2f(window->getSize()));
		this->background.setTexture(&this->bgtexture);

	}
	void initFonts()
	{
		if (!this->font.loadFromFile("Fonts/2005_iannnnnJPG.ttf"))
		{
			throw("ERROR::MAINMENUSTATE::COULD NOT LOAD FONT");
		}
	}

	void initButtons()
	{
		this->Mainbuttons["ATTACK"] = new Button(100, 600, 125, 50, &this->font, "Attack", 20, Color(70, 70, 70, 200), Color(250, 250, 250, 250), Color(20, 20, 20, 50),
			Color(70, 70, 70, 0), Color(150, 150, 150, 0), Color(20, 20, 20, 0));
		this->Mainbuttons["ITEM"] = new Button(300, 600, 125, 50, &this->font, "ITEM", 20, Color(70, 70, 70, 200), Color(250, 250, 250, 250), Color(20, 20, 20, 50),
			Color(70, 70, 70, 0), Color(150, 150, 150, 0), Color(20, 20, 20, 0));
		this->Mainbuttons["SKILL"] = new Button(500, 600, 125, 50, &this->font, "SKILL", 20, Color(70, 70, 70, 200), Color(250, 250, 250, 250), Color(20, 20, 20, 50),
			Color(70, 70, 70, 0), Color(150, 150, 150, 0), Color(20, 20, 20, 0));
		this->Mainbuttons["RUN"] = new Button(700, 600, 125, 50, &this->font, "RUN", 20, Color(70, 70, 70, 200), Color(250, 250, 250, 250), Color(20, 20, 20, 50),
			Color(70, 70, 70, 0), Color(150, 150, 150, 0), Color(20, 20, 20, 0));

		this->Itembuttons["ITEM1"] = new Button(900, 50, 125, 50, &this->font, "item1", 20, Color(70, 70, 70, 200), Color(250, 250, 250, 250), Color(20, 20, 20, 50),
			Color(70, 70, 70, 0), Color(150, 150, 150, 0), Color(20, 20, 20, 0));
		this->Itembuttons["ITEM2"] = new Button(900, 150, 125, 50, &this->font, "ITEM2", 20, Color(70, 70, 70, 200), Color(250, 250, 250, 250), Color(20, 20, 20, 50),
			Color(70, 70, 70, 0), Color(150, 150, 150, 0), Color(20, 20, 20, 0));
		this->Itembuttons["ITEM3"] = new Button(900, 250, 125, 50, &this->font, "item3", 20, Color(70, 70, 70, 200), Color(250, 250, 250, 250), Color(20, 20, 20, 50),
			Color(70, 70, 70, 0), Color(150, 150, 150, 0), Color(20, 20, 20, 0));
		this->Itembuttons["ITEM4"] = new Button(900, 350, 125, 50, &this->font, "item4", 20, Color(70, 70, 70, 200), Color(250, 250, 250, 250), Color(20, 20, 20, 50),
			Color(70, 70, 70, 0), Color(150, 150, 150, 0), Color(20, 20, 20, 0));
		this->Itembuttons["EXITITEM"] = new Button(900, 450, 125, 50, &this->font, "exit", 20, Color(70, 70, 70, 200), Color(250, 250, 250, 250), Color(20, 20, 20, 50),
			Color(70, 70, 70, 0), Color(150, 150, 150, 0), Color(20, 20, 20, 0));

		//init curerent button
		currentbutton = &this->Mainbuttons;
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
	}

	//Constructor / Destructor
	BattleState(RenderWindow* window, vector<State*>* states, int Maxround, int Stagelevel, bool Isbossstage)
		: State(window, states)
	{
		this->initVariables(Maxround, Stagelevel, Isbossstage);
		this->initPlayer();
		this->initEnemy();
		this->initBackground();
		this->initFonts();
		this->initButtons();
		this->initItemwindow();
	}

	virtual ~BattleState()
	{
		this->deleteButtons();
	}


	//Functions
	void UpdateAfterAttackStatus()
	{
		targetstatus->isDie();
		if (targetstatus->isdead)
		{
			//update Item drop
			this->ItemDrop(&this->playerstatus, targetstatus->droprate, targetstatus->dropnum);
			cout << "Item now " << this->playerstatus.itemnum[0] << this->playerstatus.itemnum[1]
				<< this->playerstatus.itemnum[2] << this->playerstatus.itemnum[3] << endl;

			//set auto next target
			for (int i = 0; i < enemynum; i++)
			{
				if (this->enemystatus[i].isdead == false)
				{
					targetstatus = &enemystatus[i];
					targetentity = &enemy[i];
					this->targetCursor.setCursorPosition(*this->targetentity);
				}
			}
			//update player status
			this->deadeneycount++;
			this->playerstatus.current_exp += 150;
			this->playerstatus.UpdatePlayerlevel();
			cout << this->playerstatus.current_exp << " " << this->playerstatus.lvl << " " << this->playerstatus.att << endl;

		}

		if (this->deadeneycount == this->enemynum)
		{
			this->isroundclear = true;
			round++;
			deadeneycount = 0;
		}

		if (this->isroundclear)
		{
			cout << "round clear " << this->round << endl;
			this->changeEnemy();
			this->addEnemy();
			this->isroundclear = false;
			this->isEnemystop = true;
		}

		if (this->round == this->roundlimit)
		{
			cout << "stage clear" << endl;
			this->isstageclear = true;
		}

		if (this->isstageclear)
		{
			this->isEnemystop = true;
			this->isstageclear = false;
			//backup player data
			*temp::TempStatus = playerstatus;
			this->endState(); //go back to game state
		}
	}
	void UpdateEnemyAttack()
	{
		//enemy turn
		if (!(this->isPlayerturn) and !(this->isEnemystop))
		{
			for (int i = 0; i < 3; i++)
			{
				if (!(this->enemystatus[i].isdead))
				{
					this->enemystatus[i].attacking(ptplayerstatus);
					cout << "enemy " << i << " attack " << "Player Hp remain " << this->playerstatus.hp << endl;

					//check player dead
					this->playerstatus.isDie();
					if (this->playerstatus.isdead)
					{
						cout << "You are dead" << endl;
						this->endState();
						break;

					}
				}
			}
			//chang to player turn
			this->isPlayerturn = true;
		}
	}
	void UpdateAttack()
	{
		//Attack button only
		//Update player input  oon turn
		if (this->Mainbuttons["ATTACK"]->isPressed() and this->isPlayerturn)
		{
			//attack
			auto targetindex = targetstatus - &enemystatus[0];
			this->playerstatus.attacking(targetstatus);
			cout << "enemy " << targetindex << " " << targetstatus->hp << endl;
			//show damage
			this->enemystatus[targetindex].Damageshow.setString("99");
			this->enemystatus[targetindex].isdamageshow = true;

			//set enemy can attack
			this->isEnemystop = false;
			//change turn to enemy 
			this->isPlayerturn = false;
		}
		//skill zone

			//update status after attack
		this->UpdateAfterAttackStatus();
		//enemy attack
		this->UpdateEnemyAttack();

		//chang turn to player while enemy stop
		if (this->isEnemystop)this->isPlayerturn = true;
	}
	void UpdateItemUse()
	{
		string itemname[4] = { "ITEM1","ITEM2","ITEM3","ITEM4" };
		for (int i = 0; i < 4; i++)
		{
			if (this->Itembuttons[itemname[i]]->isPressed() and this->isPlayerturn)
			{
				if (this->playerstatus.itemnum[i] <= 0)
				{
					cout << "Item " << i << " not enough " << endl;
					continue;
				}
				switch (i)
				{
				case 0:this->playerstatus.att += 0.2f * this->playerstatus.att;
					cout << "effect +att " << endl;
					break;
				case 1:this->playerstatus.def += 0.2f * this->playerstatus.def;
					cout << "effect +deff" << endl;
					break;
				case 2:this->playerstatus.hp += 0.5f * this->playerstatus.max_hp;
					cout << "effect +hp" << endl;
					break;
				case 3:this->playerstatus.stamina += 0.5f * this->playerstatus.max_stamina;
					cout << "effect +stamina" << endl;
					break;

				}
				cout << "Use Item " << i << endl;
				this->playerstatus.itemnum[i]--;
				//Item disappear after use ; unlike playerstatus if dead-> status back to before battle
				temp::TempStatus->itemnum[i]--;
				cout << "Item" << i << " remain " << playerstatus.itemnum[i] << endl;
			}
		}

	}
	void PlayerSetTarget()
	{
		//set target
		if (this->enemy[0].isClickat(mousePosView))
		{
			targetstatus = &enemystatus[0];
			targetentity = &enemy[0];
			this->targetCursor.setCursorPosition(*this->targetentity);
			cout << "Click enemy 0" << endl;
		}
		if (this->enemy[1].isClickat(mousePosView))
		{
			targetstatus = &enemystatus[1];
			targetentity = &enemy[1];
			this->targetCursor.setCursorPosition(*this->targetentity);
			cout << "Click enemy 1" << endl;
		}if (this->enemy[2].isClickat(mousePosView))
		{
			targetstatus = &enemystatus[2];
			targetentity = &enemy[2];
			this->targetCursor.setCursorPosition(*this->targetentity);
			cout << "Click enemy 2" << endl;
		}
	}

	void updateInput(const float& dt)
	{
		//player set target by click 
		this->PlayerSetTarget();
		//update attack zone
		this->UpdateAttack();
		//update if Item use
		this->UpdateItemUse();

	}

	void updateButtons()
	{

		/*Updates all the buttons in the state and handles their functionlaity.*/
		for (auto& it : *currentbutton)
		{
			it.second->update(this->mousePosView);
		}

		//ITem 
		if (this->Mainbuttons["ITEM"]->isPressed())
		{
			this->isItemwindowActive = true;
			this->Mainbuttons["ITEM"]->buttonState = button_states::BTN_IDLE;
			currentbutton = &this->Itembuttons;


		}
		//close Item 
		if (this->Itembuttons["EXITITEM"]->isPressed())
		{
			this->isItemwindowActive = false;
			this->Itembuttons["EXITITEM"]->buttonState = button_states::BTN_IDLE;
			currentbutton = &this->Mainbuttons;
		}

		//Quit the game
		if (this->Mainbuttons["RUN"]->isPressed())
		{
			this->endState(); // back to game state
		}
	}

	void update(const float& dt)
	{
		//enviornment update zone
		this->updateMousePositions();
		this->updateButtons();

		//player update zone
		this->updateInput(dt);

		this->player->update(dt);

		//enemy update zome
		for (int i = 0; i < enemynum; i++)this->enemy[i].update(dt);

		//update texture
		for (int i = 0; i < enemynum; i++)this->SetIdleText(
			&this->enemystatus[i].Damageshow, this->enemystatus[i].damageshowtime
			, this->enemystatus[i].isdamageshow, dt);
	}

	void SetIdleText(Text* target, double& timecount, bool& isDamageshow, double dt)
	{
		if (timecount >= 1 and isDamageshow)
		{
			target->setString("");
			isDamageshow = false;
			timecount = 0;
		}
		timecount += dt;
	}


	void renderButtons(RenderTarget* target = NULL)
	{
		for (auto& it : *currentbutton)
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
		for (int i = 0; i < enemynum; i++)this->enemy[i].render(*target);
		//enemy target cursor
		this->targetCursor.render(*target);
		//render damage to enemy
		for (int i = 0; i < enemynum; i++)target->draw(this->enemystatus[i].Damageshow);

		//render item window
		if (this->isItemwindowActive)target->draw(this->Itemwindow);
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

class GameState : public State
{
public:
	//Variables
	Player* player;
	RectangleShape background;
	CollosionBox* box;

	//Initilizer functions
	void initTexture()
	{
		if (!this->textures["PLAYER_SHEET"].loadFromFile("Images/Sprites/Player/test.png"))
			throw "ERROR::GAME_STATE::COULD_NOT_LOAD_PLAYER_TEXTURE";
	}

	void initPlayers()
	{
		this->player = new Player(0, 0, this->textures["PLAYER_SHEET"]);
		this->player->setLight(false);
	}

	void initCollosionBox()
	{
		this->box = new CollosionBox(*this->player, 500.f, 500.f, 0.01f, 0.01f);
	}

	//Constructor / Destructor
	GameState(RenderWindow* window, vector<State*>* states)
		: State(window, states)
	{
		this->background.setFillColor(Color::White);
		this->background.setSize(Vector2f(window->getSize()));
		this->initTexture();
		this->initPlayers();
		this->initCollosionBox();
		this->initFonts();
	}

	virtual ~GameState()
	{
		delete this->box;
		delete this->player;
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
			this->states->push_back(new BattleState(this->window, this->states, 2, 1, false));
		}

		if (Keyboard::isKeyPressed(Keyboard::Escape))
		{
			this->endState();
		}
	}

	void update(const float& dt)
	{
		this->updateMousePositions();
		this->updateInput(dt);

		if (this->player->getGlobalBounds().left + this->player->getGlobalBounds().width > this->window->getSize().x)
		{
			this->player->setPosition(this->window->getSize().x - this->player->getGlobalBounds().width, this->player->getPosition().y);
		}

		if (this->player->getGlobalBounds().left < 0)
		{
			this->player->setPosition(0, this->player->getPosition().y);
		}

		if (this->player->getGlobalBounds().top + this->player->getGlobalBounds().height > this->window->getSize().y)
		{
			this->player->setPosition(this->player->getPosition().x, this->window->getSize().y - this->player->getGlobalBounds().height);
		}

		if (this->player->getGlobalBounds().top < 0)
		{
			this->player->setPosition(this->player->getPosition().x, 0);
		}

		this->player->update(dt);
		this->box->update();
	}

	void render(RenderTarget* target = NULL)
	{
		if (!target)
			target = this->window;

		target->draw(background);
		this->player->render(*target);
		this->box->render(*target);

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
public:
	//Variables
	Texture backgroundTexture;
	RectangleShape background;

	map<string, Button*> buttons;

	//Initializer functions
	void initVariables()
	{

	}

	void initBackground()
	{

		if (!this->backgroundTexture.loadFromFile("Images/Backgrounds/mainmanu.jpg"))
			throw "ERROR::MAIN_MENU_STATE::FAILED_TO_LOAD_BACKGROUND_TEXTURE";

		this->background.setSize(Vector2f(this->window->getSize()));
		this->background.setTexture(&this->backgroundTexture);
	}

	void initButtons()
	{
		this->buttons["GAME_STATE"] = new Button(835, 500, 250, 50, &this->font, "��������", 50,
			Color(70, 70, 70, 200), Color(250, 250, 250, 250), Color(20, 20, 20, 50),
			Color(70, 70, 70, 0), Color(150, 150, 150, 0), Color(20, 20, 20, 0));

		this->buttons["BATTLE_STATE"] = new Button(835, 600, 250, 50, &this->font, "�֡������", 50,
			Color(70, 70, 70, 200), Color(250, 250, 250, 250), Color(20, 20, 20, 50),
			Color(70, 70, 70, 0), Color(150, 150, 150, 0), Color(20, 20, 20, 0));

		this->buttons["EXIT_STATE"] = new Button(835, 700, 250, 50, &this->font, "�͡�ҡ��", 50,
			Color(70, 70, 70, 200), Color(250, 250, 250, 250), Color(20, 20, 20, 50),
			Color(100, 100, 100, 0), Color(150, 150, 150, 0), Color(20, 20, 20, 0));
	}

	//Constructor / Destructor
	MainMenuState(RenderWindow* window, vector<State*>* states)
		: State(window, states)
	{
		this->initVariables();
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
		/*Updates all the buttons in the state and handles their functionlaity.*/
		for (auto& it : this->buttons)
		{
			it.second->update(this->mousePosView);
		}

		//New game
		if (this->buttons["GAME_STATE"]->isPressed())
		{
			this->states->push_back(new GameState(window, states));
		}

		//Quit the game
		if (this->buttons["EXIT_STATE"]->isPressed())
		{
			this->window->close();
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
		for (auto& it : this->buttons)
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

Event event;

int main()
{
	bool fullscreen = true;
	unsigned antialiasing_level = 0;
	float speed = 10;
	float dt = 0;

	Clock dtClock;

	ContextSettings windowSettings;
	windowSettings.antialiasingLevel = antialiasing_level;

	RenderWindow* window;
	if (fullscreen)
		window = new RenderWindow(VideoMode(1920, 1080), "The Curse", Style::Fullscreen, windowSettings);
	else
		window = new RenderWindow(VideoMode(1920, 1080), "The Curse", Style::Default, windowSettings);
	window->setFramerateLimit(60);
	window->setVerticalSyncEnabled(true);

	vector<State*> states;
	states.push_back(new MainMenuState(window, &states));

	while (window->isOpen())
	{
		//Update
		dt = dtClock.restart().asSeconds();

		while (window->pollEvent(event))
		{
			if (event.type == Event::Closed)
				window->close();
		}

		if (!states.empty())
		{
			states.back()->update(dt);

			if (states.back()->getQuit())
			{
				states.back()->endState();
				delete states.back();
				states.pop_back();
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
			states.back()->render();
		}

		window->display();
	}

	delete window;

	while (!states.empty())
	{
		delete states.back();
		states.pop_back();
	}

	return 0;
}
