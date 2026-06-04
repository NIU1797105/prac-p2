#ifndef SOUND_MGR_H
#define SOUND_MGR_H
#include "util.h"
#include "private/sdl_wrapper/sound.h"
#include <string>

enum class PlayOpts {
    NO_RESTART = SOUND_DO_NOT_RESTART_IF_ALREADY_PLAYING,
    FORCE_RESTART = SOUND_FORCE_RESTART
};

class SoundManager
{
public:
    using MusicToken = T_SOUND*;

    SoundManager() : m_bgMusic(nullptr) { stopMusic(); }
    ~SoundManager() { stopMusic(); }
    void stopMusic() const;
    void startMusic() const;
    void loadMusic(const char* path);
    MusicToken loadSound(const char* path) const;
    void playSound(MusicToken) const;
    void playSound(MusicToken, PlayOpts) const;
    void stopSound(MusicToken) const;
    // HACK
    static void SoundInit(void) { Sound_Init(); }
private:
    MusicToken m_current_music;
    MusicToken m_bgMusic;
};
#endif
