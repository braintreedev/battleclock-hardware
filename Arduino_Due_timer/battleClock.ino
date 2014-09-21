// The Battle Clock is (c) PayPal Braintree 2014
// This firmware is for driving the Battle Hack Battle Clock.

// V1-00 uses the LinkSprite example code for the 1602 B LCD shield
// for the Arduino development boards.

// Pins used by LCD & Keypad Shield:
// =================================

// A0:  Buttons, analog input from voltage ladder
// D4:  LCD bit 4
// D5:  LCD bit 5
// D6:  LCD bit 6
// D7:  LCD bit 7
// D8:  LCD RS
// D9:  LCD E
// D10: LCD Backlight (high = on, also has pullup high so default is on)
 
// ADC voltages for the 5 buttons on analog input pin A0:
// ======================================================
 
// RIGHT:  0.00V :   0 @ 8bit ;   0 @ 10 bit
// UP:     0.71V :  36 @ 8bit ; 145 @ 10 bit
// DOWN:   1.61V :  82 @ 8bit ; 329 @ 10 bit
// LEFT:   2.47V : 126 @ 8bit ; 505 @ 10 bit
// SELECT: 3.62V : 185 @ 8bit ; 741 @ 10 bit

#include <LiquidCrystal.h>

#define button_ADC_pin    A0  // A0 is the button ADC input
#define LCD_backlight_pin 10  // D10 controls LCD backlight

#define right_10bit_ADC   0  // right
#define up_10bit_ADC      145  // up
#define down_10bit_ADC    329  // down
#define left_10bit_ADC    505  // left
#define select_10bit_ADC  741  // right
#define button_hysteresis 10

#define button_none   0
#define button_right  1
#define button_up     2
#define button_down   3
#define button_left   4
#define button_select 5

#define LCD_top_row    0
#define LCD_bottom_row 1

#define LCD_backligt_off() digitalWrite( LCD_backlight_pin, LOW )
#define LCD_backlight_on() digitalWrite( LCD_backlight_pin, HIGH )

byte button_just_pressed  = false;
byte button_just_released = false;
byte button_was           = button_none;

const int hacking    = 1, hack_time = 1;
const int presenting = 2, pres_time = 2;

unsigned int hours_tens_current     = 0, hours_units_current   = 0,
             minutes_tens_current   = 0, minutes_units_current = 0,
             seconds_tens_current   = 0, seconds_units_current = 0,
             milli_hundreds_current = 0, milli_tens_current    = 0;

// LCD in 4-pin mode: ( RS, E, LCD-D4, LCD-D5, LCD-D6, LCD-D7 )
LiquidCrystal LCD( 8, 9, 4, 5, 6, 7 );

void setup()
{
  Serial.begin( 115200 );
  
  pinMode( button_ADC_pin, INPUT );
  pinMode( LCD_backlight_pin, OUTPUT );
   
  digitalWrite( button_ADC_pin, LOW );
  digitalWrite( LCD_backlight_pin, HIGH );
  
  // The size (16x2) of the LCD is defined & we set the inital displayed message
  LCD.begin( 16, 2 );

  LCD.setCursor( 0, LCD_top_row );
  LCD.print( "                " );  
  LCD.setCursor( 0, LCD_bottom_row );
  LCD.print( "                " );
}


void loop()
{
   // A state machine is declared to organise the two processes.
   // By default, we run the 24-hour clock
   
   
   state_machine(  );
}  
  

void state_machine( int new_state )
{
  LCD.setCursor( 0, LCD_bottom_row );
  LCD.print( "  BATTLE CLOCK  " );
  
  switch( new_state )
  {
    case hack_time:
    {
      hours_tens_current    = 2;
      hours_units_current   = 4;
      minutes_tens_current  = 0;
      minutes_units_current = 0;
      seconds_tens_current  = 0;
      seconds_units_current = 0;
      
      countdown_timer( hacking );
    }
    case pres_time:
    {
      minutes_tens_current   = 0;
      minutes_units_current  = 2;
      seconds_tens_current   = 0;
      seconds_units_current  = 0;
      milli_hundreds_current = 0;
      milli_tens_current     = 0;
      
      countdown_timer( presenting );
    }    
  }
}


void countdown_timer( int timing_mode )   
{ 
  bool timer_done = false;
  
  if( timing_mode == hacking )
  {
    while( timer_done == false )
    {
      write_to_LCD_hack();
    
      delay( 1000 );
    
      timer_done = timer_query_hacking();
    }
    
    write_to_LCD_hack();
  }
  else if( timing_mode == presenting )
  {
    while( timer_done == false )
    {
      write_to_LCD_pres();
      
      delay( 10 );
      
      timer_done = timer_query_presenting();
    }
    
    write_to_LCD_pres();
  }
  
  
  LCD.setCursor( 0, LCD_bottom_row );
  LCD.print( "   All done!!   " );
  
  for(;;);
}

bool timer_query_hacking( void )
{
  bool change_occured = false;
  
  static unsigned int hours_tens_new   = 2, hours_units_new   = 4,
                      minutes_tens_new = 0, minutes_units_new = 0,
                      seconds_tens_new = 0, seconds_units_new = 0;
  
  if( seconds_units_current == 0 )
  {
    seconds_units_new = 9;
    
    if( seconds_tens_current > 0 )
    {
      seconds_tens_new--;
    }
    
    change_occured = true;
  }
  else
  {
    seconds_units_current--; 
  }
  
  if( seconds_tens_current == 0 && seconds_units_current == 0 )
  {
    seconds_tens_new = 5;
    
    if( minutes_units_current > 0 )
    {
      minutes_units_new--;
    }
    
    change_occured = true;
  }
  
  if( minutes_units_current == 0 && seconds_tens_current == 0 && seconds_units_current == 0 )
  {
    minutes_units_new = 9;
    
    if( minutes_tens_current > 0 )
    {
      minutes_tens_new--;
    }
    
    change_occured = true;
  }
  
  if( minutes_tens_current == 0 && minutes_units_current == 0 && seconds_tens_current == 0 && seconds_units_current == 0 )
  {
    minutes_tens_new = 5;
    
    if( hours_units_current > 0 )
    {
      hours_units_new--;
    }
    
    change_occured = true;
  }
  
  if( hours_units_current == 0 && minutes_tens_current == 0 && minutes_units_current == 0 && seconds_tens_current == 0 && seconds_units_current == 0 )
  {
    hours_units_new = 9;
    
    if( hours_tens_current > 0 )
    {
      hours_tens_new--;
    }
    
    change_occured = true;
  }
  
  if( hours_tens_current == 0 && hours_units_current == 0 && minutes_tens_current == 0 && minutes_units_current == 0 && seconds_tens_current == 0 && seconds_units_current == 0 )
  {
     return true;
  }
    
  if( change_occured )
  {
    hours_tens_current    = hours_tens_new;
    hours_units_current   = hours_units_new;
    minutes_tens_current  = minutes_tens_new;
    minutes_units_current = minutes_units_new;
    seconds_tens_current  = seconds_tens_new;
    seconds_units_current = seconds_units_new;
  }
  
  return false;
}

bool timer_query_presenting( void )
{  
  bool change_occured = false;
  
  static unsigned int minutes_tens_new   = 0, minutes_units_new = 2,
                      seconds_tens_new   = 0, seconds_units_new = 0,
                      milli_hundreds_new = 0, milli_tens_new    = 0;

  if( milli_tens_current == 0 )
  {
    milli_tens_new = 9;
   
    if( milli_hundreds_current > 0 )
    {
      milli_hundreds_new--;
    }
    
    change_occured = true;
  }
  else
  {
    milli_tens_current--;
  }
  
  if( milli_hundreds_current == 0 && milli_tens_current == 0 )
  {
    milli_hundreds_new = 9;
    
    if( seconds_units_current > 0 )
    {
      seconds_units_new--;
    }
    
    change_occured = true;
  }
  
  if( seconds_units_current == 0 && milli_hundreds_current == 0 && milli_tens_current == 0 )
  {
    seconds_units_new = 9;
    
    if( seconds_tens_current > 0 )
    {
      seconds_tens_new--;
    }
    
    change_occured = true;
  }
  
  if( seconds_tens_current == 0 && seconds_units_current == 0 && milli_hundreds_current == 0 && milli_tens_current == 0 )
  {
    seconds_tens_new = 5;
    
    if( minutes_units_current > 0 )
    {
      minutes_units_new--;
    }
    
    change_occured = true;
  }
  
  if( minutes_units_current == 0 && seconds_tens_current == 0 && seconds_units_current == 0 && milli_hundreds_current == 0 && milli_tens_current == 0 )
  {
    return true;
  }
   
  if( change_occured == true )
  {
    minutes_units_current  = minutes_units_new;
    seconds_tens_current   = seconds_tens_new;
    seconds_units_current  = seconds_units_new;
    milli_hundreds_current = milli_hundreds_new;
    milli_tens_current     = milli_tens_new;
  }
  
  return false;
}

void write_to_LCD_hack( void )
{
  LCD.setCursor( 4, LCD_top_row );
  LCD.print( hours_tens_current );
  
  LCD.setCursor( 5, LCD_top_row );
  LCD.print( hours_units_current );
  
  LCD.setCursor( 6, LCD_top_row );
  LCD.print( ":" );

  LCD.setCursor( 7, LCD_top_row );
  LCD.print( minutes_tens_current );
  
  LCD.setCursor( 8, LCD_top_row );
  LCD.print( minutes_units_current );
  
  LCD.setCursor( 9, LCD_top_row );
  LCD.print( ":" );
  
  LCD.setCursor( 10, LCD_top_row );
  LCD.print( seconds_tens_current );
  
  LCD.setCursor( 11, LCD_top_row );
  LCD.print( seconds_units_current );
}

void write_to_LCD_pres( void )
{  
  LCD.setCursor( 4, LCD_top_row );
  LCD.print( minutes_tens_current );
  
  LCD.setCursor( 5, LCD_top_row );
  LCD.print( minutes_units_current );
  
  LCD.setCursor( 6, LCD_top_row );
  LCD.print( ":" );

  LCD.setCursor( 7, LCD_top_row );
  LCD.print( seconds_tens_current );
  
  LCD.setCursor( 8, LCD_top_row );
  LCD.print( seconds_units_current );
  
  LCD.setCursor( 9, LCD_top_row );
  LCD.print( ":" );
  
  LCD.setCursor( 10, LCD_top_row );
  LCD.print( milli_hundreds_current );
  
  LCD.setCursor( 11, LCD_top_row );
  LCD.print( milli_tens_current );
}

