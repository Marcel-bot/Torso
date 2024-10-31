#include <main.h>

struct command {
    int part, degree;
};

void send_motion_metadata(const float metadata[], const int address) {
    Wire.beginTransmission(address);

    Wire.write(MOTION_METADATA);

    for (int i = 0; i < 5; ++i) {
        const auto floatBytes = reinterpret_cast<const byte *>(&metadata[i]);
        for (int b = 0; b < 4; ++b) {
            Wire.write(floatBytes[b]);
        }
    }

    Wire.endTransmission();
}

void send_motion_commands(const command commands[], const int len, const int address) {
    Serial.println("Sending motion commands");

    Wire.beginTransmission(address);

    Wire.write(MOTION_DATA);

    for (int i = 0; i < len; i++) {
        Wire.write(commands[i].part);
        Wire.write(commands[i].degree);
    }

    Wire.endTransmission();
}

void move_arms(const float params[30], const int len) {
    command leftCommands[8], rightCommands[8];

    int leftCommandsIndex = 0, rightCommandsIndex = 0;

    for (int i = 5; i < len; i += 2) {
        if (params[i] < 6) {
            leftCommands[leftCommandsIndex++] = {static_cast<int>(params[i]), static_cast<int>(params[i + 1])};
        } else {
            rightCommands[rightCommandsIndex++] = {static_cast<int>(params[i]) - 6, static_cast<int>(params[i + 1])};
        }
    }

    send_motion_metadata(params, LEFT_HAND_ADDRESS);
    send_motion_commands(leftCommands, leftCommandsIndex, LEFT_HAND_ADDRESS);

    // send_motion_metadata(params, RIGHT_HAND_ADDRESS);
    // send_motion_commands(rightCommands, rightCommandsIndex, RIGHT_HAND_ADDRESS);
}

void stop_arms() {
}

void display() {
}

void processCommand(String command) {
    command.trim();
    const char cmd = command.charAt(0);


    const String paramString = command.substring(2);
    float params[30];
    int len = 0;


    int startIndex = 0;
    int spaceIndex = paramString.indexOf(' ');

    while (spaceIndex >= 0) {
        params[len++] = paramString.substring(startIndex, spaceIndex).toFloat();
        startIndex = spaceIndex + 1;
        spaceIndex = paramString.indexOf(' ', startIndex);
    }

    if (startIndex < paramString.length()) {
        params[len++] = paramString.substring(startIndex).toFloat();
    }

    switch (cmd) {
        case 'M':
            move_arms(params, len);
            break;

        case 'S':
            stop_arms();
            break;

        case 'D':
            display();
            break;

        default:
            break;
    }
}

void setup() {
    setup_uart();
    setup_i2c();
}

void loop() {
    if (Serial.available()) {
        String command;

        char c = Serial.read();

        while (c != '\n') {
            command += c;

            c = Serial.read();
        }

        processCommand(command);
    }
}
