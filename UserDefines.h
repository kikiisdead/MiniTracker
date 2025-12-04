#pragma once

#ifndef USER_DEFINES_H
#define USER_DEFINES_H

/*************************************************
 * BUTTON PIN DEFINITIONS                        *
 * will be changed after PCB testing             *
 *************************************************/
#define DownPin             24  /**< Down Button Pin (Dx on Daisy Pinout) */
#define RightPin            23  /**< Right Button Pin (Dx on Daisy Pinout) */
#define LeftPin             22  /**< Left Button Pin (Dx on Daisy Pinout) */
#define UpPin               21  /**< Up Button Pin (Dx on Daisy Pinout) */
#define LeftShoulderPin     20  /**< Left Bumper Button Pin (Dx on Daisy Pinout) */
#define RightShoulderPin    19  /**< Right Bumper Button Pin (Dx on Daisy Pinout) */
#define PlayPin             18  /**< Play Button Pin (Dx on Daisy Pinout) */
#define ShiftPin            17  /**< Shift Button Pin (Dx on Daisy Pinout) */
#define BPin                16  /**< B Button Pin (Dx on Daisy Pinout) */
#define APin                15  /**< A Button Pin (Dx on Daisy Pinout) */


#define StandbyPin          25  /**< Standby / Wakeup pin must be (PI8, PC13, PI11, PC1, PA0, PA2) on STM32 Pinout */  
#define AmpSDPin            seed::D13  /**< Shut Down pin for the PAM8302 Amplifier */ 
#define HeadDetPin          seed::D14   /**< Headphone Detect Pin PB7 on STM */
#define BacklightPin        7   /**< Backlight PWM pin for ST7789 */       

#define SLEEP_TIME (10 * 60 * 1000) /**< Time in milliseconds before it board enters standby when inactive */

#define USE_DAISYSP_LGPL 1  /**< For FX */

#endif