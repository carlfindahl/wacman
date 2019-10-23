#pragma once

template<typename T>
class BaseCrypt
{
public:
    virtual ~BaseCrypt() noexcept = default;

    /*!
     * \brief encrypt encrypts the given value
     */
    virtual void encrypt(T& val) = 0;

    /*!
     * \brief decrypt decrypts the given value that was encrypted with the same
     * encryptor.
     */
    virtual void decrypt(T& val) = 0;
};
