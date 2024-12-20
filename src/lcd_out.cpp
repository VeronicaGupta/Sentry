#include <mbed.h>
#include "drivers/LCD_DISCO_F429ZI.h"

LCD_DISCO_F429ZI lcd;

/**
 * @brief Displays a snowman on the screen.
 */
void display_snowman(char *text) {
    lcd.Clear(LCD_COLOR_BLACK);
    lcd.SetBackColor(LCD_COLOR_BLACK);
    lcd.SetTextColor(LCD_COLOR_LIGHTGRAY);
    lcd.DisplayStringAt(0, 0, (uint8_t *)text, LEFT_MODE);

    // Snow
    lcd.FillRect(0, LINE(18), 236, 30);

    // Snowman body
    lcd.FillCircle(120, LINE(15), 50); // Bottom part
    lcd.FillCircle(120, 160, 39);     // Middle part
    lcd.FillCircle(120, LINE(6), 28); // Head

    // Eyes
    lcd.SetTextColor(LCD_COLOR_BLACK);
    lcd.FillCircle(108, 90, 4); // Left eye
    lcd.FillCircle(132, 90, 4); // Right eye

    // Nose
    lcd.SetTextColor(LCD_COLOR_ORANGE);
    lcd.FillTriangle(120, 120, 155, 95, 105, 100); 
    lcd.FillCircle(120, 100, 5);

    // Smile
    lcd.SetTextColor(LCD_COLOR_BLACK);
    lcd.FillCircle(108, 110, 2);
    lcd.FillCircle(114, 114, 2);
    lcd.FillCircle(120, 115, 2);
    lcd.FillCircle(126, 114, 2);
    lcd.FillCircle(132, 110, 2);

    // Hat
    lcd.SetTextColor(LCD_COLOR_DARKGRAY);
    lcd.FillRect(90, LINE(4), 60, 10);
    lcd.FillRect(100, LINE(2), 40, 40);

    printf("Displayed snowman screen.\n");
}

/**
 * @brief Displays a Christmas tree on the screen.
 */
void display_christmas_tree(char *text) {
    lcd.Clear(LCD_COLOR_BLACK);
    lcd.SetBackColor(LCD_COLOR_BLACK);
    lcd.SetTextColor(LCD_COLOR_WHITE);
    lcd.DisplayStringAt(0, 0, (uint8_t *)text, LEFT_MODE);
    lcd.DisplayStringAt(0, LINE(2), (uint8_t *)"Merry Christmas!", CENTER_MODE);

    // Log (stump)
    lcd.SetTextColor(LCD_COLOR_ORANGE);
    lcd.FillRect(115, LINE(15), 10, 50);

    // Floor
    lcd.SetTextColor(LCD_COLOR_DARKRED);
    lcd.FillRect(0, LINE(18), 236, 30);

    // Tree
    lcd.SetTextColor(LCD_COLOR_DARKGREEN);
    lcd.FillTriangle(70, 120, 170, LINE(16), LINE(10), LINE(16));
    lcd.FillTriangle(85, 120, 155, LINE(13), LINE(8), LINE(13));
    lcd.FillTriangle(88, 120, 152, LINE(11), LINE(6), LINE(11));

    // Star
    lcd.SetTextColor(LCD_COLOR_LIGHTYELLOW);
    lcd.FillTriangle(110, 120, 130, 85, 103, 85);
    lcd.FillTriangle(110, 120, 130, LINE(6), LINE(5), LINE(6));

    // Lights on tree
    lcd.FillCircle(127, LINE(10), 3);
    lcd.FillCircle(113, LINE(8), 3);
    lcd.FillCircle(103, LINE(12), 3);
    lcd.FillCircle(138, LINE(14), 3);
    lcd.FillCircle(88, LINE(15), 3);

    printf("Displayed Christmas tree screen.\n");
}

/**
 * @brief Displays a loading animation on the screen.
 */
void display_loading_screen(char *text) {
    lcd.Clear(LCD_COLOR_BLACK);
    lcd.SetBackColor(LCD_COLOR_BLACK);
    lcd.SetTextColor(LCD_COLOR_GRAY);
    lcd.DisplayStringAt(0, 0, (uint8_t *)text, LEFT_MODE);

    uint16_t centerX = lcd.GetXSize() / 2;
    uint16_t centerY = lcd.GetYSize() / 2;
    uint16_t radius = 80;
    uint16_t circle_size = 10;

    // Draw outline
    lcd.SetTextColor(LCD_COLOR_LIGHTGRAY);
    for (int angle = 0; angle < 360; angle += 10) {
        float rad = angle * M_PI / 180;
        uint16_t endX = centerX + radius * cos(rad);
        uint16_t endY = centerY + radius * sin(rad);
        lcd.FillCircle(endX, endY, circle_size + 5);
    }

    // Erase inner part of outline
    lcd.SetTextColor(LCD_COLOR_BLACK);
    for (int angle = 0; angle < 360; angle += 10) {
        float rad = angle * M_PI / 180;
        uint16_t endX = centerX + radius * cos(rad);
        uint16_t endY = centerY + radius * sin(rad);
        lcd.FillCircle(endX, endY, circle_size);
    }

    // Animation
    // while (1) {
        for (int angle = 0; angle < 360; angle += 10) {
            float rad = angle * M_PI / 180;
            uint16_t endX = centerX + radius * cos(rad);
            uint16_t endY = centerY + radius * sin(rad);
            lcd.SetTextColor(LCD_COLOR_LIGHTGRAY);
            lcd.FillCircle(endX, endY, circle_size - 2);
            thread_sleep_for(50);
            lcd.SetTextColor(LCD_COLOR_BLACK);
            lcd.FillCircle(endX, endY, circle_size);
        }
    // }
}