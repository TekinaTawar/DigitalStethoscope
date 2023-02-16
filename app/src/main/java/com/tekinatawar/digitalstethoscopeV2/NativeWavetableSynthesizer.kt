package com.tekinatawar.digitalstethoscopeV2

import androidx.lifecycle.DefaultLifecycleObserver
import androidx.lifecycle.LifecycleOwner
import kotlinx.coroutines.Dispatchers
import kotlinx.coroutines.withContext

class NativeWavetableSynthesizer : WavetableSynthesizer, DefaultLifecycleObserver {
    private var synthesizerHandle: Long = 0   //will hold the memory address of native handler
    private val synthesizerMutex = Object() // this is to make our native code exclusive for example kt code don't call it when it's destroying...

    private external fun create(): Long
    private external fun delete(synthesizerHandle: Long)
    private external fun play(synthesizerHandle: Long)
    private external fun stop(synthesizerHandle: Long)
    private external fun isPlaying(synthesizerHandle: Long): Boolean
    private external fun setFrequency(synthesizerHandle: Long, frequencyInHz: Float)
    private external fun setVolume(synthesizerHandle: Long, volumeInDb: Float)
    private external fun setWavetable(synthesizerHandle: Long, wavetable: Int)

    companion object {
        init {
            System.loadLibrary("wavetablesynthesizer")
        }
    }

    override fun onResume(owner: LifecycleOwner) {
        super.onResume(owner)
        // synchronized is a kotlin function that will make sure that only one thread can access the code inside the block
        // so if something tries to access this code while it's being executed, it won't be able to.
        synchronized(synthesizerMutex){
            createNativeHandleIfNotExists()
        }
    }

    override fun onPause(owner: LifecycleOwner) {
        super.onPause(owner)

        synchronized(synthesizerMutex){
            if (synthesizerHandle == 0L){
                return
            }
            delete(synthesizerHandle)
            synthesizerHandle = 0L
        }
    }

    private fun createNativeHandleIfNotExists() {
        if (synthesizerHandle != 0L) {
            return
        }
        // create is an external function that will create a native object and return the memory address of it
        synthesizerHandle = create()
    }

    // next we implement the remaining methods from the interface of model. each of these methods will look basically the same.
    // first we will move the execution to a differnt thread not to put load on the UI thread,
    // ...with Context(Dispatchers.Default) will do that for us. (we can do this because this is a suspended function which will be called in some coroutine context)
    // ...dispather's default store a certain pool of threads and body will be moved to it.
    // Next we will declare a synchronized block (also called lock object) then ensure that native handle exists and then call the native function...

    override suspend fun play()  = withContext(Dispatchers.Default){
        synchronized(synthesizerMutex){
            createNativeHandleIfNotExists()
            play(synthesizerHandle)
        }
    }

    override suspend fun stop() = withContext(Dispatchers.Default){
        synchronized(synthesizerMutex){
            createNativeHandleIfNotExists()
            stop(synthesizerHandle)
        }
    }

    override suspend fun isPlaying(): Boolean = withContext(Dispatchers.Default) {
        synchronized(synthesizerMutex){
            createNativeHandleIfNotExists()
            // @withContext is used to tell we are returning from differnt context
            return@withContext isPlaying(synthesizerHandle)
        }
    }

    override suspend fun setFrequency(frequencyInHz: Float)  = withContext(Dispatchers.Default){
        synchronized(synthesizerMutex) {
            createNativeHandleIfNotExists()
            setFrequency(synthesizerHandle, frequencyInHz)
        }
    }

    override suspend fun setVolume(volumeInDb: Float) = withContext(Dispatchers.Default){
        synchronized(synthesizerMutex){
            createNativeHandleIfNotExists()
            setVolume(synthesizerHandle, volumeInDb)
        }
    }

    override suspend fun setWavetable(wavetable: Wavetable) {
        synchronized(synthesizerMutex) {
            createNativeHandleIfNotExists()
            // ordinal is the int value of the enum
            setWavetable(synthesizerHandle, wavetable.ordinal)
        }
    }

}