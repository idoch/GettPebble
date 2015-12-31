#include <pebble.h>

#include "windows/order_routing_window.h"
#include "windows/profile_window.h"

#define NUM_MENU_SECTIONS 1
#define NUM_MENU_ICONS 0
#define NUM_FIRST_MENU_ITEMS 2

static Window *s_window;
static Window *s_order_window;
static MenuLayer *s_menu_layer;

static uint16_t menu_get_num_sections_callback(MenuLayer *menu_layer, void *data) {
  return NUM_MENU_SECTIONS;
}

static uint16_t menu_get_num_rows_callback(MenuLayer *menu_layer, uint16_t section_index, void *data) {
  switch (section_index) {
    case 0:
      return NUM_FIRST_MENU_ITEMS;
    default:
      return 0;
  }
}

static int16_t menu_get_header_height_callback(MenuLayer *menu_layer, uint16_t section_index, void *data) {
  return MENU_CELL_BASIC_HEADER_HEIGHT;
}

static void menu_draw_header_callback(GContext *ctx, const Layer *cell_layer, uint16_t section_index, void *data) {
  switch (section_index) {
    case 0: 
      menu_cell_basic_header_draw(ctx, cell_layer, "");
      break;
  }
}

static void menu_draw_row_callback(GContext *ctx, const Layer *cell_layer, MenuIndex *cell_index, void *data) {
  switch (cell_index->section) {
    case 0:
      switch(cell_index->row) {
        case 0:
          menu_cell_basic_draw(ctx, cell_layer, "Order a Ride", "find taxi now...", NULL);
          break;
        case 1:
          menu_cell_basic_draw(ctx, cell_layer, "My Account Details", "", NULL);
          break;
      }
  }
}

static void menu_select_click_callback(MenuLayer *menu_layer, MenuIndex *cell_index, void *data) {
  switch(cell_index->row) {
    case 0:
      order_window_push();
      break;
    case 1: 
      profile_window_push();
      break;
  }
}

static void window_load(Window *window) {
  Layer *s_window_layer = window_get_root_layer(window);
  GRect bounds = layer_get_bounds(s_window_layer);
  s_menu_layer = menu_layer_create(bounds);
  menu_layer_set_normal_colors(s_menu_layer, GColorYellow, GColorBlack);


  menu_layer_set_callbacks(s_menu_layer, NULL, (MenuLayerCallbacks) {
    .get_num_sections = menu_get_num_sections_callback,
    .get_num_rows = menu_get_num_rows_callback,
    .get_header_height = menu_get_header_height_callback,
    .draw_header = menu_draw_header_callback,
    .draw_row = menu_draw_row_callback,
    .select_click = menu_select_click_callback
  });
  menu_layer_set_click_config_onto_window(s_menu_layer, window);
  layer_add_child(s_window_layer, menu_layer_get_layer(s_menu_layer));
}

static void window_unload(Window *window) {
  menu_layer_destroy(s_menu_layer);
}

static void init() {
  s_window = window_create();
  window_set_window_handlers(s_window, (WindowHandlers) {
    .load = window_load,
    .unload = window_unload,
  });
  const bool animated = true;
  window_stack_push(s_window, animated);
}

static void deinit() {
  window_destroy(s_window);
}

int main() {
  init();
  app_event_loop();
  deinit();
}