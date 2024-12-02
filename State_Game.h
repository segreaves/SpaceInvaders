#pragma once
#include "State.h"
#include "LevelManager.h"
#include "ActorManager.h"

class StateManager;
class LevelManager;

/// <summary>
/// Runs gameplay.
/// Receives player input and transmits to the handler systems.
/// Counts number of invaders and resets when it reaches zero.
/// Communicates with the LevelManager to distribute invaders in a grid and
/// query their spawn points when resetting positions.
/// </summary>
class State_Game : public State
{
public:
	State_Game(StateManager* stateManager);
private:
	void update(const float& deltaTime) override;
	void draw() override;
	void onCreate() override;
	void activate() override;
	void deactivate() override;

	void loadNextLevel();
	void onPlayerMove(sf::Vector2f xy);
	void onPlayerShoot();
	void onToggleHelp();
	void setSound(bool soundOn);
	void setMusic(bool musicOn);
	void onToggleSound();
	void onToggleMusic();
	void gameOverScreen();
	void newGame();

	void updateHUD();
	void drawGame();
	void drawHUD();
	void initializeHUD();
	void initializeHUDText(sf::Text& text);
	void setWindowOutline();
	void setupHelpPanel();
	sf::FloatRect getGameViewSpace();
	void showNewScore(const unsigned int score);

	LevelManager m_levelManager;
	sf::View m_gameView;
	sf::View m_view;
	bool m_newGame;
	unsigned int m_fps;
	sf::RectangleShape m_background;
	bool m_soundOn;
	bool m_musicOn;
	bool m_showingNewScore = false;
	const float m_showNewScoreDuration = 1.f;
	float m_showNewScoreTimer;
	// HUD elements
	const float m_outlineThickness = 3;
	float m_hudUpdateTimer;
	const float m_hudUpdateInterval = 0.2f;
	sf::Vector2f m_hudPadding = { 50.f, 50.f };
	// HUD text
	sf::Text m_scoreText;
	sf::Text m_newScoreText;
	sf::Text m_levelText;
	sf::Text m_killsText;
	sf::Text m_fpsText;
	sf::Text m_helpText;
	sf::Text m_soundText;
	sf::Text m_musicText;
	// player lives
	sf::Sprite m_playerIcon;
	sf::Vector2f m_playerIconPosition;
	// help panel
	bool m_showHelp = false;
	sf::RectangleShape m_helpPanel;
	sf::Vector2f m_helpPanelSize = { 400, 550 };
	sf::Text m_helpPanelTitle, m_helpPanelText;
};
