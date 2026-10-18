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
            text = "C to Triggers"
            textSize = 28f
            gravity = Gravity.CENTER
        }
        val subtitle = TextView(this).apply {
            text = "Launcher experimental para Geometry Dash 2.2090\n\nEste prototipo abre el juego y puede mostrar un botón flotante. Todavía no inyecta código ni crea triggers dentro del juego."
            textSize = 16f
            gravity = Gravity.CENTER
            setPadding(0, 16, 0, 24)
        }
        val launch = Button(this).apply {
            text = "ABRIR GEOMETRY DASH"
            setOnClickListener { launchGame() }
        }
        val enableOverlay = Button(this).apply {
            text = "ACTIVAR BOTÓN FLOTANTE"
            setOnClickListener { enableFloatingButton() }
        }
        val stopOverlay = Button(this).apply {
            text = "DESACTIVAR BOTÓN FLOTANTE"
            setOnClickListener {
                stopService(Intent(this@LauncherActivity, OverlayService::class.java))
                Toast.makeText(this@LauncherActivity, "Botón flotante desactivado", Toast.LENGTH_SHORT).show()
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
        Toast.makeText(this, "Botón flotante activado", Toast.LENGTH_SHORT).show()
    }

    private fun launchGame() {
        val launchIntent = packageManager.getLaunchIntentForPackage(gamePackage)
        if (launchIntent == null) {
            Toast.makeText(
                this,
                "No encuentro Geometry Dash instalado con el paquete esperado. Comprueba que tienes la versión oficial instalada.",
                Toast.LENGTH_LONG
            ).show()
            return
        }
        launchIntent.addFlags(Intent.FLAG_ACTIVITY_NEW_TASK)
        startActivity(launchIntent)
    }
}
