#include "order_routing_window.h"

static Window *s_window;
static Layer *s_circle_layer;
static TextLayer *s_text_layer;
static ProgressLayer *s_progress_layer;

static AppTimer *s_timer;
static int s_progress;
static int s_circle_animation_interval;

typedef struct {
  GColor background_color;
} CircleLayerData;

static void progress_callback(void *context);

static void next_timer() {
  s_timer = app_timer_register(PROGRESS_LAYER_WINDOW_DELTA, progress_callback, NULL);
}

static void progress_callback(void *context) {
  s_progress += (s_progress < 100) ? 1 : -100;
  progress_layer_set_progress(s_progress_layer, s_progress);
  next_timer();
}

static void circle_layer_update_proc(Layer *layer, GContext *ctx) {
  CircleLayerData *data = (CircleLayerData *)layer_get_data(layer);
  GRect bounds = layer_get_bounds(layer);

  uint16_t circle_radius_size = 0;
  if (s_circle_animation_interval >= 0) {
    uint16_t increase_size = ((s_circle_animation_interval * 10) / CIRCLE_ANIMATION_DELTA);
    circle_radius_size = (bounds.size.h * (0.80 + (increase_size / 100.0)));
  } else {
    uint16_t decrease_size = (((CIRCLE_ANIMATION_DELTA + s_circle_animation_interval) * 10) / CIRCLE_ANIMATION_DELTA);
    circle_radius_size = (bounds.size.h * (0.90 - (decrease_size / 100.0)));
  }
  s_circle_animation_interval++;
  if (s_circle_animation_interval > CIRCLE_ANIMATION_DELTA) {
    s_circle_animation_interval = -CIRCLE_ANIMATION_DELTA;
  }
  graphics_context_set_fill_color(ctx, data->background_color);
  graphics_fill_circle(ctx, grect_center_point(&bounds), circle_radius_size / 2);
}

static void create_order() {
  // window_stack_remove(s_window, true);
  // ride_compass_push();
}

static void window_load(Window *window) {
  window_set_background_color(window, GColorWhite);
  Layer *window_layer = window_get_root_layer(window);
  GRect bounds = layer_get_bounds(window_layer);
  APP_LOG(APP_LOG_LEVEL_DEBUG, "%i,%i,%i,%i", bounds.origin.x, bounds.origin.y,
                                              bounds.size.w, bounds.size.h);

  create_order();

  //layer with yellow circle
  Layer *s_circle_layer = layer_create_with_data(bounds, sizeof(CircleLayerData));
  CircleLayerData *data = (CircleLayerData *)layer_get_data(s_circle_layer);
  data->background_color = GColorYellow;
  s_circle_animation_interval = 0;
  layer_set_update_proc(s_circle_layer, circle_layer_update_proc);
  layer_add_child(window_layer, s_circle_layer);

  const GEdgeInsets text_insets = {
    .top = (bounds.size.h / 2) - 24
  };
  s_text_layer = text_layer_create(grect_inset(bounds, text_insets));
  text_layer_set_text(s_text_layer, "Ordering a Taxi");
  text_layer_set_text_color(s_text_layer, GColorBlack);
  text_layer_set_background_color(s_text_layer, GColorClear);
  text_layer_set_font(s_text_layer, fonts_get_system_font(FONT_KEY_GOTHIC_24_BOLD));
  text_layer_set_text_alignment(s_text_layer, GTextAlignmentCenter);
  layer_add_child(window_layer, text_layer_get_layer(s_text_layer));

  GRect text_layer_bounds = layer_get_bounds(text_layer_get_layer(s_text_layer));
  s_progress_layer = progress_layer_create(GRect((bounds.size.w - PROGRESS_LAYER_WINDOW_WIDTH) / 2,
                                                 text_layer_bounds.origin.y + 100, 
                                                 PROGRESS_LAYER_WINDOW_WIDTH, 6));
  progress_layer_set_progress(s_progress_layer, 0);
  progress_layer_set_corner_radius(s_progress_layer, 2);
  progress_layer_set_foreground_color(s_progress_layer, GColorWhite);
  progress_layer_set_background_color(s_progress_layer, GColorBlack);
  layer_add_child(window_layer, s_progress_layer);
}

static void window_unload(Window *window) {
  layer_destroy(s_circle_layer);
  text_layer_destroy(s_text_layer);
  progress_layer_destroy(s_progress_layer);
  window_destroy(s_window);
  s_window = NULL;
}

static void window_appear(Window *window) {
  s_progress = 0;
  next_timer();
}

static void window_disappear(Window *window) {
  if(s_timer) {
    app_timer_cancel(s_timer);
    s_timer = NULL;
  }
}

void order_window_push() {
  if (!s_window) {
    s_window = window_create();
    window_set_background_color(s_window, GColorWhite);
    window_set_window_handlers(s_window, (WindowHandlers) {
      .load = window_load,
      .unload = window_unload,
      .appear = window_appear,
      .disappear = window_disappear
    });
  }

  window_stack_push(s_window, true);
}