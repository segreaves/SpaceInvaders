#include "State_Game.h"
#include "StateManager.h"
#include "Comp_Position.h"
#include "Comp_Movement.h"
#include "Comp_Sprite.h"
#include "Comp_Invader.h"
#include "LevelManager.h"
#include "Utils.h"

State_Game::State_Game(StateManager* stateManager) :
	State(stateManager),
	m_playerBulletIndex(0),
	m_remainingInvaders(0),
	m_kills(0),
	m_currentInvaderSpeed(0)
{
	m_gameView.setViewport(sf::FloatRect(0.15f, 0, 0.7f, 1));
	m_hudView.setViewport(sf::FloatRect(0, 0, 1, 1));

	m_levelManager.setActorManager(stateManager->getContext()->m_actorManager);
	setHUDStyle();
	setWindowOutline();
}

void State_Game::update(const float& deltaTime)
{
	m_fps = 1.0f / deltaTime;
	m_stateManager->getContext()->m_systemManager->update(deltaTime);
	updateHUD();
}

void State_Game::draw()
{
	drawGame();
	drawHUD();
}

void State_Game::onCreate()
{
	m_levelManager.setViewSpace(getGameViewSpace());
	m_stateManager->getContext()->m_systemManager->getSystem<Sys_InvaderControl>(SystemType::InvaderControl)->setLevelManager(&m_levelManager);
	m_stateManager->getContext()->m_systemManager->getSystem<Sys_PlayerControl>(SystemType::PlayerControl)->setLevelManager(&m_levelManager);
	m_stateManager->getContext()->m_systemManager->getSystem<Sys_BulletControl>(SystemType::BulletControl)->setLevelManager(&m_levelManager);
	m_levelManager.createPlayer();
	m_levelManager.createPlayerBullets();
	m_levelManager.createInvaderBullets();
	m_levelManager.createInvaders(getGameViewSpace());
	m_levelManager.createShockwaves(m_levelManager.getInvaderIds().size());
	m_levelManager.createBunkers(getGameViewSpace());
	ActorManager* actorManager = m_stateManager->getContext()->m_actorManager;
	actorManager->enableActor(m_levelManager.getPlayerId());
	for (auto& bunkerId : m_levelManager.getBunkerIds())
	{
		actorManager->enableActor(bunkerId);
		Comp_Position* posComp = actorManager->getActor(bunkerId)->getComponent<Comp_Position>(ComponentType::Position);
		posComp->setPosition(m_levelManager.getBunkerSpawn(bunkerId));
	}
	loadNextLevel();
}

void State_Game::onDestroy()
{
}

void State_Game::activate()
{
	m_stateManager->getContext()->m_controller->m_onMove.addCallback("Game_onMove", std::bind(&State_Game::onPlayerMove, this, std::placeholders::_1));
	m_stateManager->getContext()->m_controller->m_onShoot.addCallback("Game_onShoot", std::bind(&State_Game::onPlayerShoot, this));
	m_stateManager->getContext()->m_systemManager->getSystem<Sys_InvaderControl>(SystemType::InvaderControl)->m_invaderDefeated.addCallback("Game_onInvaderDefeated", std::bind(&State_Game::onInvaderDefeated, this, std::placeholders::_1));
	m_stateManager->getContext()->m_systemManager->getSystem<Sys_InvaderControl>(SystemType::InvaderControl)->m_invaderShoot.addCallback("Game_onInvaderShoot", std::bind(&State_Game::onInvaderShoot, this, std::placeholders::_1));
}

void State_Game::deactivate()
{
	m_stateManager->getContext()->m_controller->m_onMove.removeCallback("Game_onMove");
	m_stateManager->getContext()->m_controller->m_onMove.removeCallback("Game_onShoot");
	m_stateManager->getContext()->m_systemManager->getSystem<Sys_InvaderControl>(SystemType::InvaderControl)->m_invaderDefeated.removeCallback("Game_onInvaderDefeated");
	m_stateManager->getContext()->m_systemManager->getSystem<Sys_InvaderControl>(SystemType::InvaderControl)->m_invaderShoot.removeCallback("Game_onInvaderShoot");
}

void State_Game::loadNextLevel()
{
	// increase level
	m_levelManager++;
	// reset invader count
	m_remainingInvaders = m_levelManager.getInvaderIds().size();
	// enable (or re-enable) all actors
	for (auto& invaderId : m_levelManager.getInvaderIds())
		m_stateManager->getContext()->m_actorManager->enableActor(invaderId);
	// start (or re-start) all systems
	m_stateManager->getContext()->m_systemManager->start();
}

void State_Game::onPlayerMove(sf::Vector2f xy)
{
	Comp_Target* targetComp = m_stateManager->getContext()->m_actorManager->getActor(m_levelManager.getPlayerId())->getComponent<Comp_Target>(ComponentType::Target);
	targetComp->setTarget(targetComp->getTarget() + xy);
}

void State_Game::onPlayerShoot()
{
	const int bulletId = m_levelManager.getPlayerBulletIds()[m_playerBulletIndex];
	m_stateManager->getContext()->m_actorManager->enableActor(bulletId);
	onActorShoot(m_levelManager.getPlayerId(), bulletId, sf::Vector2f(0, -1), 1000000);
	m_playerBulletIndex = ++m_playerBulletIndex % m_levelManager.getPlayerBulletIds().size();
}

void State_Game::onInvaderShoot(int invaderId)
{
	const int bulletId = m_levelManager.getInvaderBulletIds()[m_invaderBulletIndex];
	m_stateManager->getContext()->m_actorManager->enableActor(bulletId);
	onActorShoot(invaderId, bulletId, sf::Vector2f(0, 1), 25000);
	m_invaderBulletIndex = ++m_invaderBulletIndex % m_levelManager.getInvaderBulletIds().size();
}

void State_Game::onActorShoot(const ActorId& shooterId, const ActorId& bulletId, const sf::Vector2f direction, const float& knockbackForce)
{
	ActorManager* actorManager = m_stateManager->getContext()->m_actorManager;
	Actor* bullet = actorManager->getActor(bulletId);
	Actor* shooter = m_stateManager->getContext()->m_actorManager->getActor(shooterId);
	Comp_Position* shooterPos = shooter->getComponent<Comp_Position>(ComponentType::Position);
	Comp_Collision* shooterCol = shooter->getComponent<Comp_Collision>(ComponentType::Collision);

	Comp_Position* bulletPos = bullet->getComponent<Comp_Position>(ComponentType::Position);
	Comp_Collision* bulletCol = bullet->getComponent<Comp_Collision>(ComponentType::Collision);
	bulletPos->setPosition(shooterPos->getPosition() +
		direction * (bulletCol->getAABB().getSize().y / 2 + shooterCol->getAABB().getSize().y / 2.f));
	Comp_Movement* bulletMove = bullet->getComponent<Comp_Movement>(ComponentType::Movement);
	Comp_Bullet* bulletComp = bullet->getComponent<Comp_Bullet>(ComponentType::Bullet);
	bulletMove->setVelocity(direction * bulletComp->getbulletSpeed());
	// knock-back
	Comp_Movement* moveComp = m_stateManager->getContext()->m_actorManager->getActor(shooterId)->getComponent<Comp_Movement>(ComponentType::Movement);
	moveComp->accelerate(sf::Vector2f(0, -knockbackForce * direction.y));
}

void State_Game::instantiateShockwave(sf::Vector2f position)
{
	const int shockwaveId = m_levelManager.getShockwaveIds()[m_shockwaveIndex++ % m_levelManager.getShockwaveIds().size()];
	Comp_Position* shockwavePos = m_stateManager->getContext()->m_actorManager->getActor(shockwaveId)->getComponent<Comp_Position>(ComponentType::Position);
	Comp_Collision* shockwaveCol = m_stateManager->getContext()->m_actorManager->getActor(shockwaveId)->getComponent<Comp_Collision>(ComponentType::Collision);
	shockwavePos->setPosition(position);
	Comp_Shockwave* shockwaveComp = m_stateManager->getContext()->m_actorManager->getActor(shockwaveId)->getComponent<Comp_Shockwave>(ComponentType::Shockwave);
	shockwaveComp->setRadius(0);
	shockwaveComp->resetTime();
	m_stateManager->getContext()->m_actorManager->enableActor(shockwaveId);
}

void State_Game::onInvaderDefeated(const int& invaderId)
{
	m_kills++;
	instantiateShockwave(m_stateManager->getContext()->m_actorManager->getActor(invaderId)->getComponent<Comp_Position>(ComponentType::Position)->getPosition());
	if (--m_remainingInvaders == 0)
		loadNextLevel();
}

void State_Game::updateHUD()
{
	m_scoreText.setString("Score: " + std::to_string(0));
	m_levelText.setString("Level: " + std::to_string(m_levelManager.getLevel()));
	m_livesText.setString("Lives: " + std::to_string(3));
	m_killsText.setString("Kills: " + std::to_string(m_kills));
	m_fpsText.setString("FPS: " + std::to_string(m_fps));
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
	windowManager->getRenderWindow()->setView(m_hudView);
	windowManager->getRenderWindow()->draw(m_scoreText);
	windowManager->getRenderWindow()->draw(m_levelText);
	windowManager->getRenderWindow()->draw(m_livesText);
	windowManager->getRenderWindow()->draw(m_killsText);
	windowManager->getRenderWindow()->draw(m_fpsText);
}

void State_Game::setHUDStyle()
{
	m_font.loadFromFile(Utils::getWorkingDirectory() + "assets/fonts/game_over.ttf");
	m_scoreText.setFont(m_font);
	m_scoreText.setCharacterSize(m_fontSize);
	m_scoreText.setPosition(m_hudPadding, m_hudPadding);
	m_scoreText.setFillColor(APP_COLOR);
	m_levelText.setFont(m_font);
	m_levelText.setCharacterSize(m_fontSize);
	m_levelText.setPosition(m_hudPadding, m_hudPadding + m_fontSize);
	m_levelText.setFillColor(APP_COLOR);
	m_livesText.setFont(m_font);
	m_livesText.setCharacterSize(m_fontSize);
	m_livesText.setPosition(m_hudPadding, m_hudPadding + 2 * m_fontSize);
	m_livesText.setFillColor(APP_COLOR);
	m_killsText.setFont(m_font);
	m_killsText.setCharacterSize(m_fontSize);
	m_killsText.setPosition(m_hudPadding, m_hudPadding + 3 * m_fontSize);
	m_killsText.setFillColor(APP_COLOR);
	m_fpsText.setFont(m_font);
	m_fpsText.setCharacterSize(m_fontSize);
	m_fpsText.setPosition(m_hudPadding, m_hudPadding + 4 * m_fontSize);
	m_fpsText.setFillColor(APP_COLOR);
}

void State_Game::setWindowOutline()
{
	int outlineThickness = 2;
	m_background.setSize(m_gameView.getSize() - sf::Vector2f(2 * outlineThickness, 2 * outlineThickness));
	m_background.setPosition(outlineThickness, outlineThickness);
	m_background.setFillColor(sf::Color::Black);
	m_background.setOutlineColor(APP_COLOR);
	m_background.setOutlineThickness(outlineThickness);
}

sf::FloatRect State_Game::getGameViewSpace()
{
	sf::Vector2f viewCenter = m_gameView.getCenter();
	sf::Vector2f viewSize = m_gameView.getSize();
	return sf::FloatRect(viewCenter - viewSize / 2.f, viewSize);
}

sf::FloatRect State_Game::getHUDViewSpace()
{
	sf::Vector2f viewCenter = m_hudView.getCenter();
	sf::Vector2f viewSize = m_hudView.getSize();
	return sf::FloatRect(viewCenter - viewSize / 2.f, viewSize);
}
