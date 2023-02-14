#include "WavetableOscillator.h"
#include <cmath>
#include "MathConstants.h"

namespace wavetablesynthesizer {

    // here we initilize our phaseIncrement and phase, phase increment is initilized with 44o and then changed into digital by dividing it by sample rate and multiplying by
    // ... 2pi
    A4Oscillator::A4Oscillator(float sampleRate)
        : _phaseIncrement{440.f / sampleRate * 2.f * PI} {}

    float A4Oscillator::getSample(){
        // multiplication by 0.5f is to reduce the amplitude we generate
        const auto sample = 0.5f * std::sin(_phase);
        // then we increase our phase but we cannot do it just by using += sign because we may actually overflow right becuase if the phase will be increasing up to inifinity
        // ...then we'll overflow  the floating point numbers so what we want to do is increment and check if we have left 2pi range if we did let's go back to this range...
        _phase = std::fmod(_phase + _phaseIncrement, 2.f * PI);
        return sample;

    };

    void A4Oscillator::onPlaybackStopped() {
        _phase = 0.f;
    };

}