#ifndef GAMEENGINE_HPP_INCLUDED
#define GAMEENGINE_HPP_INCLUDED

#include <SFML/Graphics.hpp>
#include <vector>
#include <string>
#include "Controls.hpp"
#include "Content.hpp"

class Display;

class GameState;

class GameEngine {
public:
    void Init(const char* title, bool fullscreen, bool maintainAspectRatio, sf::Vector2f scale, bool useShaders);
    void CleanUp();

    void ChangeState(GameState* state);
    void PushState(GameState* state);
    void PopState(GameState* state);

    void ProcessInput();
    void Update();
    void RenderFrame();
    void CheckFlags();

    bool Running() { return m_running; }
    void Quit() { m_running = false; }

    void Pause();

    void LevelLoadNext();
    void LevelLoadCurrent();

    // SFML Render Window
    sf::RenderWindow* window;

    // Time functions
    sf::Clock* gameClock;
    sf::Time deltaTime; // Use .asSeconds() to convert to float

    // ASCII display
    Display* display;

    // Controls
    Controls* controls;

    // Content
    Content* content;
private:
    std::vector<GameState*> states;

    bool m_running;

    bool levelLoadNext = false;
    bool levelLoadCurrent = false;
    };

#endif // GAMEENGINE_HPP_INCLUDED
