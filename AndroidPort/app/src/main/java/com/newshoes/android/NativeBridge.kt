package com.newshoes.android

import android.view.Surface

object NativeBridge {
    init {
        System.loadLibrary("cnc_android")
    }

    external fun onSurfaceCreated(surface: Surface)
    external fun onSurfaceChanged(width: Int, height: Int)
    external fun onSurfaceDestroyed()
    external fun onTouch(action: Int, x: Float, y: Float)
    external fun onHostResume()
    external fun onHostPause()
    external fun shutdown()
}
