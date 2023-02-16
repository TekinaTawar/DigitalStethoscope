#include "WavetableOscillator.h"
#include <cmath>
#include "MathConstants.h"

namespace wavetablesynthesizer {
    WavetableOscillator::WavetableOscillator(std::vector<float> waveTable, float sampleRate)
    : waveTable{std::move(waveTable)}, sampleRate{sampleRate} {}

    float WavetableOscillator::getSample() {
        swapWavetableIfNecessary();

        index = std::fmod(index, static_cast<float>(waveTable.size()));
        const auto sample = interpolateLinearly();
        index += indexIncrement;
        return amplitude * sample;
    }

    float WavetableOscillator::interpolateLinearly() const {
        const auto truncatedIndex = static_cast<typename decltype(waveTable)::size_type>(index);
        const auto nextIndex = (truncatedIndex + 1u) % waveTable.size();
        const auto nextIndexWeight = index - static_cast<float>(truncatedIndex);
        return waveTable[nextIndex] * nextIndexWeight +  (1.f - nextIndexWeight) * waveTable[truncatedIndex];
    }

    void WavetableOscillator::swapWavetableIfNecessary() {
        // this is called from audio thread so we cannot log also we need to be extra careful becuase two threads are accessing same memroy
        // memory_order_release updates this on all the threads
        wavetableIsBeingSwapped.store(true, std::memory_order_release);

        // this indicates that we kind of pull the value of this atomic value variable from all other threads up to date.
        if (swapWavetable.load(std::memory_order_acquire)) {
            std::swap(waveTable, waveTableToSwap);
            swapWavetable.store(false, std::memory_order_relaxed);
        }
        wavetableIsBeingSwapped.store(false, std::memory_order_release);
    }

    void WavetableOscillator::setWavetable(const std::vector<float> &wavetable) {
        swapWavetable.store(false, std::memory_order_release);
        // this is  not a mutex this is a live lock this will spin and spin and spin until the other thread is done swapping the wavetable
        while (wavetableIsBeingSwapped.load(std::memory_order_acquire)) {
        }
        waveTableToSwap = wavetable;
        swapWavetable.store(true, std::memory_order_release);
    }

    void WavetableOscillator::setFrequency(float frequency) {
        indexIncrement = frequency * static_cast<float>(waveTable.size()) / static_cast<float>(sampleRate);
    }

    void WavetableOscillator::setAmplitude(float newAmplitude) {
       amplitude.store(newAmplitude);
    }

    void WavetableOscillator::onPlaybackStopped() {
        index = 0.f;
    }

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