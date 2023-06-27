#include <common.hpp>
#include <ui.hpp>
#define ICONS_IMPLEMENTATION
#include <fan_icons.hpp>
#define OPENSANS_REGULAR_IMPLEMENTATION
#include <fonts/OpenSans_Regular.hpp>
using namespace uix;
using namespace gfx;
const open_font& text_font = OpenSans_Regular;
uint8_t lcd_buffer1[lcd_buffer_size],
              lcd_buffer2[lcd_buffer_size];


fan_mode fan_op_mode = fan_mode::rpm;

screen_t main_screen(lcd_buffer_size,lcd_buffer1,lcd_buffer2);
label_t detect_fan_label(main_screen);
canvas_t icon_canvas(main_screen);
label_t value_label(main_screen);
canvas_t bars_canvas(main_screen);
canvas_t graph_canvas(main_screen);

graph_buffer_t graph;

static uint32_t graph_ts = 0;

static void on_paint_icon(canvas_t::control_surface_type& destination, 
                    const gfx::srect16& clip, 
                    void* state) {
        draw::icon(destination,spoint16::zero(),fan_op_mode==fan_mode::rpm?faFan:faWaveSquare,color_t::white,main_screen.background_color(),false,false,&clip);
}
static void on_paint_bars(canvas_t::control_surface_type& destination, 
                    const gfx::srect16& clip, 
                    void* state) {
        rgba_pixel<32> col = color32_t::white;
        col.template channel<channel_name::A>(95);                        
        draw::filled_rectangle(destination,srect16(0,destination.bounds().y1,destination.bounds().x2,destination.dimensions().height/2-1),col,&clip);
        float f;
        if(fan_op_mode==fan_mode::rpm) {
            f = fan.target_rpm()/fan.max_rpm();
        } else {
            f = (float)fan.pwm_duty()/65535.0f;
        }
        col.template channel<channel_name::A>(255);
        draw::filled_rectangle(destination,srect16(0,destination.bounds().y1,destination.bounds().x2*f,destination.dimensions().height/2-1),col,&clip);

        col = color32_t::yellow;
        col.template channel<channel_name::A>(95);                        
        draw::filled_rectangle(destination,srect16(0,destination.dimensions().height/2-1,destination.bounds().x2,destination.bounds().y2),col,&clip);
        f = fan.rpm()/fan.max_rpm();
        col.template channel<channel_name::A>(255);
        draw::filled_rectangle(destination,srect16(0,destination.dimensions().height/2-1,destination.bounds().x2*f,destination.bounds().y2),col,&clip);
}
static void on_paint_graph(canvas_t::control_surface_type& destination, 
                    const gfx::srect16& clip, 
                    void* state) {
    
    if(!graph.empty()) {
        spoint16 pt , pt2;
        uint8_t b = *graph.peek(0);
        float v = (float)b/255.0;
        pt.x = 0;
        pt.y = (destination.dimensions().height-1)-(v*(destination.dimensions().height-1));
        for(size_t i = 1; i < graph.size();++i) {
            b = *graph.peek(i);
            v = (float)b/255.0;
            pt2.x = i*(destination.dimensions().width/100);
            pt2.y = (destination.dimensions().height-1)-(v*(destination.dimensions().height-1));
            draw::line_aa(destination,srect16(pt,pt2),color_t::white,main_screen.background_color(),false,&clip);
            pt=pt2;
        }
    }

}

void update_graph() {
    if(millis()>=graph_ts+100) {
        graph_ts = millis();
        uint8_t val;
        if(graph.full()) {
            graph.get(&val);
        }
        val = (fan.rpm()/fan.max_rpm())*255;
        graph.put(val);
        graph_canvas.invalidate();
    }    
}
void main_screen_init() {
    main_screen.background_color(color_t::purple);
    if(fan_max_rpm!=fan_max_rpm) {
        detect_fan_label.text("detecting fan...");
    }
    rgba_pixel<32> bg;
    convert(main_screen.background_color(),&bg);
    rgba_pixel<32> transparent(0,0,0,0);
    uint16_t y = 5;
    icon_canvas.bounds(srect16(0,y,31,y+31));
    icon_canvas.on_paint(on_paint_icon);
    main_screen.register_control(icon_canvas);
    y=icon_canvas.bounds().y1;
    value_label.bounds(srect16(34,y,89,y+31));
    value_label.background_color(bg);
    value_label.border_color(bg);
    value_label.text_color(color32_t::white);
    value_label.text_open_font(&text_font);
    value_label.text_line_height(icon_canvas.dimensions().height-2);
    value_label.text_justify(uix_justify::center_right);
    value_label.padding({0,0});
    main_screen.register_control(value_label);
    bars_canvas.bounds(srect16(value_label.bounds().x2+2,value_label.bounds().y1,main_screen.bounds().x2-1,value_label.bounds().y1+31));
    bars_canvas.on_paint(on_paint_bars);
    main_screen.register_control(bars_canvas);
    graph_canvas.bounds(srect16(4,bars_canvas.bounds().y2+3,main_screen.dimensions().width-5,main_screen.dimensions().height-5));
    graph_canvas.on_paint(on_paint_graph);
    main_screen.register_control(graph_canvas);
    detect_fan_label.text_open_font(&text_font);
    detect_fan_label.text_line_height(main_screen.dimensions().height/4);
    detect_fan_label.text_justify(uix_justify::center);
    detect_fan_label.background_color(bg);
    detect_fan_label.text_color(color32_t::white);
    detect_fan_label.border_color(bg);
    detect_fan_label.bounds(main_screen.bounds());
    detect_fan_label.visible(true);
    main_screen.register_control(detect_fan_label);
}