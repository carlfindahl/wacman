#pragma once

#include <vector>
#include <string>

#include "robinhood/robinhood.h"

/* Forward Declarations */
typedef struct ALCcontext_struct ALCcontext;
typedef struct ALCdevice_struct ALCdevice;

namespace pac
{
/*!
 * \brief The SoundManager class is a singleton responsible for playing audio and internally managing the audio buffers,
 * sources and listeners. It can also load music, although it is not streamed, but loaded up front.
 */
class SoundManager
{
private:
    /* Map of sound names to buffers (the name is the filename without an extension) */
    robin_hood::unordered_map<std::string, unsigned> m_sound_buffers{};

    /* All actively playing sound sources */
    std::vector<unsigned> m_active_sources = {};

    /* All inactive (pending) sound sources */
    std::vector<unsigned> m_inactive_sources = {};

    /* Audio Device */
    ALCdevice* m_audio_device = nullptr;

    /* OpenAL Context */
    ALCcontext* m_audio_context = nullptr;

public:
    /*!
     * \brief play the sound with the given name
     * \param sound_name is the name of the sound to play
     * \param looped true if you want the sound to loop forever (like a music track maybe?)
     * \return id of sound that started  playing. Use this to later stop the sound.
     */
    unsigned play(const std::string& sound_name, bool looped = false);

    /*!
     * \brief stop stops a currently playing sound if it is still playing
     * \param sound_id_from_play is the id of the sound you want to stop that play returned to you
     */
    void stop(unsigned sound_id_from_play);

    ~SoundManager();

private:
    SoundManager();

    friend SoundManager& get_sound();
};

/*!
 * \brief get_sound returns access to the SoundManager singleton
 * \return the sound manager for playing sounds
 */
SoundManager& get_sound();

}  // namespace pac
