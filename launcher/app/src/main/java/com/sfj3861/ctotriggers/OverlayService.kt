package com.sfj3861.ctotriggers

import android.app.Service
import android.content.Intent
import android.graphics.PixelFormat
import android.graphics.Typeface
import android.os.IBinder
import android.view.Gravity
import android.view.MotionEvent
import android.view.WindowManager
import android.widget.TextView
import android.widget.Toast

class OverlayService : Service() {
    private var windowManager: WindowManager? = null
    private var bubble: TextView? = null
    private var params: WindowManager.LayoutParams? = null

    override fun onCreate() {
        super.onCreate()
        if (!android.provider.Settings.canDrawOverlays(this)) {
            stopSelf()
            return
        }

        windowManager = getSystemService(WINDOW_SERVICE) as WindowManager
        val type = WindowManager.LayoutParams.TYPE_APPLICATION_OVERLAY
        params = WindowManager.LayoutParams(
            WindowManager.LayoutParams.WRAP_CONTENT,
            WindowManager.LayoutParams.WRAP_CONTENT,
            type,
            WindowManager.LayoutParams.FLAG_NOT_FOCUSABLE,
            PixelFormat.TRANSLUCENT
        ).apply {
            gravity = Gravity.TOP or Gravity.START
            x = 24
            y = 220
        }

        bubble = TextView(this).apply {
            text = "C→T"
            textSize = 17f
            typeface = Typeface.DEFAULT_BOLD
            gravity = Gravity.CENTER
            setTextColor(0xFFFFFFFF.toInt())
            setBackgroundColor(0xFF5B36B5.toInt())
            setPadding(18, 14, 18, 14)
            setOnTouchListener { _, event ->
                val p = params ?: return@setOnTouchListener false
                when (event.action) {
                    MotionEvent.ACTION_DOWN -> {
                        setTag(R.id.overlay_touch_x, event.rawX)
                        setTag(R.id.overlay_touch_y, event.rawY)
                        setTag(R.id.overlay_start_x, p.x)
                        setTag(R.id.overlay_start_y, p.y)
                        true
                    }
                    MotionEvent.ACTION_MOVE -> {
                        val downX = (getTag(R.id.overlay_touch_x) as? Float) ?: event.rawX
                        val downY = (getTag(R.id.overlay_touch_y) as? Float) ?: event.rawY
                        val startX = (getTag(R.id.overlay_start_x) as? Int) ?: p.x
                        val startY = (getTag(R.id.overlay_start_y) as? Int) ?: p.y
                        p.x = startX + (event.rawX - downX).toInt()
                        p.y = startY + (event.rawY - downY).toInt()
                        windowManager?.updateViewLayout(this, p)
                        true
                    }
                    MotionEvent.ACTION_UP -> {
                        val downX = (getTag(R.id.overlay_touch_x) as? Float) ?: event.rawX
                        val downY = (getTag(R.id.overlay_touch_y) as? Float) ?: event.rawY
                        if (kotlin.math.abs(event.rawX - downX) < 12 && kotlin.math.abs(event.rawY - downY) < 12) {
                            Toast.makeText(this@OverlayService, "C to Triggers: panel de inyección aún no implementado.", Toast.LENGTH_LONG).show()
                        }
                        true
                    }
                    else -> false
                }
            }
        }
        try {
            windowManager?.addView(bubble, params)
        } catch (_: Exception) {
            stopSelf()
        }
    }

    override fun onDestroy() {
        try {
            bubble?.let { windowManager?.removeView(it) }
        } catch (_: Exception) {
        }
        bubble = null
        windowManager = null
        super.onDestroy()
    }

    override fun onBind(intent: Intent?): IBinder? = null
}
