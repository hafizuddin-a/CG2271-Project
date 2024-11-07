  #include <Bluepad32.h>
  #include <uni.h>

// REQUIRES bluepad + esp32 library
// https://bluepad32.readthedocs.io/en/latest/plat_arduino/ 

#define RXD2 16
#define TXD2 17
#define LED 2

ControllerPtr myControllers[BP32_MAX_GAMEPADS];

static const char* controller_addr_string = "48:18:8D:08:54:67";

// This callback gets called any time a new gamepad is connected.
// Up to 4 gamepads can be connected at the same time.
void onConnectedController(ControllerPtr ctl) {
    bool foundEmptySlot = false;
    for (int i = 0; i < BP32_MAX_GAMEPADS; i++) {
        if (myControllers[i] == nullptr) {
            Serial.printf("CALLBACK: Controller is connected, index=%d\n", i);

            // Get controller properties
            ControllerProperties properties = ctl->getProperties();
            
            // Format Bluetooth address from byte array to readable format
            char btAddrStr[18];  // "XX:XX:XX:XX:XX:XX" + null terminator
            snprintf(btAddrStr, sizeof(btAddrStr), "%02X:%02X:%02X:%02X:%02X:%02X",
                     properties.btaddr[0], properties.btaddr[1], properties.btaddr[2],
                     properties.btaddr[3], properties.btaddr[4], properties.btaddr[5]);

            // Print the controller details
            Serial.printf("Controller model: %s, VID=0x%04x, PID=0x%04x, BTAddr=%s\n", 
                          ctl->getModelName().c_str(), properties.vendor_id, properties.product_id, btAddrStr);

            myControllers[i] = ctl;
            foundEmptySlot = true;
            break;
        }
    }
    if (!foundEmptySlot) {
        Serial.println("CALLBACK: Controller connected, but could not found empty slot");
    }
}


void onDisconnectedController(ControllerPtr ctl) {
    bool foundController = false;

    for (int i = 0; i < BP32_MAX_GAMEPADS; i++) {
        if (myControllers[i] == ctl) {
            Serial.printf("CALLBACK: Controller disconnected from index=%d\n", i);
            myControllers[i] = nullptr;
            foundController = true;
            break;
        }
    }

    if (!foundController) {
        Serial.println("CALLBACK: Controller disconnected, but not found in myControllers");
    }
}

void dumpGamepad(ControllerPtr ctl) {
    Serial.printf(
        "idx=%d, dpad: 0x%02x, buttons: 0x%04x, axis L: %4d, %4d, axis R: %4d, %4d, brake: %4d, throttle: %4d, "
        "misc: 0x%02x, gyro x:%6d y:%6d z:%6d, accel x:%6d y:%6d z:%6d\n",
        ctl->index(),        // Controller Index
        ctl->dpad(),         // D-pad
        ctl->buttons(),      // bitmask of pressed buttons
        ctl->axisX(),        // (-511 - 512) left X Axis
        ctl->axisY(),        // (-511 - 512) left Y axis
        ctl->axisRX(),       // (-511 - 512) right X axis
        ctl->axisRY(),       // (-511 - 512) right Y axis
        ctl->brake(),        // (0 - 1023): brake button
        ctl->throttle(),     // (0 - 1023): throttle (AKA gas) button
        ctl->miscButtons(),  // bitmask of pressed "misc" buttons
        ctl->gyroX(),        // Gyro X
        ctl->gyroY(),        // Gyro Y
        ctl->gyroZ(),        // Gyro Z
        ctl->accelX(),       // Accelerometer X
        ctl->accelY(),       // Accelerometer Y
        ctl->accelZ()        // Accelerometer Z
    );
}

void sendSerial(ControllerPtr ctl) {
    long dpad = ctl->dpad();
    long buttons = ctl->buttons() ? ctl->buttons() + 10 : 0; // 11 backwards, 18 forwards
    uint8_t dataPacket = dpad + buttons;
    Serial.printf("dpad: 0x%02x, buttons: 0x%04x, packet: %X \n", dpad, buttons, dataPacket);
    Serial2.write(dataPacket);
}

void processGamepad(ControllerPtr ctl) {
    // dumpGamepad(ctl);
    digitalWrite(LED, HIGH); // Indicate Bluetooth connected - Blue light on ESP32
    sendSerial(ctl);
}

void processControllers() {
    for (auto myController : myControllers) {
        if (myController && myController->isConnected() && myController->hasData()) {
            if (myController->isGamepad()) {
                processGamepad(myController);
            } else {
                Serial.println("Unsupported controller");
            }
        }
    }
}

// Arduino setup function. Runs in CPU 1
void setup() {
    Serial.begin(115200);
    Serial.printf("Firmware: %s\n", BP32.firmwareVersion());
    const uint8_t* addr = BP32.localBdAddress();
    Serial.printf("BD Addr: %2X:%2X:%2X:%2X:%2X:%2X\n", addr[0], addr[1], addr[2], addr[3], addr[4], addr[5]);

    // Serial to the KL25Z board
    Serial2.begin(9600, SERIAL_8N1, RXD2, TXD2);
    pinMode(LED, OUTPUT);

    // Setup the Bluepad32 callbacks
    BP32.setup(&onConnectedController, &onDisconnectedController);

    // "forgetBluetoothKeys()" should be called when the user performs
    // a "device factory reset", or similar.
    // Calling "forgetBluetoothKeys" in setup() just as an example.
    // Forgetting Bluetooth keys prevents "paired" gamepads to reconnect.
    // But it might also fix some connection / re-connection issues.
    // BP32.forgetBluetoothKeys();

    // Enables mouse / touchpad support for gamepads that support them.
    // When enabled, controllers like DualSense and DualShock4 generate two connected devices:
    // - First one: the gamepad
    // - Second one, which is a "virtual device", is a mouse.
    // By default, it is disabled.
    BP32.enableVirtualDevice(false);

    // Allowlist configuration to only allow a specific controller
    static const char* controller_addr_string = "48:18:8D:08:54:67"; // Replace with the Bluetooth address of your controller
    bd_addr_t controller_addr;
    sscanf_bd_addr(controller_addr_string, controller_addr);

    // Add the controller's Bluetooth address to the allowlist
    uni_bt_allowlist_add_addr(controller_addr);

    // Enable the allowlist
    uni_bt_allowlist_set_enabled(true);

    Serial.println("Allowlist enabled for specific controller.");
}

// Arduino loop function. Runs in CPU 1.
void loop() {
    // This call fetches all the controllers' data.
    // Call this function in your main loop.
    bool dataUpdated = BP32.update();
    if (dataUpdated)
        processControllers(); // In here, Serial2 packet sent

    // The main loop must have some kind of "yield to lower priority task" event.
    // Otherwise, the watchdog will get triggered.
    // If your main loop doesn't have one, just add a simple `vTaskDelay(1)`.
    // Detailed info here:
    // https://stackoverflow.com/questions/66278271/task-watchdog-got-triggered-the-tasks-did-not-reset-the-watchdog-in-time
    vTaskDelay(1);
    // delay(150);
}
