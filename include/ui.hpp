#pragma once
#include <stdint.h>
#include <lcd_config.h>
#include <uix.hpp>
#include <gfx.hpp>
#include <circular_buffer.hpp>
using screen_t = uix::screen<LCD_WIDTH,LCD_HEIGHT,gfx::rgb_pixel<LCD_BIT_DEPTH>>;
using label_t = uix::label<screen_t::control_surface_type>;
using canvas_t = uix::canvas<screen_t::control_surface_type>;
using color_t = gfx::color<typename screen_t::pixel_type>;
using color32_t = gfx::color<gfx::rgba_pixel<32>>;

using graph_buffer_t = circular_buffer<uint8_t,100>;

constexpr static const size_t lcd_buffer_size = 32*1024;
extern uint8_t lcd_buffer1[],
              lcd_buffer2[];


extern screen_t main_screen;
extern label_t detect_fan_label;
extern canvas_t icon_canvas;
extern label_t value_label;
extern canvas_t bars_canvas;
extern canvas_t graph_canvas;

extern graph_buffer_t graph;

extern void update_graph();
extern void main_screen_init();