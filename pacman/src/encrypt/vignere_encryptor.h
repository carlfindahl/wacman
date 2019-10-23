#pragma once

#include "base_encryptor.h"

#include <string>

namespace pac
{
/*!
 * \brief The Vignere Encryptor will offset each letter in the given string by the letter
 * value of the key it is constructed with. When it reaches the end of the key, it will start
 * over with the first letter. */
class VignereEncryption : public BaseCrypt<std::string>
{
private:
    /* The encryption key to use */
    std::string m_key;

public:
    /*!
     * \brief Construct a Vignere Encryptor using this as the "pass-key"
     */
    VignereEncryption(const char* key);

    virtual void encrypt(std::string& str) override;

    virtual void decrypt(std::string& str) override;
};
}  // namespace pac
