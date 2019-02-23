#include "vignere_encryptor.h"

#include <string_view>

#include <gfx.h>
#include <cglutil.h>

namespace pac
{
VignereEncryption::VignereEncryption(const char* key)
{
    GFX_ASSERT(key, "Vignere Encryption key must not be nullptr.");

    /* Convert key to uppercase */
    std::string new_key(key);
    for (auto& letter : new_key)
    {
        if (isalpha(letter))
        {
            letter = toupper(letter);
        }
    }

    m_key = new_key;
}

void VignereEncryption::encrypt(std::string& str)
{
    std::size_t key_idx = 0;
    for (auto& letter : str)
    {
        bool was_uppercase = isupper(letter);
        letter = toupper(letter);
        if (isalpha(letter))
        {
            letter += m_key[key_idx++] - 65;
            if (letter > 90)
            {
                letter = 64 + (letter - 90);
            }
            if (key_idx == m_key.size())
            {
                key_idx = 0;
            }
        }

        /* Put letter back to original case */
        if (!was_uppercase)
        {
            letter = tolower(letter);
        }
    }
}

void VignereEncryption::decrypt(std::string& str)
{
    std::size_t key_idx = 0;
    for (auto& letter : str)
    {
        bool was_uppercase = isupper(letter);
        letter = toupper(letter);
        if (isalpha(letter))
        {
            letter -= m_key[key_idx++] - 65;
            if (letter < 65)
            {
                letter = 91 - (65 - letter);
            }
            if (key_idx == m_key.size())
            {
                key_idx = 0;
            }
        }

        /* Put letter back to original case */
        if (!was_uppercase)
        {
            letter = tolower(letter);
        }
    }
}
}  // namespace pac
