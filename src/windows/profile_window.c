#include "profile_window.h"

#define KEY_PROFILE_INFO 0

static Window *s_window;
static TextLayer *s_info_text_layer;
static char profile_info_buffer[32];


static void inbox_received_callback(DictionaryIterator *iterator, void *context) {
  Tuple *profile_info_tuple = dict_find(iterator, KEY_PROFILE_INFO);
  APP_LOG(APP_LOG_LEVEL_DEBUG, "Got a Message!");

  // If all data is available, use it
  if(profile_info_tuple) {
    snprintf(profile_info_buffer, sizeof(profile_info_buffer), "%s", profile_info_tuple->value->cstring);
    text_layer_set_text(s_info_text_layer, profile_info_buffer);
    layer_mark_dirty(text_layer_get_layer(s_info_text_layer));
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

static void window_load(Window *window) {
  app_message_register_inbox_received(inbox_received_callback);
  app_message_register_inbox_dropped(inbox_dropped_callback);
  app_message_register_outbox_failed(outbox_failed_callback);
  app_message_register_outbox_sent(outbox_sent_callback);

  Layer *window_layer = window_get_root_layer(window);
  GRect bounds = layer_get_bounds(window_layer);

  const GEdgeInsets text_insets = {
    .top = (bounds.size.h / 2) - 24
  };
  s_info_text_layer = text_layer_create(grect_inset(bounds, text_insets));
  text_layer_set_text_color(s_info_text_layer, GColorBlack);
  text_layer_set_background_color(s_info_text_layer, GColorClear);
  text_layer_set_text_alignment(s_info_text_layer, GTextAlignmentCenter);
  text_layer_set_font(s_info_text_layer, fonts_get_system_font(FONT_KEY_GOTHIC_24_BOLD));
  text_layer_set_text(s_info_text_layer, "Loading...");
  layer_add_child(window_layer, text_layer_get_layer(s_info_text_layer));

  app_message_open(app_message_inbox_size_maximum(), app_message_outbox_size_maximum());
}

static void window_unload(Window *window) {
  window_destroy(s_window);
  text_layer_destroy(s_info_text_layer);
  s_window = NULL;
}

void profile_window_push() {
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