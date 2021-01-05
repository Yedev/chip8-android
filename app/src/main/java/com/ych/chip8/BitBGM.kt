package com.ych.chip8

import android.media.AudioAttributes
import android.media.SoundPool

class BitBGM {

    val soundPool: SoundPool

    var beepBgmId: Int = -1

    var bgmLoaded = false

    init {
        val attrs = AudioAttributes.Builder().apply {
            setUsage(AudioAttributes.USAGE_GAME)
            setContentType(AudioAttributes.CONTENT_TYPE_MUSIC)
        }.build()

        soundPool = SoundPool.Builder().apply {
            setMaxStreams(8)
            setAudioAttributes(attrs)
        }.build()

        soundPool.setOnLoadCompleteListener { soundPool, sampleId, status ->
            if (sampleId == beepBgmId && status == 0) {
                bgmLoaded = true
            }
        }

//        beepBgmId = soundPool.load(CApplication.getApplication(), R.id.item_tv_key, 1)
    }

    fun beep() {
        if (bgmLoaded) {
            soundPool.play(beepBgmId, 1.0F, 1.0F, 100, 0, 1.0F)
        }
    }

    companion object {
        private var bitBGM: BitBGM? = null;
        @JvmStatic
        fun get(): BitBGM {
            bitBGM = BitBGM()
            return bitBGM!!
        }
    }
}