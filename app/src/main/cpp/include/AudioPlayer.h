#pragma once

//containes definitino of the interface
namespace wavetablesynthesizer {
    //purlely virtual class which is c++ equivalent of interface
    class AudioPlayer {
    public:
        //virtual destructor so that class can be freed properly
        virtual ~AudioPlayer() = default;
        // play method that will return a number indicating succes or failure of operation.
        // virtual keyword mean this class will be overwritten and will behave polymorphically for all the classes that inherit from audioplayer.
        virtual int32_t play() = 0;
        // stop method which will not return any value.
        // =0 mean that this function is abstract so it's not implemented and this class cannot be instantiated..
        virtual void stop() = 0;


    };
}

// what's nice aobut this is that this interface doesn't depend on anything as you can see there are no includes and therefore it will decouple our code from
// the specifics of the audio driver or in this case oboe library