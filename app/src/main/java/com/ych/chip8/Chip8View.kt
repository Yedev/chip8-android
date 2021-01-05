package com.ych.chip8

import android.content.Context
import android.graphics.Color
import android.graphics.Paint
import android.graphics.RectF
import android.util.AttributeSet
import android.view.SurfaceView

class Chip8View(context: Context, attrs: AttributeSet? = null) :
    SurfaceView(context, attrs), Runnable, Chip8 {
    private val PIX_SCALE = 10f
    private val CHIP8_P_WIDTH = 64
    private val CHIP8_P_HEIGHT = 32
    private var nativePtr: Long
    private var thread: Thread? = null
    private var state = VMState.IDLE
    private var isRunning = true
    private val pixs = ByteArray(CHIP8_P_WIDTH * CHIP8_P_HEIGHT)
    private val paint = Paint().apply {
        setAntiAlias(true)
        style = Paint.Style.FILL
    }

    init {
        nativePtr = ChipJNI.jcreateChip8()
        thread = Thread(this)
        thread?.start()
    }

    override fun onMeasure(widthMeasureSpec: Int, heightMeasureSpec: Int) {

        val w = MeasureSpec.makeMeasureSpec(
            PIX_SCALE.toInt() * CHIP8_P_WIDTH,
            MeasureSpec.EXACTLY
        )
        val h = MeasureSpec.makeMeasureSpec(
            PIX_SCALE.toInt() * CHIP8_P_HEIGHT,
            MeasureSpec.EXACTLY
        )
        super.onMeasure(w, h)
    }

    override fun loadGame(path: String) {
        ChipJNI.jload(nativePtr, path)
        if (state != VMState.RUNNING) {
            state = VMState.RUNNING
        }
    }

    override fun onResume() {
        if (state == VMState.IDLE)
            return
        if (state != VMState.RUNNING) {
            state = VMState.RUNNING
        }
    }

    override fun onStop() {
        if (state == VMState.IDLE)
            return
        state = VMState.STOP
    }

    override fun onDestory() {
        ChipJNI.jdestory(nativePtr)
        state = VMState.IDLE
        isRunning = false
        try {
            thread?.join()
        } catch (ex: InterruptedException) {
        }
    }

    override fun keyEvent(index: Int, pressed: Boolean) {
        if (state != VMState.RUNNING)
            return
        val p = if (pressed) 1 else 0
        ChipJNI.jkeyEvent(nativePtr, index, p)
    }

    override fun run() {
        while (isRunning) {
            loop()
        }
    }

    override fun loop() {
        if (state == VMState.RUNNING) {
            if (ChipJNI.jloop(nativePtr, pixs) == 1)
                drawFrame()
        }
    }

    private fun drawFrame() {
        val canvas = holder.lockCanvas()
        if (canvas == null)
            return
        canvas.apply {
            save()
            drawColor(Color.WHITE)
            val rf = RectF()//10
            for (i in 0..pixs.size - 1) {
                if (pixs[i] == 1.toByte())
                    paint.color = Color.WHITE
                else
                    paint.color = Color.BLACK
                val row = i % 64 * PIX_SCALE
                val col = i / 64 * PIX_SCALE
                rf.set(row, col, row + PIX_SCALE, col + PIX_SCALE)
                drawRect(rf, paint)
            }
        }
        holder.unlockCanvasAndPost(canvas)
    }
}
