#include "common.h"

#include <fstream>

#include <cglutil.h>

namespace pac
{
std::vector<ScoreEntry> load_high_score_entries_from_file(const char* filepath)
{
    /* Open file to read */
    std::ifstream file(cgl::native_absolute_path(filepath));

    /* Store each new entry temporarily in e, then based on the line count, read data into line and parse relevant information */
    ScoreEntry e;
    int line_count = 0;
    std::string line{};
    std::vector<ScoreEntry> out{};
    while (std::getline(file, line))
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
    std::ofstream file(cgl::native_absolute_path(filepath));
    for (const auto& entry : entries)
    {
        file << entry.name << '\n' << entry.score << '\n';
    }
}

}  // namespace pac
