#pragma once

#include <vector>
#include <string>
#include <cstdint>

#include <glm/vec2.hpp>

namespace pac
{
class StateManager;

namespace detail
{
/*!
 * \brief The custom_ivec2_hash struct is a quick hash that puts x in upper 32 bits and y in lower. Used for mapping dirs to
 * textures
 */
struct custom_ivec2_hash
{
    std::size_t operator()(const glm::ivec2& vec) const noexcept
    {
        const uint64_t hash = (static_cast<uint64_t>(vec.x) << 32u) | vec.y;
        return hash;
    }
};
}  // namespace detail

/*!
 * \brief The Context struct hold relevant context information about the Game that various sub systems might care about.
 */
struct GameContext
{
    StateManager* state_manager = nullptr;
};

/*!
 * \brief The Entry struct represents a single entry in the high score table
 */
struct ScoreEntry
{
    std::string name = {};
    int score = 0;
};

/*!
 * \brief load_entries_from_file loads high score entries from highscores.txt in order to display them in the menu
 * \return a vector of entries
 */
std::vector<ScoreEntry> load_high_score_entries_from_file(const char* filepath = "res/highscores.txt");

/*!
 * \brief write_high_score_entries_to_file writes all high scores in the vector to the given file
 * \param entries are the entries to write
 * \param filepath is the file to write to
 */
void write_high_score_entries_to_file(const std::vector<ScoreEntry>& entries, const char* filepath = "res/highscores.txt");
}  // namespace pac
