//copy and paste from txt onto here, make sure main is empty before you paste
#include <mbed.h>
#include "drivers/LCD_DISCO_F429ZI.h" 

const uint8_t spacing = 20; // Adjust spacing for characters
#define PI 3.14159


int main() {
    // Initialize the LCD object
    LCD_DISCO_F429ZI lcd;
    
    // Clear the screen
    lcd.Clear(LCD_COLOR_BLACK);
    //Text
    lcd.SetBackColor(LCD_COLOR_BLACK);
    lcd.SetTextColor(LCD_COLOR_LIGHTGRAY);
    lcd.DisplayStringAt(0, 0, (uint8_t *)"Locked!", LEFT_MODE);
    
    //SNOW
    lcd.SetTextColor(LCD_COLOR_LIGHTGRAY);
    lcd.FillRect(0, LINE(18), 236, 30);
    //start of snowman
    lcd.SetTextColor(LCD_COLOR_LIGHTGRAY);
    lcd.FillCircle(120, LINE(15), 50); //bottom part of snowman
    lcd.FillCircle(120, 160, 39); //Middle part of snow man
    lcd.FillCircle(120, LINE(6), 28); //head of snowman

    //eyes
    lcd.SetTextColor(LCD_COLOR_BLACK);
    lcd.FillCircle(108, 90, 4); //left eye
    lcd.FillCircle(132, 90, 4); //right eye
    
    
    //nose
    lcd.SetTextColor(LCD_COLOR_ORANGE);
    lcd.FillTriangle(120, 120, 155, 95, 105, 100); 
    lcd.FillCircle(120, 100, 5);

    //smile
    lcd.SetTextColor(LCD_COLOR_BLACK);
    lcd.FillCircle(108, 110, 2); // left most
    lcd.FillCircle(114, 114, 2);
    lcd.FillCircle(120, 115, 2); //middle bottom piece
    lcd.FillCircle(126, 114, 2);
    lcd.FillCircle(132, 110, 2); //right most

    //hat
    lcd.SetTextColor(LCD_COLOR_DARKGRAY);
    lcd.FillRect(90, LINE(4), 60, 10);//bottom part
    
    lcd.FillRect(100, LINE(2), 40, 40); //top part

    int count = 0;
    //small cute animation for the buttons
    while(1){
        count++;
        if (count%2 == 0){
            lcd.SetTextColor(LCD_COLOR_DARKGREEN);
            lcd.FillCircle(120, LINE(9), 5);
            lcd.FillCircle(120, LINE(11), 5);

            lcd.FillCircle(120, LINE(14), 5);
            lcd.FillCircle(120, LINE(16), 5);
        }
        else{
            lcd.SetTextColor(LCD_COLOR_BLUE);
            lcd.FillCircle(120, LINE(9), 5);
            lcd.FillCircle(120, LINE(14), 5);

            lcd.SetTextColor(LCD_COLOR_RED);
            lcd.FillCircle(120, LINE(11), 5);
            lcd.FillCircle(120, LINE(16), 5);
        }
        thread_sleep_for(1600);

    }

}