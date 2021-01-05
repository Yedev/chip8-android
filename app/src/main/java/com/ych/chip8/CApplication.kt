package com.ych.chip8

import android.app.Application

class CApplication : Application() {
    override fun onCreate() {
        super.onCreate()
        application = this
    }

    companion object {
        private var application: Application? = null
        fun getApplication() = application!!

    }
}