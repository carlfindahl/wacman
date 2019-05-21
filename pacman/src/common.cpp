#include "common.h"
#include "config.h"
#include "encrypt/vignere_encryptor.h"

#include <sstream>
#include <fstream>

#include <cglutil.h>

namespace pac
{
robin_hood::unordered_map<std::string, std::vector<ScoreEntry>> load_high_score_entries_from_file(const char* filepath)
{
    /* Decrypt the level file */
    auto scorestring = cgl::read_entire_file(filepath);
    VignereEncryption decryptor(ENCRYPTION_STRING);
    decryptor.decrypt(scorestring);

    /* Out data */
    robin_hood::unordered_map<std::string, std::vector<ScoreEntry>> out{};

    /* Per level variables:
     * Store each new entry temporarily in e, then read data into line and parse relevant information
     */
    ScoreEntry e;
    int entry_count = 0;
    std::string level_name{};
    std::stringstream tmp_sstream(scorestring);
    while (tmp_sstream >> level_name >> entry_count)
    {
        tmp_sstream.ignore();
        out.emplace(level_name);

        /* Read each entry per level */
        for (int i = 0; i < entry_count; ++i)
        {
            tmp_sstream >> e.name;
            tmp_sstream.ignore();
            tmp_sstream >> e.score;
            tmp_sstream.ignore();
        }
    }

    return out;
}

void write_high_score_entries_to_file(const robin_hood::unordered_map<std::string, std::vector<ScoreEntry>>& entries,
                                      const char* filepath)
{
    /* First write to a string */
    std::stringstream tmpstring;
    for (const auto& [level, scores] : entries)
    {
        tmpstring << level << ' ' << scores.size() << '\n';

        for (const auto& entry : scores)
        {
            tmpstring << entry.name << '\n' << entry.score << '\n';
        }
    }

    /* Encrypt the string we just created */
    VignereEncryption encryptor(ENCRYPTION_STRING);
    std::string levelstring = tmpstring.str();
    encryptor.encrypt(levelstring);

    /* Write it to file */
    std::ofstream file(cgl::native_absolute_path(filepath));
    file.sync_with_stdio(false);
    file.write(levelstring.c_str(), levelstring.size());
}

int manhattan_distance(glm::ivec2 from, glm::ivec2 to) noexcept { return std::abs(from.x - to.x) + std::abs(from.y - to.y); }

}  // namespace pac
