#include <vector>
#include <stack>
#include <map>
#include <iostream>
#include <sstream>

#include "SFML/Graphics.hpp"
#include "SFML/System.hpp"
#include "SFML/Window.hpp"
#include "SFML/Audio.hpp"
#include "SFML/Network.hpp"

using namespace std;
using namespace sf;

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
		target.draw(this->hitbox);
	}
};

class MovementComponent
{
public:
	//Variables
	Sprite& sprite;
	Vector2f velocity;

	enum movementState { IDLE = 0, MOVING_UP, MOVING_LEFT, MOVING_DOWN, MOVING_RIGHT };
	float maxVelocity;
	short unsigned state;

	//Initilizer functions

	//Constructor / Destructor
	MovementComponent(Sprite& sprite, float maxVelocity)
		: sprite(sprite), velocity(Vector2f(0, 0)), maxVelocity(maxVelocity), state(IDLE)
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
			case MOVING_UP:
				return 1;
				break;
			case MOVING_LEFT:
				return 2;
				break;
			case MOVING_DOWN:
				return 3;
				break;
			case MOVING_RIGHT:
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
			this->state = MOVING_UP;
		else if (this->velocity.x < 0)
			this->state = MOVING_LEFT;
		else if (this->velocity.y > 0)
			this->state = MOVING_DOWN;
		else if (this->velocity.x > 0)
			this->state = MOVING_RIGHT;
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
		IntRect startRect;
		IntRect currentRect;
		IntRect endRect;

		Animation(Sprite& sprite, Texture& textureSheet, float animation_timer, int start_frame_x, int start_frame_y, int frames_x, int frames_y, int width, int height)
			: sprite(sprite), textureSheet(textureSheet), animationTimer(animation_timer), width(width), height(height)
		{
			this->timer = 0.f;
			this->startRect = IntRect(start_frame_x * width, start_frame_y * height, width, height);
			this->currentRect = this->startRect;
			this->endRect = IntRect(frames_x * width, frames_y * height, width, height);

			this->sprite.setTexture(this->textureSheet, true);
			this->sprite.setTextureRect(this->startRect);
		}

		//Functions
		void play(const float& dt)
		{
			//Update timer
			this->timer += 10.f * dt;
			if (this->timer >= this->animationTimer)
			{
				//reset timer
				this->timer = 0.f;

				if (this->currentRect != this->endRect)
				{
					this->currentRect.left += this->width;
				}
				else //Reset
				{
					this->currentRect.left = this->startRect.left;
				}

				this->sprite.setTextureRect(this->currentRect);
			}
		}

		void reset()
		{
			this->timer = 0.f;
			this->currentRect = this->startRect;
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

	void play(const string key, const float& dt)
	{
		this->animations[key]->play(dt);
	}
};

class Entity
{
public:
	//Variables
	Sprite sprite;
	Texture* texture;

	HitboxComponent* hitboxComponent;
	MovementComponent* movementComponent;
	AnimationComponent* animationComponent;

	//Initilizer functions
	virtual void initVariables()
	{
		this->hitboxComponent = NULL;
		this->texture = NULL;
		this->movementComponent = NULL;
	}

	//Constructor / Destructor
	Entity()
	{
		this->initVariables();
	}

	virtual ~Entity()
	{
		delete this->hitboxComponent;
		delete this->movementComponent;
		delete this->animationComponent;
	}

	//Accessors
	const bool 

	//Component functions
	void setTexture(Texture& texture)
	{
		this->texture = &texture;
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

		if (this->hitboxComponent)
			this->hitboxComponent->render(target);
	}
};

class Player : public Entity
{
public:
	//Variables

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

		this->createHitboxComponent(this->sprite, 0.f, 0.f, 45.f, 110.f);
		this->createMovementComponent(300.f);
		this->createAnimationComponent(texture_sheet);

		this->animationComponent->addAnimation("IDLE", 0.f, 0, 0, 0, 0, 125, 125);
		this->animationComponent->addAnimation("IDLE_UP", 1.f, 1, 1, 1, 1, 125, 125);
		this->animationComponent->addAnimation("IDLE_LEFT", 1.f, 1, 2, 1, 2, 125, 125);
		this->animationComponent->addAnimation("IDLE_DOWN", 1.f, 1, 3, 1, 3, 125, 125);
		this->animationComponent->addAnimation("IDLE_RIGHT", 1.f, 1, 4, 1, 4, 125, 125);
		this->animationComponent->addAnimation("UP", 1.f, 0, 1, 3, 1, 125, 125);
		this->animationComponent->addAnimation("LEFT", 1.f, 0, 2, 3, 2, 125, 125);
		this->animationComponent->addAnimation("DOWN", 1.f, 0, 3, 3, 3, 125, 125);
		this->animationComponent->addAnimation("RIGHT", 1.f, 0, 4, 3, 4, 125, 125);
	}

	virtual ~Player()
	{

	}

	//Functions
	virtual void update(const float& dt)
	{
		this->movementComponent->update();

		switch (this->movementComponent->getState())
		{
		case 1:
			this->animationComponent->play("IDLE_UP", dt);
			break;
		case 2:
			this->animationComponent->play("IDLE_LEFT", dt);
			break;
		case 3:
			this->animationComponent->play("IDLE_DOWN", dt);
			break;
		case 4:
			this->animationComponent->play("IDLE_RIGHT", dt);
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
	}
};

class CollosionBox
{
public:
	//Variables
	RectangleShape shape;
	RectangleShape& playerShape;
	Player& player;

	//Constructor / Destructor
	CollosionBox(Player& player, float x, float y, float width, float height)
		:player(player), playerShape(player.hitboxComponent->hitbox)
	{
		this->shape.setPosition(x, y);
		this->shape.setSize(Vector2f(width, height));
		this->shape.setFillColor(Color::Black);
		this->shape.setOutlineThickness(1.f);
		this->shape.setOutlineColor(Color::Black);
	}

	virtual ~CollosionBox()
	{

	}

	//Functions
	void update()
	{
		if (this->shape.getGlobalBounds().intersects(playerShape.getGlobalBounds()))
		{
			if (this->playerShape.getGlobalBounds().top - this->shape.getGlobalBounds().top <= this->shape.getGlobalBounds().height
				&& (this->playerShape.getGlobalBounds().left + this->playerShape.getGlobalBounds().width >= this->shape.getGlobalBounds().left
					|| this->playerShape.getGlobalBounds().left <= this->shape.getGlobalBounds().left + this->shape.getGlobalBounds().width))
				this->player.setPosition(this->playerShape.getGlobalBounds().left, this->shape.getGlobalBounds().top + this->shape.getGlobalBounds().height);
			
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
	stack<State*>* states;
	map<string, int>* supportedKeys;
	map<string, int> keybinds;
	bool quit;

	Vector2i mousePosScreen;
	Vector2i mousePosWindow;
	Vector2f mousePosView;

	//Resources
	map<string, Texture> textures;

	//Constructor / Destructor
	State(RenderWindow* window, stack<State*>* states)
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

class BattleState : public State
{
public:
	//Variables
	RectangleShape background;

	//Constructor / Destructor
	BattleState(RenderWindow* window, stack<State*>* states)
		: State(window, states)
	{
		this->background.setFillColor(Color::White);
		this->background.setSize(Vector2f(window->getSize()));
	}

	virtual ~BattleState()
	{

	}

	//Functions
	void updateInput(const float& dt)
	{
		//Update player input
		if (Keyboard::isKeyPressed(Keyboard::Key::Escape))
			this->endState();
	}

	void update(const float& dt)
	{
		this->updateInput(dt);
	}

	void render(RenderTarget* target = NULL)
	{
		if (!target)
			target = this->window;

		target->draw(background);
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
		{
			throw "ERROR::GAME_STATE::COULD_NOT_LOAD_PLAYER_TEXTURE";
		}
	}

	void initPlayers()
	{
		this->player = new Player(0, 0, this->textures["PLAYER_SHEET"]);
	}

	void initCollosionBox()
	{
		this->box = new CollosionBox(*this->player,500, 500, 100, 300);
	}

	//Constructor / Destructor
	GameState(RenderWindow* window, stack<State*>* states)
		: State(window, states)
	{
		this->background.setFillColor(Color::White);
		this->background.setSize(Vector2f(window->getSize()));
		this->initTexture();
		this->initPlayers();
		this->initCollosionBox();
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
		if (Keyboard::isKeyPressed(Keyboard::Escape))
			this->endState();
	}

	void update(const float& dt)
	{
		this->updateMousePositions();
		this->updateInput(dt);

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
	}
};

enum button_states { BTN_IDLE = 0, BTN_HOVER, BTN_ACTIVE };

class Button
{
public:
	short unsigned buttonState;

	RectangleShape shape;
	Font* font;
	Text text;

	Color textIdleColor;
	Color textHoverColor;
	Color textActiveColor;

	Color idleColor;
	Color hoverColor;
	Color activeColor;

	Button(float x, float y, float width, float height, Font* font, string text, unsigned character_size, Color text_idle_color, Color text_hover_color, Color text_active_color,
		Color idle_color, Color hover_color, Color active_color)
	{
		this->buttonState = BTN_IDLE;

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
		if (this->buttonState == BTN_ACTIVE)
			return true;

		return false;
	}

	//Functions
	void update(const Vector2f mousePos)
	{
		//Update the booleans for hover and pressed

		//Idle
		this->buttonState = BTN_IDLE;

		//Hover
		if (this->shape.getGlobalBounds().contains(mousePos))
		{
			this->buttonState = BTN_HOVER;

			if (Mouse::isButtonPressed(Mouse::Left))
			{
				this->buttonState = BTN_ACTIVE;
			}
		}

		switch (this->buttonState)
		{
		case BTN_IDLE:
			this->shape.setFillColor(this->idleColor);
			this->text.setFillColor(this->textIdleColor);
			break;

		case BTN_HOVER:
			this->shape.setFillColor(this->hoverColor);
			this->text.setFillColor(this->textHoverColor);
			break;

		case BTN_ACTIVE:
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

class MainMenuState : public State
{
public:
	//Variables
	Texture backgroundTexture;
	RectangleShape background;
	Font font;

	map<string, Button*> buttons;

	//Initializer functions
	void initVariables()
	{

	}

	void initBackground()
	{

		if (!this->backgroundTexture.loadFromFile("Images/Backgrounds/mainmanu.jpg"))
		{
			throw "ERROR::MAIN_MENU_STATE::FAILED_TO_LOAD_BACKGROUND_TEXTURE";
		}

		this->background.setSize(Vector2f(this->window->getSize()));
		this->background.setTexture(&this->backgroundTexture);

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
	MainMenuState(RenderWindow* window, stack<State*>* states)
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
			this->states->push(new GameState(this->window, this->states));
		}

		//Quit the game
		if (this->buttons["EXIT_STATE"]->isPressed())
		{
			this->endState();
		}

		if (this->buttons["BATTLE_STATE"]->isPressed())
		{
			this->states->push(new BattleState(this->window, this->states));
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

		this->renderButtons(*target);
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

int main()
{
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
		window = new RenderWindow(VideoMode(1920, 1080), "The Curse", Style::Fullscreen, windowSettings);
	else
		window = new RenderWindow(VideoMode(1920, 1080), "The Curse", Style::Default, windowSettings);
	window->setFramerateLimit(60);
	window->setVerticalSyncEnabled(true);

	vector<VideoMode> videoModes;
	videoModes = VideoMode::getFullscreenModes();

	stack<State*> states;
	states.push(new MainMenuState(window, &states));

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
			states.top()->update(dt);

			if (states.top()->getQuit())
			{
				states.top()->endState();
				delete states.top();
				states.pop();
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
			states.top()->render();
		}

		window->display();
	}

	delete window;

	while (!states.empty())
	{
		delete states.top();
		states.pop();
	}

	return 0;
}
