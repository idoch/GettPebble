#include "profile_window.h"

 
#define MAX_MESSAGE_SIZE_IN 90
#define MAX_MESSAGE_SIZE_OUT 90
#define KEY_PROFILE_INFO_NAME 0
#define KEY_PROFILE_INFO_POINTS 1

static Window *s_window;
static TextLayer *s_info_name_text_layer;
static TextLayer *s_info_points_text_layer;
static char profile_info_points_buffer[32];


static void inbox_received_callback(DictionaryIterator *iterator, void *context) {
  APP_LOG(APP_LOG_LEVEL_DEBUG, "Got a Message!");
  Tuple *profile_info_points_tuple = dict_find(iterator, KEY_PROFILE_INFO_POINTS);
  Tuple *profile_info_name_tuple = dict_find(iterator, KEY_PROFILE_INFO_NAME);

  // If all data is available, use it
  if(profile_info_name_tuple) {
    text_layer_set_text(s_info_name_text_layer, profile_info_name_tuple->value->cstring);
  }
  if(profile_info_points_tuple) {
    snprintf(profile_info_points_buffer, sizeof(profile_info_points_buffer), "%d", profile_info_points_tuple->value->uint16);
    text_layer_set_text(s_info_points_text_layer, profile_info_points_buffer);
  }
}

static void inbox_dropped_callback(AppMessageResult reason, void *context) {
  APP_LOG(APP_LOG_LEVEL_ERROR, "Message dropped!");
}

static void outbox_failed_callback(DictionaryIterator *iterator, AppMessageResult reason, void *context) {
  APP_LOG(APP_LOG_LEVEL_ERROR, "Outbox send failed!");
}

static void outbox_sent_callback(DictionaryIterator *iterator, void *context) {
  APP_LOG(APP_LOG_LEVEL_INFO, "Outbox send success!");
}

static void get_profile_info() {
  DictionaryIterator *iter;
  app_message_outbox_begin(&iter);
  dict_write_uint8(iter, 0, 0);

  app_message_outbox_send();
}

static void window_load(Window *window) {
  Layer *window_layer = window_get_root_layer(window);
  GRect bounds = layer_get_bounds(window_layer);

  const GEdgeInsets name_text_insets = {
    .top = (bounds.size.h / 2) - 24
  };
  s_info_name_text_layer = text_layer_create(grect_inset(bounds, name_text_insets));
  text_layer_set_text_color(s_info_name_text_layer, GColorBlack);
  text_layer_set_background_color(s_info_name_text_layer, GColorClear);
  text_layer_set_text_alignment(s_info_name_text_layer, GTextAlignmentCenter);
  text_layer_set_font(s_info_name_text_layer, fonts_get_system_font(FONT_KEY_GOTHIC_24_BOLD));
  text_layer_set_text(s_info_name_text_layer, "Loading...");
  layer_add_child(window_layer, text_layer_get_layer(s_info_name_text_layer));
  
  GRect name_text_layer_bounds = layer_get_bounds(text_layer_get_layer(s_info_name_text_layer));
  const GEdgeInsets points_text_insets = {
    .top = name_text_layer_bounds.size.h + 6
  };
  s_info_points_text_layer = text_layer_create(grect_inset(bounds, points_text_insets));
  text_layer_set_text_color(s_info_points_text_layer, GColorBlack);
  text_layer_set_background_color(s_info_points_text_layer, GColorClear);
  text_layer_set_text_alignment(s_info_points_text_layer, GTextAlignmentCenter);
  text_layer_set_font(s_info_points_text_layer, fonts_get_system_font(FONT_KEY_GOTHIC_24_BOLD));
  text_layer_set_text(s_info_points_text_layer, "");
  layer_add_child(window_layer, text_layer_get_layer(s_info_points_text_layer));
  get_profile_info();
}

static void window_unload(Window *window) {
  window_destroy(s_window);
  text_layer_destroy(s_info_name_text_layer);
  text_layer_destroy(s_info_points_text_layer);
  s_window = NULL;
}

static void assign_inbox_callbacks() {
  app_message_register_inbox_received(inbox_received_callback);
  app_message_register_inbox_dropped(inbox_dropped_callback);
  app_message_register_outbox_failed(outbox_failed_callback);
  app_message_register_outbox_sent(outbox_sent_callback);
}

void profile_window_push() {
  assign_inbox_callbacks();
  app_message_open(MAX_MESSAGE_SIZE_IN, MAX_MESSAGE_SIZE_OUT);
  if (!s_window) {
    s_window = window_create();
    window_set_background_color(s_window, GColorYellow);
    window_set_window_handlers(s_window, (WindowHandlers) {
      .load = window_load,
      .unload = window_unload
    });
  }

  window_stack_push(s_window, true);
}