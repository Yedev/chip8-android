package com.ych.chip8


interface Chip8 {

    fun loadGame(path: String)

    fun loop()

    fun onStop()

    fun onResume()

    fun onDestory()

    fun keyEvent(index: Int, pressed: Boolean)
}

enum class VMState {
    IDLE,
    RUNNING,
    STOP
}
