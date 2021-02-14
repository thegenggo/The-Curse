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

class Entity
{
public:
	//Variables
	RectangleShape shape;
	float movementSpeed;

	//Constructor / Destructor
	Entity()
	{
		this->shape.setSize(Vector2f(50.f, 50.f));
		this->shape.setFillColor(Color::White);
		movementSpeed = 100.f;
	}

	virtual ~Entity()
	{

	}

	//Functions
	virtual void move(const float& dt, const float dir_x, const float dir_y)
	{
		this->shape.move(dir_x * this->movementSpeed * dt, dir_y * this->movementSpeed * dt);
	}

	virtual void update(const float& dt)
	{

	}

	virtual void render(RenderTarget* target)
	{
		target->draw(this->shape);
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
	vector<Texture> textures;

	//Functions
	void initKeybinds()
	{

	}

	//Constructor / Destructor
	State(RenderWindow* window, map<string, int>* supportedKeys, stack<State*>* states)
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

class GameState : public State
{
public:
	//Variables
	Entity player;

	//Initilizer functions
	void initKeybinds()
	{
		this->keybinds["CLOSE"] = this->supportedKeys->at("Escape");
		this->keybinds["MOVE_UP"] = this->supportedKeys->at("W");
		this->keybinds["MOVE_LEFT"] = this->supportedKeys->at("A");
		this->keybinds["MOVE_DOWN"] = this->supportedKeys->at("S");
		this->keybinds["MOVE_RIGHT"] = this->supportedKeys->at("D");
	}

	GameState(RenderWindow* window, map<string, int>* supportedKeys, stack<State*>* states)
		: State(window, supportedKeys, states)
	{
		this->initKeybinds();
	}

	virtual ~GameState()
	{

	}

	//Functions
	void endState()
	{

	}

	void updateInput(const float& dt)
	{
		//Update player input
		if (Keyboard::isKeyPressed(Keyboard::Key(this->keybinds.at("MOVE_UP"))))
			this->player.move(dt, 0.f, -1.f);
		if (Keyboard::isKeyPressed(Keyboard::Key(this->keybinds.at("MOVE_LEFT"))))
			this->player.move(dt, -1.f, 0.f);
		if (Keyboard::isKeyPressed(Keyboard::Key(this->keybinds.at("MOVE_DOWN"))))
			this->player.move(dt, 0.f, 1.f);
		if (Keyboard::isKeyPressed(Keyboard::Key(this->keybinds.at("MOVE_RIGHT"))))
			this->player.move(dt, 1.f, 0.f);
		if (Keyboard::isKeyPressed(Keyboard::Key(this->keybinds.at("CLOSE"))))
			this->endState();
	}

	void update(const float& dt)
	{
		this->updateMousePositions();
		this->updateInput(dt);

		this->player.update(dt);
	}

	void render(RenderTarget* target = NULL)
	{
		if (!target)
			target = this->window;

		this->player.render(target);
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

	Color idleColor;
	Color hoverColor;
	Color activeColor;

	Button(float x, float y, float width, float height, Font* font, string text, Color idleColor, Color hoverColor, Color activeColor)
	{
		this->buttonState = BTN_IDLE;

		this->shape.setPosition(Vector2f(x, y));
		this->shape.setSize(Vector2f(width, height));

		this->font = font;
		this->text.setFont(*this->font);
		this->text.setString(text);
		this->text.setFillColor(Color::White);
		this->text.setCharacterSize(20);
		this->text.setPosition(
			this->shape.getPosition().x + this->shape.getGlobalBounds().width / 2.f - this->text.getGlobalBounds().width / 2.f,
			this->shape.getPosition().y + this->shape.getGlobalBounds().height / 2.f - this->text.getCharacterSize() / 2.f);

		this->idleColor = idleColor;
		this->hoverColor = hoverColor;
		this->activeColor = activeColor;

		this->shape.setFillColor(this->idleColor);
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
			break;

		case BTN_HOVER:
			this->shape.setFillColor(this->hoverColor);
			break;

		case BTN_ACTIVE:
			this->shape.setFillColor(this->activeColor);
			break;

		default:
			this->shape.setFillColor(Color::Red);
			break;
		}
	}

	void render(RenderTarget* target)
	{
		target->draw(this->shape);
		target->draw(this->text);
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

		if (!this->backgroundTexture.loadFromFile("Images/mainmanu.jpg"))
		{
			throw "ERROR::MAIN_MENU_STATE::FAILED_TO_LOAD_BACKGROUND_TEXTURE";
		}

		this->background.setSize(Vector2f(window->getSize().x, window->getSize().y));
		this->background.setTexture(&this->backgroundTexture);

	}

	void initFonts()
	{
		if (!this->font.loadFromFile("Fonts/2005_iannnnnJPG.ttf"))
		{
			throw("ERROR::MAINMENUSTATE::COULD NOT LOAD FONT");
		}
	}

	void initKeybinds()
	{
		this->keybinds["CLOSE"] = this->supportedKeys->at("Escape");
	}

	void initButtons()
	{
		this->buttons["GAME_STATE"] = new Button(100, 100, 250, 50, &this->font, "เข้าสู่เกม", Color(70, 70, 70, 200), Color(150, 150, 150, 255), Color(20, 20, 20, 200));

		this->buttons["SETTINGS"] = new Button(100, 200, 250, 50, &this->font, "ตั้งค่า", Color(70, 70, 70, 200), Color(150, 150, 150, 255), Color(20, 20, 20, 200));

		this->buttons["EXIT_STATE"] = new Button(100, 300, 250, 50, &this->font, "ออกจากเกม", Color(70, 70, 70, 200), Color(150, 150, 150, 255), Color(20, 20, 20, 200));
	}

	//Constructor / Destructor
	MainMenuState(RenderWindow* window, map<string, int>* supportedKeys, stack<State*>* states)
		: State(window, supportedKeys, states)
	{
		this->initVariables();
		this->initBackground();
		this->initFonts();
		this->initKeybinds();
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
			this->states->push(new GameState(this->window, this->supportedKeys, this->states));
		}

		//Quit the game
		if (this->buttons["EXIT_STATE"]->isPressed())
		{
			this->endState();
		}
	}

	void update(const float& dt)
	{
		this->updateMousePositions();
		this->updateInput(dt);

		this->updateButtons();
	}

	void renderButtons(RenderTarget* target = NULL)
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

		this->renderButtons(target);
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

	map<string, int> supportedKeys;
	supportedKeys["Escape"] = Keyboard::Key::Escape;
	supportedKeys["W"] = Keyboard::Key::W;
	supportedKeys["A"] = Keyboard::Key::A;
	supportedKeys["S"] = Keyboard::Key::S;
	supportedKeys["D"] = Keyboard::Key::D;

	stack<State*> states;
	states.push(new MainMenuState(window, &supportedKeys, &states));

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
