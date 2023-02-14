#pragma once

namespace wavetablesynthesizer {
    // this soumething that will be seen by the class interacting with the class oboe audio driver.
    // and we don't want to be specific that we have synthesizer as our audio Source we want to keep it quite general so that we can playback anything we want and in our case
    // .. we want to play back a test signal
    class AudioSource {
    public:
        virtual ~AudioSource() = default;
        // this function will return only once sample of audio output typically this kind of functions is implemented by returning whole block of sample but our applicaiton is
        //.. simple enough to return sample, sample by sample...
        virtual float getSample() = 0;
        // this is a callback and it will be invoked by the interoactor with the audio driver when the playback was stopped so audio source may reset it's buffers and audio state
        virtual void onPlaybackStopped() = 0;
    };
}