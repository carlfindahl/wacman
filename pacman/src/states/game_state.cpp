#include "game_state.h"

namespace pac
{
GameState::GameState() {}

void GameState::on_enter() {}

void GameState::on_exit() {}

bool GameState::update(float dt) { return false; }

bool GameState::draw() { return false; }
}  // namespace pac
