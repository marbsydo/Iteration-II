#ifndef PLAYSTATE_HPP_INCLUDED
#define PLAYSTATE_HPP_INCLUDED

#include "Chronact.hpp"

class PlayState : public GameState {
public:
    void Init();
    void CleanUp();

    void Pause();
    void Resume();

    void ProcessInput(GameEngine* game);
    void Udpate(GameEngine* game);
    void RenderFrame(GameEngine* game);

    static PlayState* Instance() {
        return &m_PlayState;
    }

protected:
    PlayState() {}

private:
    static PlayState m_PlayState;
    };

#endif // PLAYSTATE_HPP_INCLUDED