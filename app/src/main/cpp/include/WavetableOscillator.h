#pragma once
#include <vector>
// including since we are implementing it.
#include "AudioSource.h"


namespace wavetablesynthesizer{
    class WavetableOscillator : public AudioSource {
    public:
        WavetableOscillator() = default;
        WavetableOscillator(std::vector<float> wavetable, float sampleRate);

        // we need to implement getsample method from audio source interface

        float getSample() override;

        void onPlaybackStopped() override;


        // we also need some method to control wavetable oscillator that will be used by wavetable synthesizer.
        virtual void setFrequency(float frequency);
        virtual void setAmplitude(float newAmplitude);
        virtual void setWavetable(const std::vector<float> &wavetable);

    private:
        float interpolateLinearly() const;
        void swapWavetableIfNecessary();

        float index = 0.f;
        // this needs to be atomic because it may be changed while it is being accessed by audio thread
        std::atomic<float> indexIncrement{0.f};
        std::vector<float> waveTable;
        float sampleRate;
        std::atomic<float> amplitude{1.f};

        // wavetable swaping mechanism, if this is true means there is another wavetable waiting to be swapped
        std::atomic<bool> swapWavetable{false};
        std::vector<float> waveTableToSwap;
        std::atomic<bool> wavetableIsBeingSwapped{false}; //only true when wavetable is bieng swapped.
    };
    class A4Oscillator : public AudioSource {
    public:
        // constructor which must be explicit because it has only one argument ???
        explicit A4Oscillator(float sampleRate);

        float getSample() override;

        void onPlaybackStopped() override;

    private:
        float _phase{0.f};
        float _phaseIncrement{0.f};
    };
}