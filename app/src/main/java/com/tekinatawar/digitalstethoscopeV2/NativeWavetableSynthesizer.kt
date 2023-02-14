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
            System.loadLibrary("digitalstethoscopeV2")
        }
    }

    override fun onResume(owner: LifecycleOwner) {
        super.onResume(owner)

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

        synthesizerHandle = create()
    }


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
            setWavetable(synthesizerHandle, wavetable.ordinal)
        }
    }

}