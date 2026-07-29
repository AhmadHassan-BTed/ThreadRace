#ifndef AUDIO_SYNTHESIZER_HPP
#define AUDIO_SYNTHESIZER_HPP

#include <SDL2/SDL.h>
#include <vector>
#include <mutex>

namespace Engine {

struct ActiveNote {
    double frequency;
    double phase;
    double amplitude;
    double decay;
};

class AudioSynthesizer {
public:
    AudioSynthesizer();
    ~AudioSynthesizer();

    bool init();
    void setEnabled(bool enabled);
    bool isEnabled() const { return m_enabled; }

    void playNote(int depth, bool isResolve = false);
    void playComplete();

    void audioCallback(Uint8* stream, int len);

private:
    bool m_enabled;
    bool m_initialized;
    SDL_AudioDeviceID m_dev;
    std::vector<ActiveNote> m_notes;
    std::mutex m_mutex;

    static const double SCALE[15];
};

}

#endif
