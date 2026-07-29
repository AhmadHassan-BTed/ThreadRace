#include "../../include/visualizer/AudioSynthesizer.hpp"
#include <cmath>
#include <iostream>

namespace Engine {

const double AudioSynthesizer::SCALE[15] = {
    130.81, 146.83, 164.81, 196.00, 220.00, // C3-A3
    261.63, 293.66, 329.63, 392.00, 440.00, // C4-A4
    523.25, 587.33, 659.25, 783.99, 880.00  // C5-A5
};

static void sdlAudioCallbackProxy(void* userdata, Uint8* stream, int len) {
    AudioSynthesizer* synth = static_cast<AudioSynthesizer*>(userdata);
    synth->audioCallback(stream, len);
}

AudioSynthesizer::AudioSynthesizer() : m_enabled(false), m_initialized(false), m_dev(0) {}

AudioSynthesizer::~AudioSynthesizer() {
    if (m_dev) {
        SDL_CloseAudioDevice(m_dev);
    }
}

bool AudioSynthesizer::init() {
    if (m_initialized) return true;

    SDL_AudioSpec wanted, obtained;
    SDL_zero(wanted);
    wanted.freq = 44100;
    wanted.format = AUDIO_S16SYS;
    wanted.channels = 1;
    wanted.samples = 512;
    wanted.callback = sdlAudioCallbackProxy;
    wanted.userdata = this;

    m_dev = SDL_OpenAudioDevice(NULL, 0, &wanted, &obtained, 0);
    if (m_dev == 0) {
        std::cerr << "SDL_OpenAudioDevice failed: " << SDL_GetError() << std::endl;
        return false;
    }

    SDL_PauseAudioDevice(m_dev, 0);
    m_initialized = true;
    return true;
}

void AudioSynthesizer::setEnabled(bool enabled) {
    m_enabled = enabled;
    if (m_enabled && !m_initialized) {
        init();
    }
}

void AudioSynthesizer::playNote(int depth, bool isResolve) {
    if (!m_enabled || !m_initialized) return;

    int idx = depth % 15;
    double freq = SCALE[idx];

    std::lock_guard<std::mutex> lock(m_mutex);
    ActiveNote note;
    note.frequency = freq;
    note.phase = 0.0;
    note.amplitude = isResolve ? 0.25 : 0.15;
    note.decay = isResolve ? 0.992 : 0.985;

    m_notes.push_back(note);
}

void AudioSynthesizer::playComplete() {
    if (!m_enabled || !m_initialized) return;

    std::lock_guard<std::mutex> lock(m_mutex);
    double chord[4] = { 523.25, 659.25, 783.99, 1046.50 }; // C Major
    for (int i = 0; i < 4; ++i) {
        ActiveNote note;
        note.frequency = chord[i];
        note.phase = 0.0;
        note.amplitude = 0.3;
        note.decay = 0.997;
        m_notes.push_back(note);
    }
}

void AudioSynthesizer::audioCallback(Uint8* stream, int len) {
    int16_t* buffer = reinterpret_cast<int16_t*>(stream);
    int numSamples = len / sizeof(int16_t);

    std::lock_guard<std::mutex> lock(m_mutex);

    for (int i = 0; i < numSamples; ++i) {
        double sample = 0.0;

        for (auto it = m_notes.begin(); it != m_notes.end(); ) {
            sample += std::sin(it->phase) * it->amplitude;
            it->phase += (2.0 * M_PI * it->frequency) / 44100.0;
            if (it->phase > 2.0 * M_PI) it->phase -= 2.0 * M_PI;

            it->amplitude *= it->decay;

            if (it->amplitude < 0.001) {
                it = m_notes.erase(it);
            } else {
                ++it;
            }
        }

        if (sample > 1.0) sample = 1.0;
        if (sample < -1.0) sample = -1.0;

        buffer[i] = static_cast<int16_t>(sample * 32767.0);
    }
}

}
