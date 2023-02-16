#pragma once

#include <memory>
#include <mutex>
#include "Wavetable.h"
#include "WavetableFactory.h"

namespace wavetablesynthesizer{
    // to forward declare them
    class WavetableOscillator;
    class AudioPlayer;

    constexpr auto sampleRate = 48000;

    class WavetableSynthesizer {
    public:
        WavetableSynthesizer();
        ~WavetableSynthesizer();
        void play();
        void stop();
        bool isPlaying() const;
        void setFrequency(float frequencyInHz);
        void setVolume(float volumeInDb);
        void setWavetable(Wavetable wavetable);

    private:
        std::atomic<bool> _isPlaying = false;
        std::mutex _mutex;
        WavetableFactory _wavetableFactory;
        Wavetable _currentWavetable{Wavetable::SINE};
        // wavetable synthesizer will hold the references of both the source and player, it will also connect the two and it also needs to interact with it like in case of audio
        // ... source wavetableSynthesizer can change the frequency that's bieng played or we need to change the waveform and we also need the refernce to the player because we need
        // ... to play the player and stop the player
        std::shared_ptr<WavetableOscillator> _oscillator;
        std::unique_ptr<AudioPlayer> _audioPlayer;
    };
}