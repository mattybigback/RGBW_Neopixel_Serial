
/*
Control a single RGBW Neopixel by sending 4 byte strings over serial

Author: Matt Harrison
Repo: 

Required Libraries:
    * NeoPixelBus v2.7.5 - https://github.com/Makuna/NeoPixelBus
*/
#include <NeoPixelBus.h>

#define NUM_LEDS 1
#define NEO_PIN  3

NeoPixelBus<NeoGrbwFeature, NeoWs2812xMethod> pixel(NUM_LEDS, NEO_PIN);

void setup()
{
    Serial.begin(115200);
    while (!Serial);    // wait for serial

    Serial.setTimeout(30);

    // Initialise and turn off the LED
    pixel.Begin();
    pixel.Show();

    Serial.println();
    Serial.println("Ready to receive commands");
}

void loop()
{

    if (Serial.available())
    {
        uint8_t *rgbw;
        rgbw = serial_rgbw();
        pixel.SetPixelColor(0, RgbwColor(rgbw[0], rgbw[1], rgbw[2], rgbw[3]));
        pixel.Show();
        for(uint8_t i=0; i<4; i++){
            Serial.println(rgbw[i]);
        }
    }

}

uint8_t * serial_rgbw() {
    static uint8_t serial_instruction[4] = {0, 0, 0, 0};
    while (Serial.available() > 0)
    {
        size_t ser_buffer = Serial.readBytes(serial_instruction, 4);
    }
    return serial_instruction;
}
