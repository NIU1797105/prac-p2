#include "sound_mgr.h"
#include "private/sdl_wrapper/sound.h"
#include "util.h"

using MusicToken = SoundManager::MusicToken;
extern struct T_SOUND* g_current_music;

void SoundManager::loadMusic(const char* path)
{
    auto data = getDataDirPath();
    m_bgMusic = Sound_LoadMusic((char*)(data + "audio/" + path).c_str(), 1);
    m_bgMusic->bLoop = PLAY_THEN_LOOP_AT_END;
    m_bgMusic->estado = SOUND_STATE_PLAYING;
    g_current_music = m_bgMusic;
    playSound(m_bgMusic);
}

MusicToken SoundManager::loadSound(const char* path) const
{
    auto data = getDataDirPath();
    return Sound_LoadSound((char*)(data + "audio/" + path).c_str());
}

void SoundManager::startMusic() const
{
    stopMusic();
    g_current_music = m_bgMusic;
    m_bgMusic->estado = SOUND_STATE_PLAYING;
}

void SoundManager::stopMusic() const
{
    if (g_current_music)
        Sound_Pause(m_bgMusic);
    g_current_music = nullptr;
}

void SoundManager::playSound(MusicToken token, PlayOpts opts) const
{
    stopMusic();
    Sound_Play(token, static_cast<int>(opts));
    startMusic();
}

void SoundManager::playSound(MusicToken token) const
{
    Sound_Play(token, SOUND_FORCE_RESTART);
}

void SoundManager::stopSound(MusicToken token) const
{
    Sound_Stop(token);
}
