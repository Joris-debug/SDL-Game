#include "GameHandler.h"
#include "Interface.h"
#include "World.h"
#include "Resources.h"
#include "Enemy.h"
#include "Player.h"
#include "SDL_image.h"
#include "Serial.h"
int GameHandler::gameLoop()
{
	int x_input = 0, y_input = 0, attackTrigger = false;
	Uint32 currentTime = SDL_GetTicks(); //Calculate delta time
	Uint32 lastTime = currentTime;
	while (true)
	{
		currentTime = SDL_GetTicks();
		m_deltaTime_ = (currentTime - lastTime);

		if (m_deltaTime_ > float(1000 / 60)) //Limit FPS auf 60
		{
			lastTime = currentTime;

			while (SDL_PollEvent(m_p_interface_->getInputQueue()) != 0) //Needed for basic window responses (scaling, moving, closing)
			{
				switch (m_p_interface_->getInputQueue()->type)
				{
				case SDL_QUIT:
					exit(1);
					break;
				}
			}

			if (m_p_serial_->dataAvailable()) {
				std::string input = m_p_serial_->readLine();
				std::string direction = input.substr(0, input.find(';'));
				std::string attack = input.substr(input.find(';') + 1);
				attack = attack.substr(0, attack.size() - 1);

				if (attack == "true") {
					attackTrigger = true;
				}

				this->m_lastDirection_ = direction;
			}

			do {

				if (m_lastDirection_ == "C") {
					x_input = 0;
					y_input = 0;
					break;
				}

				if (m_lastDirection_ == "N") {
					x_input = 0;
					y_input = 1;
					break;
				}

				if (m_lastDirection_ == "NE") {
					x_input = -1;
					y_input = 1;
					break;
				}

				if (m_lastDirection_ == "E") {
					x_input = -1;
					y_input = 0;
					break;
				}

				if (m_lastDirection_ == "SE") {
					x_input = -1;
					y_input = -1;
					break;
				}

				if (m_lastDirection_ == "S") {
					x_input = 0;
					y_input = -1;
					break;
				}

				if (m_lastDirection_ == "SW") {
					x_input = 1;
					y_input = -1;
					break;
				}

				if (m_lastDirection_ == "W") {
					x_input = 1;
					y_input = 0;
					break;
				}

				if (m_lastDirection_ == "NW") {
					x_input = 1;
					y_input = 1;
					break;
				}

			} while (false);

			if (attackTrigger) {	
				m_p_currentWorld_->triggerPlayerAttack();
				attackTrigger = false;
			}

			m_p_interface_->getPixelPerPixel();
			m_p_currentWorld_->moveWorld(x_input, y_input, m_deltaTime_ * 0.2);
			renderEverything();

			if (!m_p_currentWorld_->getPlayer()->get()->getCurrentLives()) {
				break;
			}
		}
		
	}

	m_p_interface_->waitForInput(1000);
	return 0;
}

GameHandler::GameHandler(Interface* m_p_interface_, SDL_Renderer* m_p_renderer_)
{
	this->m_p_interface_ = m_p_interface_;
	this->m_p_renderer_ = m_p_renderer_;
	this->m_deltaTime_ = 1;

	SDL_Surface* p_tmpSurface;
	//Load all Mantis Spritesheets
	p_tmpSurface = IMG_Load(RSC_MANTIS_IDLE);
	m_enemyTexturesIdle_.push_back(SDL_CreateTextureFromSurface(m_p_renderer_, p_tmpSurface));
	SDL_FreeSurface(p_tmpSurface);
	p_tmpSurface = IMG_Load(RSC_MANTIS_WALK);
	m_enemyTexturesWalk_.push_back(SDL_CreateTextureFromSurface(m_p_renderer_, p_tmpSurface));	
	SDL_FreeSurface(p_tmpSurface);
	p_tmpSurface = IMG_Load(RSC_MANTIS_HIT);
	m_enemyTexturesHit_.push_back(SDL_CreateTextureFromSurface(m_p_renderer_, p_tmpSurface));
	SDL_FreeSurface(p_tmpSurface);

	//Load all Maggot Spritesheets
	p_tmpSurface = IMG_Load(RSC_MAGGOT_IDLE);
	m_enemyTexturesIdle_.push_back(SDL_CreateTextureFromSurface(m_p_renderer_, p_tmpSurface));
	SDL_FreeSurface(p_tmpSurface);
	p_tmpSurface = IMG_Load(RSC_MAGGOT_WALK);
	m_enemyTexturesWalk_.push_back(SDL_CreateTextureFromSurface(m_p_renderer_, p_tmpSurface));
	SDL_FreeSurface(p_tmpSurface);
	p_tmpSurface = IMG_Load(RSC_MAGGOT_HIT);
	m_enemyTexturesHit_.push_back(SDL_CreateTextureFromSurface(m_p_renderer_, p_tmpSurface));
	SDL_FreeSurface(p_tmpSurface);

	//Load all hud textures
	p_tmpSurface = IMG_Load(RSC_HEALTHBAR_BORDER);
	m_hudTextures_.push_back(SDL_CreateTextureFromSurface(m_p_renderer_, p_tmpSurface));
	SDL_FreeSurface(p_tmpSurface);

	p_tmpSurface = IMG_Load(RSC_HEALTHBAR_LIVES);
	m_hudTextures_.push_back(SDL_CreateTextureFromSurface(m_p_renderer_, p_tmpSurface));
	SDL_FreeSurface(p_tmpSurface);

	p_tmpSurface = IMG_Load(RSC_HEALTHBAR_STAMINA);
	m_hudTextures_.push_back(SDL_CreateTextureFromSurface(m_p_renderer_, p_tmpSurface));
	SDL_FreeSurface(p_tmpSurface);

	p_tmpSurface = IMG_Load(RSC_HEALTHBAR_CHECK);
	m_hudTextures_.push_back(SDL_CreateTextureFromSurface(m_p_renderer_, p_tmpSurface));
	SDL_FreeSurface(p_tmpSurface);

	std::string comName;
	std::cout << "Please specify port [Nr]: ";
	std::cin >> comName;
	this->m_p_serial_ = new Serial("COM" + comName, 9600, 8, 1, 0);
	if (!m_p_serial_->open()) {
		std::cout << "Error" << std::endl;
	}
	this->m_lastDirection_ = "C"; //No direction
}

GameHandler::~GameHandler()
{
	int numberOfTextures = m_enemyTexturesIdle_.size();

	for (int i = 0; i < numberOfTextures; i++) {
		SDL_DestroyTexture(m_enemyTexturesIdle_.back());
		m_enemyTexturesIdle_.pop_back();
		SDL_DestroyTexture(m_enemyTexturesWalk_.back());
		m_enemyTexturesWalk_.pop_back();
		SDL_DestroyTexture(m_enemyTexturesHit_.back());
		m_enemyTexturesHit_.pop_back();
	}

	numberOfTextures = m_hudTextures_.size();

	for (int i = 0; i < numberOfTextures; i++) {
		SDL_DestroyTexture(m_hudTextures_.back());
		m_hudTextures_.pop_back();
	}

	m_p_serial_->close();
}

int GameHandler::initLevel1()
{
	m_p_currentWorld_ = std::unique_ptr<World>(new World(IMG_Load(RSC_LEVEL_1), { -1232,-1280,1632 * 2,1632 * 2 }, m_p_renderer_, &m_randomNumberEngine_));
	m_p_currentWorld_->addVinicityToMap(new Vicinity(IMG_Load(RSC_LEVEL_1_TOP), { -1232,-1280,1632 * 2,1632 * 2 }, m_p_renderer_));

	m_p_currentWorld_->addEntityToMap(new Entity({ -1232, -1280, 3264, 32 })); // Border left
	m_p_currentWorld_->addEntityToMap(new Entity({ -1232, -1280 + 1616*2, 3264, 32 })); // Border bottom
	m_p_currentWorld_->addEntityToMap(new Entity({ -1232 + 1616 * 2, -1280, 32, 3264 })); // Border right
	m_p_currentWorld_->addEntityToMap(new Entity({ -1232, -1280, 32,  3264})); // Border top
	m_p_currentWorld_->addEntityToMap(new Entity({ -1232 + 528 * 2, -1280 + 768 * 2, 55, 64 })); //First sign on the left from spawn
	m_p_currentWorld_->addEntityToMap(new Entity({ -1232 + 835 * 2, -1280 + 1088 * 2, 55, 64 })); //First sign on the bottom from spawn
	m_p_currentWorld_->addEntityToMap(new Entity({ -1232 + 336 * 2, -1280 + 608 * 2, 192, 64 })); //Holy statue
	m_p_currentWorld_->addEntityToMap(new Entity({ -1232 + 1344 * 2, -1280 + 624 * 2, 64, 64 })); //Shrine right from spawn

	//m_p_currentWorld_->addEnemyToMap(new Enemy(m_enemyTexturesIdle_[0], m_enemyTexturesWalk_[0], m_enemyTexturesHit_[0], { 800, 200, 64, 64 }, { 800, 200, 64, 64 }, 2));
	//m_p_currentWorld_->addEnemyToMap(new Enemy(m_enemyTexturesIdle_[0], m_enemyTexturesWalk_[0], m_enemyTexturesHit_[0], { 500, 500, 64, 64 }, { 500, 500, 64, 64 }, 2));
	//m_p_currentWorld_->addEnemyToMap(new Enemy(m_enemyTexturesIdle_[1], m_enemyTexturesWalk_[1], m_enemyTexturesHit_[1], { -800, -68, 64, 32 }, { -800, -100, 64, 64 }, 1));


	return gameLoop();
}

void GameHandler::renderHud()
{
	SDL_FRect hudRect = { 10, 10, 64 * 5, 10 * 5 };
	SDL_FRect healthRect = { 10 + 5 * 5, 10 + 1 * 5, 58 * 5, 4 * 5 };
	SDL_FRect staminaRect = { 20, 10 + 6 * 5, 56 * 5, 2 * 5 };

	Player* p_player = m_p_currentWorld_->getPlayer()->get();
	
	float healthPercent = float(p_player->getCurrentLives()) / float(p_player->getMaxLives());
	healthRect.w *= healthPercent;
	SDL_Rect tmpTextureCoords = { 0, 0, 58 * healthPercent, 4 };
	SDL_RenderCopyF(m_p_renderer_, m_hudTextures_[1], &tmpTextureCoords, &healthRect); //Health

	float staminaPercent = (SDL_GetTicks() - p_player->getLastAttack()) / PLAYER_ATTACK_COOLDOWN;
	if (staminaPercent > 1) {
		staminaPercent = 1;
		SDL_RenderCopyF(m_p_renderer_, m_hudTextures_[3], NULL, &hudRect); //Check
	}
	staminaRect.w = round(staminaRect.w * staminaPercent);
	SDL_RenderCopyF(m_p_renderer_, m_hudTextures_[2], NULL, &staminaRect); //Stamina
	SDL_RenderCopyF(m_p_renderer_, m_hudTextures_[0], NULL, &hudRect); //Border
}

void GameHandler::renderEverything()
{
	SDL_RenderClear(m_p_renderer_);
	m_p_currentWorld_->renderWorld(m_p_renderer_);
	renderHud();
	SDL_RenderPresent(m_p_renderer_);
}
