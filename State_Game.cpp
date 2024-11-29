#include "State_Game.h"
#include "StateManager.h"
#include "Comp_Position.h"
#include "Comp_Movement.h"
#include "Comp_Invader.h"
#include "LevelManager.h"
#include "Utils.h"

State_Game::State_Game(StateManager* stateManager) :
	State(stateManager),
	m_levelManager(stateManager->getContext()->m_actorManager),
	m_hudUpdateTimer(0),
	m_newGame(true),
	m_fps(0),
	m_soundOn(true),
	m_musicOn(false)
{
}

void State_Game::update(const float& deltaTime)
{
	if (m_levelManager.getEnemyCount() <= 0)
		loadNextLevel();
	m_fps = static_cast<unsigned int>(1.f / deltaTime);
	// game update
	m_stateManager->getContext()->m_systemManager->update(deltaTime);
	// HUD update
	m_hudUpdateTimer += deltaTime;
	if (m_hudUpdateTimer >= m_hudUpdateInterval)
	{
		updateHUD();
		m_hudUpdateTimer -= m_hudUpdateInterval;
	}
	// if player dead, prepare to switch to Game Over screen
	switch (m_levelManager.getState())
	{
		case LevelState::PlayerAlive:
			break;
		case LevelState::PlayerDestroyed:
			gameOverScreen();
			break;
		case LevelState::PlayerInvaded:
			gameOverScreen();
			break;
	}
}

void State_Game::draw()
{
	drawGame();
	drawHUD();
}

void State_Game::onCreate()
{
	sf::Vector2u windowSize = m_stateManager->getContext()->m_windowManager->getRenderWindow()->getSize();
	m_view.setSize(static_cast<float>(windowSize.x), static_cast<float>(windowSize.y));
	m_view.setCenter(static_cast<float>(windowSize.x) / 2, static_cast<float>(windowSize.y) / 2);
	m_view.setViewport(sf::FloatRect(0, 0, 1, 1));
	m_gameView.setViewport(sf::FloatRect(0.15f, 0, 0.7f, 1));

	m_stateManager->getContext()->m_systemManager->setLevelManager(&m_levelManager);
	m_levelManager.setViewSpace(getGameViewSpace());
	initializeHUD();
	setupHelpPanel();
	// load music
	m_stateManager->getContext()->m_soundManager->loadSoundProfile("assets/profiles/soundProfiles/game_state.sound");
	// play  music
	m_stateManager->getContext()->m_soundManager->playMusic("game_music");
	setSound(true);
	setMusic(false);
	// set player icon position to upper right of screen
	m_playerIconPosition = sf::Vector2f(
		m_view.getCenter().x + m_view.getSize().x / 2 - m_hudPadding.x,
		m_view.getCenter().y - m_view.getSize().y / 2 + m_hudPadding.y
	);
}

void State_Game::activate()
{
	m_stateManager->getContext()->m_controller->m_onPause.addCallback("Game_onPause", std::bind(&StateManager::switchTo, m_stateManager, StateType::Paused));
	m_stateManager->getContext()->m_controller->m_onMove.addCallback("Game_onMove", std::bind(&State_Game::onPlayerMove, this, std::placeholders::_1));
	m_stateManager->getContext()->m_controller->m_onShoot.addCallback("Game_onShoot", std::bind(&State_Game::onPlayerShoot, this));
	m_stateManager->getContext()->m_controller->m_onToggleHelp.addCallback("Game_onToggleHelp", std::bind(&State_Game::onToggleHelp, this));
	m_stateManager->getContext()->m_controller->m_onToggleSound.addCallback("Game_onToggleSound", std::bind(&State_Game::onToggleSound, this));
	m_stateManager->getContext()->m_controller->m_onToggleMusic.addCallback("Game_onToggleMusic", std::bind(&State_Game::onToggleMusic, this));
	if (m_newGame)
		newGame();
}

void State_Game::deactivate()
{
	m_stateManager->getContext()->m_controller->m_onPause.removeCallback("Game_onPause");
	m_stateManager->getContext()->m_controller->m_onMove.removeCallback("Game_onMove");
	m_stateManager->getContext()->m_controller->m_onShoot.removeCallback("Game_onShoot");
	m_stateManager->getContext()->m_controller->m_onToggleHelp.removeCallback("Game_onToggleHelp");
	m_stateManager->getContext()->m_controller->m_onToggleSound.removeCallback("Game_onToggleSound");
	m_stateManager->getContext()->m_controller->m_onToggleMusic.removeCallback("Game_onToggleMusic");
}

void State_Game::loadNextLevel()
{
	// increase level
	m_levelManager++;
	// reset invader count
	m_levelManager.resetInvaderCount();
	// enable (or re-enable) all actors
	//for (auto& invaderId : m_levelManager.getInvaderIds())
	//	m_stateManager->getContext()->m_actorManager->enableActor(invaderId);
	auto invaderSys = m_stateManager->getContext()->m_systemManager->getSystem<Sys_InvaderControl>(SystemType::InvaderControl);
	invaderSys->queueInvaders(m_levelManager.getInvaderIds());
	// start (or re-start) all systems
	m_stateManager->getContext()->m_systemManager->start();
}

void State_Game::onPlayerMove(sf::Vector2f xy)
{
	unsigned int playerId = m_levelManager.getPlayerId();
	auto actor = m_stateManager->getContext()->m_actorManager->getActor(playerId);
	auto targetComp = actor->getComponent<Comp_Target>(ComponentType::Target);
	targetComp->setTarget(targetComp->getTarget() + xy);
}

void State_Game::onPlayerShoot()
{
	m_stateManager->getContext()->m_systemManager->addEvent(m_levelManager.getPlayerId(), (EventId)ActorEventType::Shoot);
}

void State_Game::onToggleHelp()
{
	m_showHelp = !m_showHelp;
}

void State_Game::setSound(bool soundOn)
{
	m_soundOn = soundOn;
	auto soundSys = m_stateManager->getContext()->m_systemManager->getSystem<Sys_Sound>(SystemType::Sound);
	soundSys->setSound(m_soundOn);
	m_soundText.setFillColor(m_soundOn ? APP_COLOR : APP_COLOR_TRANSP);
}

void State_Game::setMusic(bool musicOn)
{
	m_musicOn = musicOn;
	m_stateManager->getContext()->m_soundManager->setMusic(m_musicOn ? 100.f : 0.f);
	m_musicText.setFillColor(m_musicOn ? APP_COLOR : APP_COLOR_TRANSP);
}

void State_Game::onToggleSound()
{
	setSound(!m_soundOn);
}

void State_Game::onToggleMusic()
{
	setMusic(!m_musicOn);
}

void State_Game::gameOverScreen()
{
	m_newGame = true;
	m_stateManager->remove(StateType::Paused);
	m_stateManager->switchTo(StateType::GameOver);
	m_levelManager.purge();
}

/// <summary>
/// Purges all actors and resets the game state.
/// </summary>
void State_Game::newGame()
{
	m_newGame = false;
	m_stateManager->getContext()->m_actorManager->purge();
	m_levelManager.newGame();
	m_stateManager->getContext()->m_actorManager->enableActor(m_levelManager.getPlayerId());
	// set player icon to use in HUD
	const auto& playerSprite = m_stateManager->getContext()->m_actorManager->getActor(m_levelManager.getPlayerId())->getComponent<Comp_SpriteSheet>(ComponentType::SpriteSheet);
	m_playerIcon = *playerSprite->getSpriteSheet()->getSprite();
}

void State_Game::updateHUD()
{
	m_scoreText.setString("Score:\n" + std::to_string(m_levelManager.getScore()));
	m_levelText.setString("Level:\n" + std::to_string(m_levelManager.getLevel()));
	m_killsText.setString("Kills:\n" + std::to_string(m_levelManager.getKills()));
	m_fpsText.setString("FPS:\n" + std::to_string(m_fps));
}

void State_Game::drawGame()
{
	m_stateManager->getContext()->m_windowManager->getRenderWindow()->setView(m_gameView);
	m_stateManager->getContext()->m_windowManager->getRenderWindow()->draw(m_background);
	m_stateManager->getContext()->m_systemManager->draw(m_stateManager->getContext()->m_windowManager);
}

void State_Game::drawHUD()
{
	WindowManager* windowManager = m_stateManager->getContext()->m_windowManager;
	windowManager->getRenderWindow()->setView(m_view);
	windowManager->getRenderWindow()->draw(m_scoreText);
	windowManager->getRenderWindow()->draw(m_levelText);
	windowManager->getRenderWindow()->draw(m_killsText);
	windowManager->getRenderWindow()->draw(m_fpsText);
	windowManager->getRenderWindow()->draw(m_helpText);
	windowManager->getRenderWindow()->draw(m_soundText);
	windowManager->getRenderWindow()->draw(m_musicText);
	// draw player lives
	float iconWidth = m_playerIcon.getGlobalBounds().width;
	for (unsigned int i = 0; i < m_levelManager.getPlayerLives(); ++i)
	{
		m_playerIcon.setPosition(m_playerIconPosition.x + i * iconWidth - 3 * iconWidth, m_playerIconPosition.y);
		windowManager->getRenderWindow()->draw(m_playerIcon);
	}
	// draw help panel if enabled
	if (m_showHelp)
	{
		windowManager->getRenderWindow()->draw(m_helpPanel);
		windowManager->getRenderWindow()->draw(m_helpPanelTitle);
		windowManager->getRenderWindow()->draw(m_helpPanelText);
	}
}

void State_Game::initializeHUD()
{
	setWindowOutline();
	initializeHUDText(m_scoreText);
	m_scoreText.setPosition(m_hudPadding.x, 0);
	initializeHUDText(m_levelText);
	m_levelText.setPosition(m_hudPadding.x, m_hudPadding.y + m_fontSize);
	initializeHUDText(m_killsText);
	m_killsText.setPosition(m_hudPadding.x, 2 * (m_hudPadding.y + m_fontSize));
	initializeHUDText(m_fpsText);
	m_fpsText.setPosition(m_hudPadding.x, 3 * (m_hudPadding.y + m_fontSize));
	initializeHUDText(m_helpText);
	m_helpText.setPosition(m_hudPadding.x, m_levelManager.getViewSpace().height - m_fontSize);
	m_helpText.setString("HELP (H)");
	initializeHUDText(m_soundText);
	m_soundText.setString("SOUND (S)");
	m_soundText.setPosition(m_view.getSize().x - m_hudPadding.x - 200, m_levelManager.getViewSpace().height - 2 * m_fontSize);
	initializeHUDText(m_musicText);
	m_musicText.setString("MUSIC (M)");
	m_musicText.setPosition(m_view.getSize().x - m_hudPadding.x - 200, m_levelManager.getViewSpace().height - m_fontSize);
}

void State_Game::initializeHUDText(sf::Text& text)
{
	text.setFont(m_font);
	text.setCharacterSize(m_fontSize);
	text.setFillColor(APP_COLOR);
}

void State_Game::setWindowOutline()
{
	m_background.setSize(m_gameView.getSize() - sf::Vector2f(2 * m_outlineThickness, 2 * m_outlineThickness));
	m_background.setPosition(m_levelManager.getScreenCenter() - m_background.getSize() / 2.f);
	m_background.setFillColor(sf::Color::Transparent);
	m_background.setOutlineColor(APP_COLOR);
	m_background.setOutlineThickness(m_outlineThickness);
}

void State_Game::setupHelpPanel()
{
	m_helpPanel.setSize(m_helpPanelSize);
	m_helpPanel.setOrigin(m_helpPanel.getSize() / 2.f);
	m_helpPanel.setPosition(m_view.getCenter());
	m_helpPanel.setFillColor(BGD_COLOR);
	m_helpPanel.setOutlineColor(APP_COLOR);
	m_helpPanel.setOutlineThickness(m_outlineThickness);
	m_helpPanelTitle.setFont(m_font);
	m_helpPanelTitle.setCharacterSize(m_fontSize + 20);
	m_helpPanelTitle.setString("HELP");
	m_helpPanelTitle.setOrigin(m_helpPanelTitle.getLocalBounds().width / 2.f, m_helpPanelTitle.getLocalBounds().height / 2.f);
	m_helpPanelTitle.setFillColor(APP_COLOR);
	m_helpPanelTitle.setPosition(m_helpPanel.getPosition().x, m_helpPanel.getPosition().y - m_helpPanel.getSize().y / 2.f);
	m_helpPanelText.setFont(m_font);
	m_helpPanelText.setCharacterSize(m_fontSize);
	m_helpPanelText.setString("Move (Mouse)\nShoot (Mouse L)\nPause (P)\nHelp (H)\nMusic (M)\nSound (S)");
	m_helpPanelText.setFillColor(APP_COLOR);
	m_helpPanelText.setOrigin(m_helpPanelText.getLocalBounds().width / 2.f, m_helpPanelText.getLocalBounds().height / 2.f);
	m_helpPanelText.setPosition(m_helpPanel.getPosition().x, m_helpPanel.getPosition().y);
}

sf::FloatRect State_Game::getGameViewSpace()
{
	sf::Vector2f viewCenter = m_gameView.getCenter();
	sf::Vector2f viewSize = m_gameView.getSize();
	return sf::FloatRect(viewCenter - viewSize / 2.f, viewSize);
}
