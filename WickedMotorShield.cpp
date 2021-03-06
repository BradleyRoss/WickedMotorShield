/**
 *  @mainpage
 *  This is the code for the motor shield by Wicked Device.
 *
 *  The following are my questions about the code.
 *  -  It is assumed that this code applies to both the MOTO4 (4 DC motors)
 *     and MOTO6 (6 DC motors).  Is this correct, or are any modifications
 *     required?
 *  -  What are hard brake and soft brake?  Does hard brake refer to
 *     dynamic braking where the leads of the DC motor are wired together?
 *     Does the setting of the pulse width modulation (PWM pin) affect the
 *     percentage of the time that the leads are connected?  Does soft
 *     brake mean that the leads to the DC motor are isolated from grounds
 *     and voltage sources.
 *  -  In Wicked_DCMotor#setBrake(), the value of the previous value of
 *     the direction bit should be copied to WickedMotorShield#old_dir only
 *     if the brake bit was previously clear (indicating BRAKE_OFF) and the BRAKE_HARD or
 *     BRAKE_SOFT condition is the new brake condition.  It appears that the
 *     WickedMotorShield#old_dir value is currently updated if going from
 *     BRAKE_SOFT to BRAKE_HARD or from BRAKE_HARD to BRAKE_SOFT.
 *  -  It appears that either the brake status should be set to #BRAKE_OFF
 *     when the direction is set or no action should be taken if the brake status
 *     is not #BRAKE_OFF.
 *  -  Are the alternate pins for use with the Arduino Mega?  If the alternate
 *     pins are to be used, how is this represented in the position of the
 *     the jumpers?  Is some other method used to indicate the use of
 *     alternate pin locations?
 *  -  Can I use the two RC in pins to provide for two way communication using I2C
 *     with another device such as a Bluetooth serial card?  Should the configuration
 *     of the pins be removed from the constructor since the motor shield library does
 *     not use them.  The pins would then be configured by the software that
 *     communicates with the outside world.
 *  @file
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

#include "WickedMotorShield.h"
/**
 * Clock pin used for loading shift registers.
 */
#define SERIAL_CLOCK_PIN (2)
/**
 *  Latch pin used for loading shift registers.
 */
#define SERIAL_LATCH_PIN (7)

#define OPERATION_CLEAR  (0)
#define OPERATION_SET    (1)
#define OPERATION_NONE   (2)
/**
 *  Contains direction and brake status for motors
 *  #M1, #M2, #M3, and #M4.
 */
uint8_t WickedMotorShield::first_shift_register = 0xff;
/**
 *  Contains direction and brake status for motors
 *  #M5 and #M6.
 */
uint8_t WickedMotorShield::second_shift_register = 0xff;
/**
 *  Digital Arduino pin used to send data to motor shield.
 *
 *  Pin 12 for standard pins, pin 0 for alternate pins.
 */
uint8_t WickedMotorShield::SERIAL_DATA_PIN = 12;
/**
 *   Digital pin used for Radio Control Input Pin 1.
 *
 *   Pin 4 used for standard pins, pin 3 for alternate pins.
 */
uint8_t WickedMotorShield::RCIN1_PIN = 4;
/**
 *  Digital pin used for Radio Control Input Pin 2.
 *
 *  Pin 8 used for standard pins, pin 11 for alternate pins.
 */
uint8_t WickedMotorShield::RCIN2_PIN = 8;
/**
 * Digital pin for M1 PWM (pulse width modulation).
 *
 * Pin 11 for standard pins, use pin 8 for alternate pins.
 */
uint8_t WickedMotorShield::M1_PWM_PIN = 11;
/**
 *  Digital pin for M6 PWM (pulse width modulation).
 *
 *  Pin 3 for standard pins, pin 4 for alternate pins.
 */
uint8_t WickedMotorShield::M6_PWM_PIN = 3;
/**
 *  Contains the old value of the direction bit if the
 *  value of the brake bit is changed from 0 to 1.
 *
 *  When the brake bit is changed from 0 to 1, the
 *  direction bit indicates whether it is BRAKE_SOFT or
 *  BRAKE_HARD.  If the direction bit wasn't copied to
 *  this array, the value would be lost.
 */
uint8_t WickedMotorShield::old_dir[6] = {0,0,0,0,0,0};

/**
 * Constructor for WickedMotorShield, which has Wicked_DCMotor and
 * Wicked_Stepper as subclasses.
 *
 * @param use_alternate_pins if the value is equal to #USE_ALTERNATE_PINS,
 *        the values used for WickedMotorShield#SERIAL_DATA_PIN,
 *        WickedMotorShield#RCIN1_PIN, WickedMotorShield#RCIN2_PIN,
 *        WickedMotorShield#M1_PWM_PIN, and WickedMotorShield#M6_PWM_PIN
 *        changed to an alternate set.
 *
 * <table>
 * <tr><td>Symbol</td><td>Standard</td><td>Alternate</td></tr>
 * <tr><td>WickedMotorShield#SERIAL_DATA_PIN</td><td>12</td><td>0</td></tr>
 * <tr><td>WickedMotorShield#RCIN1_PIN</td><td>4</td><td>3</td></tr>
 * <tr><td>WickedMotorShield#RCIN2_PIN</td><td>8</td><td>11</td></tr>
 * <tr><td>WickedMotorShield#M1_PWM_PIN</td><td>11</td><td>8</td></tr>
 * <tr><td>#M2_PWM_PIN</td><td>9</td><td>9</td></tr>
 * <tr><td>#M3_PWM_PIN</td><td>5</td><td>5</td></tr>
 * <tr><td>#M4_PWM_PIN</td><td>10</td><td>10</td></tr>
 * <tr><td>#M5_PWM_PIN</td><td>6</td><td>6</td></tr>
 * <tr><td>WickedMotorShield#M6_PWM_PIN</td><td>3</td><td>4</td></tr>
 * </table>
 *
 * <p>Pins 4 and 8 do not support PWM on Arduino Uno R3 microcontroller board.
 *    However, all of these pins support PWM on the
 *    <a href="https://www.electroschematics.com/arduino-mega-2560-pinout/" target="_blank">
 *    Arduino Mega</a></p>
 */
WickedMotorShield::WickedMotorShield(uint8_t use_alternate_pins){

  if( use_alternate_pins == USE_ALTERNATE_PINS){
    WickedMotorShield::SERIAL_DATA_PIN = 0;
    WickedMotorShield::RCIN1_PIN = 3;
    WickedMotorShield::RCIN2_PIN = 11;
    WickedMotorShield::M1_PWM_PIN = 8;
    WickedMotorShield::M6_PWM_PIN = 4;
  }

  // intialize pins
  pinMode(SERIAL_CLOCK_PIN, OUTPUT);
  pinMode(SERIAL_LATCH_PIN, OUTPUT);
  pinMode(SERIAL_DATA_PIN, OUTPUT);

  pinMode(RCIN1_PIN, INPUT);
  pinMode(RCIN2_PIN, INPUT);

  for(uint8_t ii = 0; ii < 6; ii++){
    old_dir[ii] = DIR_CW; // initial direction coming out of brake is clockwise
  }

  // load the initial values so the motors are set to a brake state initially
  load_shift_register();
}
/**
 *  Load the contents of second_shift_register and first_shift_register to the
 *  motor shield using SERIAL_LATCH_PIN, SERIAL_DATA_PIN, and SERIAL_CLOCK_PIN
 *  pins.
 *
 *  Data is only moved from the memory values on the Arduino board to the
 *  motor shield.  No data is moved in the other direction.
 */
void WickedMotorShield::load_shift_register(void){
  digitalWrite(SERIAL_LATCH_PIN, LOW);
  shiftOut(SERIAL_DATA_PIN, SERIAL_CLOCK_PIN, LSBFIRST, second_shift_register);
  shiftOut(SERIAL_DATA_PIN, SERIAL_CLOCK_PIN, LSBFIRST, first_shift_register);
  digitalWrite(SERIAL_LATCH_PIN, HIGH);
}
/**
 *  Get the shift register information for a specific motor.
 *  @param motor_number  Number of motor for which information is desired.
 *
 *  The information for motors M1 to M4 are contained in the first shift
 *  register.  The information for motors M5 and M6 is contained in the
 *  second shift register.
 */
uint8_t WickedMotorShield::get_shift_register_value(uint8_t motor_number){
  uint8_t temp = first_shift_register;
  if(motor_number == M5 || motor_number == M6){
    temp = second_shift_register;
  }

  return temp;
}
/**
 *  Copy the shift register data for the specified motor into the correct data structure.
 *
 *  @param  motor_number number of motor for which data has been modified.
 *  @param  value  information to be moved to shift register.
 */
void WickedMotorShield::set_shift_register_value(uint8_t motor_number, uint8_t value){
  if(motor_number == M5 || motor_number == M6){
    second_shift_register = value;
  }
  else{
    first_shift_register = value;
  }
}

/**
  * Carry out bitwise or/and operation .
  * @param shift_register_value Address of shift register to be altered.
  *     This will be the address of either WickedDeviceShield#first_shift_register
  *     or WickedDeviceShield#second_shift_register.  Only the bit whose location is
  *     given by the mask parameter will be altered.
  * @param mask with bit set in position to be changed.
  * @param operation flag indicating whether bit to be set (#OPERATION_SET)
  *     or cleared (#OPERATION_CLEAR).  A value of #OPERATION_NONE indicates
  *     that no action is to be taken.
  *
  * If operation clear do a bitwise and operation on the shift register byte
  * and the mask with all bits inverted.  In the inverted mask, the bit to be cleared
  * is 0 and the other bits are 1.
  *
  * If operation set do a bitwise or operation on the shift register byte and the
  * mask.  The mask has a one in the bit of the shift register to be set to be set.
  */
void WickedMotorShield::apply_mask(uint8_t * shift_register_value, uint8_t mask, uint8_t operation){
  switch(operation){
  case OPERATION_CLEAR:
    *shift_register_value &= ~mask;
    break;
  case OPERATION_SET:
    *shift_register_value |= mask;
    break;
  case OPERATION_NONE:
    // do nothing
    break;
  }
}
/**
 *  Apply a mask to an eight bit integer using bitwise and.
 *  @param shift_register_value byte to which mask is to be applied
 *  @param mask byte containing mask to be applied
 *  @return 0 if bit indicated by mask is 0, otherwise return 1
 */
uint8_t WickedMotorShield::filter_mask(uint8_t shift_register_value, uint8_t mask){
  if((shift_register_value & mask) == 0){
    return 0;
  }

  // else
  return 1;
}

uint32_t WickedMotorShield::getRCIN(uint8_t rc_input_number, uint32_t timeout){

  uint8_t rc_input_pin = get_rc_input_pin(rc_input_number);
  if(rc_input_pin == 0xff){
    return 0xffffffff; //invalid RCIN number
  }

  if(timeout == 0){
    return pulseIn(rc_input_pin, HIGH);
  }

  //else
  return pulseIn(rc_input_pin, HIGH, timeout);
}

uint8_t WickedMotorShield::get_rc_input_pin(uint8_t rc_input_number){
  if(rc_input_number == RCIN1){
    return RCIN1_PIN;
  }
  else if(rc_input_number == RCIN2){
    return RCIN2_PIN;
  }

  //else
  return 0xff;
}

// for pwm value use a value between 0 and 255
void WickedMotorShield::setSpeedM(uint8_t motor_number, uint8_t pwm_val){
  switch(motor_number){
  case M1:
    analogWrite(M1_PWM_PIN, pwm_val);
    break;
  case M2:
    analogWrite(M2_PWM_PIN, pwm_val);
    break;
  case M3:
    analogWrite(M3_PWM_PIN, pwm_val);
    break;
  case M4:
    analogWrite(M4_PWM_PIN, pwm_val);
    break;
  case M5:
    analogWrite(M5_PWM_PIN, pwm_val);
    break;
  case M6:
    analogWrite(M6_PWM_PIN, pwm_val);
    break;
  }
}
/**
 * Set the value for the direction in Mx_DIR_MASK bit and the element of the
 * old_dir directory.
 * @param motor_number number of motor for which direction is to be set
 * @param direction value to be set for direction (#DIR_CW or #DIR_CCW).  The
 *        value of DIR_CW is 1 while the value for DIR_CCW is set to 0.
 *
 * Change Jan, 2020 - No action to be taken if brake bit is set.
 *
 * This needs to be reviewed.  Either the brake status should be set to #BRAKE_OFF
 * when the direction is set or no action should be taken if the brake status
 * is not #BRAKE_OFF.  Should the brake status always be changed to BRAKE_OFF
 * if this method is called.
 */
void WickedMotorShield::setDirectionData(uint8_t motor_number, uint8_t direction){
  uint8_t shift_register_value = get_shift_register_value(motor_number);
  uint8_t * p_shift_register_value = &shift_register_value;
  uint8_t dir_operation   = OPERATION_NONE;
  uint8_t brake_status = getMotorBrakeM(motor_number);


  if(motor_number >= 6){
    return; // invalid motor_number, go no further
  }
  if (brake_status > 0) { return; }
  //TODO: is this the "correct" sense of DIR_CW / DIR_CCW
  // this explicitly becomes the old direction value
  if(direction == DIR_CW){
    dir_operation = OPERATION_SET;
    old_dir[motor_number] = 1;
  }
  else if(direction == DIR_CCW){
    dir_operation = OPERATION_CLEAR;
    old_dir[motor_number] = 0;
  }

  switch(motor_number){
  case M1:
    apply_mask(p_shift_register_value, M1_DIR_MASK, dir_operation);
    break;
  case M2:
    apply_mask(p_shift_register_value, M2_DIR_MASK, dir_operation);
    break;
  case M3:
    apply_mask(p_shift_register_value, M3_DIR_MASK, dir_operation);
    break;
  case M4:
    apply_mask(p_shift_register_value, M4_DIR_MASK, dir_operation);
    break;
  case M5:
    apply_mask(p_shift_register_value, M5_DIR_MASK, dir_operation);
    break;
  case M6:
    apply_mask(p_shift_register_value, M6_DIR_MASK, dir_operation);
    break;
  }

  set_shift_register_value(motor_number, shift_register_value);
}
/**
 * Set the contents of the shift_registers to indicate the desired brake condition.
 * @param motor_number number of motor for which data is to be set
 * @param brake_type value is #BRAKE_OFF, BRAKE_HARD, or #BRAKE_SOFT.  Value  is
 *                   the new brake condition.
 *
 * Change Jan, 2020 :  The value of #WickedMotorShield#old_dir is only changed if brake status is
 *     changed from #BRAKE_OFF to (#BRAKE_HARD or #BRAKE_SOFT) or from
 *     (#BRAKE_HARD or #BRAKE_SOFT) to #BRAKE_OFF.
 */
void WickedMotorShield::setBrakeData(uint8_t motor_number, uint8_t brake_type){
  uint8_t shift_register_value = get_shift_register_value(motor_number);
  uint8_t * p_shift_register_value = &shift_register_value;
  uint8_t brake_operation = OPERATION_NONE;
  uint8_t dir_operation   = OPERATION_NONE;

  if(motor_number >= 6){
    return; // invalid motor_number, go no further
  }

  // calculate the affect on the relevant shift register bits
  //  TODO: Merging this if/else with next if/else would make code more readable
  if(brake_type == BRAKE_OFF){
    brake_operation = OPERATION_CLEAR;
    dir_operation = OPERATION_NONE;
  }
  else if(brake_type == BRAKE_SOFT){
    brake_operation = OPERATION_SET;
    dir_operation = OPERATION_CLEAR;

  }
  else if(brake_type == BRAKE_HARD){
    brake_operation = OPERATION_SET;
    dir_operation = OPERATION_SET;
  }
  uint8_t brake_status = getMotorBrakeM(motor_number);

  // save / restore directionality
  // we already know motor_number is a safe index into old_dir because we checked earlier
  if(brake_type == BRAKE_OFF && brake_status > 0){
    // when clearing the brake, restore the old_dir value
    if(old_dir[motor_number] == 1){
      dir_operation = OPERATION_SET;
    }
    else{
      dir_operation = OPERATION_CLEAR;
    }
  }
  else if(brake_status == 0 && ((brake_type == BRAKE_SOFT) || (brake_type == BRAKE_HARD))){
    // when applying the brake, save the old_dir value
    old_dir[motor_number] = get_motor_directionM(motor_number);
  }

  switch(motor_number){
  case M1:
    apply_mask(p_shift_register_value, M1_BRAKE_MASK, brake_operation);
    apply_mask(p_shift_register_value, M1_DIR_MASK, dir_operation);
    break;
  case M2:
    apply_mask(p_shift_register_value, M2_BRAKE_MASK, brake_operation);
    apply_mask(p_shift_register_value, M2_DIR_MASK, dir_operation);
    break;
  case M3:
    apply_mask(p_shift_register_value, M3_BRAKE_MASK, brake_operation);
    apply_mask(p_shift_register_value, M3_DIR_MASK, dir_operation);
    break;
  case M4:
    apply_mask(p_shift_register_value, M4_BRAKE_MASK, brake_operation);
    apply_mask(p_shift_register_value, M4_DIR_MASK, dir_operation);
    break;
  case M5:
    apply_mask(p_shift_register_value, M5_BRAKE_MASK, brake_operation);
    apply_mask(p_shift_register_value, M5_DIR_MASK, dir_operation);
    break;
  case M6:
    apply_mask(p_shift_register_value, M6_BRAKE_MASK, brake_operation);
    apply_mask(p_shift_register_value, M6_DIR_MASK, dir_operation);
    break;
  }

  set_shift_register_value(motor_number, shift_register_value);
}
/**
 * Return motor direction for a specific motor.
 * @param motor_number integer representing motor
 * @return shift register value for motor having done a bitwise
 *         and operation with the direction mask for the motor.  Value will
 *         zero if direction bit is set (counter clockwise).
 *         Otherwise it will be greater than zero
 *         if direction bit is not set (clockwise).
 */
uint8_t WickedMotorShield::get_motor_directionM(uint8_t motor_number){
  uint8_t shift_register_value = get_shift_register_value(motor_number);

  switch(motor_number){
  case M1:
    return filter_mask(shift_register_value, M1_DIR_MASK);
  case M2:
    return filter_mask(shift_register_value, M2_DIR_MASK);
  case M3:
    return filter_mask(shift_register_value, M3_DIR_MASK);
  case M4:
    return filter_mask(shift_register_value, M4_DIR_MASK);
  case M5:
    return filter_mask(shift_register_value, M5_DIR_MASK);
  case M6:
    return filter_mask(shift_register_value, M6_DIR_MASK);
  }

  return 0xff; // indicate error - bad motor_number argument

}
/**
 * Return motor brake status for a specific motor.
 * @param motor_number integer representing motor
 * @return shift register value for motor having done a bitwise
 *         and operation with the brake mask for the motor.  This
 *         will be zero if the brake bit is not set and greater
 *         than zero if the brake bit is set.
 */
uint8_t WickedMotorShield::get_motor_brakeM(uint8_t motor_number){
  uint8_t shift_register_value = get_shift_register_value(motor_number);

  switch(motor_number){
  case M1:
    return filter_mask(shift_register_value, M1_BRAKE_MASK);
  case M2:
    return filter_mask(shift_register_value, M2_BRAKE_MASK);
  case M3:
    return filter_mask(shift_register_value, M3_BRAKE_MASK);
  case M4:
    return filter_mask(shift_register_value, M4_BRAKE_MASK);
  case M5:
    return filter_mask(shift_register_value, M5_BRAKE_MASK);
  case M6:
    return filter_mask(shift_register_value, M6_BRAKE_MASK);
  }

  return 0xff; // indicate error - bad motor_number argument

}

/**
 *  Version number is hard-coded to a value of 1.
 */
uint8_t WickedMotorShield::version(void){
  return 1;
}



Wicked_Stepper::Wicked_Stepper(uint16_t number_of_steps, uint8_t m1, uint8_t m2, uint8_t use_alternate_pins)
  :WickedMotorShield(use_alternate_pins){

  this->step_number = 0;                      // which step the motor is on
  this->speed = 0;                            // the motor speed, in revolutions per minute
  this->direction = 0;                        // motor direction
  this->last_step_time = 0;                   // time stamp in ms of the last step taken
  this->number_of_steps = number_of_steps;    // total number of steps for this motor

  this->m1 = m1;
  this->m2 = m2;

  setSpeedM(m1, 255);
  setSpeedM(m2, 255);
  setDirectionData(m1, DIR_CW);
  setDirectionData(m2, DIR_CW);
  setBrakeData(m1, BRAKE_OFF);
  setBrakeData(m2, BRAKE_OFF);
  load_shift_register();
}

void Wicked_Stepper::setSpeed(uint32_t speed){
  this->step_delay = 60L * 1000L / this->number_of_steps / speed;
}

void Wicked_Stepper::step(int16_t number_of_steps){
  int steps_left = abs(number_of_steps);  // how many steps to take

  // determine direction based on whether steps_to_mode is + or -:
  if (number_of_steps > 0) {this->direction = 1;}
  if (number_of_steps < 0) {this->direction = 0;}


  // decrement the number of steps, moving one step each time:
  while(steps_left > 0) {
  // move only if the appropriate delay has passed:
  if (millis() - this->last_step_time >= this->step_delay) {
      // get the timeStamp of when you stepped:
      this->last_step_time = millis();
      // increment or decrement the step number,
      // depending on direction:
      if (this->direction == 1) {
        this->step_number++;
        if (this->step_number == this->number_of_steps) {
          this->step_number = 0;
        }
      }
      else {
        if (this->step_number == 0) {
          this->step_number = this->number_of_steps;
        }
        this->step_number--;
      }
      // decrement the steps left:
      steps_left--;
      // step the motor to step number 0, 1, 2, or 3:
      stepMotor(this->step_number % 4);
    }
  }
}

//TODO: convert the code below into analogous shift register loads
void Wicked_Stepper::stepMotor(int this_step){
  switch (this_step) {
  case 0:    // 1010
    setDirectionData(m1, DIR_CW);
    setDirectionData(m2, DIR_CCW);
    break;
  case 1:    // 0110
    setDirectionData(m1, DIR_CCW);
    setDirectionData(m2, DIR_CCW);
    break;
  case 2:    // 0101
    setDirectionData(m1, DIR_CCW);
    setDirectionData(m2, DIR_CW);
    break;
  case 3:    // 1001
    setDirectionData(m1, DIR_CW);
    setDirectionData(m2, DIR_CW);
    break;
  }

  load_shift_register();
}


Wicked_DCMotor::Wicked_DCMotor(uint8_t motor_number, uint8_t use_alternate_pins)
  :WickedMotorShield(use_alternate_pins){

  this->motor_number = motor_number;
}

// for direction use one of the symbols: DIR_CW, DIR_CC
void Wicked_DCMotor:: setDirection(uint8_t direction){
  setDirectionData(motor_number, direction);
  load_shift_register();
}

// for brake_type use one of the symbols: HARD, SOFT, OFF
void Wicked_DCMotor::setBrake(uint8_t brake_type){
  setBrakeData(motor_number, brake_type);
  load_shift_register();
}

uint8_t Wicked_DCMotor::get_motor_direction(void){
  return get_motor_directionM(motor_number);
}

uint16_t Wicked_DCMotor::currentSense(void){
  switch(motor_number){
  case M1:
    return analogRead(A0);
  case M2:
    return analogRead(A2);
  case M3:
    return analogRead(A1);
  case M4:
    return analogRead(A3);
  case M5:
    return analogRead(A4);
  case M6:
    return analogRead(A5);
  }

  return 0xffff; // indicate error - bad motor_number argument
}

void Wicked_DCMotor::setSpeed(uint8_t pwm_val){
  setSpeedM(motor_number, pwm_val);
}
