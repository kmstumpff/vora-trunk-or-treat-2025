package com.example.voratrunkortreat2025

import android.Manifest
import android.bluetooth.BluetoothAdapter
import android.bluetooth.BluetoothDevice
import android.bluetooth.BluetoothSocket
import android.content.pm.PackageManager
import android.os.Bundle
import android.widget.Toast
import androidx.appcompat.app.AlertDialog
import androidx.appcompat.app.AppCompatActivity
import androidx.core.app.ActivityCompat
import com.google.android.material.switchmaterial.SwitchMaterial
import java.io.IOException
import java.io.OutputStream
import java.util.UUID

class MainActivity : AppCompatActivity() {
    private lateinit var toggleButton: SwitchMaterial
    private var btAdapter: BluetoothAdapter? = null
    private var btSocket: BluetoothSocket? = null
    private var outputStream: OutputStream? = null

    // Standard UUID for SPP (Serial Port Profile)
    private val SPP_UUID: UUID = UUID.fromString("00001101-0000-1000-8000-00805F9B34FB")

    // Replace with your Bluetooth device's MAC address
    private val DEVICE_ADDRESS = "24:6F:28:AD:D4:CA"

    companion object {
        private const val REQUEST_BLUETOOTH_PERMISSIONS = 1
    }

    override fun onCreate(savedInstanceState: Bundle?) {
        super.onCreate(savedInstanceState)
        setContentView(R.layout.activity_main)

        toggleButton = findViewById(R.id.toggleButton)
        btAdapter = BluetoothAdapter.getDefaultAdapter()

        if (btAdapter == null) {
            Toast.makeText(this, "Bluetooth not supported", Toast.LENGTH_LONG).show()
            finish()
            return
        }

        // Check and request permissions
        checkBluetoothPermissions()

        toggleButton.setOnCheckedChangeListener { _, isChecked ->
            if (isChecked) {
                sendBluetoothData("on\n")
            } else {
                sendBluetoothData("off\n")
            }
        }

        // Connect to Bluetooth device on start
        connectToDevice()
    }

    private fun checkBluetoothPermissions() {
        val permissions = if (android.os.Build.VERSION.SDK_INT >= android.os.Build.VERSION_CODES.S) {
            arrayOf(
                Manifest.permission.BLUETOOTH_CONNECT,
                Manifest.permission.BLUETOOTH_SCAN
            )
        } else {
            arrayOf(
                Manifest.permission.BLUETOOTH,
                Manifest.permission.BLUETOOTH_ADMIN
            )
        }

        val missingPermissions = permissions.filter {
            ActivityCompat.checkSelfPermission(this, it) != PackageManager.PERMISSION_GRANTED
        }

        if (missingPermissions.isNotEmpty()) {
            ActivityCompat.requestPermissions(
                this,
                missingPermissions.toTypedArray(),
                REQUEST_BLUETOOTH_PERMISSIONS
            )
        }
    }

    private fun connectToDevice() {
        if (ActivityCompat.checkSelfPermission(this, Manifest.permission.BLUETOOTH_CONNECT)
            != PackageManager.PERMISSION_GRANTED &&
            android.os.Build.VERSION.SDK_INT >= android.os.Build.VERSION_CODES.S) {
            Toast.makeText(this, "Bluetooth permission not granted", Toast.LENGTH_SHORT).show()
            return
        }

        Thread {
            try {
                val device: BluetoothDevice = btAdapter?.getRemoteDevice(DEVICE_ADDRESS)
                    ?: throw IOException("Device not found")

                btSocket = device.createRfcommSocketToServiceRecord(SPP_UUID)
                btAdapter?.cancelDiscovery()
                btSocket?.connect()
                outputStream = btSocket?.outputStream

                runOnUiThread {
                    Toast.makeText(this, "Connected to device", Toast.LENGTH_SHORT).show()
                    toggleButton.isEnabled = true
                }
            } catch (e: IOException) {
                runOnUiThread {
                    Toast.makeText(this, "Connection failed: ${e.message}", Toast.LENGTH_LONG).show()
                    toggleButton.isEnabled = false
                }
            }
        }.start()
    }

    private fun sendBluetoothData(data: String) {
        Thread {
            try {
                outputStream?.write(data.toByteArray())
                outputStream?.flush()
                runOnUiThread {
                    Toast.makeText(this, "Sent: $data", Toast.LENGTH_SHORT).show()
                }
            } catch (e: IOException) {
                runOnUiThread {
                    Toast.makeText(this, "Send failed: ${e.message}", Toast.LENGTH_SHORT).show()
                }
            }
        }.start()
    }

    override fun onDestroy() {
        super.onDestroy()
        try {
            outputStream?.close()
            btSocket?.close()
        } catch (e: IOException) {
            e.printStackTrace()
        }
    }

    override fun onRequestPermissionsResult(
        requestCode: Int,
        permissions: Array<out String>,
        grantResults: IntArray
    ) {
        super.onRequestPermissionsResult(requestCode, permissions, grantResults)
        if (requestCode == REQUEST_BLUETOOTH_PERMISSIONS) {
            if (grantResults.all { it == PackageManager.PERMISSION_GRANTED }) {
                connectToDevice()
            } else {
                Toast.makeText(this, "Bluetooth permissions required", Toast.LENGTH_LONG).show()
            }
        }
    }
}