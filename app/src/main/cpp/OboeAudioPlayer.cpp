#include "OboeAudioPlayer.h"
#include "AudioSource.h"

// for simplicity since this is an implementation file we will use...
using namespace oboe;

namespace wavetablesynthesizer {

    OboeAudioPlayer::OboeAudioPlayer(std::shared_ptr <AudioSource> source, int samplingRate)
    : _source{std::move(source)}, _samplingRate{samplingRate}{    }

    OboeAudioPlayer::~OboeAudioPlayer() {
        OboeAudioPlayer::stop();
    }

    int32_t OboeAudioPlayer::play() {
        //oboe uses a builder pattern i.e., audioStreamBuilder and we simply call it's methods which configures the audio stream for us and finally we created at the very end
        AudioStreamBuilder builder;

        // result of building our audio stream
        // setDirection tells we are only playing back music and we are not recording...
        // setDataCallback makes oboes to call this instance of audio player onAudioReady...
        // setSharingMode is used to tell if we want to allow other apps to play sound while this app is playing or not...
        // setFormat sometimes we don't get the format in float in gneral you should check for this whether it's actually float. you can check it by querying audio stream properties
        // audioDriver might not support the sampleRate that you have set in that case setSampleRateConversionQuality is used to tell which kind of converstion the better the
        // ...conversion longer it will take.
        const auto result =
                builder.setPerformanceMode(PerformanceMode::LowLatency)
                    ->setDirection(Direction::Output)
                    ->setSampleRate(_samplingRate)
                    ->setDataCallback(this)
                    ->setSharingMode(SharingMode::Exclusive)
                    ->setFormat(AudioFormat::Float)
                    ->setChannelCount(channelCount)
                    ->setSampleRateConversionQuality(SampleRateConversionQuality::Best)
                    ->openStream(_stream);

        // if the result is not ok then return the same error but in form of an int
        if (result != Result::OK){
            return static_cast<int32_t>(result);
        }

        // requestStart is differnt from start in terms of we are telling our stream to start playing but we are not waiting for it to start playing
        const  auto playResult = _stream->requestStart();

        return static_cast<int32_t>(playResult);
    };

    void OboeAudioPlayer::stop() {
        // if stream is a valid pointer
        if (_stream) {
            // here we are not requesting we are waiting for the stop to actually happen
            _stream->stop();
            _stream->close();
            _stream.reset();
        }
        _source->onPlaybackStopped();
    };

    // the is the main Audio Callback, this is our bread and butter of audio processing which is called from audio thread
    oboe::DataCallbackResult OboeAudioPlayer::onAudioReady(oboe::AudioStream * audioStream,
                                          void* audioData,
                                          int32_t framesCount) {
        // first we retrieve a pointer to array of float from our audioData where void* audioData is pointer to some block of memory which is not associated with any datatype
        auto* floatData = reinterpret_cast<float*>(audioData);

        // no we go over each frame and we fill the float data with the samples that we retrieve from our audio source...
        // so frame is package of samples for each channel. here we have mono but it can be stereo in some ohter case so we will write this code in channel agnostic way
        // ... so it can be used even when we have stereo audio
        for (auto frame = 0; frame < framesCount; ++frame) {
            const auto sample = _source->getSample();

            for (auto channel = 0; channel < channelCount; ++channel) {
                // we have interleaved samples i.e., for stereo 5 frames will contain 10 samples
                floatData[frame * channelCount + channel] = sample;
            }
        }
        //which indicates we want to continue playing back nothing unexpected happen
        return oboe::DataCallbackResult::Continue;
    };

}