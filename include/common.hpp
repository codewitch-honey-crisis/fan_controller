#pragma once
#include <Arduino.h>
#include <ui.hpp>
#include <button.hpp>
#include <encoder.hpp>
#include <fan_controller.hpp>

constexpr const uint8_t pin_button_a = 35;
constexpr const uint8_t pin_button_b = 0;
constexpr const uint8_t pin_encoder_data = 17;
constexpr const uint8_t pin_encoder_clk = 13;
constexpr const uint8_t pin_fan_tach = 33;
constexpr const uint8_t pin_fan_pwm = 32;
constexpr const float fan_max_rpm = NAN;

enum struct fan_mode {
    rpm = 0,
    pwm = 1
};

using button_a_raw_t = arduino::int_button<pin_button_a,10,true>;
using button_b_raw_t = arduino::int_button<pin_button_b,10,true>;
using button_t = arduino::multi_button;
using encoder_t = arduino::int_encoder<pin_encoder_data,pin_encoder_clk,true>;

extern fan_mode fan_op_mode;

extern button_a_raw_t button_a_raw;
extern button_b_raw_t button_b_raw;
extern button_t button_a;
extern button_t button_b;
extern  encoder_t knob;
extern arduino::fan_controller fan;

extern void init_devices();
extern void update_devices();