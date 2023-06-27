#include <common.hpp>
button_a_raw_t button_a_raw;
button_b_raw_t button_b_raw;
button_t button_a(button_a_raw);
button_t button_b(button_b_raw);
encoder_t knob;
arduino::fan_controller fan(
    [](uint16_t duty,void* state){ ledcWrite(0,duty>>8); },
    nullptr,
    pin_fan_tach, fan_max_rpm,2);

static char fan_detect_label_text[64];

void init_devices() {
    Serial.begin(115200);
    button_a.initialize();
    button_b.initialize();
    knob.initialize();
    ledcSetup(0,25*1000,8);
    ledcAttachPin(pin_fan_pwm,0);
    if(!fan.initialize()) {
        detect_fan_label.text("fan not found");
    } else {
        snprintf(fan_detect_label_text,sizeof(fan_detect_label_text),"max rpm: %d",(int)fan.max_rpm());
        detect_fan_label.text(fan_detect_label_text);
    }
}
void update_devices() {
    button_a.update();
    button_b.update();
    fan.update();
}