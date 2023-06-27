#include <common.hpp>
#define LCD_IMPLEMENTATION
#include <lcd_init.h>
using namespace arduino;
using namespace gfx;
using namespace uix;

static char value_text[64];
static float old_value = NAN;
constexpr static const float pwm_mult = 6535.5f;
static long long old_knob = 0;
static bool update_label = true;
static bool lcd_flush_ready(esp_lcd_panel_io_handle_t panel_io,
                            esp_lcd_panel_io_event_data_t* edata,
                            void* user_ctx) {
    main_screen.set_flush_complete();
    return true;
}
static void uix_flush(const rect16& bounds,
                      const void* bmp,
                      void* state) {
    lcd_panel_draw_bitmap(bounds.x1,
                          bounds.y1,
                          bounds.x2,
                          bounds.y2,
                          (void*)bmp);
}
void button_on_click(int clicks,void* state) {
  fan_op_mode = (fan_mode)((((int)fan_op_mode)+(clicks&1))&1);
  update_label = true;
  float target;
  if(fan_op_mode==fan_mode::rpm) {
    target = fan.pwm_duty()/65535.0f;
    fan.rpm(target*fan.max_rpm());
    knob.position(target*fan.max_rpm()/25.0f);
  } else {
    if(fan.target_rpm()!=fan.target_rpm()) {
      target = fan.rpm()/fan.max_rpm();
    } else {
      target = fan.target_rpm()/fan.max_rpm();
    }
    knob.position(target*400);
  }
  icon_canvas.invalidate();
  bars_canvas.invalidate();

}
void setup() {
    main_screen.on_flush_callback(uix_flush);
    main_screen_init();
    lcd_panel_init(lcd_buffer_size, lcd_flush_ready);
    main_screen.update();
    init_devices();
    button_a.on_click(button_on_click);
    button_b.on_click(button_on_click);
    knob.position(0);
    if (fan.initialized()) {
        fan.pwm_duty(0);
    }
    main_screen.update();
    delay(1000);
    detect_fan_label.visible(false);
}

void loop() {
    bool update = update_label;
    update_label = false;
    main_screen.update();
    if (!fan.initialized()) {
        return;
    }
    update_devices();
    update_graph();
    if(knob.position()<0) {
      knob.position(0);
    }
    if (fan_op_mode == fan_mode::pwm) {
        if(knob.position()>400) {
          knob.position(400);
        }
        if (knob.position() > old_knob) {
            update = true;
            fan.pwm_duty((knob.position()/4)*655.35f);
            old_knob = knob.position();
        } else if (knob.position() < old_knob) {
            update = true;
            fan.pwm_duty((knob.position()/4)*655.35f);
            old_knob = knob.position();
        }
        if (update) {
            snprintf(value_text, sizeof(value_text), "%d%%", (int)(fan.pwm_duty()/655.35f));
            value_label.text(value_text);
            bars_canvas.invalidate();
        }
    } else {
        const float fan_max = fan.max_rpm()/25.0f;
        if(knob.position()>fan_max) {
          knob.position((int)(fan_max/25)*25);
        }
        if (knob.position() > old_knob) {
            update = true;
            fan.rpm(knob.position()*25);
            old_knob = knob.position();
        } else if (knob.position() < old_knob) {
            update = true;
            fan.rpm(knob.position()*25);
            old_knob = knob.position();
        }
        if (update) {
            float f = (fan.target_rpm()!=fan.target_rpm())?fan.rpm():fan.target_rpm();
            snprintf(value_text, sizeof(value_text), "%d", (int)(f + .5));
            value_label.text(value_text);
        }
    }
    bars_canvas.invalidate();
}
