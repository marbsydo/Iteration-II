#include "Events.hpp"
#include <iostream>
#include <sstream>

namespace Event {

    /// Test
    Test::Test(int time) : Base(time) {
        debugName = "Test";
    }

    Result Test::ForwardEvent() {
        Result result;
        result.success = true;
        result.msg = "";

        std::cout << "This ForwardEvent should occur just after time " << time << std::endl;

        return result;
    }

    Result Test::BackwardEvent() {
        Result result;
        result.success = true;
        result.msg = "";

        std::cout << "This BackwardEvent should occur just before time " << time << std::endl;

        return result;
    }

    /// PlayerMove
    PlayerMove::PlayerMove(int time, GameObject::Player* player, int xFrom, int yFrom, int xTo, int yTo) : Base(time) {
        debugName = "PlayerMove";
        this->player = player;
        this->xFrom = xFrom;
        this->yFrom = yFrom;
        this->xTo = xTo;
        this->yTo = yTo;
    }

    Result PlayerMove::ForwardEvent() {
        Result result;
        result.success = true;
        result.msg = "";

        if (player->x == xFrom && player->y == yFrom) {
            //TODO: Also check that (xTo,yTo) are free!
            player->x = xTo;
            player->y = yTo;
        } else {
            result.success = false;
            std::ostringstream msg;
            msg << "Player (" << player->cloneDesignation << ") was not at (xFrom:" << xFrom << ",yFrom:" << yFrom << ")";
            result.msg = msg.str();
        }

        return result;
    }

    Result PlayerMove::BackwardEvent() {
        Result result;
        result.success = true;
        result.msg = "";

        if (player->x == xTo && player->y == yTo) {
            //TODO: Also check that (xFrom,yFrom) are free!
            player->x = xFrom;
            player->y = yFrom;
        } else {
            result.success = false;
            std::ostringstream msg;
            msg << "Player (" << player->cloneDesignation << ") was not at (xTo:" << xTo << ",yTo:" << yTo << ")";
            result.msg = msg.str();
        }

        return result;
    }

    /// PlayerExpire
    PlayerExpire::PlayerExpire(int time, GameObject::Player* player, int xFrom, int yFrom) : PlayerMove(time, player, xFrom, yFrom, -1, -1) {
        debugName = "PlayerExpire";
    }

    ///PlayerDie_Linkable
    PlayerDie_Linkable::PlayerDie_Linkable(int time, GameObject::Player* player, int xPlayer, int yPlayer, GameObject::StaticLinkable* killer, int xKiller, int yKiller, bool stateKiller) : Base(time) {
        debugName = "PlayerDie_Linkable";
        this->player = player;
        this->xPlayer = xPlayer;
        this->yPlayer = yPlayer;
        this->killer = killer;
        this->xKiller = xKiller;
        this->yKiller = yKiller;
        this->stateKiller = stateKiller;
    }

    Result PlayerDie_Linkable::ForwardEvent() {
        Result result;
        result.success = false;
        result.msg = "";
        std::ostringstream msg;

        if (player->x == xPlayer && player->y == yPlayer) {
            if (killer->x == xKiller && killer->y == yKiller) {
                if (killer->state == stateKiller) {
                    if (player->dead == false) {
                        player->dead = true;
                        result.success = true;
                    } else {
                        msg << "Player (" << player->cloneDesignation << ") was already dead";
                    }
                } else {
                    msg << "Killer was not in expected state (stateKiller:" << stateKiller << ")";
                }
            } else {
                msg << "Killer was not at (xKiller:" << xKiller << ",yKiller:" << yKiller << ")";
            }
        } else {
            msg << "Player (" << player->cloneDesignation << ") was not at (xPlayer:" << xPlayer << ",yPlayer:" << yPlayer << ")";
        }

        result.msg = msg.str();
        return result;
    }

    Result PlayerDie_Linkable::BackwardEvent() {
        Result result;
        result.success = false;
        result.msg = "";
        std::ostringstream msg;

        if (player->x == xPlayer && player->y == yPlayer) {
            if (killer->x == xKiller && killer->y == yKiller) {
                if (killer->state == stateKiller) {
                    if (player->dead == true) {
                        player->dead = false;
                        result.success = true;
                    } else {
                        msg << "Player (" << player->cloneDesignation << ") was already alive";
                    }
                } else {
                    msg << "Killer was not in expected state (stateKiller:" << stateKiller << ")";
                }
            } else {
                msg << "Killer was not at (xKiller:" << xKiller << ",yKiller:" << yKiller << ")";
            }
        } else {
            msg << "Player (" << player->cloneDesignation << ") was not at (xPlayer:" << xPlayer << ",yPlayer:" << yPlayer << ")";
        }

        result.msg = msg.str();
        return result;
    }

    /// LinkableStateChange
    LinkableStateChange::LinkableStateChange(int time, GameObject::StaticLinkable* linkable, GameObject::Player* player, int xPlayer, int yPlayer, bool stateFrom) : Base(time) {
        debugName = "LinkableStateChange";
        this->linkable = linkable;
        this->player = player;
        this->xPlayer = xPlayer;
        this->yPlayer = yPlayer;
        this->stateFrom = stateFrom;
        this->stateTo = !stateFrom;
    }

    Result LinkableStateChange::ForwardEvent() {
        Result result;
        result.success = false;
        result.msg = "";
        std::ostringstream msg;

        if (player->x == xPlayer && player->y == yPlayer) {
            if (linkable->state == stateFrom) {
                linkable->state = stateTo;
                result.success = true;
            } else {
                msg << "StaticLinkable was not in the expected state (stateFrom:" << stateFrom << ")";
            }
        } else {
            msg << "Player (" << player->cloneDesignation << ") was not at (xPlayer:" << xPlayer << ",yPlayer:" << yPlayer << ")";
        }

        result.msg = msg.str();
        return result;
    }

    Result LinkableStateChange::BackwardEvent() {
        Result result;
        result.success = false;
        result.msg = "";
        std::ostringstream msg;

        if (player->x == xPlayer && player->y == yPlayer) {
            if (linkable->state == stateTo) {
                linkable->state = stateFrom;
                result.success = true;
            } else {
                msg << "StaticLinkable was not in the expected state (stateTo:" << stateTo << ")";
            }
        } else {
            msg << "Player (" << player->cloneDesignation << ") was not at (xPlayer:" << xPlayer << ",yPlayer:" << yPlayer << ")";
        }

        result.msg = msg.str();
        return result;
    }
}
