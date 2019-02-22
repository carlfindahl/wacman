#include "common.h"
#include "config.h"
#include "encrypt/vignere_encryptor.h"

#include <sstream>
#include <fstream>

#include <cglutil.h>

namespace pac
{
std::vector<ScoreEntry> load_high_score_entries_from_file(const char* filepath)
{
    /* Decrypt the level file */
    auto scorestring = cgl::read_entire_file(filepath);
    VignereEncryption decryptor(ENCRYPTION_STRING);
    decryptor.decrypt(scorestring);

    /* Store each new entry temporarily in e, then based on the line count, read data into line and parse relevant information */
    ScoreEntry e;
    int line_count = 0;
    std::string line{};
    std::vector<ScoreEntry> out{};
    std::stringstream tmp_sstream(scorestring);
    while (std::getline(tmp_sstream, line))
    {
        switch (line_count % 2)
        {
        case 0: e.name = line; break;
        case 1:
            e.score = std::stoi(line);
            out.push_back(std::move(e));  // It is fine to move here, since we always assign values again before moving next
            break;
        }

        ++line_count;
    }

    return out;
}

void write_high_score_entries_to_file(const std::vector<ScoreEntry>& entries, const char* filepath)
{
    /* First write to a string */
    std::stringstream tmpstring;
    for (const auto& entry : entries)
    {
        tmpstring << entry.name << '\n' << entry.score << '\n';
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
