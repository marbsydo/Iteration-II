#include "SceneState.hpp"
#include "TitleState.hpp"
#include "../Display.hpp"
#include "../Tiles.hpp"
#include <iostream>
#include <string>
#include <sstream>
#include <fstream>

SceneState SceneState::m_SceneState;

void SceneState::Init(GameEngine* game) {
    text.clear();

    std::string filename = game->content->GetCurrentContentFilename();

    std::cout << "Reading file '" << filename << "'" << std::endl;

    std::string line;
    std::ifstream myfile(filename);
    if (myfile.is_open()) {
        while (getline(myfile, line)) {
            // Read lines one at a time
            text.push_back(line);
        }
    } else {
        std::cout << "WARNING: Cannot read '" << filename << "'" << std::endl;
    }

    clock.restart();
    timeSkip = 0.0f;
}

void SceneState::CleanUp(GameEngine* game) {}

void SceneState::Pause() {}
void SceneState::Resume() {}

void SceneState::ProcessInput(GameEngine* game) {
    sf::Event event;

    // Loop through all window events
    while (game->window->pollEvent(event)) {
        switch (event.type) {
        case sf::Event::Closed:
            game->Quit();
            break;
        case sf::Event::KeyPressed:
            if (event.key.code == sf::Keyboard::Escape)
                game->ChangeState(TitleState::Instance());
            break;
        default:
            break;
        }
    }
}
void SceneState::Update(GameEngine* game) {}
void SceneState::RenderFrame(GameEngine* game) {
    game->window->clear();
    game->display->SetAll(TILE_BLANK);

    float time = clock.getElapsedTime().asSeconds() + timeSkip;
    float time2 = (time * 2) - 1;

    for (int i = 0; i < (int)text.size(); i++) {
        if (time2 > i)
            game->display->WriteText(2, 2 + i, text[i].c_str());
    }

    bool keyEnter = game->controls->GetKeyDown(InputKey::Enter);
    if (time2 > (int)text.size()) {
        if ((int)(time / 0.25f) % 4 != 0)
            game->display->WriteText(2, game->display->GetHeight() - 2, "Press <Enter> to continue");

        if (keyEnter) {
            // Continue on to the next content item
            game->controls->ResetKeyDelay();
            game->content->Next();
            game->content->Load();
        }
    } else {
        if (keyEnter) {
            // Force all the cutscene text to appear instantly
            game->controls->ResetKeyDelay();
            timeSkip = 9999;
        }
    }

    game->display->Render(game);
    game->window->display();
}
