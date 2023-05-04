/*
 * File:   rg_877a.c
 * Author: zero
 *
 * Created on November 11, 2021, 9:46 AM
 */

int sec = 00;
int min = 54;
int hour = 21;
int date = 12;
int month = 01;
int year = 22;


#include <xc.h>
#include "config.h"
#include "I2C_LCD.h"
#include "DS3231_1.h"
#include "../c90/stdlib.h"
#include "../c90/string.h"
#include "../c90/stdbool.h"

bool flag;
bool flag1;
bool flag2;

char *button_press;
char zero_six;
char twelve_hundred;
char eighteen_hundred;
char zero_zero;

float btn_press;

int status;
int rr;

void interrupt ISR_sample(void);
void display_Time(void);
void write_x(void);
void display_zero_six(void);
void display_twelve_hundred(void);
void display_eighteen_hundred(void);
void display_zero_zero(void);
void reset(void);
void reset_eeprom(void);


void main(void) {
    TRISB = 0xFF;
    TRISD = 0x00;
    
    PORTD = 0x00;
    
    nRBPU = 0;
    GIE = 1;
    PEIE = 1;
    INTE = 1;
    INTF = 0;
    flag = 0; 
    
    I2C_Master_Init();
    LCD_Init(0x4E);
    
//    Set_Time_Date();

    
    while(1){
        Update_Current_Date_Time();
//        LCD_Set_Cursor(1, 9);
//        display_Time();
        write_x();
        flag1 = 1;
        flag2 = 0;
        
        if(RB1 == 0) reset();
        
        if(flag == 1){
            RD0 = 1;
            btn_press += 0.5;
            rr = (int)btn_press;
            if(btn_press-0.5 != rr) flag1 = 0;
            eeprom_write(0, rr);
            eeprom_write(1, flag1);
            eeprom_write(2, flag2);
            LCD_Set_Cursor(1, 9);
            display_Time();
            __delay_ms(250);
            RD0 = 0;
            flag = 0;
            
        }
        
     
        btn_press = eeprom_read(0);
        flag1 = eeprom_read(1);
        flag2 = eeprom_read(2);
        
        if(flag1 == 1 && flag2 == 0) btn_press += 0.5;

        button_press = ftoa(btn_press, &status);
        
        LCD_Set_Cursor(1, 1);
        LCD_Write_Char(67);
        LCD_Write_Char(45);
        LCD_Write_F1(button_press);
        
        LCD_Set_Cursor(2, 16);
        LCD_Write_Char(80);
        
        LCD_Set_Cursor(2, 5);
        if(sec<=15){
            display_zero_six();
        }
        else if(sec<=30){
            display_twelve_hundred();
        }
        else if(sec<=45){
            display_eighteen_hundred();
        }
        else{
            display_zero_zero();
        }
        __delay_ms(1000);
    }
    
}


void interrupt ISR_sample(void){      
    if(INTF == 1){
        flag = 1;
        INTF = 0;                    
    }  
}

void display_Time(void){
    LCD_Write_Char(76);
    LCD_Write_Char(84);
    LCD_Write_Char(45); 
    
    hour = hour-8;
    
//    char sec_0 = sec%10;
//    char sec_1 = sec/10;
    
    char min_0 = min%10;
    char min_1 = min/10;
    char hour_0 = hour%10;
    char hour_1 = hour/10;
    

    LCD_Write_Char(hour_1+'0');
    LCD_Write_Char(hour_0+'0');
    LCD_Write_Char(':');
    LCD_Write_Char(min_1+'0');
    LCD_Write_Char(min_0+'0');  
//    LCD_Write_Char(':');
//    LCD_Write_Char(sec_1+'0');
//    LCD_Write_Char(sec_0+'0');
}

void display_zero_six(void){
    LCD_Write_Char(48);
    LCD_Write_Char(54);
    LCD_Write_Char(45);
    for(int i=0;i<5;i++){
        zero_six = eeprom_read(i+6);
        LCD_Write_Char(zero_six);
    } 
}

void display_twelve_hundred(void){
    LCD_Write_Char(49);
    LCD_Write_Char(50);
    LCD_Write_Char(45);
    for(int i=0;i<5;i++){
        twelve_hundred = eeprom_read(i+11);
        LCD_Write_Char(twelve_hundred);
    } 

}

void display_eighteen_hundred(void){
    LCD_Write_Char(49);
    LCD_Write_Char(56);
    LCD_Write_Char(45);
    for(int i=0;i<5;i++){
        eighteen_hundred = eeprom_read(i+16);
        LCD_Write_Char(eighteen_hundred);
    } 

}

void display_zero_zero(void){
    LCD_Write_Char(50);
    LCD_Write_Char(52);
    LCD_Write_Char(45);
    for(int i=0;i<5;i++){
        zero_zero = eeprom_read(i+21);
        LCD_Write_Char(zero_zero);
    } 

}

void reset(void){
    eeprom_write(2, 1);
    if(flag == 1) btn_press = 0.5;
    else btn_press = 0.0;
    rr = 0;
    eeprom_write(0, 0);
}

void reset_eeprom(void){
    for(int i=6;i<26;i++){
        if(i == 7 || i == 12 || i == 17 || i == 22){
            eeprom_write(i, '.');
        }
        else{
            eeprom_write(i, '0');
        }
        
    }
    
}

void write_x(void){ 
    
    if(hour == 14 && min == 0 && sec == 0){
        for (int i=0;i<5;i++){
            eeprom_write(i+6, button_press[i]);
        }
        __delay_ms(1000);
        reset();        
    }
    
    
    else if(hour == 20 && min == 0 && sec == 0){
        for (int i=0;i<5;i++){
            eeprom_write(i+11, button_press[i]);
        }
        __delay_ms(1000);
        reset();        
    }  
    
    else if(hour == 2 && min == 0 && sec == 0){
        for (int i=0;i<5;i++){
            eeprom_write(i+16, button_press[i]);
        }
        __delay_ms(1000);
        reset();        
    }    
    
    
    else if(hour == 8 && min == 0 && sec == 0){
        for (int i=0;i<5;i++){
            eeprom_write(i+21, button_press[i]);
        }                                
        __delay_ms(1000);
        reset();       
    }


    else if(hour == 9 && min == 0 && sec == 0){
        __delay_ms(1000);
        reset_eeprom();        
    }    
        
}
