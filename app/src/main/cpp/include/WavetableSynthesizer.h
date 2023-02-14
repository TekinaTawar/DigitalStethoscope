#pragma once

#include <memory>

namespace wavetablesynthesizer{
    enum class Wavetable {
        SINE, TRIANGLE, SQUARE, SAW
    };

    // to forward declare them
    class AudioSource;
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
        bool _isPlaying = false;
        // wavetable synthesizer will hold the references of both the source and player, it will also connect the two and it also needs to interact with it like in case of audio
        // ... source wavetableSynthesizer can change the frequency that's bieng played or we need to change the waveform and we also need the refernce to the player because we need
        // ... to play the player and stop the player
        std::shared_ptr<AudioSource> _oscillator;
        std::unique_ptr<AudioPlayer> _audioPlayer;
    };
}