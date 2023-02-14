#pragma once

// imported to make shared pointer to work
#include <memory>
#include <oboe/Oboe.h>
#include "AudioPlayer.h"

namespace wavetablesynthesizer {

    // allows us to declare pointer to object of this class...
    class AudioSource;
// this class inherits from a oboe interface called AudioStreamData
// we are also implementing the Audio Player interface
    class OboeAudioPlayer : public oboe::AudioStreamDataCallback,
                            public AudioPlayer {
    public:
        // helper constant that tells how many channels we are using which is equal to constant taken from oboe library.
        static constexpr auto channelCount = oboe::ChannelCount::Mono;

        // this is constructor that takes in a shared pointer to an audio source and sampling rate
        OboeAudioPlayer(std::shared_ptr <AudioSource> source, int samplingRate);

        ~OboeAudioPlayer();


        // implementors of method defined in audio player
        int32_t play() override;

        void stop() override;

        // implementing a method from AudioStreamDataCallback which need to have a very specific signature.
        // takes in pointer to audio stream, pointer to audioData and number of frames.
        oboe::DataCallbackResult onAudioReady(oboe::AudioStream * audioStream,
                                              void * audioData,
                                              int32_t framesCount) override;

    private:
        std::shared_ptr <AudioSource> _source;
        std::shared_ptr <oboe::AudioStream> _stream;
        int _samplingRate;
    };

}