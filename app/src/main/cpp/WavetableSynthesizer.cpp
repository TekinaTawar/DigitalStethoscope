#include "WavetableSynthesizer.h"
#include "Log.h"
#include "OboeAudioPlayer.h"
#include "WavetableOscillator.h"
#include <cmath>

namespace wavetablesynthesizer {
    // this constructor for an oscillator will create an A4 oscillator using the sampleRate
    WavetableSynthesizer::WavetableSynthesizer()
    : _oscillator{std::make_shared<WavetableOscillator>(
            _wavetableFactory.getWaveTable(_currentWavetable)
            ,sampleRate)},
        _audioPlayer{std::make_unique<OboeAudioPlayer>(_oscillator, sampleRate)} {};

    WavetableSynthesizer::~WavetableSynthesizer() = default;

    void WavetableSynthesizer::play() {
        // LOGD("isPlaying() called.");
        std::lock_guard<std::mutex> lock(_mutex);
        const auto result = _audioPlayer->play();
        if (result == 0 ) {
            _isPlaying = true;
        } else {
            LOGD("Could not start playback.");
        }
    }

    void WavetableSynthesizer::stop() {
        // LOGD("stop() called.");
        std::lock_guard<std::mutex> lock(_mutex);
        _audioPlayer->stop();
        _isPlaying = false;
    }

    bool WavetableSynthesizer::isPlaying() const {
        LOGD("isPlaying() called.");
        return _isPlaying;
    }

    void WavetableSynthesizer::setFrequency(float frequencyInHz) {
//        LOGD("setFrequency() called with %.2f Hz argument.", frequencyInHz);
        _oscillator->setFrequency(frequencyInHz);
    }

    float dbToAmplitude(float db) {
        return std::pow(10.f, db / 20.f);
    }
    void WavetableSynthesizer::setVolume(float volumeInDb) {
        // LOGD("setVolume() called with %.2f dB argument.", volumeInDb);
        const auto amplitude = dbToAmplitude(volumeInDb);
        _oscillator->setAmplitude(amplitude);
    }

    void WavetableSynthesizer::setWavetable(Wavetable wavetable){
        // LOGD("setWavetable() called with %.d argument.", static_cast<int>(wavetable));
        if(_currentWavetable != wavetable){
            _currentWavetable = wavetable;
            _oscillator->setWavetable(_wavetableFactory.getWaveTable(wavetable));
        }
    }
}