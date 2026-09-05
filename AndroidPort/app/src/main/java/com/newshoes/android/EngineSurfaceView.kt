package com.newshoes.android

import android.content.Context
import android.graphics.Color
import android.view.MotionEvent
import android.view.SurfaceHolder
import android.view.SurfaceView
import java.util.concurrent.atomic.AtomicBoolean

class EngineSurfaceView(context: Context) : SurfaceView(context), SurfaceHolder.Callback {
    private val ready = AtomicBoolean(false)

    init {
        holder.addCallback(this)
        setBackgroundColor(Color.BLACK)
        isFocusable = true
    }

    override fun surfaceCreated(holder: SurfaceHolder) {
        NativeBridge.onSurfaceCreated(holder.surface)
        ready.set(true)
    }

    override fun surfaceChanged(holder: SurfaceHolder, format: Int, width: Int, height: Int) {
        if (ready.get()) NativeBridge.onSurfaceChanged(width, height)
    }

    override fun surfaceDestroyed(holder: SurfaceHolder) {
        ready.set(false)
        NativeBridge.onSurfaceDestroyed()
    }

    override fun onTouchEvent(event: MotionEvent): Boolean {
        if (!ready.get()) return true
        when (event.actionMasked) {
            MotionEvent.ACTION_DOWN -> NativeBridge.onTouch(0, event.x, event.y)
            MotionEvent.ACTION_UP -> NativeBridge.onTouch(1, event.x, event.y)
            MotionEvent.ACTION_MOVE -> NativeBridge.onTouch(2, event.x, event.y)
            MotionEvent.ACTION_CANCEL -> NativeBridge.onTouch(3, event.x, event.y)
        }
        return true
    }

    fun onHostResume() = NativeBridge.onHostResume()
    fun onHostPause() = NativeBridge.onHostPause()
    fun shutdown() = NativeBridge.shutdown()
}
