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


class Status
{
public:

	//variables
	bool isdead = false;
	int hp = 200;
	int max_hp = 200;
	int att = 110;
	int def = 10;
	int stamina = 100;
	int max_stamina = 100;
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
		this->hp = 100 * stagelevel;
		this->att = 30 * stagelevel;
		this->def = 10 * stagelevel;
		this->droprate = 30;
		this->dropnum = 1;
	}


	void attacking(Status* target) {
		int critrate = rand() % 100 + 1;
		int damage;

		if (critrate <= 10) damage = this->att * 2 - target->def;  //crit
		else damage = this->att - target->def;              //normal

		if (damage <= 0) damage = 0;   //def>atk

		target->hp -= damage;
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


class Entity
{
public:
	//Variables
	RectangleShape shape;
	float movementSpeed;
	//check click
	bool isPressthenRelease[2] = { false,false };

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

	//check click
	bool IsMousePress()
	{
		if (Mouse::isButtonPressed(Mouse::Left))
		{
			return true;
		}
		else return false;
	}

	bool IsMouseRelease()
	{
		if (!(Mouse::isButtonPressed(Mouse::Left)))
		{
			return true;
		}
		else return false;
	}
	bool isClickat(const Vector2f mousePos)
	{
		if (this->shape.getGlobalBounds().contains(mousePos)) {
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

	// set position cursor target
	void SetCursorposition(Entity &target)
	{
		this->shape.setPosition(
			target->getPosition().x + target->getGlobalBounds().width / 2.f,
			target->getPosition().y - target->getGlobalBounds().height / 5.f);
	}

	Vector2f getPosition()
	{
		return this->shape.getPosition();
	}
	FloatRect getGlobalBounds()
	{
		return this->shape.getGlobalBounds();
	}


	void Setscale(float x, float y)
	{
		this->shape.setScale(x, y);
	}
	void Settexture(Texture* playertexture)
	{

		this->shape.setTexture(playertexture);
	}
	void Setposition(float x, float y)
	{
		this->shape.setPosition(x, y);
	}
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
	//check click
	bool isPressthenRelease[3] = { false,false,false };

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
	//check click
	bool IsMousePress()
	{
		if (Mouse::isButtonPressed(Mouse::Left))
		{
			return true;
		}
		else return false;
	}

	bool IsMouseRelease()
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
		this->buttonState = BTN_IDLE;

		//Hover
		if (this->shape.getGlobalBounds().contains(mousePos))
		{
			this->buttonState = BTN_HOVER;


			if (this->IsMousePress())
			{
				isPressthenRelease[0] = true;
				isPressthenRelease[1] = false;
			}
			if (this->IsMouseRelease())isPressthenRelease[1] = true;

			if (isPressthenRelease[0] and isPressthenRelease[1])
			{
				this->buttonState = BTN_ACTIVE;
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

//temp status player
namespace temp {

	Status* TempStatus = new Status;
}


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
	Entity player, * enemy;
	int enemynum = 3;     //enemy number
	Texture playertexture, enemytexture[3];
	string playertexname = "Images/char1.png";
	string enemytexname[3] = { "Images/bossmon1.jpg","Images/slime.png","Images/slime.png" };
	float playerposition[2] = { 200.f,100.f };
	float enemyposition[3][2] = { {600.f,250.f},{600.f,100.f},{600.f,400.f} };

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


		if (!this->playertexture.loadFromFile(this->playertexname))
		{
			throw "ERROR::MAIN_MENU_STATE::FAILED_TO_LOAD_BACKGROUND_TEXTURE";
		}

		this->player.Setscale(2.5f, 2.5f);
		this->player.Setposition(this->playerposition[0], this->playerposition[1]);
		this->player.Settexture(&this->playertexture);




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
		this->enemytexname[0] = "Images/char1.png";
		delete[] this->enemy;
		delete[] this->enemystatus;

		this->enemystatus = new Status[enemynum];
		this->enemy = new Entity[enemynum];
	}

	void addEnemy()
	{

		for (int i = 0; i < enemynum; i++)
		{
			if (!this->enemytexture[i].loadFromFile(this->enemytexname[i]))
			{
				throw "ERROR::MAIN_MENU_STATE::FAILED_TO_LOAD_BACKGROUND_TEXTURE";
			}
			this->enemy[i].Setscale(2.5f, 2.5f);
			this->enemy[i].Setposition(this->enemyposition[i][0], this->enemyposition[i][1]);
			this->enemy[i].Settexture(&this->enemytexture[i]);

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
		this->targetCursor.Settexture(&this->targetCursortex);
		this->targetCursor.SetCursorposition(this->targetentity);
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

		this->background.setSize(Vector2f(window->getSize().x, window->getSize().y));
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
		this->Mainbuttons["ATTACK"] = new Button(100, 600, 125, 50, &this->font, "Attack", Color(200, 0, 0, 125), Color(sf::Color::White), Color(sf::Color::Black));
		this->Mainbuttons["ITEM"] = new Button(300, 600, 125, 50, &this->font, "ITEM", Color(0, 0, 250, 125), Color(150, 150, 150, 255), Color(20, 20, 20, 200));
		this->Mainbuttons["SKILL"] = new Button(500, 600, 125, 50, &this->font, "SKILL", Color(0, 250, 0, 125), Color(150, 150, 150, 255), Color(20, 20, 20, 200));
		this->Mainbuttons["RUN"] = new Button(700, 600, 125, 50, &this->font, "RUN", Color(70, 70, 70, 200), Color(150, 150, 150, 255), Color(20, 20, 20, 200));

		this->Itembuttons["ITEM1"] = new Button(900, 50, 125, 50, &this->font, "item1", Color(200, 0, 0, 125), Color(sf::Color::White), Color(sf::Color::Black));
		this->Itembuttons["ITEM2"] = new Button(900, 150, 125, 50, &this->font, "ITEM2", Color(0, 0, 250, 125), Color(150, 150, 150, 255), Color(20, 20, 20, 200));
		this->Itembuttons["ITEM3"] = new Button(900, 250, 125, 50, &this->font, "item3", Color(0, 250, 0, 125), Color(150, 150, 150, 255), Color(20, 20, 20, 200));
		this->Itembuttons["ITEM4"] = new Button(900, 350, 125, 50, &this->font, "item4", Color(0, 250, 0, 125), Color(150, 150, 150, 255), Color(20, 20, 20, 200));
		this->Itembuttons["EXITITEM"] = new Button(900, 450, 125, 50, &this->font, "exit", Color(70, 70, 70, 200), Color(150, 150, 150, 255), Color(20, 20, 20, 200));

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
	BattleState(RenderWindow* window, map<string, int>* supportedKeys, stack<State*>* states, int Maxround, int Stagelevel, bool Isbossstage)
		: State(window, supportedKeys, states)
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

		delete[] this->enemy;
		delete[] this->enemystatus;
		delete this->targetentity;
		delete this->targetstatus;
		delete this->ptplayerstatus;
		delete this->currentbutton;
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
					this->targetCursor.SetCursorposition(this->targetentity);
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

			this->states->pop();
			this->endState();    //go back to game state

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
						this->states->pop();
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
			int targetindex = targetstatus - &enemystatus[0];
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
				case 0:this->playerstatus.att += 0.2 * this->playerstatus.att;
					cout << "effect +att " << endl;
					break;
				case 1:this->playerstatus.def += 0.2 * this->playerstatus.def;
					cout << "effect +deff" << endl;
					break;
				case 2:this->playerstatus.hp += 0.5 * this->playerstatus.max_hp;
					cout << "effect +hp" << endl;
					break;
				case 3:this->playerstatus.stamina += 0.5 * this->playerstatus.max_stamina;
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
			this->targetCursor.SetCursorposition(this->targetentity);
			cout << "Click enemy 0" << endl;
		}
		if (this->enemy[1].isClickat(mousePosView))
		{
			targetstatus = &enemystatus[1];
			targetentity = &enemy[1];
			this->targetCursor.SetCursorposition(this->targetentity);
			cout << "Click enemy 1" << endl;
		}if (this->enemy[2].isClickat(mousePosView))
		{
			targetstatus = &enemystatus[2];
			targetentity = &enemy[2];
			this->targetCursor.SetCursorposition(this->targetentity);
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
			this->Mainbuttons["ITEM"]->buttonState = BTN_IDLE;
			currentbutton = &this->Itembuttons;


		}
		//close Item 
		if (this->Itembuttons["EXITITEM"]->isPressed())
		{
			this->isItemwindowActive = false;
			this->Itembuttons["EXITITEM"]->buttonState = BTN_IDLE;
			currentbutton = &this->Mainbuttons;
		}

		//Quit the game
		if (this->Mainbuttons["RUN"]->isPressed())
		{

			this->states->pop();
			this->endState();    // back to game state
		}
	}

	void update(const float& dt)
	{
		//enviornment update zone
		this->updateMousePositions();
		this->updateButtons();

		//player update zone
		this->updateInput(dt);

		this->player.update(dt);

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
			it.second->render(target);
		}
	}

	void render(RenderTarget* target = NULL)
	{
		if (!target)
			target = this->window;

		//update environment zone
		target->draw(this->background);

		//render player zone
		this->player.render(target);

		//render enemy zone
		for (int i = 0; i < enemynum; i++)this->enemy[i].render(target);
		//enemy target cursor
		this->targetCursor.render(target);
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
	Entity player;

	//Initilizer functions
	void initKeybinds()
	{
		this->keybinds["CLOSE"] = this->supportedKeys->at("Escape");
		this->keybinds["MOVE_UP"] = this->supportedKeys->at("W");
		this->keybinds["MOVE_LEFT"] = this->supportedKeys->at("A");
		this->keybinds["MOVE_DOWN"] = this->supportedKeys->at("S");
		this->keybinds["MOVE_RIGHT"] = this->supportedKeys->at("D");
		this->keybinds["CALL"] = this->supportedKeys->at("B");

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
	/*void endState()
	{
	}*/

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
		if (Keyboard::isKeyPressed(Keyboard::Key(this->keybinds.at("CALL"))))
			this->callBattle();

		if (Keyboard::isKeyPressed(Keyboard::Key(this->keybinds.at("CLOSE"))))

		{
			this->endState();
			this->states->pop();

		}


	}
	void callBattle()
	{
		bool isBossStage = false;  //for test
		this->states->push(new BattleState(this->window, this->supportedKeys, this->states, 2, 1, isBossStage));  //maxround = 2  stage level=1  isbossstage =false
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

	virtual void initBackground()
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
		this->buttons["GAME_STATE"] = new Button(100, 100, 250, 50, &this->font, "à¢éÒÊÙèà¡Á", Color(70, 70, 70, 200), Color(150, 150, 150, 255), Color(20, 20, 20, 200));

		this->buttons["SETTINGS"] = new Button(100, 200, 250, 50, &this->font, "µÑé§¤èÒ", Color(70, 70, 70, 200), Color(150, 150, 150, 255), Color(20, 20, 20, 200));

		this->buttons["EXIT_STATE"] = new Button(100, 300, 250, 50, &this->font, "ÍÍ¡¨Ò¡à¡Á", Color(70, 70, 70, 200), Color(150, 150, 150, 255), Color(20, 20, 20, 200));
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
	//  rand
	std::srand(time(0));


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
		window = new RenderWindow(VideoMode(1280, 760), "The Curse", Style::Resize, windowSettings);
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
	supportedKeys["B"] = Keyboard::Key::B;    ///for test

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