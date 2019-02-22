#include "sound_manager.h"
#include "waveloader.h"

#include <future>
#include <algorithm>
#include <filesystem>

#include <gfx.h>
#include <cglutil.h>

namespace pac
{
pac::SoundManager::SoundManager()
{
    /* Initialize device and context */
    m_audio_device = alcOpenDevice(nullptr);
    GFX_ASSERT(m_audio_device, "Audio device failed to initialize.");

    m_audio_context = alcCreateContext(m_audio_device, nullptr);
    GFX_ASSERT(m_audio_device, "Audio context failed to initialize.");
    alcMakeContextCurrent(m_audio_context);

    /* Iterate all files in resource folder */
    const auto res_path = std::filesystem::path("res/");
    for (auto entry = std::filesystem::directory_iterator(res_path); entry != std::filesystem::directory_iterator(); ++entry)
    {
        /* If file is a .wav file, load it! */
        if (entry->path().extension() == ".wav")
        {
            GFX_DEBUG("Loading audio file (%s) as (%s)", entry->path().c_str(), entry->path().stem().c_str());
            auto sound = loadio::WaveFile(entry->path().c_str());
            m_sound_buffers[entry->path().stem()] = sound.createOpenalBuffer();
        }
    }

    GFX_INFO("Loaded %u sound effects / music tracks.", m_sound_buffers.size());

    /* Generate a reasonable number of sources for multiple SFX playback and overlap */
    constexpr int num_sources = 12;
    m_inactive_sources.resize(num_sources);
    alGenSources(num_sources, m_inactive_sources.data());

    /* Set all positions and velocities to 0 */
    for (auto source : m_inactive_sources)
    {
        alSource3f(source, AL_POSITION, 0.f, 0.f, 0.f);
        alSource3f(source, AL_VELOCITY, 0.f, 0.f, 0.f);
        alSourcei(source, AL_LOOPING, 0);
    }

    /* Set listener position and velocity to 0 */
    alListener3f(AL_POSITION, 0.f, 0.f, 0.f);
    alListener3f(AL_VELOCITY, 0.f, 0.f, 0.f);
}

void SoundManager::play(const std::string& sound_name)
{
    /* Before we play anything, move finished active sources back to the inactive pool. We only need to do this whenever a new
     * sound is requested playing, since otherwise no state will have changed since last time */
    auto itrs = std::partition_copy(m_active_sources.begin(), m_active_sources.end(), std::back_inserter(m_inactive_sources),
                                    m_active_sources.begin(), [](unsigned n) {
                                        int state;
                                        alGetSourcei(n, AL_SOURCE_STATE, &state);
                                        return AL_STOPPED == state;
                                    });

    m_active_sources.erase(itrs.second, m_active_sources.end());

    GFX_DEBUG("Currently %u active and %u inactive sources playing audio.", m_active_sources.size(), m_inactive_sources.size());
    GFX_ASSERT(!m_inactive_sources.empty(), "No available sound sources to play audio!");

    /* Get an available source */
    auto source = m_inactive_sources.back();
    m_inactive_sources.pop_back();

    /* Queue it up */
    alSourcei(source, AL_BUFFER, m_sound_buffers.at(sound_name));
    alSourcePlay(source);

    /* Add it to the active sources */
    m_active_sources.push_back(source);
}

SoundManager::~SoundManager()
{
    /* Stop all playing sounds */
    for (auto source : m_active_sources)
    {
        int state;
        alGetSourcei(source, AL_SOURCE_STATE, &state);
        if (state != AL_STOPPED)
        {
            alSourceStop(source);
        }

        m_inactive_sources.push_back(source);
    }

    m_active_sources.clear();

    /* Delete Buffers */
    for (auto buffer : m_sound_buffers)
    {
        alDeleteBuffers(1, &buffer.second);
    }

    /* Delete sources and close device / context */
    alDeleteSources(m_inactive_sources.size(), m_inactive_sources.data());
    alcDestroyContext(m_audio_context);
    alcCloseDevice(m_audio_device);
}

SoundManager& get_sound()
{
    static SoundManager sm;
    return sm;
}

}  // namespace pac
