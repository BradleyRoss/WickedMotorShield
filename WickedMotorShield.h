/** @file 
 */
/* Copyright (C) 2014 by Victor Aprea <victor.aprea@wickeddevice.com>

Permission is hereby granted, free of charge, to any person obtaining
a copy of this software and associated documentation files (the
"Software"), to deal in the Software without restriction, including
without limitation the rights to use, copy, modify, merge, publish,
distribute, sublicense, and/or sell copies of the Software, and to
permit persons to whom the Software is furnished to do so, subject to
the following conditions:

The above copyright notice and this permission notice shall be
included in all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE
LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION
OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION
WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.  */

#ifndef _WICKED_MOTOR_SHIELD_H
#define _WICKED_MOTOR_SHIELD_H

#if ARDUINO >= 100
  #include <Arduino.h>  // Arduino 1.0
#else
  #include <WProgram.h> // Arduino 0022
#endif

#include <stdint.h>
/**  Integer value defining counterclockwise rotation.  (Value = 0) */
#define DIR_CCW	(0)
/** Integer value defining clockwise rotation. (Value = 1) */
#define DIR_CW  (1)
/**  Integer value defining "brake off". */
#define BRAKE_OFF  (0)
/**
 *  Integer value defining "brake hard".
 */
#define BRAKE_HARD (1)
/**
 *  Integer value defining "brake soft".
 */
#define BRAKE_SOFT (2)
/**
 * Integer value identifying motor M1.
 */
#define M1  (0)
/**
 * Integer value identifying motor M2.
 */
#define M2  (1)
/**
 *  Integer value identifying motor M3.
 */
#define M3  (2)
/**
 * Integer value identifying motor M4.
 */
#define M4  (3)
/**
 * Intger value identifying motor M5.
 */
#define M5  (4)
/**
 * Integer value identifying motor M6.
 */
#define M6  (5)

// these bits are in shift register 1
/**
 * Shows location of bit for direction status of motor #M4 in 
 *   WickedMotorShield#shift_register_1.
 *
 * See #M1_DIR_MASK and #M1_BRAKE_MASK for more information.
 */
#define M4_DIR_MASK    (0x80)
/**
 * Shows location of bit for brake status of motor #M4 in 
 *   WickedMotorShield#shift_register_1.
 *
 * See #M1_DIR_MASK and #M1_BRAKE_MASK for more information.
 */
#define M4_BRAKE_MASK  (0x40)
/**
 * Shows location of bit for brake status for motor #M1 in
 * WickedMotorShield#first_shift_register.
 *
 * See #M1_BRAKE_MASK
 */
#define M1_DIR_MASK    (0x20)
/**
 * Shows location of bit for brake status of motor #M1 in 
 *   WickedMotorShield#first_shift_register.
 *
 *  If M1_BRAKE_MASK bit is zero, brake
 *  is set to BRAKE_OFF and M1_DIR_MASK bit
 *  indicates direction of rotation.  If M1_BRAKE_MASK
 *  bit is one, M1_DIR_MASK indicates whether hard
 *  brake or soft brake.
 * 
 *  There are equivalent macro definitions BRAKE_MASK and
 *  DIR_MASK for the other motors (M2, M3, M4, M5, M6).
 */
#define M1_BRAKE_MASK  (0x10)
/**
 * Shows location of bit for direction status of motor #M2 in 
 *   WickedMotorShield#first_shift_register.
 *
 * See #M1_DIR_MASK and #M1_BRAKE_MASK for more information.
 */
#define M2_DIR_MASK    (0x08)
/**
 * Shows location of bit for brake status of motor #M2 in 
 *   WickedMotorShield#first_shift_register.
 *
 * See #M1_DIR_MASK and #M1_BRAKE_MASK for more information.
 */
#define M2_BRAKE_MASK  (0x04)
/**
 * Shows location of bit for direction status of motor #M3 in 
 *   WickedMotorShield#first_shift_register.
 *
 * See #M1_DIR_MASK and #M1_BRAKE_MASK for more information.
 */
#define M3_DIR_MASK    (0x02)
/**
 * Shows location of bit for brake status of motor #M3 in 
 *   WickedMotorShield#first_shift_register.
 *
 * See #M1_DIR_MASK and #M1_BRAKE_MASK for more information.
 */
#define M3_BRAKE_MASK  (0x01)

// these bits are in shift register 2
/**
 * Shows location of bit for direction status of motor #M6 in 
 *   WickedMotorShield#second_shift_register.
 *
 * See #M1_DIR_MASK and #M1_BRAKE_MASK for more information.
 */
#define M6_DIR_MASK    (0x80)
/**
 * Shows location of bit for brake status of motor #M6 in 
 *   WickedMotorShield#second_shift_register.
 *
 * See #M1_DIR_MASK and #M1_BRAKE_MASK for more information.
 */
#define M6_BRAKE_MASK  (0x40)
/**
 * Shows location of bit for direction status of motor #M5 in 
 *   WickedMotorShield#second_shift_register.
 *
 * See #M1_DIR_MASK and #M1_BRAKE_MASK for more information.
 */
#define M5_DIR_MASK    (0x20)
/**
 * Shows location of bit for brake status of motor #M5 in 
 *   WickedMotorShield#second_shift_register.
 *
 * See #M1_DIR_MASK and #M1_BRAKE_MASK for more information.
 */
#define M5_BRAKE_MASK  (0x10)

/**
 * Digital pin used for specfying speed of motor M2.
 */
#define M2_PWM_PIN (9)
/**
 *  Digital pin used for specifying speed of motor M3.
 */
#define M3_PWM_PIN (5)
/**
 * Digital pin used for specifying speed of motor M4.
 */
#define M4_PWM_PIN (10)
/**
 * Digital pin used for specifying speed of motor M5.
 */
#define M5_PWM_PIN (6)

#define RCIN1      (1) 
#define RCIN2      (2)

#define SERIAL_CLOCK_PIN (2)
#define SERIAL_LATCH_PIN (7)

#define OPERATION_CLEAR  (0)
#define OPERATION_SET    (1)
#define OPERATION_NONE   (2)

#define USE_ALTERNATE_PINS (1)

class WickedMotorShield{
 private:
   static uint8_t first_shift_register;
   static uint8_t second_shift_register;
   static uint8_t SERIAL_DATA_PIN;
   static uint8_t RCIN1_PIN;
   static uint8_t RCIN2_PIN;
   static uint8_t get_rc_input_pin(uint8_t rc_input_number);
 protected:
    /* Digital pin to be used for setting PWM (pulse width modulation) duty cycle for motor M1.
     *
     * Value is different for standard and alternate pins.
     */
   static uint8_t M1_PWM_PIN;
    /* Digital pin to be used for setting PWM (pulse width modulation) duty cycle for motor M6.
     *
     * Value is different for standard and alternate pins.
     */
   static uint8_t M6_PWM_PIN;
   static uint8_t old_dir[6];
   uint8_t get_shift_register_value(uint8_t motor_number);   
   void apply_mask(uint8_t * shift_register_value, uint8_t mask, uint8_t operation);
   uint8_t filter_mask(uint8_t shift_register_value, uint8_t mask);
   void set_shift_register_value(uint8_t motor_number, uint8_t value);       
   void load_shift_register(void);    
   uint8_t get_motor_directionM(uint8_t motor_number);     
   uint8_t get_motor_brakeM(uint8_t motor_number);     
    
   void setSpeedM(uint8_t motor_number, uint8_t pwm_val);               // 0..255
   void setDirectionData(uint8_t motor_number, uint8_t direction);      // DIR_CCW, DIR_CW
   void setBrakeData(uint8_t motor_number, uint8_t brake_type);         // BRAKE_HARD, BRAKE_SOFT, BRAKE_OFF       
 public:
   WickedMotorShield(uint8_t use_alternate_pins = 0); // defaults for arduino uno                        
   static uint32_t getRCIN(uint8_t rc_input_number, uint32_t timeout = 0); // returns the result for pulseIn for the requested channel
   static uint8_t version(void);
};

class Wicked_Stepper : public WickedMotorShield{
 private:
    void stepMotor(int this_step);

    uint8_t direction;             // Direction of rotation
    uint16_t speed;                // Speed in RPMs
    uint32_t step_delay;           // delay between steps, in ms, based on speed
    uint16_t number_of_steps;      // total number of steps this motor can take
    uint16_t step_number;          // which step the motor is on
    uint32_t last_step_time;       // time stamp in ms of when the last step was taken
    uint8_t m1;                    // the M-number of the first coil
    uint8_t m2;                    // the M-number of the second coil

 public:
   Wicked_Stepper(uint16_t number_of_steps, uint8_t m1, uint8_t m2, uint8_t use_alternate_pins = 0);
   void setSpeed(uint32_t speed);
   void step(int16_t number_of_steps);
};


class Wicked_DCMotor : public WickedMotorShield {
 private:
   uint8_t get_motor_direction(void);  
   uint8_t motor_number;
 public:
   Wicked_DCMotor(uint8_t motor_number, uint8_t use_alternate_pins = 0);
   /**
    *   Set the speed of rotation as a value in the range 0..255.
    *   @param pwm_val Value for speed with an integer in the range 0 to 255.
    */
   void setSpeed(uint8_t pwm_val);            // 0..255
   /**
    *   Set the direction of rotation of the motor.
    */
   void setDirection(uint8_t direction);      // DIR_CCW, DIR_CW
   /**
    *   Set the type of braking for the motor.
    *   @param brake_type Options are BRAKE_HARD, BRAKE_SOFT, and BRAKE_OFF.
    *
    *   It is assumed that BRAKE_HARD refers to dynamic braking with the 
    *   leads on the motor connected together, BRAKE_SOFT refers to the 
    *   situation where no power flows through the motor or both leads
    *   are connected to ground, and BRAKE_OFF refers to power being 
    *   applied to DC motor.
    */
   void setBrake(uint8_t brake_type);         // BRAKE_HARD, BRAKE_SOFT, BRAKE_OFF
   uint16_t currentSense(void);
};

#endif /* _WICKED_MOTOR_SHIELD_H */

