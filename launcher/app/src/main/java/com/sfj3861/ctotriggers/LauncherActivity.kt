package com.sfj3861.ctotriggers

import android.app.Activity
import android.content.Intent
import android.net.Uri
import android.os.Bundle
import android.provider.Settings
import android.view.Gravity
import android.view.ViewGroup
import android.widget.Button
import android.widget.LinearLayout
import android.widget.TextView
import android.widget.Toast

class LauncherActivity : Activity() {
    private val gamePackage = "com.robtopx.geometryjump"

    override fun onCreate(savedInstanceState: Bundle?) {
        super.onCreate(savedInstanceState)

        val root = LinearLayout(this).apply {
            orientation = LinearLayout.VERTICAL
            gravity = Gravity.CENTER
            setPadding(28, 32, 28, 32)
        }

        val title = TextView(this).apply {
            text = "GPTM • GPT Mod Menu"
            textSize = 26f
            gravity = Gravity.CENTER
        }
        val subtitle = TextView(this).apply {
            text = "C to Triggers + GPTM para Geometry Dash 2.2090\n\nEl botón flotante abre el prototipo del menú. Sus opciones todavía no modifican el juego: falta implementar y probar la integración nativa."
            textSize = 16f
            gravity = Gravity.CENTER
            setPadding(0, 16, 0, 24)
        }
        val launch = Button(this).apply {
            text = "ABRIR GEOMETRY DASH"
            setOnClickListener { launchGame() }
        }
        val enableOverlay = Button(this).apply {
            text = "ACTIVAR BOTÓN GPTM"
            setOnClickListener { enableFloatingButton() }
        }
        val stopOverlay = Button(this).apply {
            text = "DESACTIVAR BOTÓN GPTM"
            setOnClickListener {
                stopService(Intent(this@LauncherActivity, OverlayService::class.java))
                Toast.makeText(this@LauncherActivity, "GPTM desactivado", Toast.LENGTH_SHORT).show()
            }
        }

        root.addView(title, LinearLayout.LayoutParams(ViewGroup.LayoutParams.MATCH_PARENT, ViewGroup.LayoutParams.WRAP_CONTENT))
        root.addView(subtitle, LinearLayout.LayoutParams(ViewGroup.LayoutParams.MATCH_PARENT, ViewGroup.LayoutParams.WRAP_CONTENT))
        root.addView(launch, LinearLayout.LayoutParams(ViewGroup.LayoutParams.MATCH_PARENT, ViewGroup.LayoutParams.WRAP_CONTENT))
        root.addView(enableOverlay, LinearLayout.LayoutParams(ViewGroup.LayoutParams.MATCH_PARENT, ViewGroup.LayoutParams.WRAP_CONTENT))
        root.addView(stopOverlay, LinearLayout.LayoutParams(ViewGroup.LayoutParams.MATCH_PARENT, ViewGroup.LayoutParams.WRAP_CONTENT))
        setContentView(root)
    }

    private fun enableFloatingButton() {
        if (!Settings.canDrawOverlays(this)) {
            val intent = Intent(
                Settings.ACTION_MANAGE_OVERLAY_PERMISSION,
                Uri.parse("package:$packageName")
            )
            startActivity(intent)
            Toast.makeText(this, "Activa 'Mostrar sobre otras apps' y vuelve al launcher.", Toast.LENGTH_LONG).show()
            return
        }
        startService(Intent(this, OverlayService::class.java))
        Toast.makeText(this, "GPTM activado", Toast.LENGTH_SHORT).show()
    }

    private fun launchGame() {
        val launchIntent = packageManager.getLaunchIntentForPackage(gamePackage)
        if (launchIntent == null) {
            Toast.makeText(
                this,
                "No encuentro Geometry Dash con el paquete esperado. Puede que esta versión use otro identificador.",
                Toast.LENGTH_LONG
            ).show()
            return
        }
        launchIntent.addFlags(Intent.FLAG_ACTIVITY_NEW_TASK)
        startActivity(launchIntent)
    }
}
