package com.ych.chip8

import android.os.Bundle
import android.view.LayoutInflater
import android.view.MotionEvent
import android.view.View
import android.view.ViewGroup
import android.widget.TextView
import androidx.appcompat.app.AppCompatActivity
import androidx.recyclerview.widget.GridLayoutManager
import androidx.recyclerview.widget.RecyclerView
import java.io.File
import java.io.FileOutputStream

class MainActivity : AppCompatActivity() {
    lateinit var chip8: Chip8View

    override fun onCreate(savedInstanceState: Bundle?) {
        super.onCreate(savedInstanceState)
        setContentView(R.layout.main)
        val path = initFile()
        chip8 = findViewById(R.id.main_chip8)
        chip8.loadGame(path)
        initKeyboard()
    }

    private fun initFile(): String {
        val cacheDir = externalCacheDir
        val name = "Pong [Paul Vervalin, 1990].ch8"
        val pong = File(cacheDir, name)
        if (!pong.exists()) {
            pong.createNewFile()
            val manager = assets
            val input = manager.open("games/$name")
            val outputStream = FileOutputStream(pong)
            input.copyTo(outputStream, 2048)
            outputStream.flush()
            input.close()
            outputStream.close()
        }
        return pong.absolutePath
    }

    private fun initKeyboard() {
        val rv: RecyclerView = findViewById(R.id.main_keyboard)
        val layoutManager = GridLayoutManager(this, 4)
        rv.layoutManager = layoutManager
        val adapter = KeyboardAdapter { i, p ->
            chip8.keyEvent(i, p)
        }
        rv.adapter = adapter
    }

    override fun onPause() {
        super.onPause()
        chip8.onStop()
    }

    override fun onResume() {
        super.onResume()
        chip8.onResume()
    }

    override fun onDestroy() {
        super.onDestroy()
        chip8.onDestory()
    }
}

class KeyboardViewHolder(itemView: View) : RecyclerView.ViewHolder(itemView) {}
class KeyboardAdapter(val onKeyEvent: (index: Int, pressed: Boolean) -> Unit) :
    RecyclerView.Adapter<RecyclerView.ViewHolder>() {
    val keys =
        arrayOf('0', '1', '2', '3', '4', '5', '6', '7', '8', '9', 'A', 'B', 'C', 'D', 'E', 'F')

    override fun onCreateViewHolder(parent: ViewGroup, viewType: Int): RecyclerView.ViewHolder {
        val context = parent.context
        val itemView = LayoutInflater.from(context).inflate(R.layout.item_key, parent, false)
        return KeyboardViewHolder(itemView)
    }

    override fun getItemCount(): Int {
        return keys.size
    }

    override fun onBindViewHolder(holder: RecyclerView.ViewHolder, position: Int) {
        val itemView = holder.itemView
        itemView.findViewById<TextView>(R.id.item_tv_key).text = keys[position].toString()

        itemView.setOnTouchListener { v, event ->
            val action = event.action
            when (action) {
                MotionEvent.ACTION_DOWN -> {
                    v.isPressed = true
                    onKeyEvent(position, true)
                }
                MotionEvent.ACTION_UP -> {
                    v.isPressed = false
                    onKeyEvent(position, false)
                }
            }
            true
        }
    }

}