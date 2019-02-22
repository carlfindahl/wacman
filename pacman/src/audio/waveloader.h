/*!
 * Waveloader.hpp: This file contains the .wav loader
 * 13. Oct 2018: Initial File
 * 13. Oct 2018: Add basic OpenAL support
 * 18. Oct 2018: Make Exceptions Optional
 */

#ifndef WAVELOADER_H
#define WAVELOADER_H

#include <string>
#include <vector>
#include <cstdio>
#include <cstdint>
#include <iostream>
#include <stdexcept>

#ifdef LOADIO_OPENAL
#include <AL/al.h>
#endif

namespace loadio
{
namespace detail
{
/*!
 * \brief The HeaderRiff struct is the special case of a header. This is always the first in a wave file and
 * denotes the total size of the file.
 */
struct HeaderRiff
{
    uint8_t chunkID[4]{};
    uint32_t chunkSize{};
    uint8_t format[4]{};
};

/*!
 * \brief The Header struct is a generic header for all wav chunks. Used to get the ID and size of the next
 * chunk that should be read.
 */
struct Header
{
    uint8_t ID[4];
    uint32_t size;
};

/*!
 * \brief The HeaderFmt struct contains all format specific information in this wave file
 */
struct HeaderFmt
{
    uint16_t audioFormat{};
    uint16_t numChannels{};
    uint32_t sampleRate{};
    uint32_t byteRate{};
    uint16_t blockAlign{};
    uint16_t bitsPerSample{};
};

using HeaderData = std::vector<uint8_t>;
}  // namespace detail

/*!
 * \brief The WaveFile class can read wavefiles
 */
class WaveFile
{
private:
    detail::HeaderFmt m_fmt;

    detail::HeaderData m_data;

public:
    WaveFile(const char* filepath) { loadFromFile(filepath); }

    /*!
     * \brief loadFromFile loads a new wave file from file.
     * \param fp is the filepath to load from. If this is invalid, throws an invalid_argument exception
     * \throws invalid_argument if the filepath does not exist
     */
    void loadFromFile(const char* fp)
    {
        detail::HeaderRiff riff;

        /* Open File */
        FILE* f = fopen(fp, "rb");

        if (!f)
        {
            throw std::invalid_argument(std::string("Filepath (fp) ") + fp + " does not exist!");
        }

        /* Read RIFF Header */
        fread(&riff, sizeof riff, 1, f);

        /* Local storage to check for completion, and reading next header */
        detail::Header nextHeader;
        bool bFMT{false}, bDATA{false};

        do
        {
            fread(&nextHeader, sizeof nextHeader, 1, f);

            /* Read FMT */
            if (nextHeader.ID[0] == 102 && nextHeader.ID[1] == 109 && nextHeader.ID[2] == 116 &&
                nextHeader.ID[3] == 32)
            {
                fread(&m_fmt, nextHeader.size, 1, f);
                bFMT = true;
            }
            /* Read DATA */
            else if (nextHeader.ID[0] == 100 && nextHeader.ID[1] == 97 && nextHeader.ID[2] == 116 &&
                     nextHeader.ID[3] == 97)
            {
                m_data.resize(nextHeader.size);
                fread(m_data.data(), nextHeader.size, 1, f);
                bDATA = true;
            }
            /* Just move on otherwise (MUST SUPPORT COMPRESSED WAV IN FUTURE) */
            else
            {
                detail::HeaderData junk(nextHeader.size);
                fread(junk.data(), nextHeader.size, 1, f);
            }
        } while (!(bFMT && bDATA));

        fclose(f);
    }

    /*!
     * \brief data returns the raw waveform data
     * \return the waveform data
     */
    const uint8_t* data() const { return m_data.data(); }

    /*!
     * \brief size retuns the data size in bytes
     * \return the size of the wave data in bytes
     */
    size_t size() const { return m_data.size(); }

    /*!
     * \brief frequency gets the frequency of the wave file
     * \return the frequency
     */
    uint32_t frequency() const { return m_fmt.sampleRate; }

#ifdef LOADIO_OPENAL
    /*!
     * \brief getALformat gets one of the four supported OpenAL formats. STEREO16, MONO16, STEREO8 and MONO8
     * If another format has been loaded this function will thrown a runtime_error
     * \throws runtime_error if the loaded wav file is not one of the four supported formats and exceptions
     * are enabled. Otherwise it will return 0 if the format is not known!
     * \return An OpenAL enum representing the format in a way OpenAL understands
     */
    ALenum getALformat() const
    {
        if (m_fmt.numChannels == 2 && m_fmt.bitsPerSample == 16)
        {
            return AL_FORMAT_STEREO16;
        }
        else if (m_fmt.numChannels == 2 && m_fmt.bitsPerSample == 8)
        {
            return AL_FORMAT_STEREO8;
        }
        else if (m_fmt.numChannels == 1 && m_fmt.bitsPerSample == 16)
        {
            return AL_FORMAT_MONO16;
        }
        else if (m_fmt.numChannels == 1 && m_fmt.bitsPerSample == 8)
        {
            return AL_FORMAT_MONO8;
        }

#ifdef LOADIO_USE_EXCEPTIONS
        throw std::runtime_error("Non Supported OpenAL Format (#CH: " + std::to_string(m_fmt.numChannels) +
                                 " BPS: " + std::to_string(m_fmt.bitsPerSample) + ")");
#else
     return 0;
#endif
    }

    /*!
     * \brief createOpenalBuffer uses the internal state of the wave file to create an openal buffer.
     * \note Remember that it is the caller's responsibility to delete the buffer using alDeleteBuffers!
     * \return A handle to the newly created OpenAL buffer
     */
    ALuint createOpenalBuffer() const
    {
        uint32_t buf;
        alGenBuffers(1, &buf);
        alBufferData(buf, getALformat(), data(), size(), frequency());
        return buf;
    }
#endif
};

}  // namespace loadio

#endif  // WAVELOADER_H
