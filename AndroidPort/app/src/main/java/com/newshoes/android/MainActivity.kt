package com.newshoes.android

import android.app.Activity
import android.os.Bundle
import android.view.Window
import android.view.WindowManager

class MainActivity : Activity() {
    private lateinit var surface: EngineSurfaceView

    override fun onCreate(savedInstanceState: Bundle?) {
        super.onCreate(savedInstanceState)
        requestWindowFeature(Window.FEATURE_NO_TITLE)
        window.setFlags(
            WindowManager.LayoutParams.FLAG_FULLSCREEN,
            WindowManager.LayoutParams.FLAG_FULLSCREEN
        )
        surface = EngineSurfaceView(this)
        setContentView(surface)
    }

    override fun onResume() {
        super.onResume()
        if (::surface.isInitialized) surface.onHostResume()
    }

    override fun onPause() {
        if (::surface.isInitialized) surface.onHostPause()
        super.onPause()
    }

    override fun onDestroy() {
        if (::surface.isInitialized) surface.shutdown()
        super.onDestroy()
    }
}
