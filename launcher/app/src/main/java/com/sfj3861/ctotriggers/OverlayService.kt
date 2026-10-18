package com.sfj3861.ctotriggers

import android.app.Service
import android.content.Intent
import android.graphics.Color
import android.graphics.PixelFormat
import android.graphics.Typeface
import android.os.IBinder
import android.view.Gravity
import android.view.MotionEvent
import android.view.View
import android.view.WindowManager
import android.widget.Button
import android.widget.LinearLayout
import android.widget.ScrollView
import android.widget.Switch
import android.widget.TextView
import android.widget.Toast
import kotlin.math.abs

/**
 * GPTM = GPT Mod Menu.
 *
 * This is currently an interface prototype only. Switches demonstrate the planned
 * settings UI; they do not hook or modify Geometry Dash's native process.
 */
class OverlayService : Service() {
    private var windowManager: WindowManager? = null
    private var bubble: TextView? = null
    private var bubbleParams: WindowManager.LayoutParams? = null
    private var panel: View? = null
    private var panelParams: WindowManager.LayoutParams? = null

    override fun onCreate() {
        super.onCreate()
        if (!android.provider.Settings.canDrawOverlays(this)) {
            stopSelf()
            return
        }

        windowManager = getSystemService(WINDOW_SERVICE) as WindowManager
        createBubble()
    }

    private fun createBubble() {
        bubbleParams = WindowManager.LayoutParams(
            WindowManager.LayoutParams.WRAP_CONTENT,
            WindowManager.LayoutParams.WRAP_CONTENT,
            WindowManager.LayoutParams.TYPE_APPLICATION_OVERLAY,
            WindowManager.LayoutParams.FLAG_NOT_FOCUSABLE,
            PixelFormat.TRANSLUCENT
        ).apply {
            gravity = Gravity.TOP or Gravity.START
            x = dp(18)
            y = dp(220)
        }

        bubble = TextView(this).apply {
            text = "GPTM"
            textSize = 15f
            typeface = Typeface.DEFAULT_BOLD
            gravity = Gravity.CENTER
            setTextColor(Color.WHITE)
            setBackgroundColor(Color.rgb(84, 55, 165))
            setPadding(dp(14), dp(12), dp(14), dp(12))
            var downX = 0f
            var downY = 0f
            var startX = 0
            var startY = 0
            setOnTouchListener { view, event ->
                val p = bubbleParams ?: return@setOnTouchListener false
                when (event.action) {
                    MotionEvent.ACTION_DOWN -> {
                        downX = event.rawX
                        downY = event.rawY
                        startX = p.x
                        startY = p.y
                        true
                    }
                    MotionEvent.ACTION_MOVE -> {
                        p.x = startX + (event.rawX - downX).toInt()
                        p.y = startY + (event.rawY - downY).toInt()
                        windowManager?.updateViewLayout(view, p)
                        true
                    }
                    MotionEvent.ACTION_UP -> {
                        if (abs(event.rawX - downX) < dp(10) && abs(event.rawY - downY) < dp(10)) {
                            togglePanel()
                        }
                        true
                    }
                    else -> false
                }
            }
        }

        try {
            windowManager?.addView(bubble, bubbleParams)
        } catch (_: Exception) {
            stopSelf()
        }
    }

    private fun togglePanel() {
        if (panel != null) {
            removePanel()
        } else {
            showPanel()
        }
    }

    private fun showPanel() {
        if (panel != null || windowManager == null) return

        val root = LinearLayout(this).apply {
            orientation = LinearLayout.VERTICAL
            setBackgroundColor(Color.rgb(25, 25, 35))
            setPadding(dp(12), dp(10), dp(12), dp(10))
        }

        val heading = LinearLayout(this).apply {
            orientation = LinearLayout.HORIZONTAL
            gravity = Gravity.CENTER_VERTICAL
        }
        val title = TextView(this).apply {
            text = "GPTM  •  GPT Mod Menu"
            textSize = 18f
            typeface = Typeface.DEFAULT_BOLD
            setTextColor(Color.WHITE)
        }
        val close = Button(this).apply {
            text = "✕"
            setOnClickListener { removePanel() }
        }
        heading.addView(title, LinearLayout.LayoutParams(0, dp(48), 1f))
        heading.addView(close, LinearLayout.LayoutParams(dp(48), dp(44)))
        root.addView(heading)

        val notice = TextView(this).apply {
            text = "PROTOTIPO DE INTERFAZ\nLos ajustes aún no están conectados al motor del juego."
            textSize = 12f
            setTextColor(Color.rgb(255, 211, 120))
            setPadding(0, dp(4), 0, dp(10))
        }
        root.addView(notice)

        val scroll = ScrollView(this)
        val list = LinearLayout(this).apply { orientation = LinearLayout.VERTICAL }
        scroll.addView(list)
        addSection(list, "PLAYER / PRACTICE")
        addToggle(list, "Practice checkpoints", "Herramientas de práctica")
        addToggle(list, "Start Position helper", "Gestión de start positions")
        addToggle(list, "Auto-retry", "Reintento automático")
        addToggle(list, "Practice music sync", "Sincronización de música en práctica")
        addToggle(list, "Noclip (testing)", "Prueba de colisiones")
        addToggle(list, "Ignore damage", "Prueba de daño")
        addToggle(list, "Speedhack", "Control de velocidad")
        addToggle(list, "TPS/FPS control", "Control de tasa de simulación")
        addToggle(list, "Show trajectory", "Predicción visual de trayectoria")
        addToggle(list, "Input display", "Visualización de pulsaciones")
        addSection(list, "VISUALS / HUD")
        addToggle(list, "FPS counter", "Contador de FPS")
        addToggle(list, "Show hitboxes", "Visualización de hitboxes")
        addToggle(list, "Attempt counter", "Contador de intentos")
        addToggle(list, "Disable screen shake", "Desactivar sacudida de pantalla")
        addToggle(list, "Disable pulses", "Desactivar pulsos visuales")
        addToggle(list, "Disable particles", "Reducir partículas")
        addToggle(list, "Low Detail helper", "Ayuda para modo de bajo detalle")
        addToggle(list, "Show level progress", "Progreso del nivel")
        addToggle(list, "Click indicator", "Indicador de clics")
        addToggle(list, "Show level layout", "Visualización de layout")
        addSection(list, "EDITOR / BUILD")
        addToggle(list, "C→T translator", "Traductor de comandos a plan de triggers")
        addToggle(list, "Object ID inspector", "Inspector de objetos")
        addToggle(list, "Group ID helper", "Ayuda para Group IDs")
        addToggle(list, "Copy / paste objects", "Herramientas de copia de objetos")
        addToggle(list, "Free move helper", "Ayuda de movimiento libre")
        addToggle(list, "Rotate step helper", "Ajustes de rotación")
        addToggle(list, "Trigger plan preview", "Previsualización del plan de triggers")
        addToggle(list, "Object count", "Recuento de objetos")
        addToggle(list, "Layout helper", "Ayudas para layouts")
        addSection(list, "LEVEL / QOL")
        addToggle(list, "Copy level ID", "Copiar identificador de nivel")
        addToggle(list, "Copy level stats", "Herramientas de estadísticas")
        addToggle(list, "Song info", "Información de canción")
        addToggle(list, "Quick restart", "Reinicio rápido")
        addToggle(list, "Hide menu in gameplay", "Ocultar menú durante la partida")
        addToggle(list, "Remember settings", "Guardar preferencias")
        addSection(list, "DIAGNOSTICS")
        addToggle(list, "Compatibility report", "Informe de compatibilidad")
        addToggle(list, "Native hook status", "Estado de integración nativa")
        addToggle(list, "Debug logging", "Registro de diagnóstico")

        root.addView(scroll, LinearLayout.LayoutParams(
            WindowManager.LayoutParams.MATCH_PARENT, 0, 1f
        ))

        val footer = TextView(this).apply {
            text = "GPTM • Compatibilidad GD 2.2090 por verificar"
            textSize = 11f
            gravity = Gravity.CENTER
            setTextColor(Color.LTGRAY)
            setPadding(0, dp(8), 0, 0)
        }
        root.addView(footer)

        panelParams = WindowManager.LayoutParams(
            dp(310),
            dp(490),
            WindowManager.LayoutParams.TYPE_APPLICATION_OVERLAY,
            WindowManager.LayoutParams.FLAG_NOT_FOCUSABLE or
                WindowManager.LayoutParams.FLAG_NOT_TOUCH_MODAL,
            PixelFormat.TRANSLUCENT
        ).apply {
            gravity = Gravity.CENTER
        }
        panel = root
        try {
            windowManager?.addView(root, panelParams)
        } catch (_: Exception) {
            panel = null
            panelParams = null
            Toast.makeText(this, "No se pudo mostrar el menú GPTM.", Toast.LENGTH_SHORT).show()
        }
    }

    private fun addSection(parent: LinearLayout, name: String) {
        val section = TextView(this).apply {
            text = name
            textSize = 13f
            typeface = Typeface.DEFAULT_BOLD
            setTextColor(Color.rgb(184, 160, 255))
            setPadding(0, dp(12), 0, dp(4))
        }
        parent.addView(section)
    }

    private fun addToggle(parent: LinearLayout, label: String, description: String) {
        val toggle = Switch(this).apply {
            text = label
            textSize = 14f
            setTextColor(Color.WHITE)
            contentDescription = description
            setPadding(0, dp(5), 0, dp(5))
            setOnCheckedChangeListener { _, _ ->
                Toast.makeText(
                    this@OverlayService,
                    "$label: cambio solo visual; integración del juego pendiente.",
                    Toast.LENGTH_SHORT
                ).show()
            }
        }
        parent.addView(toggle, LinearLayout.LayoutParams(
            WindowManager.LayoutParams.MATCH_PARENT, WindowManager.LayoutParams.WRAP_CONTENT
        ))
    }

    private fun removePanel() {
        try {
            panel?.let { windowManager?.removeView(it) }
        } catch (_: Exception) {
        }
        panel = null
        panelParams = null
    }

    private fun dp(value: Int): Int =
        (value * resources.displayMetrics.density).toInt()

    override fun onDestroy() {
        removePanel()
        try {
            bubble?.let { windowManager?.removeView(it) }
        } catch (_: Exception) {
        }
        bubble = null
        bubbleParams = null
        windowManager = null
        super.onDestroy()
    }

    override fun onBind(intent: Intent?): IBinder? = null
}
