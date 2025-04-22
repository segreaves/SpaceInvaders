#include "State_Game.h"
#include "StateManager.h"
#include "Comp_Position.h"
#include "Comp_Movement.h"
#include "Comp_Invader.h"
#include "LevelManager.h"
#include "Trigger.h"
#include "Utils.h"

State_Game::State_Game(StateManager* stateManager) :
	State(stateManager),
	m_levelManager(stateManager->getContext()->m_actorManager),
	m_newGame(true),
	m_fps(0),
	m_soundOn(true),
	m_musicOn(true),
	m_showNewScoreTimer(0),
	m_showScoreAlpha(0),
	m_hudUpdateTimer(0),
	m_scoreText(m_font),
	m_newScoreText(m_font),
	m_levelText(m_font),
	m_killsText(m_font),
	m_fpsText(m_font),
	m_helpText(m_font),
	m_soundText(m_font),
	m_musicText(m_font),
	m_playerIcon(nullptr),
	m_helpPanelTitle(m_font), 
	m_helpPanelText(m_font)
{
}

void State_Game::update(const float& deltaTime)
{
	if (m_levelManager.getInvaderCount() <= 0)
		loadNextLevel();
	m_fps = static_cast<unsigned int>(1.f / deltaTime);
	// handle player movement input
	handlePlayerPosition();
	// game update
	m_stateManager->getContext()->m_systemManager->update(deltaTime);
	// HUD update
	updateHUD(deltaTime);
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
	// draw game
	auto* window = m_stateManager->getContext()->m_windowManager->getRenderWindow();
	window->setView(m_gameView);
	window->draw(m_background);
	m_stateManager->getContext()->m_systemManager->draw(m_stateManager->getContext()->m_windowManager);
	// draw HUD
	window->setView(m_view);
	window->draw(m_scoreText);
	window->draw(m_levelText);
	window->draw(m_killsText);
#ifdef DEBUG
	window->draw(m_fpsText);
#endif
	window->draw(m_helpText);
	window->draw(m_soundText);
	window->draw(m_musicText);
	// show new score if needed
	if (m_showingNewScore)
		window->draw(m_newScoreText);
	// draw player lives
	float iconWidth = m_playerIcon->getGlobalBounds().size.x;
	for ( int i = 0; i < m_levelManager.getPlayerLives(); ++i)
	{
		m_playerIcon->setPosition({m_playerIconPosition.x + i * iconWidth - 3 * iconWidth, m_playerIconPosition.y});
		window->draw(*m_playerIcon);
	}
	// draw help panel if enabled
	if (m_showHelp)
	{
		window->draw(m_helpPanel);
		window->draw(m_helpPanelTitle);
		window->draw(m_helpPanelText);
	}
}

void State_Game::onCreate()
{
	sf::Vector2u windowSize = m_stateManager->getContext()->m_windowManager->getRenderWindow()->getSize();
	m_view.setSize({static_cast<float>(windowSize.x), static_cast<float>(windowSize.y)});
	m_view.setCenter({static_cast<float>(windowSize.x) / 2, static_cast<float>(windowSize.y) / 2});
	m_view.setViewport(sf::FloatRect({0, 0}, {1, 1}));
	m_gameView.setViewport(sf::FloatRect({0.15f, 0}, {0.7f, 1}));

	m_stateManager->getContext()->m_systemManager->setLevelManager(&m_levelManager);
	m_levelManager.setViewSpace(getGameViewSpace());
	initializeHUD();
	setupHelpPanel();
	// load music
	m_stateManager->getContext()->m_soundManager->loadSoundProfile("profiles/soundProfiles/game_state.sound");
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
	// add callbacks
	m_stateManager->getContext()->m_controller->m_onPause.addCallback("Game_onPause", std::bind(&StateManager::switchTo, m_stateManager, StateType::Paused));
	m_stateManager->getContext()->m_controller->m_onShoot.addCallback("Game_onShoot", std::bind(&State_Game::onPlayerShoot, this));
	m_stateManager->getContext()->m_controller->m_onToggleHelp.addCallback("Game_onToggleHelp", std::bind(&State_Game::onToggleHelp, this));
	m_stateManager->getContext()->m_controller->m_onToggleSound.addCallback("Game_onToggleSound", std::bind(&State_Game::onToggleSound, this));
	m_stateManager->getContext()->m_controller->m_onToggleMusic.addCallback("Game_onToggleMusic", std::bind(&State_Game::onToggleMusic, this));
	m_levelManager.m_updateScore.addCallback("Game_onUpdateScore", std::bind(&State_Game::showNewScore, this, std::placeholders::_1));
	// start new game if one is required
	if (m_newGame)
		newGame();
}

void State_Game::deactivate()
{
	m_stateManager->getContext()->m_controller->m_onPause.removeCallback("Game_onPause");
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

void State_Game::handlePlayerPosition()
{
	// get render window
	const auto& window = *m_stateManager->getContext()->m_windowManager->getRenderWindow();
	// get mouse position relative to window
	sf::Vector2i mousePos = sf::Mouse::getPosition(window);
	// clamp mouse position to view space
	if (mousePos.x < 0) mousePos.x = 0;
	else if (mousePos.x > static_cast<int>(window.getSize().x)) 
		mousePos.x = window.getSize().x;
	sf::Mouse::setPosition(mousePos, window);
	// calculate mouse position relative to window
	const auto& mousePosInWindow = mousePos.x / static_cast<float>(window.getSize().x);
	// calculate player position relative to view space
	const auto& playerPosInView = m_levelManager.getViewSpace().position.x + m_controlMargin + (m_levelManager.getViewSpace().size.x - 2 * m_controlMargin) * mousePosInWindow;
	// set player target to its position
	unsigned int playerId = m_levelManager.getPlayerId();
	// get player target component
	const auto& actor = m_stateManager->getContext()->m_actorManager->getActor(playerId);
	const auto& targetComp = actor->getComponent<Comp_Target>(ComponentType::Target);
	// set player target position
	targetComp->setTarget(sf::Vector2f(playerPosInView, m_levelManager.getPlayerSpawnPoint().y));
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
	m_stateManager->getContext()->m_soundManager->setMusic(m_musicOn);
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
	m_controlMargin = m_stateManager->getContext()->m_actorManager->getActor(m_levelManager.getPlayerId())->getComponent<Comp_Collision>(ComponentType::Collision)->getAABB().size.x / 2.f;
	m_stateManager->getContext()->m_actorManager->enableActor(m_levelManager.getPlayerId());
	// set player icon to use in HUD
	const auto& playerSprite = m_stateManager->getContext()->m_actorManager->getActor(m_levelManager.getPlayerId())->getComponent<Comp_SpriteSheet>(ComponentType::SpriteSheet);
	m_playerIcon = new sf::Sprite(*playerSprite->getSpriteSheet()->getSprite());
}

void State_Game::updateHUD(const float& deltaTime)
{
	m_hudUpdateTimer -= deltaTime;
	// if enough time has passed, update the score
	if (m_hudUpdateTimer < 0)
	{
		m_scoreText.setString("Score:\n" + std::to_string(m_levelManager.getScore()));
		m_levelText.setString("Level:\n" + std::to_string(m_levelManager.getLevel()));
		m_killsText.setString("Kills:\n" + std::to_string(m_levelManager.getKills()));
		m_fpsText.setString("FPS:\n" + std::to_string(m_fps));
		m_hudUpdateTimer = m_hudUpdateInterval;
	}
	// if new score needs to be shown
	if (m_showingNewScore)
	{
		m_showNewScoreTimer += deltaTime;
		if (m_showNewScoreTimer < m_showNewScoreDuration)
		{
			const auto pos = sf::Vector2f(m_hudPadding.x + m_fontSize, m_hudPadding.x);
			const float ratio = m_showNewScoreTimer / m_showNewScoreDuration;
			m_showScoreAlpha = ratio < 0.5f ? 1.f : 1.f - ratio;
			sf::Color color = APP_COLOR;
			color.a = m_showScoreAlpha * 255;
			m_newScoreText.setFillColor(color);
			const auto new_pos = pos + sf::Vector2f(0, m_newScoreOffset * ratio);
			m_newScoreText.setPosition(new_pos);
		}
		else
			m_showingNewScore = false;
	}
}

void State_Game::initializeHUD()
{
	setWindowOutline();
	initializeHUDText(m_scoreText);
	m_scoreText.setPosition({m_hudPadding.x, 0});
	initializeHUDText(m_newScoreText);
	initializeHUDText(m_levelText);
	m_levelText.setPosition({m_hudPadding.x, m_hudPadding.y + m_fontSize});
	initializeHUDText(m_killsText);
	m_killsText.setPosition({m_hudPadding.x, 2 * (m_hudPadding.y + m_fontSize)});
	initializeHUDText(m_fpsText);
	m_fpsText.setPosition({m_hudPadding.x, 3 * (m_hudPadding.y + m_fontSize)});
	initializeHUDText(m_helpText);
	m_helpText.setPosition({m_hudPadding.x, m_levelManager.getViewSpace().size.y - m_fontSize});
	m_helpText.setString("HELP (H)");
	initializeHUDText(m_soundText);
	m_soundText.setString("SOUND (S)");
	m_soundText.setPosition({m_view.getSize().x - m_hudPadding.x - 200, m_levelManager.getViewSpace().size.y - 2 * m_fontSize});
	initializeHUDText(m_musicText);
	m_musicText.setString("MUSIC (M)");
	m_musicText.setPosition({m_view.getSize().x - m_hudPadding.x - 200, m_levelManager.getViewSpace().size.y - m_fontSize});
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
	m_helpPanelTitle.setOrigin({m_helpPanelTitle.getLocalBounds().size.x / 2.f, m_helpPanelTitle.getLocalBounds().size.y / 2.f});
	m_helpPanelTitle.setFillColor(APP_COLOR);
	m_helpPanelTitle.setPosition({m_helpPanel.getPosition().x, m_helpPanel.getPosition().y - m_helpPanel.getSize().y / 2.f});
	m_helpPanelText.setFont(m_font);
	m_helpPanelText.setCharacterSize(m_fontSize);
	m_helpPanelText.setString("Move (Mouse)\nShoot (Mouse L)\nPause (P)\nHelp (H)\nMusic (M)\nSound (S)");
	m_helpPanelText.setFillColor(APP_COLOR);
	m_helpPanelText.setOrigin({m_helpPanelText.getLocalBounds().size.x / 2.f, m_helpPanelText.getLocalBounds().size.y / 2.f});
	m_helpPanelText.setPosition({m_helpPanel.getPosition().x, m_helpPanel.getPosition().y});
}

sf::FloatRect State_Game::getGameViewSpace()
{
	sf::Vector2f viewCenter = m_gameView.getCenter();
	sf::Vector2f viewSize = m_gameView.getSize();
	return sf::FloatRect(viewCenter - viewSize / 2.f, viewSize);
}

void State_Game::showNewScore(const unsigned int score)
{
	m_showingNewScore = true;
	m_showNewScoreTimer = 0.f;
	m_newScoreText.setString("+" + std::to_string(score));
}
