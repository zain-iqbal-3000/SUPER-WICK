
#include<iostream>
#include"SFML\Graphics.hpp"
#include"SFML\Window.hpp"
#include"SFML\System.hpp"
#include<SFML\Audio.hpp>
#include<math.h>
#include<cstdlib>
#include<ctime>
#include<vector>
#include <cmath>
#include<string>

using namespace sf;
using namespace std;


void displayLandingPage(RenderWindow& window, Sprite& landingpagebackgroundsprite, RectangleShape& newGameBt
	, RectangleShape& exitGameBt, Text& newgameText, Text& loadgameText)
{
	window.draw(landingpagebackgroundsprite);
	window.draw(newGameBt);
	window.draw(exitGameBt);
	window.draw(newgameText);
	window.draw(loadgameText);
}

//   <!----------------------------------------------LEVEL1 CLASS---------------------------------!>   //
class Level1
{
public:
	Sprite shape;
	int HP;
	int HPMax;
	unsigned int row;
	float speed;
	bool faceRight;
public:

	virtual void Update(float deltaTime) = 0;
};
//   <!-----------------------------------------LEVEL1 ANIMATION CLASS------------------------------!>   //
class Animation
{

public:
	Animation(Texture* texture, Vector2u imageCount, float switchTime);
	void Update(int row, float deltaTime, bool faceRight);
public:
	IntRect uvRect;
private:
	Vector2u imageCount;
	Vector2u currentImage;

	float totalTime;
	float switchTime;
};
Animation::Animation(Texture* texture, Vector2u imageCount, float switchTime)
{
	this->imageCount = imageCount;
	this->switchTime = switchTime;
	totalTime = 0.0f;
	currentImage.x = 0;

	uvRect.width = texture->getSize().x / float(imageCount.x);
	uvRect.height = texture->getSize().y / float(imageCount.y);
}
void Animation::Update(int row, float deltaTime, bool faceRight)
{
	currentImage.y = row;
	totalTime += deltaTime;

	if (totalTime >= switchTime)
	{
		totalTime -= switchTime;
		currentImage.x++;

		if (currentImage.x >= imageCount.x)
		{
			currentImage.x = 0;
		}
	}

	uvRect.top = currentImage.y * uvRect.height;

	if (faceRight)
	{
		uvRect.left = currentImage.x * uvRect.width;
		uvRect.width = abs(uvRect.width);

	}
	else
	{
		uvRect.left = (currentImage.x + 1) * abs(uvRect.width);
		uvRect.width = -abs(uvRect.width);
	}
}

//   <!---------------------------------------LEVEL1 BULLET CLASS---------------------------------!>   //
class Bullet
{
public:
	Sprite shape;

	Bullet(Texture* texture, Vector2f pos);
	~Bullet() {};
};
Bullet::Bullet(Texture* texture, Vector2f pos)
{
	this->shape.setTexture(*texture);
	this->shape.setScale(0.08f, 0.08f);
	pos.y += 30.f;
	this->shape.setPosition(pos);
}

//   <!----------------------------------------LEVEL1 PLAYER CLASS---------------------------------!>   //
class Player : public Level1
{
private:
	Animation animation;
public:

	Texture* texture;
	std::vector<Bullet> bullets;

	Player(Texture* texture, RenderWindow& window, Vector2u imageCount, float switchTime, float speed);
	void playerCollisionWindow(Window& window);
	void Update(float deltaTime);
};
Player::Player(Texture* texture, RenderWindow& window, Vector2u imageCount, float switchTime, float speed) :animation(texture, imageCount, switchTime)
{
	this->HPMax = 10;
	this->HP = this->HPMax;

	this->texture = texture;
	this->shape.setTexture(*texture);
	this->shape.setScale(0.4f, 0.4f);
	this->shape.setPosition(0.0f, window.getSize().y * 0.75f);
	this->speed = speed;
	row = 0;
	faceRight = true;


}
void Player::Update(float deltaTime)
{
	Vector2f movement(0.0f, 0.0f);

	if (Keyboard::isKeyPressed(Keyboard::A))
		movement.x -= speed * deltaTime;
	if (Keyboard::isKeyPressed(Keyboard::D))
		movement.x += speed * deltaTime;
	if (Keyboard::isKeyPressed(Keyboard::W))
		movement.y -= speed * deltaTime;
	if (Keyboard::isKeyPressed(Keyboard::S))
		movement.y += speed * deltaTime;

	if (movement.x == 0.0f)
	{
		row = 1;
	}
	else
	{
		row = 1;
		if (movement.x > 0.0f)
			faceRight = true;
		else
			faceRight = false;
	}

	animation.Update(row, deltaTime, faceRight);
	this->shape.setTextureRect(animation.uvRect);
	this->shape.move(movement);
}
void Player::playerCollisionWindow(Window& window)
{
	if (this->shape.getPosition().x <= 0) //Left
		this->shape.setPosition(0.f, this->shape.getPosition().y);
	if (this->shape.getPosition().x >= window.getSize().x - this->shape.getGlobalBounds().width) //Right
		this->shape.setPosition(window.getSize().x - this->shape.getGlobalBounds().width, this->shape.getPosition().y);
	if (this->shape.getPosition().y <= window.getSize().y * 0.55f) //Top
		this->shape.setPosition(this->shape.getPosition().x, window.getSize().y * 0.55f);
	if (this->shape.getPosition().y >= 600.f - this->shape.getGlobalBounds().height) //Bottom
		this->shape.setPosition(this->shape.getPosition().x, 600.f - this->shape.getGlobalBounds().height);
}

//   <!-------------------------------------LEVEL1 ENEMY CLASS----------------------------------!>   //
class Enemy :public Level1
{
public:
	Animation animateEnemy;

	Enemy(Texture* texture, RenderWindow& window, Vector2u imageCount, float switchTime, float speed, Vector2u windowSize);
	void Update(float deltatTime);
};
Enemy::Enemy(Texture* texture, RenderWindow& window, Vector2u imageCount, float switchTime, float speed,
	Vector2u windowSize) :animateEnemy(texture, imageCount, switchTime)
{
	this->HPMax = rand() % 3 + 1;
	this->HP = this->HPMax;
	this->shape.setTexture(*texture);
	this->shape.setScale(0.75f, 0.75f);

	// lower 45% of the window
	float lower_window_height = windowSize.y * 0.52f;
	//  y-coordinate for the enemy within the lower 45% of the window
	float random_y = lower_window_height + rand() % (int)(windowSize.y * 0.25f - this->shape.getGlobalBounds().height);
	// Set the position of the enemy
	this->shape.setPosition(windowSize.x, random_y);
}

void Enemy::Update(float deltaTime)
{
	row = rand() % 3;
	animateEnemy.Update(row, deltaTime,true);
	this->shape.setTextureRect(animateEnemy.uvRect);
}

//   <!------------------------------------LEVEL1 BACKGROUND CLASS------------------------------!>   //
class Background
{
public:
	Animation animateBackground;
	Sprite shape;
	unsigned int row;
	float speed;
	float faceRight = true;
	Background(Texture* texture, RenderWindow& window, Vector2u imageCount, float switchTime, float speed);
	void Update(float, int);
	~Background() {};
};
Background::Background(Texture* texture, RenderWindow& window, Vector2u imageCount, float switchTime, float speed) :animateBackground(texture, imageCount, switchTime)
{
	this->shape.setTexture(*texture);
	this->shape.setOrigin(0.f, window.getPosition().y - this->shape.getGlobalBounds().height);
	this->row = 0;
	this->speed = 0.f;

}
void Background::Update(float deltaTime, int row)
{
	row = row;//rand() % 3;
	animateBackground.Update(row, deltaTime, faceRight);
	this->shape.setTextureRect(animateBackground.uvRect);
}
void updateSpawnTimer(int& spawnTimer, const int& spawnTimerLimit)
{
	if (spawnTimer < spawnTimerLimit)
		spawnTimer++;
}
//    <!-----------------------------------UPDATE LEVEL1 BACKGROUND-------------------------------------!>    //
void updateBackground(Background& cloudBackground, vector<Background>& clouds, Background& rain, vector<Background>& rains
	, Background& cityBackground, vector<Background>& buildings, Background& road, vector<Background>& roads,
	Background& lightning1, Background& lightning2, Background& lightning3, int& cloudSpawnTimer, int& rainSpawnTimer,
	int& citySpawnTimer, int& roadSpawnTimer, RenderWindow& window, float& deltaTime, Background& trafficLight,
	vector<Background>& trafficLights, int& trafficLightSpawnTimer, Background& man_running, vector<Background>& manRunning
	, Background& kid_running, vector<Background>& kidRunning, Background& women_walking, vector<Background>& womenWalking,
	int& manSpawnTimer, int& kidSpawnTimer, int& womenSpawnTimer)
{
	//Set Cloud Spawn
	updateSpawnTimer(cloudSpawnTimer, 35);

	if (cloudSpawnTimer >= 35)
	{
		clouds.push_back(cloudBackground);
		cloudSpawnTimer = 0; //reset timer
	}

	//Set Rain Spawn
	updateSpawnTimer(rainSpawnTimer, 3);

	if (rainSpawnTimer >= 3)
	{
		rains.push_back(rain);
		rainSpawnTimer = 0; //reset timer
	}

	//Set Building Spawn
	updateSpawnTimer(citySpawnTimer, 205);

	if (citySpawnTimer >= 205)
	{
		buildings.push_back(cityBackground);
		citySpawnTimer = 0; //reset timer
	}

	//Set Road Spawn
	updateSpawnTimer(roadSpawnTimer, 0);

	if (roadSpawnTimer >= 0)
	{
		roads.push_back(road);
		roadSpawnTimer = 0; //reset timer
	}

	//Set Traffic Light Spawn
	updateSpawnTimer(trafficLightSpawnTimer, 450);

	if (trafficLightSpawnTimer >= 450)
	{
		trafficLights.push_back(trafficLight);
		trafficLightSpawnTimer = 0;
	}
	//Set Man Spawn
	updateSpawnTimer(manSpawnTimer, 350);

	if (manSpawnTimer >= 350)
	{
		manRunning.push_back(man_running);
		manSpawnTimer = 0;
	}
	//Set Kid Spawn
	updateSpawnTimer(kidSpawnTimer, 350);

	if (kidSpawnTimer >= 350)
	{
		kidRunning.push_back(kid_running);
		kidSpawnTimer = 0;
	}
	//Set Women Spawn
	updateSpawnTimer(womenSpawnTimer, 350);

	if (womenSpawnTimer >= 350)
	{
		womenWalking.push_back(women_walking);
		womenSpawnTimer = 0;
	}

	//Update Building
	for (size_t i = 0; i < buildings.size(); i++)
	{
		buildings[i].Update(deltaTime, rand() % 1);
		buildings[i].shape.move(-4.f, 0.f);
		if (buildings[i].shape.getPosition().x <= 0 - buildings[i].shape.getGlobalBounds().width)
		{
			buildings.erase(buildings.begin() + i);
			break;
		}
	}

	//Update Road
	for (size_t i = 0; i < roads.size(); i++)
	{
		roads[i].Update(deltaTime, rand() % 1);
		roads[i].shape.move(-7.f, 0.f);
		if (roads[i].shape.getPosition().x <= 0 - roads[i].shape.getGlobalBounds().width)
		{
			roads.erase(roads.begin() + i);
			break;
		}
	}

	//Update Cloud
	for (size_t i = 0; i < clouds.size(); i++)
	{
		clouds[i].Update(deltaTime, rand() % 1);
		clouds[i].shape.move(-5.f, 0.f);
		if (clouds[i].shape.getPosition().x <= 0 - clouds[i].shape.getGlobalBounds().width)
		{
			clouds.erase(clouds.begin() + i);
			break;
		}
	}

	//Update Rain
	rain.shape.setPosition(rand() % (int)window.getSize().x, 0.f);
	for (size_t i = 0; i < rains.size(); i++)
	{
		rain.Update(deltaTime, 0);
		rains[i].shape.move(-10.f, 5.f);
	}

	//Update Lightning
	lightning1.Update(deltaTime, 0);
	lightning2.Update(deltaTime, 0);
	lightning3.Update(deltaTime, 0);

	//Update Traffic Lights
	for (size_t i = 0; i < trafficLights.size(); i++)
	{
		trafficLights[i].Update(deltaTime, rand() % 1);
		trafficLights[i].shape.move(-2.f, 0.f);
		if (trafficLights[i].shape.getPosition().x <= 0 - trafficLights[i].shape.getGlobalBounds().width)
		{
			trafficLights.erase(trafficLights.begin() + i);
			break;
		}
	}
	//Update Man
	for (size_t i = 0; i < manRunning.size(); i++)
	{
		manRunning[i].Update(deltaTime, rand() % 1);
		manRunning[i].shape.move(-3.f, 0.f);
		if (manRunning[i].shape.getPosition().x <= 0 - manRunning[i].shape.getGlobalBounds().width)
		{
			manRunning.erase(manRunning.begin() + i);
			break;
		}
	}
	//Update Kid
	for (size_t i = 0; i < kidRunning.size(); i++)
	{
		kidRunning[i].Update(deltaTime, rand() % 1);
		kidRunning[i].shape.move(-3.f, 0.f);
		if (kidRunning[i].shape.getPosition().x <= 0 - kidRunning[i].shape.getGlobalBounds().width)
		{
			kidRunning.erase(kidRunning.begin() + i);
			break;
		}
	}
	//Update Women
	for (size_t i = 0; i < womenWalking.size(); i++)
	{
		womenWalking[i].Update(deltaTime, rand() % 1);
		womenWalking[i].shape.move(-2.f, 0.f);
		if (womenWalking[i].shape.getPosition().x <= 0 - womenWalking[i].shape.getGlobalBounds().width)
		{
			womenWalking.erase(womenWalking.begin() + i);
			break;
		}
	}
}
//    <!-----------------------------------DRAW LEVEL1 BACKGROUND-------------------------------------!>    //
void drawBackground(Sprite& skyBackgroundSprite, vector<Background>& roads, vector<Background>& buildings,
	Background& lightning1, Background& lightning2, Background& lightning3, vector<Background>& rains,
	vector<Background>& clouds, RenderWindow& window, vector<Background>& trafficLights,
	vector<Background>& manRunning, vector<Background>& kidRunning, vector<Background>& womenWalking)
{
	//Draw Sky
	window.draw(skyBackgroundSprite);

	//Draw Road
	for (size_t i = 0; i < roads.size(); i++)
		window.draw(roads[i].shape);

	//Draw Building
	for (size_t i = 0; i < buildings.size(); i++)
		window.draw(buildings[i].shape);

	//Draw Lightning
	window.draw(lightning1.shape);
	window.draw(lightning2.shape);
	window.draw(lightning3.shape);

	//Draw Rain
	for (size_t i = 0; i < rains.size(); i++)
		window.draw(rains[i].shape);

	//Draw Clouds
	for (size_t i = 0; i < clouds.size(); i++)
		window.draw(clouds[i].shape);

	//Draw Traffic Lights
	for (size_t i = 0; i < trafficLights.size(); i++)
		window.draw(trafficLights[i].shape);
	//Draw Man
	for (size_t i = 0; i < manRunning.size(); i++)
		window.draw(manRunning[i].shape);
	//Draw Kid
	for (size_t i = 0; i < kidRunning.size(); i++)
		window.draw(kidRunning[i].shape);
	//Draw Women
	for (size_t i = 0; i < womenWalking.size(); i++)
		window.draw(womenWalking[i].shape);
}
//    <!-----------------------------------UPDATE LEVEL1 PLAYER-------------------------------------!>    //
void updatePlayer(Player& playerWalking, float& deltaTime, RenderWindow& window, Text& hpText, int& shootTimer, Music& gameMusic,
	Music& gameOverMusic, Texture& bulletTex, bool& spacePressed, vector<Enemy>& enemies, int& score, Text& scoreText, bool& winGame
	, RectangleShape&hpBar, bool&loseGame)
{
	//Update Player
	playerWalking.Update(deltaTime);

	//Collision with window
	playerWalking.playerCollisionWindow(window);

	//Update HP Text
	hpText.setPosition(playerWalking.shape.getPosition().x, playerWalking.shape.getPosition().y - hpText.getGlobalBounds().height);
	hpText.setString(std::to_string(playerWalking.HP) + "/" + std::to_string(playerWalking.HPMax));


	//Update Controls
	if (shootTimer < 15)
		shootTimer++;
	
	//Update HpBar
	hpBar.setSize(Vector2f((float)playerWalking.HP * 20.f, 20.f));;

	if (Mouse::isButtonPressed(Mouse::Left) && shootTimer >= 15) //Shooting
	{
		playerWalking.bullets.push_back(Bullet(&bulletTex, playerWalking.shape.getPosition()));
		shootTimer = 0; //reset timer
	}
	if (spacePressed && shootTimer >= 15) //Shooting
	{
		playerWalking.bullets.push_back(Bullet(&bulletTex, playerWalking.shape.getPosition()));
		shootTimer = 0; //reset timer



	}

	//Update Bullets
	for (size_t i = 0; i < playerWalking.bullets.size(); i++)
	{
		//Move Bullets
		playerWalking.bullets[i].shape.move(20.f, 0.f);

		//Out of window bounds
		if (playerWalking.bullets[i].shape.getPosition().x > window.getSize().x)
		{
			playerWalking.bullets.erase(playerWalking.bullets.begin() + i);
			break;
		}
		//Enemy collision
		for (size_t k = 0; k < enemies.size(); k++)
		{
			if (playerWalking.bullets[i].shape.getGlobalBounds().intersects(enemies[k].shape.getGlobalBounds()))
			{
				if (enemies[k].HP <= 1)
				{
					score += enemies[k].HPMax;
					enemies.erase(enemies.begin() + k);

				}
				else
					enemies[k].HP--; 

				playerWalking.bullets.erase(playerWalking.bullets.begin() + i);
				break;
			}
		}
		//UI Update
		scoreText.setString("Score: " + std::to_string(score));
	}

	if (playerWalking.HP <= 1)
	{
		
		loseGame = true;
		gameMusic.stop();
		//delete& playerWalking;
	}
	if (score >= 50)
	{
		winGame = true;
		gameMusic.stop();
		//gameOverMusic.play();

	}

}
//    <!-----------------------------------UPDATE LEVEL1 ENEMIES-------------------------------------!>    //
void updateEnemy(int& enemySpawnTimer, int& bulletSpawnTimer, vector<Enemy>& enemies, RenderWindow& window,
	vector<Bullet>& enemyBullets, Texture& bulletTex, Vector2f& enemySize, Player& playerWalking,
	Music& gameOverMusic, Texture& enemyTex, float& deltaTime)
{
	//Enemy
	updateSpawnTimer(enemySpawnTimer, 55);

	//Enemy Bullets
	updateSpawnTimer(bulletSpawnTimer, 50);

	//Enemy spawn
	if (enemySpawnTimer >= 55)
	{
		enemies.push_back(Enemy(&enemyTex, window, Vector2u(3, 3), 0.3f, 50.f, window.getSize()));
		enemySpawnTimer = 0;
	}

	//Bullet Spawn
	if (bulletSpawnTimer >= 50)
	{
		enemyBullets.push_back(Bullet(&bulletTex, enemySize));
		bulletSpawnTimer = 0;
	}
	//Enemy move
	for (size_t i = 0; i < enemies.size(); i++)
	{
		enemies[i].Update(deltaTime);
		enemies[i].shape.move(-3.f, 0.f);
		enemySize = enemies[i].shape.getPosition();

		if (enemies[i].shape.getPosition().x <= 10 - enemies[i].shape.getGlobalBounds().width)
		{
			enemies.erase(enemies.begin() + i);
			break;
		}

		if (enemies[i].shape.getGlobalBounds().intersects(playerWalking.shape.getGlobalBounds()))
		{

			enemies.erase(enemies.begin() + i);


			playerWalking.HP--;
			
			break;
		}
	}

	//Enemy Bullets Move
	for (size_t i = 0; i < enemyBullets.size(); i++)
	{
		enemyBullets[i].shape.move(-20.f, 0.f);

		if (enemyBullets[i].shape.getPosition().x <= 0 - enemyBullets[i].shape.getGlobalBounds().width)
		{
			enemyBullets.erase(enemyBullets.begin() + i);
			break;
		}
	}
}
//    <!-----------------------------------DRAW LEVEL1 PLAYER AND ENEMIES-------------------------------------!>    //
void drawPlayerAndEnemies(Player& playerWalking, vector<Enemy>& enemies, Text& eHpText, RenderWindow& window,
	vector<Bullet>& enemyBullets, Text& scoreText, Music& gameMusic, Text& hpText, Music& gameOverMusic,RectangleShape&hpBar)
{
	//Bullets
	for (size_t i = 0; i < playerWalking.bullets.size(); i++)
	{
		window.draw(playerWalking.bullets[i].shape);
	}

	//enemy
	for (size_t i = 0; i < enemies.size(); i++)
	{
		eHpText.setString(std::to_string(enemies[i].HP) + "/" + std::to_string(enemies[i].HPMax));
		eHpText.setPosition(enemies[i].shape.getPosition().x, enemies[i].shape.getPosition().y - eHpText.getGlobalBounds().height);
		window.draw(eHpText);
		window.draw(enemies[i].shape);
	}
	//Enemy Bullets
	for (size_t i = 0; i < enemyBullets.size(); i++)
	{
		window.draw(enemyBullets[i].shape);
	}

	//UI
	window.draw(scoreText);
	window.draw(hpText);
	window.draw(hpBar);
	window.draw(playerWalking.shape);

}



//   <!----------------------------------------------LEVEL2 CLASS---------------------------------!>   //
class Level2 {
protected:
	Texture Texture;
	Sprite sprite;

public:
	void draw(RenderWindow& window) {
		window.draw(sprite);
	}

	void setPosition(float x, float y) {
		sprite.setPosition(x, y);
	}
	void setScale(float x, float y) {
		sprite.setScale(x, y);
	}
	float getPositionY() {
		return sprite.getPosition().y;
	}

	float getPositionX() {
		return sprite.getPosition().x;
	}
	void setRotation(float r) {
		sprite.setRotation(r);
	}

	void move(float speedx, float speedy) {
		sprite.move(speedx, speedy);
	}

	FloatRect getGlobalBounds() {
		return sprite.getGlobalBounds();

	}

};

class Obstacles : public Level2 {
protected:
	int ob_num;

public:
	Obstacles() {
		ob_num = 0;
	}

	int getobnum() {
		return ob_num;
	}
};

class WoodObstacle : public Obstacles {
public:
	WoodObstacle() {
		ob_num = 1;
		if (!Texture.loadFromFile("Textures/asteroid1.png")) {
			cout << "error loading from file";
		}

		sprite.setTexture(Texture);
		sprite.setPosition(0.0f, 511);
	}
};

class MetalObstacle : public Obstacles {

public:
	MetalObstacle() {
		ob_num = 2;
		if (!Texture.loadFromFile("Textures/asteroid2.png")) {
			cout << "error loading from file";
		}

		sprite.setTexture(Texture);
		sprite.setPosition(0.0f, 574);
	}
};

class Coin : public Level2 {
private:
	float animationtimer;
	float animationspeed = 0.3f;
	int numframes;
	int currentframe;


public:
	Coin() {
		if (!Texture.loadFromFile("Textures/coinspritesheet.png")) {
			cout << "error loading from file";
		}

		sprite.setTexture(Texture);
		sprite.setTextureRect(IntRect(0, 0, 90, 85));

		sprite.setPosition(0.0f, 0.0f);
	}


	void animate(float deltatime) {
		animationtimer += deltatime;

		if (animationtimer >= animationspeed) {
			animationtimer = 0.0f;

			// calculate the next frame based on the elapsed time
			numframes = Texture.getSize().x / Texture.getSize().y; // assuming square frames
			currentframe = (currentframe + 1) % numframes;

			// set the Texture rect to the current frame
			int frameoffset = currentframe * Texture.getSize().y; // assuming square frames
			sprite.setTextureRect(IntRect(frameoffset, 0, Texture.getSize().y, Texture.getSize().y));
		}
	}
};

class MainCharacter : public Level2 {
	int coinscollected = 0;
public:
	MainCharacter() {
		if (!Texture.loadFromFile("Textures/car sprite.png")) {
			cout << "error loading from file";
		}

		sprite.setTexture(Texture);
		sprite.setTextureRect(IntRect(0, 25, 140, 80));

		sprite.setPosition(50.0f, 300.0f);
	}
	void animate(float deltatime) {
		static float animationtimer = 0.0f;
		static float animationspeed = 0.2f;
		static int numframes = 3;
		static int currentframe = 0;

		animationtimer += deltatime;

		if (animationtimer >= animationspeed) {
			animationtimer = 0.0f;

			// calculate the next frame
			currentframe = (currentframe + 1) % numframes;

			// set the Texture rect to the current frame
			int framewidth = Texture.getSize().x / numframes;
			int frameoffset = currentframe * framewidth;
			sprite.setTextureRect(IntRect(frameoffset, 0, framewidth, Texture.getSize().y));
		}
	}
	void collectcoin() {
		coinscollected++;
	}
	void setScale(float x, float y) {
		sprite.setScale(x, y);
	}
	int getcoinscollected() {
		return coinscollected;
	}

	void jump(float jumpspeed) {
		sprite.move(0.0f, -jumpspeed);
	}
};
void playLevel2();
void displayWinLevelScreen();
void displayLoseLevel1Screen(Music&,bool&);
void displayLoseLevel2Screen();

class Helpers {
public:
	static float euclideanDistance(float x1, float y1, float x2, float y2) {
		float dx = x2 - x1;
		float dy = y2 - y1;
		return std::sqrt(dx * dx + dy * dy);
	}
	static void updateSpawnTimer(int& spawnTimer, const int& spawnTimerLimit)
	{
		if (spawnTimer < spawnTimerLimit)
			spawnTimer++;
	}
	static void displayLandingPage(RenderWindow& window, Sprite& landingPageBackgroundSprite, RectangleShape& newGameBt
		, RectangleShape& exitGameBt, Text& newgameText, Text& loadgameText)
	{
		window.draw(landingPageBackgroundSprite);
		window.draw(newGameBt);
		window.draw(exitGameBt);
		window.draw(newgameText);
		window.draw(loadgameText);
	}
};

bool displayGameOverScreen(bool& continuePlaying) 
{
	
	RenderWindow window(sf::VideoMode(800, 600), "Game Over Screen");
	
	sf::Font font;
	if (!font.loadFromFile("Fonts/Metropolian-Display.ttf")) {
		throw("could not load file");
	}

	
	Text gameOverText("GAME OVER!", font, 50);
	gameOverText.setPosition(200.f, 200.f);
	gameOverText.setFillColor(sf::Color::Red);

	// Create a text object for the instruction
	sf::Text continueText("\nPress F to continue playing.", font, 30);
	continueText.setPosition(200.f, 400.f);
	continueText.setFillColor(sf::Color::Magenta);

	
	continuePlaying = false;
	while (window.isOpen() && !continuePlaying) {
		
		sf::Event event;

		while (window.pollEvent(event))
		{
			if (event.type == sf::Event::Closed) 
				window.close();
			
			else if (event.type == sf::Event::KeyPressed) 
			{
				if (event.key.code == sf::Keyboard::F) {
					continuePlaying = true;
					return true;
				}
				else
					return false;
			}
		}

		
		window.clear(sf::Color::Black);
		window.draw(gameOverText);
		window.draw(continueText);

		window.display();
	}
}
void playLevel1(Music& landingpagemusic,bool& continuePlaying)
{
	bool spacePressed = false, winGame = false, loseGame = false, gameOver = false;
	Vector2f enemySize;

	RenderWindow window(VideoMode(1200, 600), "Final Boss Battle!", Style::Default);
	window.setFramerateLimit(60);

	float deltaTime = 0.0f;
	Clock clock;


	//Init text
	Font font;
	font.loadFromFile("Fonts/Dosis-Light.ttf");

	//Init textures
	Texture playerTex;
	playerTex.loadFromFile("Textures/sprite sheet.png");
	playerTex.setSmooth(false);
	Texture enemyTex;
	enemyTex.loadFromFile("Textures/bike opponent.png");

	//Init Bullet 
	int bulletSpawnTimer = 0;
	Texture bulletTex;
	bulletTex.loadFromFile("Textures/bullet.png");
	vector<Bullet>enemyBullets;

	//UI init
	Text scoreText;
	scoreText.setFont(font);
	scoreText.setCharacterSize(20);
	scoreText.setFillColor(Color::White);
	scoreText.setPosition(10.f, 10.f);

	//GAME OVER Text init
	Text gameOverText;
	gameOverText.setFont(font);
	gameOverText.setCharacterSize(40);
	gameOverText.setOutlineThickness(4);
	gameOverText.setOutlineColor(Color::Black);
	gameOverText.setFillColor(Color::Red);
	gameOverText.setStyle(Text::Bold);
	gameOverText.setPosition(window.getSize().x / 3, window.getSize().y / 2);
	gameOverText.setString("GAME OVER!");

	//Player init
	int score = 0;
	Texture playerWalkingTex, playerDyingTex;
	if (!playerWalkingTex.loadFromFile("Textures/john_holding_guns.png"))
		throw("could not load file");
	if (!playerDyingTex.loadFromFile("Textures/john_dying.png"))
		throw("could not load file");
	Player* playerWalking = new Player(&playerWalkingTex, window, Vector2u(11, 2), 0.2f, 100.0f);
	int shootTimer = 20;
	Text hpText;
	hpText.setFont(font);
	hpText.setCharacterSize(12);
	hpText.setFillColor(Color::White);

	//Enemy init
	int enemySpawnTimer = 0;
	vector<Enemy>enemies;
	Text eHpText;
	eHpText.setFont(font);
	eHpText.setCharacterSize(12);
	eHpText.setFillColor(Color::White);

	//Game Background Music
	sf::Music gameMusic;
	if (!gameMusic.openFromFile("Textures/danger zone.wav")) {
		throw ("Error loading file");
	}
	Time offset = seconds(25.f);
	gameMusic.setPlayingOffset(offset);
	//Game Over Music
	sf::Music gameOverMusic;
	if (!gameOverMusic.openFromFile("Textures/game over sound.wav")) {
		std::cout << "File not Found!" << std::endl;
	}
	//Gun Audio
	sf::Music gunAudio;
	if (!gunAudio.openFromFile("Textures/gunAudio.wav")) {
		throw("Failed to load gunAudio");
	}
	gunAudio.setVolume(20);

	sf::Music music;
	if (!music.openFromFile("Textures/game over music.wav")) {
		std::cout << "File not Found!" << std::endl;
	}




	//sky background
	Texture skyBackgroundTex;
	if (!skyBackgroundTex.loadFromFile("Textures/sky background.jpg"))
		throw ("Error loading file");
	skyBackgroundTex.setSmooth(false);
	Sprite skyBackgroundSprite;
	skyBackgroundSprite.setTexture(skyBackgroundTex);
	skyBackgroundSprite.setScale(1.2f, 1.f);

	//city background
	Texture cityBackgroundTex;
	if (!cityBackgroundTex.loadFromFile("Textures/city background1.png"))
		throw ("Error loading file");
	int citySpawnTimer = 0;
	Background cityBackground(&cityBackgroundTex, window, Vector2u(1, 1), 10.5f, 0.f);
	vector<Background>buildings;
	cityBackground.shape.setScale(1.5f, 1.f);
	cityBackground.shape.setPosition(window.getPosition().x * 3, window.getSize().y - cityBackground.shape.getGlobalBounds().height * 1.8f);

	//Traffic Lights
	Texture trafficLightTex;
	if (!trafficLightTex.loadFromFile("Textures/traffic_lights.png"))
		throw("Error loading file");
	int trafficLightSpawnTimer = 0;
	Background trafficLight(&trafficLightTex, window, Vector2u(3, 1), 2.f, 0.f);
	vector<Background>trafficLights;
	trafficLight.shape.setScale(0.4f, 0.4f);
	trafficLight.shape.setPosition(window.getPosition().x * 3, window.getSize().y - trafficLight.shape.getGlobalBounds().height * 2.8f);

	//NPCs
	Texture manRunningTex, kidRunningTex, womenWalkingTex;
	if (!manRunningTex.loadFromFile("Textures/man_running.png"))
		throw("Error loading file");
	if (!kidRunningTex.loadFromFile("Textures/kid_running.png"))
		throw("Error loading file");
	if (!womenWalkingTex.loadFromFile("Textures/women_walking.png"))
		throw("Error loading file");
	int manSpawnTimer = 0;
	int kidSpawnTimer = 0;
	int womenSpawnTimer = 0;
	Background man_running(&manRunningTex, window, Vector2u(4, 2), 0.3f, 0.f);
	Background kid_running(&kidRunningTex, window, Vector2u(4, 2), 0.3f, 0.f);
	Background women_walking(&womenWalkingTex, window, Vector2u(5, 1), 0.3f, 0.f);
	vector<Background>manRunning;
	vector<Background>kidRunning;
	vector<Background>womenWalking;
	man_running.shape.setScale(0.6f, 0.6f);
	kid_running.shape.setScale(0.4f, 0.4f);
	women_walking.shape.setScale(0.5f, 0.5f);
	man_running.shape.setPosition(window.getPosition().x * 3, window.getSize().y - trafficLight.shape.getGlobalBounds().height * 3.1f);
	kid_running.shape.setPosition(window.getPosition().x * 3 + 50.f, window.getSize().y - trafficLight.shape.getGlobalBounds().height * 2.6f);
	women_walking.shape.setPosition(window.getPosition().x * 3 + 200.f, window.getSize().y - trafficLight.shape.getGlobalBounds().height * 2.8f);



	//Road Background
	Texture roadBackgroundTex;
	if (!roadBackgroundTex.loadFromFile("Textures/road strip.png"))
		throw ("Error loading file");
	int roadSpawnTimer = 0;
	Background road(&roadBackgroundTex, window, Vector2u(1, 1), 1000.5f, 200.f);
	vector<Background>roads;
	road.shape.setScale(0.2f, 0.3f);
	road.shape.setPosition(window.getPosition().x * 3, window.getSize().y - road.shape.getGlobalBounds().height * 1.7f);


	//Clouds
	Texture cloudTex;
	if (!cloudTex.loadFromFile("Textures/cloud sprite sheet 2.png"))
		throw ("Error loading file");
	int cloudSpawnTimer = 0;
	Background cloudBackground(&cloudTex, window, Vector2u(4, 3), 1000.5f, 200.f);
	cloudBackground.shape.setScale(1.5f, 2.9f);
	cloudBackground.shape.setPosition(window.getPosition().x * 3, -window.getPosition().y * 2.5 + cloudBackground.shape.getGlobalBounds().height / 3.3);
	vector<Background>clouds;


	//Lightning
	Texture lightningTex;
	if (!lightningTex.loadFromFile("Textures/lightning sprite sheet.png"))
		throw ("Error loading file");
	Background lightning1(&lightningTex, window, Vector2u(4, 1), 0.4, 0.f);
	Background lightning2(&lightningTex, window, Vector2u(4, 1), 0.6, 20.f);
	Background lightning3(&lightningTex, window, Vector2u(4, 1), 0.2, 20.f);

	lightning1.shape.setPosition(window.getPosition().x * 0.75, -window.getPosition().y * 2.5 + lightning1.shape.getGlobalBounds().height * 1.62);
	lightning2.shape.setPosition(window.getPosition().x * 1.50, -window.getPosition().y * 2.5 + lightning1.shape.getGlobalBounds().height * 1.62);
	lightning3.shape.setPosition(window.getPosition().x * 2.75, -window.getPosition().y * 2.5 + lightning1.shape.getGlobalBounds().height * 1.62);
	lightning1.shape.setScale(1.f, 1.f);
	lightning2.shape.setScale(1.f, 1.f);
	lightning3.shape.setScale(1.f, 1.f);

	//Rain
	Texture rainTex;
	if (!rainTex.loadFromFile("Textures/rain_heavy.png"))
		throw ("Error loading file");
	int rainSpawnTimer = 0;
	Background rain(&rainTex, window, Vector2u(2, 1), 20.4, 0.f);
	rain.shape.setScale(1.5f, 2.2f);
	vector<Background>rains;

	sf::Time duration = gunAudio.getDuration();
	sf::Time fiveSeconds = sf::seconds(5.f); // 5 seconds
	gameMusic.play();

	///Congratulations Message
	sf::Font winGameFont;
	font.loadFromFile("Fonts/Metropolian-Display.ttf");

	sf::Text congratulationText;
	congratulationText.setFont(winGameFont);
	congratulationText.setCharacterSize(30);
	congratulationText.setFillColor(Color::Cyan);
	congratulationText.setString("Congratulations! You won the game!");
	congratulationText.setPosition(300, 300);

	//Player HP Bar
	RectangleShape hpBar;
	hpBar.setFillColor(Color::Red);
	hpBar.setSize(Vector2f((float)playerWalking->HP * 20.f, 20.f));
	hpBar.setPosition(500.f, 20.f);

	
	//  <!---------------------------------------------GAME LOOP-------------------------------------------!>  //
	while (window.isOpen())
	{
		deltaTime = clock.restart().asSeconds();

		Event event;
		while (window.pollEvent(event))
		{
			Vector2i mousePos = Mouse::getPosition(window);
			if (event.type == Event::Closed)
				window.close();


			if (event.type == Event::MouseButtonPressed && event.mouseButton.button == Mouse::Left)
			{

				//gameMusic.play();
				gunAudio.play();



			}

			else if (event.type == Event::MouseButtonReleased && event.mouseButton.button == Mouse::Left)
			{
				gunAudio.stop();

			}
			/*if (event.type == Event::KeyPressed)
			{
				if (event.key.code == Keyboard::Space)
				{
					spacePressed = true;
					gunAudio.play();
				}
			}
			else if (event.type == Event::KeyReleased)
			{
				if (event.key.code == Keyboard::Space)
				{
					spacePressed = false;
					gunAudio.stop();
				}
			}*/



		}

		//	<!-----------------------------------------UPDATE-----------------------------------!>	//

				//UPDATE PLAYER//
		if (playerWalking->HP > 0) {
			updatePlayer(*playerWalking, deltaTime, window, hpText, shootTimer, gameMusic, music,
				bulletTex, spacePressed, enemies, score, scoreText, winGame,hpBar,loseGame);
			//UPDATE ENEMY//
			updateEnemy(enemySpawnTimer, bulletSpawnTimer, enemies, window, enemyBullets,
				bulletTex, enemySize, *playerWalking, gameOverMusic, enemyTex, deltaTime);

			//UPDATE BACKGROUND//
			updateBackground(cloudBackground, clouds, rain, rains, cityBackground, buildings, road, roads, lightning1,
				lightning2, lightning3, cloudSpawnTimer, rainSpawnTimer, citySpawnTimer, roadSpawnTimer
				, window, deltaTime, trafficLight, trafficLights, trafficLightSpawnTimer, man_running, manRunning,
				kid_running, kidRunning, women_walking, womenWalking, manSpawnTimer, kidSpawnTimer, womenSpawnTimer);
		}


		//	<!-----------------------------------------DRAW-----------------------------------!>	//

		window.clear();

		//DRAW BACKGROUND//
		drawBackground(skyBackgroundSprite, roads, buildings, lightning1, lightning2, lightning3,
			rains, clouds, window, trafficLights, manRunning, kidRunning, womenWalking);


		//DRAW PLAYER AND ENEMIES
		drawPlayerAndEnemies(*playerWalking, enemies, eHpText, window, enemyBullets,
			scoreText, gameMusic, hpText, gameOverMusic,hpBar);

		if (winGame)
		{
			gameMusic.stop();
			window.close();
			displayWinLevelScreen();
		}
		if (loseGame)
		{
			window.close();
			displayLoseLevel1Screen(landingpagemusic,continuePlaying);
		}
		window.display();

	}
}
void displaymenu(sf::Music& landingpagemusic) {
	RenderWindow window(VideoMode(1200, 600), "sfml sprite example");

	//fonts
	Font dosislight;
	if (!dosislight.loadFromFile("fonts/dosis-bold.ttf"))
		throw("could not load font!");

	//landing page background image
	Texture landingPageBackgroundTexture;
	if (!landingPageBackgroundTexture.loadFromFile("Textures/loading page background1.jpg"))
		throw ("error loading file");

	Sprite landingpagebackgroundsprite;
	landingpagebackgroundsprite.setTexture(landingPageBackgroundTexture);
	landingpagebackgroundsprite.setScale(1.65f, 1.5f);
	//backgroundTexture.setsmooth(1);


	//"NEW GAME" button
	RectangleShape newGameBt(Vector2f(275.f, 60.f));
	newGameBt.setFillColor(Color(0, 255, 0, 128));
	newGameBt.setPosition((window.getPosition().x + window.getPosition().x / 3), window.getPosition().y * 1.8);
	newGameBt.setOutlineThickness(2);
	newGameBt.setOutlineColor(Color::Black);

	//"EXIT GAME" button
	RectangleShape exitGameBt(Vector2f(275.f, 60.f));
	exitGameBt.setFillColor(Color(255, 0, 0, 128));
	exitGameBt.setPosition((window.getPosition().x + window.getPosition().x / 3), window.getPosition().y * 2.2);

	//NEW GAME Text
	Text newgameText;
	newgameText.setFont(dosislight);
	newgameText.setStyle(Text::Bold);
	newgameText.setFillColor(Color::Black);
	newgameText.setCharacterSize(26);
	newgameText.setStyle(Text::Bold);
	newgameText.setString("NEW GAME");
	newgameText.setPosition(newGameBt.getPosition().x + (newGameBt.getGlobalBounds().width / 3) - 10.f, newGameBt.getPosition().y + (newGameBt.getGlobalBounds().height / 5));

	//load game Text
	Text loadgameText;
	loadgameText.setFont(dosislight);
	loadgameText.setStyle(Text::Bold);
	loadgameText.setFillColor(Color::Black);
	loadgameText.setCharacterSize(26);
	loadgameText.setStyle(Text::Bold);
	loadgameText.setString("EXIT GAME");
	loadgameText.setPosition(exitGameBt.getPosition().x + (exitGameBt.getGlobalBounds().width / 3) - 10.f, exitGameBt.getPosition().y + (exitGameBt.getGlobalBounds().height / 5));

	//landing page music
	if (!landingpagemusic.openFromFile("Textures/john wick ost.wav")) 
	{
		throw("could not load file");
	}
	landingpagemusic.setPlayingOffset( seconds(50));
	landingpagemusic.play();


	bool newgamekey = false;
	bool exitgamekey = false;
	bool continuePlaying = true;


	while (window.isOpen()) {
		Event event;

		while (window.pollEvent(event)) {
			if (event.type == Event::Closed) {
				window.close();
			}
		}

		if (Mouse::isButtonPressed(Mouse::Left)) {
			Vector2i mousepos = Mouse::getPosition(window);
			if (newGameBt.getGlobalBounds().contains(mousepos.x, mousepos.y)) {
				newgamekey = true;
			}
			if (exitGameBt.getGlobalBounds().contains(mousepos.x, mousepos.y)) {

			}
		}

		displayLandingPage(window, landingpagebackgroundsprite, newGameBt, exitGameBt, newgameText, loadgameText);
		window.display();

		if (newgamekey) {

			window.close();
			landingpagemusic.stop();
			playLevel1(landingpagemusic, continuePlaying);

		}
		else if (exitgamekey) {
			window.close();
		}
	}

}
void displayWinLevelScreen()
{

	RenderWindow window(sf::VideoMode(1200, 600), "Congratulatory Screen");
	// Create a font for the text
	Font font;
	if (!font.loadFromFile("Fonts/Metropolian-Display.ttf")) {
		throw("Error loading file");
	}

	// Create a text object for the congratulatory message
	Text congratsText("CONGRATULATIONS!\nYOU COMPLETED THE LEVEL.\nTURNS OUT YOU'RE NOT A NOOB", font, 50);
	congratsText.setPosition(100.f, 100.f);
	congratsText.setFillColor(sf::Color::Green);

	
	Text continueText("\n\nPress ENTER to continue to the next level.", font, 30);
	continueText.setPosition(400.f, 400.f);
	continueText.setFillColor(sf::Color::Cyan);

	//Game Win Music
	Music gameWinMusic;
	if (!gameWinMusic.openFromFile("Textures/gameWinMusic.wav"))
		throw("Error loading file");
	gameWinMusic.play();

	bool continueToNextLevel = false;
	while (window.isOpen() && !continueToNextLevel) {
		
		Event event;
		while (window.pollEvent(event)) {
			if (event.type == Event::Closed) 
				window.close();
			
			else if (event.type == Event::KeyPressed) {
				if (event.key.code == Keyboard::Enter) 
					continueToNextLevel = true;
				
			}
		}
		if (continueToNextLevel)
			playLevel2();

		window.clear(Color::Black);

		
		window.draw(congratsText);
		window.draw(continueText);

		window.display();
	}
}
void displayLoseLevel1Screen(Music&landingpagemusic,bool&continuePlaying)
{
	RenderWindow window(sf::VideoMode(1200, 600), "Congratulatory Screen");
	
	Font font;
	if (!font.loadFromFile("Fonts/Metropolian-Display.ttf")) {
		throw("Error loading file");
	}

	
	Text congratsText("HAH!! NEW YOU WOULD LOSE!\nNOOB", font, 50);
	congratsText.setPosition(100.f, 100.f);
	congratsText.setFillColor(sf::Color::Red);


	Text continueText("\n\nPress ENTER to save face", font, 30);
	continueText.setPosition(400.f, 400.f);
	continueText.setFillColor(sf::Color::Yellow);

	//Game Lose Music
	Music gameLoseMusic;
	if (!gameLoseMusic.openFromFile("Textures/game over music.wav"))
		throw("Error loading file");
	gameLoseMusic.play();

	bool repeatLevel = false;
	while (window.isOpen() && !repeatLevel) {

		Event event;
		while (window.pollEvent(event)) {
			if (event.type == Event::Closed)
				window.close();

			else if (event.type == Event::KeyPressed) {
				if (event.key.code == Keyboard::Enter)
					repeatLevel = true;

			}
		}
		if (repeatLevel)
			playLevel1(landingpagemusic,continuePlaying);

		window.clear(Color::Black);


		window.draw(congratsText);
		window.draw(continueText);

		window.display();
	}
}
void displayLoseLevel2Screen()
{
	RenderWindow window(sf::VideoMode(1200, 600), "Congratulatory Screen");

	Font font;
	if (!font.loadFromFile("Fonts/Metropolian-Display.ttf")) {
		throw("Error loading file");
	}


	Text congratsText("HAH!! NEW YOU WOULD LOSE!\nNOOB", font, 50);
	congratsText.setPosition(100.f, 100.f);
	congratsText.setFillColor(sf::Color::Red);


	Text continueText("\n\nPress ENTER to save face", font, 30);
	continueText.setPosition(400.f, 400.f);
	continueText.setFillColor(sf::Color::Yellow);

	//Game Lose Music
	Music gameLoseMusic;
	if (!gameLoseMusic.openFromFile("Textures/game over music.wav"))
		throw("Error loading file");
	gameLoseMusic.play();

	bool repeatLevel = false;
	while (window.isOpen() && !repeatLevel) {

		Event event;
		while (window.pollEvent(event)) {
			if (event.type == Event::Closed)
				window.close();

			else if (event.type == Event::KeyPressed) {
				if (event.key.code == Keyboard::Enter)
					repeatLevel = true;

			}
		}
		if (repeatLevel)
			playLevel2();

		window.clear(Color::Black);


		window.draw(congratsText);
		window.draw(continueText);

		window.display();
	}
}
void playLevel2() {

	RenderWindow window(VideoMode(1200, 600), "LEVEL 2");

	srand(time(0));

	Music backgroundMusic;
	if (!backgroundMusic.openFromFile("Textures/spaceGameBackgroundMusic.wav"))
		throw("File Not Available");


	Obstacles** obstacles;
	obstacles = new Obstacles * [10];

	Coin** coins;
	coins = new Coin * [10];

	float posx = 1200;
	for (int i = 0; i < 10; i++) 
	{
		if (i % 2 == 0) {
			obstacles[i] = new WoodObstacle();
		}
		else {
			obstacles[i] = new MetalObstacle();
		}

		if (rand() % 2 != 0) {
			obstacles[i]->setPosition(posx + i * 90, obstacles[i]->getPositionY());
		}
		else {
			obstacles[i]->setPosition(posx + i * 90, 600 - obstacles[i]->getPositionY());
			obstacles[i]->setRotation(180);

		}
		obstacles[i]->setScale(0.3, 0.3);

		coins[i] = new Coin();
		coins[i]->setPosition(i * 180, rand() % 450);
		coins[i]->setScale(0.6, 0.6);

	}

	float speed = 200;
	float jumpspeed = 400;

	float objpos = 0;

	bool key = false;
	bool gameover = false;

	Clock clock;


	MainCharacter character;
	character.setPosition(0, 220);
	character.setScale(0.8, 1);

	Text Coins;
	Font font;
	if (!font.loadFromFile("fonts/dosis-bold.ttf")) {
		cout << "error loading font";
	}
	Coins.setFont(font);
	Coins.setFillColor(Color::Yellow);

	//Sky background
	Texture skyBackgroundTex;
	if (!skyBackgroundTex.loadFromFile("Textures/sky background.jpg"))
		throw ("Error loading file");
	int skySpawnTimer = 0;
	Background background(&skyBackgroundTex, window, Vector2u(1, 1), 10.5f, 0.f);
	vector<Background>backgrounds;
	background.shape.setScale(1.5f, 1.f);
	background.shape.setPosition(window.getPosition().x * 3, window.getSize().y - background.shape.getGlobalBounds().height * 1.5f);
	
	Time offset = seconds(15.f);
	backgroundMusic.setPlayingOffset(offset);
	backgroundMusic.play();
	while (window.isOpen()) {
		Event event;

		float deltatime = clock.restart().asSeconds();


		while (window.pollEvent(event)) {
			if (event.type == Event::Closed) {
				window.close();
			}
			if (Keyboard::isKeyPressed(Keyboard::Space)) {
				key = true;
			}
		}


		for (int i = 0; i < 10; i++) {

			if (obstacles[i]->getPositionX() < -150) {
				if (rand() % 2 != 0) {
					obstacles[i]->setRotation(0);

					if (obstacles[i]->getobnum() == 1) {
						obstacles[i]->setPosition(1275, 411);
					}
					else {
						obstacles[i]->setPosition(1275, 474);
					}
				}
				else {
					obstacles[i]->setRotation(180);
					if (obstacles[i]->getobnum() == 1) {
						obstacles[i]->setPosition(1275, 189);
					}
					else {
						obstacles[i]->setPosition(1275, 126);
					}
				}
				obstacles[i]->setScale(0.3, 0.3);
			}

			obstacles[i]->move(-speed * deltatime, 0);
		}
		for (int i = 0; i < 10; i++) {
			if (coins[i]->getPositionX() < -150) {
				coins[i]->setPosition(1200, rand() % 600);
				coins[i]->setScale(0.7, 0.7);
				FloatRect coinbounds = coins[i]->getGlobalBounds();
				bool intersectsobstacle = true;

				while (intersectsobstacle) {
					intersectsobstacle = false;
					for (int j = 0; j < 10; j++) {
						FloatRect obstaclebounds = obstacles[j]->getGlobalBounds();
						if (coinbounds.intersects(obstaclebounds)) {
							coins[i]->setPosition(1200, rand() % 600);
							coinbounds = coins[i]->getGlobalBounds();
							intersectsobstacle = true;
							break;
						}
					}
				}
			}
			coins[i]->move(-speed * deltatime, 0);
		}
		cout << character.getcoinscollected();

		FloatRect characterbounds = character.getGlobalBounds();

		characterbounds.width -= 15;
		characterbounds.top += 30;
		characterbounds.height -= 50;

		for (int i = 0; i < 10; i++) {

			FloatRect obstaclebounds = obstacles[i]->getGlobalBounds();
			FloatRect coinbounds = coins[i]->getGlobalBounds();

			if (characterbounds.intersects(coinbounds)) {
				coins[i]->setScale(0, 0);
				character.collectcoin();

			}


			if (characterbounds.intersects(obstaclebounds)) {


				gameover = true;
				displayLoseLevel2Screen();
				break;
			}
		}

		if (key && !gameover) {
			if (objpos - 75 >= character.getPositionY()) {
				key = false;
			}
			character.move(0, -jumpspeed * deltatime);
		}

		else if (!gameover) {
			objpos = character.getPositionY();
			character.move(0, speed * deltatime);
		}
		if (character.getPositionY() > 600) {
			gameover = true;
		}

		if (gameover) {
			character.move(0, 0);
			speed = 0;
			cout << "game over!" << endl;
		}
		for (int i = 0; i < 10; i++) {

			coins[i]->animate(deltatime);

		}
		Coins.setString(to_string(character.getcoinscollected()));
		character.animate(deltatime);
		
		updateSpawnTimer(skySpawnTimer, 200);
		if (skySpawnTimer >= 200)
		{
			backgrounds.push_back(background);
			skySpawnTimer = 0; //reset timer
		}
		//Update Sky
		for (size_t i = 0; i < backgrounds.size(); i++)
		{
			backgrounds[i].Update(deltatime, rand() % 1);
			backgrounds[i].shape.move(-4.f, 0.f);
			if (backgrounds[i].shape.getPosition().x <= 0 - backgrounds[i].shape.getGlobalBounds().width)
			{
				backgrounds.erase(backgrounds.begin() + i);
				break;
			}
		}

		// DRAW
		window.clear();

		for (size_t i = 0; i < backgrounds.size(); i++)
			window.draw(backgrounds[i].shape);

		for (int i = 0; i < 10; i++) {
			obstacles[i]->draw(window);
			coins[i]->draw(window);
		}
		window.draw(Coins);
		character.draw(window);
		//if (gameover) {
		//	window.draw(retryButton);
		//}
		if (character.getcoinscollected() >= 15)
		{
			window.close();
			backgroundMusic.stop();
			displayWinLevelScreen();
		}

		window.display();
	}

	// clean up memory
	for (int i = 0; i < 10; i++) {
		delete obstacles[i];
		delete coins[i];
	}
	delete[] obstacles;
	delete[] coins;


	//const int windowWidth = 1200;
	//const int windowHeight = 600;
	//const int obstacleWidth = 50; // Assuming obstacle width
	//const int obstacleHeight = 50; // Assuming obstacle height
	//const int minDistance = 100; // Minimum distance between obstacles
	//const int numObstacles = 8;

	//RenderWindow window(VideoMode(1200, 600), "LEVEL 2");

	//srand(time(0));

	////Music backgroundMusic;


	//Obstacles** obstacles;
	//obstacles = new Obstacles * [numObstacles];

	//Coin** coins;
	//coins = new Coin * [numObstacles];

	//float posx = 1200;
	//for (int i = 0; i < numObstacles; i++) {
	//	if (i % 2 == 0) {
	//		obstacles[i] = new WoodObstacle();
	//	}
	//	else {
	//		obstacles[i] = new MetalObstacle();
	//	}

	//	if (rand() % 2 != 0) {
	//		obstacles[i]->setPosition(posx + i * 200, obstacles[i]->getPositionY());
	//	}
	//	else {
	//		obstacles[i]->setPosition(posx + i * 200, 600 - obstacles[i]->getPositionY());
	//		obstacles[i]->setRotation(180);

	//	}
	//	obstacles[i]->setScale(0.3, 0.3);

	//	coins[i] = new Coin();
	//	coins[i]->setPosition(i * 180, rand() % 450);
	//	coins[i]->setScale(0.6, 0.6);

	//}

	//float speed = 200;
	//float jumpspeed = 400;

	//float objpos = 0;

	//bool key = false;
	//bool gameover = false;

	//Clock clock;


	//MainCharacter character;
	//character.setPosition(0, 220);
	//character.setScale(0.8, 1);

	//Text Coins;
	//Font font;
	//if (!font.loadFromFile("fonts/dosis-bold.ttf")) {
	//	cout << "error loading font";
	//}
	//Coins.setFont(font);
	//Coins.setFillColor(Color::Yellow);

	////Sky background
	//Texture skyBackgroundTex;
	//if (!skyBackgroundTex.loadFromFile("Textures/sky background.jpg"))
	//	throw ("Error loading file");
	//int skySpawnTimer = 0;
	//Background background(&skyBackgroundTex, window, Vector2u(1, 1), 10.5f, 0.f);
	//vector<Background>backgrounds;
	//background.shape.setScale(1.5f, 1.f);
	//background.shape.setPosition(window.getPosition().x * 3, window.getSize().y - background.shape.getGlobalBounds().height * 1.5f);



	//while (window.isOpen()) {
	//	Event event;

	//	float deltatime = clock.restart().asSeconds();


	//	while (window.pollEvent(event)) {
	//		if (event.type == Event::Closed) {
	//			window.close();
	//		}
	//		if (Keyboard::isKeyPressed(Keyboard::Space)) {
	//			key = true;
	//		}
	//	}

	//	bool isDistant = false;
	//	int posY;

	//	for (int i = 0; i < numObstacles; i++) {
	//		isDistant = false; // Reset the flag for each new obstacle

	//		while (!isDistant) {
	//			posY = rand() % windowHeight; // Generate a new random y-coordinate

	//			isDistant = true; // Assume the new position is distant unless proven otherwise

	//			for (int j = 0; j < numObstacles; j++) {
	//				if (i != j) { // Skip checking the distance with the same obstacle
	//					int obsDistance = Helpers::euclideanDistance(obstacles[j]->getPositionX(), obstacles[j]->getPositionY(), 1275, posY);
	//					if (obsDistance <= 150) {
	//						isDistant = false; // Distance is too close, set the flag to false
	//						break; // Exit the loop since a close obstacle is found
	//					}
	//				}
	//			}
	//		}
	//		if (obstacles[i]->getPositionX() < -150) {
	//			if (rand() % 2 != 0) {
	//				obstacles[i]->setRotation(0);

	//				if (obstacles[i]->getobnum() == 1) {
	//					obstacles[i]->setPosition(1275, posY);
	//				}
	//				else {
	//					obstacles[i]->setPosition(1275, posY);
	//				}
	//			}
	//			else {
	//				obstacles[i]->setRotation(180);
	//				if (obstacles[i]->getobnum() == 1) {
	//					obstacles[i]->setPosition(1275, posY);
	//				}
	//				else {
	//					obstacles[i]->setPosition(1275, posY);
	//				}
	//			}
	//			obstacles[i]->setScale(0.3, 0.3);
	//		}

	//		obstacles[i]->move(-speed * deltatime, 0);
	//	}


	//	for (int i = 0; i < numObstacles; i++) {
	//		if (coins[i]->getPositionX() < -150) {
	//			coins[i]->setPosition(1200, rand() % 600);
	//			coins[i]->setScale(0.7, 0.7);
	//			FloatRect coinbounds = coins[i]->getGlobalBounds();
	//			bool intersectsobstacle = true;

	//			while (intersectsobstacle) {
	//				intersectsobstacle = false;
	//				for (int j = 0; j < numObstacles; j++) {
	//					FloatRect obstaclebounds = obstacles[j]->getGlobalBounds();
	//					if (coinbounds.intersects(obstaclebounds)) {
	//						coins[i]->setPosition(1200, rand() % 600);
	//						coinbounds = coins[i]->getGlobalBounds();
	//						intersectsobstacle = true;
	//						break;
	//					}
	//				}
	//			}
	//		}
	//		coins[i]->move(-speed * deltatime, 0);
	//	}
	//	cout << character.getcoinscollected();

	//	FloatRect characterbounds = character.getGlobalBounds();

	//	float reductionAmount = 45.0f;

	//	characterbounds.width -= reductionAmount;
	//	characterbounds.height -= reductionAmount;
	//	characterbounds.left += reductionAmount / 2.0f;  // Adjust the left position when reducing the width
	//	characterbounds.top += reductionAmount / 2.0f;   // Adjust the top position when reducing the height


	//	for (int i = 0; i < numObstacles; i++) {

	//		FloatRect obstaclebounds = obstacles[i]->getGlobalBounds();
	//		FloatRect coinbounds = coins[i]->getGlobalBounds();

	//		if (characterbounds.intersects(coinbounds)) {
	//			coins[i]->setScale(0, 0);
	//			character.collectcoin();
	//			gameover = true;
	//		}


	//		if (characterbounds.intersects(obstaclebounds)) {


	//			gameover = true;
	//			displayLoseLevel2Screen();
	//			break;
	//		}
	//	}

	//	if (key && !gameover) {
	//		if (objpos - 75 >= character.getPositionY()) {
	//			key = false;
	//		}
	//		character.move(0, -jumpspeed * deltatime);
	//	}

	//	else if (!gameover) {
	//		objpos = character.getPositionY();
	//		character.move(0, speed * deltatime);
	//	}
	//	if (character.getPositionY() > 600) {
	//		gameover = true;
	//	}

	//	if (gameover) {
	//		character.move(0, 0);
	//		speed = 0;
	//		cout << "game over!" << endl;
	//	}
	//	for (int i = 0; i < numObstacles; i++) {

	//		coins[i]->animate(deltatime);

	//	}
	//	Coins.setString(to_string(character.getcoinscollected()));
	//	character.animate(deltatime);

	//	Helpers::updateSpawnTimer(skySpawnTimer, 200);
	//	if (skySpawnTimer >= 200)
	//	{
	//		backgrounds.push_back(background);
	//		skySpawnTimer = 0; //reset timer
	//	}
	//	//Update Sky
	//	for (size_t i = 0; i < backgrounds.size(); i++)
	//	{
	//		backgrounds[i].Update(deltatime, rand() % 1);
	//		backgrounds[i].shape.move(-4.f, 0.f);
	//		if (backgrounds[i].shape.getPosition().x <= 0 - backgrounds[i].shape.getGlobalBounds().width)
	//		{
	//			backgrounds.erase(backgrounds.begin() + i);
	//			break;
	//		}
	//	}

	//	// DRAW
	//	window.clear();

	//	for (size_t i = 0; i < backgrounds.size(); i++)
	//		window.draw(backgrounds[i].shape);

	//	for (int i = 0; i < numObstacles; i++) {
	//		obstacles[i]->draw(window);
	//		coins[i]->draw(window);
	//	}
	//	window.draw(Coins);
	//	character.draw(window);
	//	//if (gameover) {
	//	//	window.draw(retryButton);
	//	//}
	//	if (character.getcoinscollected() >= 15)
	//		displayWinLevelScreen();

	//	window.display();
	//}

	//// clean up memory
	//for (int i = 0; i < numObstacles; i++) {
	//	delete obstacles[i];
	//	delete coins[i];
	//}
	//delete[] obstacles;
	//delete[] coins;



}



int main() {
	Music landingPageMusic;
	displaymenu(landingPageMusic);
}
