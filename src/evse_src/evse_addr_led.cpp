
#include "evse_addr_led.h"
// #include <Arduino.h>
#include <FastLED.h>

volatile int gu8_led_count = 0;
volatile int gu8_arr_index = 0;

// array declaration for time axis.(lookup table)
// static int arr[10]={1,0,1,0,1,0,1,0,1,0};

#if 0
// declaring enum for configuration of led colours.
enum led_col_config
{
    Red,
    Green,
    Blue,
    Black,
    Yellow,
    Purple,
    White,
    Blinky_Red,
    Blinky_Green,
    Blinky_Blue,
    Blinky_Black,
    Blinky_Yellow,
    Blinky_Purple,
    Blinky_White
};

#endif

ADDR_LEDs led_rgb_obj;
ADDR_LEDs *led_rgb;

CRGB leds[NUM_LEDS];

enum led_col_config led_status; // enum variable.

void ADDR_LEDs::ADDR_LEDs_Init(void)
{
    FastLED.addLeds<WS2813, DATA_PIN, GRB>(leds, NUM_LEDS);
    // FastLED.addLeds<WS2812, DATA_PIN, RGB>(leds, NUM_LEDS);
}

void ADDR_LEDs::red_led(void)
{
    ADDR_LEDs_main(Red);
}

void ADDR_LEDs::Greeen_led(void)
{
    ADDR_LEDs_main(Green);
}

void ADDR_LEDs::blue_led(void)
{
    ADDR_LEDs_main(Blue);
}

void ADDR_LEDs::Yellow_led(void)
{
    ADDR_LEDs_main(Yellow);
}

void ADDR_LEDs::Purple_led(void)
{
    ADDR_LEDs_main(Purple);
}

void ADDR_LEDs::White_led(void)
{
    ADDR_LEDs_main(White);
}

void ADDR_LEDs::Black_led(void)
{
    ADDR_LEDs_main(Black);
}

void ADDR_LEDs::blink_red_led(void)
{
    ADDR_LEDs_main(Blinky_Red);
}

void ADDR_LEDs::blink_green_led(void)
{
    ADDR_LEDs_main(Blinky_Green);
}

void ADDR_LEDs::blink_blue_led(void)
{
    ADDR_LEDs_main(Blinky_Blue);
}

void ADDR_LEDs::blink_yellow_led(void)
{
    ADDR_LEDs_main(Blinky_Yellow);
}

void ADDR_LEDs::blink_Purple_led(void)
{
    ADDR_LEDs_main(Blinky_Purple);
}

void ADDR_LEDs::blink_black_led(void)
{
    ADDR_LEDs_main(Blinky_Black);
}

void ADDR_LEDs::blink_white_led(void)
{
    ADDR_LEDs_main(Blinky_White);
}

void ADDR_LEDs::ADDR_LEDs_main(int led_status)
{
    switch (led_status)
    {
    case Green:
        //Serial.println("green led");
    //   Serial.println("LED:G");
#if 0
         leds[NUM_LEDS] = CRGB::Green;
         FastLED.show(COLOR_BRIGHTNESS);
#else

        for (uint8_t idx = 0; idx < NUM_LEDS; idx++)
        {
            leds[idx] = CRGB::Green;
            FastLED.show(COLOR_BRIGHTNESS);
        }
#endif

        break;

    case Blue:
    //  Serial.println("LED:B");

#if 0
        leds[NUM_LEDS] = CRGB::Blue;
        FastLED.show(COLOR_BRIGHTNESS);
#else

        for (uint8_t idx = 0; idx < NUM_LEDS; idx++)
        {
            leds[idx] = CRGB::Blue;
            FastLED.show(COLOR_BRIGHTNESS);
        }
#endif
        break;

    case Red:
    //  Serial.println("LED:R");

#if 0
        leds[NUM_LEDS] = CRGB::Red;
        FastLED.show(COLOR_BRIGHTNESS);
#else
        for (uint8_t idx = 0; idx < NUM_LEDS; idx++)
        {
            leds[idx] = CRGB::Red;
            FastLED.show(COLOR_BRIGHTNESS);
        }
#endif
        break;

    case Yellow:
     Serial.println("LED:Y");

         for (uint8_t idx = 0; idx < NUM_LEDS; idx++)
         {
            leds[idx] = CRGB::Yellow;
            FastLED.show(COLOR_BRIGHTNESS);
         }

    break;    

    case Black:
     Serial.println("LED:0");

#if 0
        leds[NUM_LEDS] = CRGB::Black;
        FastLED.show(COLOR_BRIGHTNESS);
#else
        for (uint8_t idx = 0; idx < NUM_LEDS; idx++)
        {
            leds[idx] = CRGB::Black;
            FastLED.show(COLOR_BRIGHTNESS);
        }
#endif
        break;

    case White:
     Serial.println("LED:W");
        for (uint8_t idx = 0; idx < NUM_LEDS; idx++)
        {
            leds[idx] = CRGB::White;
            FastLED.show(WHITE_BRIGHTNESS);
        }

        break;

#if 1
    case Blinky_Green:
        //    Serial.println("LED:BG");
        led_rgb->led_on_off(Blinky_Green);
        break;

    case Blinky_Blue:
        // Serial.println("LED:BB");
        led_rgb->led_on_off(Blinky_Blue);
        break;

    case Blinky_Red:
        // Serial.println("LED:BR");
        led_rgb->led_on_off(Blinky_Red);
        break;

    case Blinky_Yellow:
        // Serial.println("LED:BY");
        led_rgb->led_on_off(Blinky_Yellow);
        break;

    case Blinky_Black:
    //    Serial.println("LED:B0");
        led_rgb->led_on_off(Blinky_Black);
        break;

    case Blinky_White:
        // Serial.println("LED:BW");
        led_rgb->led_on_off(Blinky_White);
        break;

    case Blinky_Purple:
        // Serial.println("LED:BP");
        led_rgb->led_on_off(Blinky_Purple);
        break;

    default :
        break;
#endif
    }
}

/*....................*/
// function to blink leds(200ms_on, 200ms_off)
void ADDR_LEDs::led_on_off(int led_status)
{
    static uint8_t lu8_led_blink = 0;
    // give led_count till reaches it the max count so that delay becomes 200ms (10*20=200ms).
    gu8_led_count++;
    // ESP_LOGI("LOG", "gu8_led_count %d \r\n ",gu8_led_count);
    if (gu8_led_count >= MAX_COUNT)
    {
        gu8_led_count = 0;

        lu8_led_blink = lu8_led_blink ^ 1;

    }

        switch (led_status)
        {
        case Blinky_Green:
           // Serial.println("green led");

            if (lu8_led_blink == 1)
            {
                for (uint8_t idx = 0; idx < NUM_LEDS; idx++)
                {
                    leds[idx] = CRGB::Green;
                    FastLED.show(COLOR_BRIGHTNESS);
                }
            }
            else
            {
                for (uint8_t idx = 0; idx < NUM_LEDS; idx++)
                {

                    leds[idx] = CRGB::Black;
                    FastLED.show(COLOR_BRIGHTNESS);
                }
            }

            break;

        case Blinky_Blue:

             if (lu8_led_blink == 1)
            {
            for (uint8_t idx = 0; idx < NUM_LEDS; idx++)
            {
                // leds[idx] = CRGB::Blue;
                // FastLED.show(COLOR_BRIGHTNESS);
                  leds[idx] = CRGB::Black;
                FastLED.show(COLOR_BRIGHTNESS);
            }
            }
            else
            {
                for (uint8_t idx = 0; idx < NUM_LEDS; idx++)
            {

                // leds[idx] = CRGB::Black;
                // FastLED.show(COLOR_BRIGHTNESS);
                  leds[idx] = CRGB::Blue;
                FastLED.show(COLOR_BRIGHTNESS);
            }

            }
        

            break;

        case Blinky_Red:

             if (lu8_led_blink == 1)
            {
            for (uint8_t idx = 0; idx < NUM_LEDS; idx++)
            {
                leds[idx] = CRGB::Red;
                FastLED.show(COLOR_BRIGHTNESS);
            }
            }
            else
            {
                for (uint8_t idx = 0; idx < NUM_LEDS; idx++)
            {

                leds[idx] = CRGB::Black;
                FastLED.show(COLOR_BRIGHTNESS);
            }

            }
           
            break;

        case Blinky_Yellow:

            #if 0
            if (lu8_led_blink == 1)
            {
            for (uint8_t idx = 0; idx < NUM_LEDS; idx++)
            {
                // leds[idx] = CRGB::Red;
                FastLED.show(COLOR_BRIGHTNESS);
            }
            }
            else
            {
                for (uint8_t idx = 0; idx < NUM_LEDS; idx++)
            {

                leds[idx] = CRGB::Black;
                FastLED.show(COLOR_BRIGHTNESS);
            }

            }
            #endif
            break;

        case Blinky_Black:

            break;

        case Blinky_White:

            if (lu8_led_blink == 1)
            {
                for (uint8_t idx = 0; idx < NUM_LEDS; idx++)
                {
                    leds[idx] = CRGB::White;
                    FastLED.show(WHITE_BRIGHTNESS);
                }
            }
            else
            {
                for (uint8_t idx = 0; idx < NUM_LEDS; idx++)
                {

                    leds[idx] = CRGB::Black;
                    FastLED.show(COLOR_BRIGHTNESS);
                }
            }
           
           default:

            break;
        }
        gu8_arr_index++;
    

// whenever the array reaches the max index position set the index value to 0 again.
#if 1
    if (gu8_arr_index > ARR_MAX_COUNT)
    {
        gu8_arr_index = 0;
    }
#endif
}
