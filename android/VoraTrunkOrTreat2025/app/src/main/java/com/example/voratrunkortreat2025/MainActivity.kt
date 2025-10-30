package com.example.voratrunkortreat2025

import android.Manifest
import android.bluetooth.*
import android.bluetooth.le.ScanCallback
import android.bluetooth.le.ScanResult
import android.content.pm.PackageManager
import android.os.Bundle
import android.widget.Button
import android.widget.TextView
import android.widget.Toast
import androidx.appcompat.app.AlertDialog
import androidx.appcompat.app.AppCompatActivity
import androidx.core.app.ActivityCompat
import com.google.android.material.switchmaterial.SwitchMaterial
import java.util.UUID

class MainActivity : AppCompatActivity() {
    private lateinit var toggleButton: SwitchMaterial
    private lateinit var selectDeviceButton: Button
    private lateinit var statusText: TextView
    private var btAdapter: BluetoothAdapter? = null
    private var bluetoothGatt: BluetoothGatt? = null
    private var commandCharacteristic: BluetoothGattCharacteristic? = null

    // Store discovered devices
    private val discoveredDevices = mutableListOf<BluetoothDevice>()
    private var isScanning = false

    // BLE UUIDs
    private val SERVICE_UUID: UUID = UUID.fromString("dd3a359d-a0fb-49c2-9ba1-aae162aa2bdc")
    private val COMMAND_CHAR_UUID: UUID = UUID.fromString("b1d3e840-56fb-41d8-9791-bd8566220d03")
    private val STATUS_CHAR_UUID: UUID = UUID.fromString("7c6479dd-f6c6-4f68-8f1f-0cb38a06b1d3")

    // Commands
    private val CMD_OPEN: Byte = 0x01
    private val CMD_CLOSE: Byte = 0x00

    companion object {
        private const val REQUEST_BLE_PERMISSIONS = 1
    }

    override fun onCreate(savedInstanceState: Bundle?) {
        super.onCreate(savedInstanceState)
        setContentView(R.layout.activity_main)

        toggleButton = findViewById(R.id.toggleButton)
        selectDeviceButton = findViewById(R.id.selectDeviceButton)
        statusText = findViewById(R.id.statusText)
        toggleButton.isEnabled = false
        updateStatusText(false)

        val btManager = getSystemService(BLUETOOTH_SERVICE) as BluetoothManager
        btAdapter = btManager.adapter

        if (btAdapter == null) {
            Toast.makeText(this, "Bluetooth not supported", Toast.LENGTH_LONG).show()
            finish()
            return
        }

        checkBlePermissions()

        selectDeviceButton.setOnClickListener {
            discoveredDevices.clear()
            startBleScan()
        }

        toggleButton.setOnCheckedChangeListener { _, isChecked ->
            if (isChecked) {
                sendBleCommand(CMD_OPEN)
            } else {
                sendBleCommand(CMD_CLOSE)
            }
        }
    }

    private fun updateStatusText(connected: Boolean) {
        // val status = if (connected == true) "Connected" else "Disconnected"
        // statusText.setText("Status: $status")
        if (connected) {
            statusText.text = "Status: Connected"
        } else {
            statusText.text = "Status: Disconnected"
        }
    }

    private fun checkBlePermissions() {
        val permissions = if (android.os.Build.VERSION.SDK_INT >= android.os.Build.VERSION_CODES.S) {
            arrayOf(
                Manifest.permission.BLUETOOTH_CONNECT,
                Manifest.permission.BLUETOOTH_SCAN,
                Manifest.permission.ACCESS_FINE_LOCATION
            )
        } else {
            arrayOf(
                Manifest.permission.BLUETOOTH,
                Manifest.permission.BLUETOOTH_ADMIN,
                Manifest.permission.ACCESS_FINE_LOCATION
            )
        }

        val missingPermissions = permissions.filter {
            ActivityCompat.checkSelfPermission(this, it) != PackageManager.PERMISSION_GRANTED
        }

        if (missingPermissions.isEmpty()) {
            selectDeviceButton.isEnabled = true
        } else {
            ActivityCompat.requestPermissions(
                this,
                missingPermissions.toTypedArray(),
                REQUEST_BLE_PERMISSIONS
            )
        }
    }

    private fun startBleScan() {
        if (ActivityCompat.checkSelfPermission(this, Manifest.permission.BLUETOOTH_SCAN)
            != PackageManager.PERMISSION_GRANTED &&
            android.os.Build.VERSION.SDK_INT >= android.os.Build.VERSION_CODES.S) {
            return
        }

        if (isScanning) {
            Toast.makeText(this, "Already scanning...", Toast.LENGTH_SHORT).show()
            return
        }

        // Disconnect from current device if connected
        bluetoothGatt?.close()
        updateStatusText(false)
        bluetoothGatt = null
        commandCharacteristic = null
        toggleButton.isEnabled = false

        isScanning = true
        discoveredDevices.clear()
        Toast.makeText(this, "Scanning for devices...", Toast.LENGTH_SHORT).show()
        selectDeviceButton.text = "Scanning..."
        selectDeviceButton.isEnabled = false

        val bleScanner = btAdapter?.bluetoothLeScanner
        bleScanner?.startScan(scanCallback)

        // Stop scan after 5 seconds and show results
        android.os.Handler(mainLooper).postDelayed({
            if (ActivityCompat.checkSelfPermission(this, Manifest.permission.BLUETOOTH_SCAN)
                == PackageManager.PERMISSION_GRANTED ||
                android.os.Build.VERSION.SDK_INT < android.os.Build.VERSION_CODES.S) {
                bleScanner?.stopScan(scanCallback)
                isScanning = false
                selectDeviceButton.text = "Select Device"
                selectDeviceButton.isEnabled = true
                showDeviceSelectionDialog()
            }
        }, 5000)
    }

    private val scanCallback = object : ScanCallback() {
        override fun onScanResult(callbackType: Int, result: ScanResult?) {
            result?.device?.let { device ->
                // Check if device has our service UUID
                if (result.scanRecord?.serviceUuids?.any {
                        it.uuid == SERVICE_UUID
                    } == true) {
                    // Avoid duplicates
                    if (!discoveredDevices.any { it.address == device.address }) {
                        discoveredDevices.add(device)
                        runOnUiThread {
                            Toast.makeText(this@MainActivity,
                                "Found device: ${getDeviceName(device)}",
                                Toast.LENGTH_SHORT).show()
                        }
                    }
                }
            }
        }

        override fun onScanFailed(errorCode: Int) {
            isScanning = false
            runOnUiThread {
                selectDeviceButton.text = "Select Device"
                selectDeviceButton.isEnabled = true
                Toast.makeText(this@MainActivity, "Scan failed: $errorCode", Toast.LENGTH_LONG).show()
            }
        }
    }

    private fun getDeviceName(device: BluetoothDevice): String {
        if (ActivityCompat.checkSelfPermission(this, Manifest.permission.BLUETOOTH_CONNECT)
            != PackageManager.PERMISSION_GRANTED &&
            android.os.Build.VERSION.SDK_INT >= android.os.Build.VERSION_CODES.S) {
            return device.address
        }
        return device.name ?: device.address
    }

    private fun showDeviceSelectionDialog() {
        if (discoveredDevices.isEmpty()) {
            AlertDialog.Builder(this)
                .setTitle("No Devices Found")
                .setMessage("No BLE devices with the service UUID were found. Make sure your device is powered on and advertising.")
                .setPositiveButton("OK", null)
                .setNeutralButton("Scan Again") { _, _ -> startBleScan() }
                .show()
            return
        }

        val deviceNames = discoveredDevices.map { device ->
            getDeviceName(device)
        }.toTypedArray()

        AlertDialog.Builder(this)
            .setTitle("Select Device (${discoveredDevices.size} found)")
            .setItems(deviceNames) { _, which ->
                val selectedDevice = discoveredDevices[which]
                connectToDevice(selectedDevice)
            }
            .setNegativeButton("Cancel", null)
            .setNeutralButton("Scan Again") { _, _ -> startBleScan() }
            .show()
    }

    private fun connectToDevice(device: BluetoothDevice) {
        if (ActivityCompat.checkSelfPermission(this, Manifest.permission.BLUETOOTH_CONNECT)
            != PackageManager.PERMISSION_GRANTED &&
            android.os.Build.VERSION.SDK_INT >= android.os.Build.VERSION_CODES.S) {
            return
        }

        runOnUiThread {
            Toast.makeText(this, "Connecting to ${getDeviceName(device)}...", Toast.LENGTH_SHORT).show()
        }

        bluetoothGatt = device.connectGatt(this, false, gattCallback, BluetoothDevice.TRANSPORT_LE)
    }

    private val gattCallback = object : BluetoothGattCallback() {
        override fun onConnectionStateChange(gatt: BluetoothGatt?, status: Int, newState: Int) {
            when (newState) {
                BluetoothProfile.STATE_CONNECTED -> {
                    runOnUiThread {
                        updateStatusText(true)
                        Toast.makeText(this@MainActivity, "Connected! Discovering services...", Toast.LENGTH_SHORT).show()
                    }
                    if (ActivityCompat.checkSelfPermission(this@MainActivity,
                            Manifest.permission.BLUETOOTH_CONNECT) == PackageManager.PERMISSION_GRANTED ||
                        android.os.Build.VERSION.SDK_INT < android.os.Build.VERSION_CODES.S) {
                        gatt?.discoverServices()
                    }
                }
                BluetoothProfile.STATE_DISCONNECTED -> {
                    val statusMsg = when (status) {
                        BluetoothGatt.GATT_SUCCESS -> "Disconnected normally"
                        133 -> "Connection failed (GATT error 133) - Try again"
                        8 -> "Connection timeout"
                        19 -> "Device disconnected"
                        22 -> "Device not found or not responding"
                        else -> "Disconnected (status: $status)"
                    }
                    runOnUiThread {
                        updateStatusText(false)
                        Toast.makeText(this@MainActivity, statusMsg, Toast.LENGTH_LONG).show()
                        toggleButton.isEnabled = false
                    }
                }
            }
        }

        override fun onServicesDiscovered(gatt: BluetoothGatt?, status: Int) {
            if (status == BluetoothGatt.GATT_SUCCESS) {
                val service = gatt?.getService(SERVICE_UUID)
                commandCharacteristic = service?.getCharacteristic(COMMAND_CHAR_UUID)

                if (commandCharacteristic != null) {
                    runOnUiThread {
                        Toast.makeText(this@MainActivity, "Ready!", Toast.LENGTH_SHORT).show()
                        toggleButton.isEnabled = true
                    }
                } else {
                    runOnUiThread {
                        Toast.makeText(this@MainActivity, "Command characteristic not found", Toast.LENGTH_LONG).show()
                    }
                }
            }
        }

        override fun onCharacteristicWrite(
            gatt: BluetoothGatt?,
            characteristic: BluetoothGattCharacteristic?,
            status: Int
        ) {
            if (status == BluetoothGatt.GATT_SUCCESS) {
                runOnUiThread {
                    val cmd = characteristic?.value?.get(0)
                    val cmdName = if (cmd == CMD_OPEN) "OPEN" else "CLOSE"
                    Toast.makeText(this@MainActivity, "Sent: $cmdName", Toast.LENGTH_SHORT).show()
                }
            } else {
                runOnUiThread {
                    Toast.makeText(this@MainActivity, "Write failed: $status", Toast.LENGTH_SHORT).show()
                }
            }
        }
    }

    private fun sendBleCommand(command: Byte) {
        commandCharacteristic?.let { char ->
            char.value = byteArrayOf(command)
            if (ActivityCompat.checkSelfPermission(this, Manifest.permission.BLUETOOTH_CONNECT)
                != PackageManager.PERMISSION_GRANTED &&
                android.os.Build.VERSION.SDK_INT >= android.os.Build.VERSION_CODES.S) {
                return
            }
            bluetoothGatt?.writeCharacteristic(char)
        }
    }

    override fun onDestroy() {
        super.onDestroy()
        if (ActivityCompat.checkSelfPermission(this, Manifest.permission.BLUETOOTH_CONNECT)
            == PackageManager.PERMISSION_GRANTED ||
            android.os.Build.VERSION.SDK_INT < android.os.Build.VERSION_CODES.S) {
            bluetoothGatt?.close()
        }
    }

    override fun onRequestPermissionsResult(
        requestCode: Int,
        permissions: Array<out String>,
        grantResults: IntArray
    ) {
        super.onRequestPermissionsResult(requestCode, permissions, grantResults)
        if (requestCode == REQUEST_BLE_PERMISSIONS) {
            if (grantResults.all { it == PackageManager.PERMISSION_GRANTED }) {
                selectDeviceButton.isEnabled = true
            } else {
                Toast.makeText(this, "BLE permissions required", Toast.LENGTH_LONG).show()
            }
        }
    }
}