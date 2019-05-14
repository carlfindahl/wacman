#include "editor_state.h"

#include <entt/signal/dispatcher.hpp>

namespace pac
{
/* The game event queue */
extern entt::dispatcher g_event_queue;

void EditorState::on_enter() { g_event_queue.sink<EvInput>().connect<&EditorState::recieve>(this); }

void EditorState::on_exit() { g_event_queue.sink<EvInput>().disconnect<&EditorState::recieve>(this); }

bool EditorState::update(float dt) { return false; }

bool EditorState::draw() { return false; }

void EditorState::recieve(const EvInput& input) {}

}  // namespace pac
