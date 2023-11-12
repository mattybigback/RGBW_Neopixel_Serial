/*
Control a single RGBW Neopixel by sending 4 byte strings over serial

Author: Matt Harrison
Repo: http://github.com/mattybigback/RGBW_Neopixel_Serial

*/
#include <Fletcher16.h>  //v0.1.8 - https://github.com/RobTillaart/Fletcher
#include <NeoPixelBus.h> //v2.7.5 - https://github.com/Makuna/NeoPixelBus

#define NUM_LEDS 1
#define NEO_PIN 3

NeoPixelBus<NeoGrbwFeature, NeoWs2812xMethod> pixel(NUM_LEDS, NEO_PIN);

void setup() {
    Serial.begin(9600);
    while (!Serial)
        ; // wait for serial

    Serial.setTimeout(30);

    // Initialise and turn off the LED
    pixel.Begin();
    pixel.Show();

    Serial.println();
    Serial.println("Ready to receive commands");
}

uint8_t *receive_serial_packet() {
    static uint8_t serial_instruction[7] = {0, 0, 0, 0, 0, 0, 0};
    while (Serial.available() > 0) {
        size_t ser_buffer = Serial.readBytes(serial_instruction, 7);

        // Grab checksum from received packet
        uint16_t received_checksum = serial_instruction[6] | serial_instruction[5] << 8;

        // Initialise checksum
        Fletcher16 checksum;
        checksum.begin();

        // Add bytes to be checked by checksum
        for (uint8_t i = 0; i < 5; i++) {
            checksum.add(serial_instruction[i]);
        }

        uint16_t calculated_checksum = checksum.getFletcher();

        if (serial_instruction[0] == 0x7f && calculated_checksum == received_checksum) {
            Serial.println("valid message!");
            return serial_instruction;
        } else {
            serial_instruction[0] = 0xff;
            return serial_instruction;
        }
    }
}

void set_LEDs(uint8_t packet[]) {
    Serial.println("setting LED");
    for (uint8_t i = 1; i < 5; i++) {
        Serial.println(packet[i], HEX);
    }
    pixel.SetPixelColor(0, RgbwColor(packet[1], packet[2], packet[3], packet[4]));
    pixel.Show();
}

void loop() {
    if (Serial.available()) {
        uint8_t *serial_packet;
        serial_packet = receive_serial_packet();

        if (serial_packet[0] != 0xff) {
            set_LEDs(serial_packet);
        }

        // clear buffer
        for(uint8_t i=0;i<7;i++){
            serial_packet[i] = 0;
        }
    }
}
