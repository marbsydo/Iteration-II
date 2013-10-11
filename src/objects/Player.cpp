#include "Player.hpp"
#include "Tags.hpp"
#include "../IterationData.hpp"
#include "../Defs.hpp"
#include <iostream> // Temporary

namespace GameObject {
    Player::Player(int x, int y, GameEngine* game, LevelManager* levelManager) : Dynamic(x, y, game, levelManager) {
        UpdateDisplayCharacter();

        TimeDataWrite();
    }

    Player::~Player() {}

    void Player::Update() {
        UpdateDisplayCharacter();
        if (Controlling()) {
            if (game->controls->GetKeyDelaySufficient()) {
                bool moved = false;

                // Only try to move if it is possible to go forward in time!
                if (levelManager->iterationData->CanGoForward()) {
                    bool keyL = game->controls->GetKey(InputKey::Left);
                    bool keyR = game->controls->GetKey(InputKey::Right);
                    bool keyU = game->controls->GetKey(InputKey::Up);
                    bool keyD = game->controls->GetKey(InputKey::Down);

                    int keyMask = (int)keyL | ((int)keyR << 1) | ((int) keyU << 2) | ((int) keyD << 3);


                    switch (keyMask) {
                        case 0b0001:    moved = SetPosRelative(-1, 0);  break;
                        case 0b0010:    moved = SetPosRelative(+1, 0);  break;
                        case 0b0100:    moved = SetPosRelative(0, -1);  break;
                        case 0b1000:    moved = SetPosRelative(0, +1);  break;
                    }
                }

                // `moved` will never be true if it was not possible to go forward in time
                if (moved) {
                    // Moved, so go forward in time and save our position
                    game->controls->ResetKeyDelay();
                    levelManager->iterationData->GoForward();
                    TimeDataWrite();
                } else {
                    bool keyAction1 = game->controls->GetKey(InputKey::Action1);
                    bool keyAction2 = game->controls->GetKey(InputKey::Action2);
                    if (keyAction1) {
                        // Perform action with objects we are on
                        int index = GetObjectIndexAtPosWithTag(x, y, TAG_TIMEMACHINE);
                        if (index >= 0) {
                            // Pass control to time machine
                            hasControl = false;
                            levelManager->levelData->SetObjectHasControl(index, true);
                            game->controls->ResetKeyDelay();
                        }
                    } else if (keyAction2 && levelManager->iterationData->CanGoForward()) {
                        // Move forward in time without moving
                        game->controls->ResetKeyDelay();
                        levelManager->iterationData->GoForward();
                        TimeDataWrite();
                    }
                }
            }
        }
    }

    void Player::UpdateTimeChanged() {
        if (!Controlling()) {
            if (TimeDataExists()) {
                TimeData currentTimeData = TimeDataRead();
                x = currentTimeData.x;
                y = currentTimeData.y;
            } else {
                // If we do not exist, simply move ourselves out of the display so we are not rendered
                x = -1;
                y = -1;
            }
        }
    }

    void Player::UpdateDisplayCharacter() {
        if (Controlling())
            displayCharacter = TILE_PLAYER_CONTROLLING;
        else
            displayCharacter = TILE_PLAYER_NOTCONTROLLING;
    }
}