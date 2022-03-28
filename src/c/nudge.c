#include <pebble.h>
#define NUM_CONTACTS 3

static Window *s_main_window;
static MenuLayer *s_menu_layer;

static uint16_t get_num_rows_callback(MenuLayer *menu_layer, void *data) {
  return NUM_CONTACTS;
}

static void draw_row_callback(GContext *ctx, const Layer *cell_layer, MenuIndex *cell_index, void *context) {
  // Determine which section we're going to draw in
  switch (cell_index->row) {
    case 0:
      // This is the first cell, which we use to give the user some basic instructions
      menu_cell_basic_draw(ctx, cell_layer, "Claudio", "Recent Message", NULL);
      break;
    case 1:
      // This is the second cell, which we use to give the user some basic instructions
      menu_cell_basic_draw(ctx, cell_layer, "Spencer", "Recent Message", NULL);
      break;
    case 2:
      menu_cell_basic_draw(ctx, cell_layer, "Kian", "Recent Message", NULL);
      break;
    default:
      // There is no other cell, so draw the default menu cell
      menu_cell_basic_draw(ctx, cell_layer, "Nudge", NULL, NULL);
      break;
    }
}

static int16_t get_cell_height_callback(struct MenuLayer *menu_layer, MenuIndex *cell_index, void *context) {
  return PBL_IF_ROUND_ELSE(
    menu_layer_is_index_selected(menu_layer, cell_index) ?
      MENU_CELL_ROUND_FOCUSED_SHORT_CELL_HEIGHT : MENU_CELL_ROUND_UNFOCUSED_TALL_CELL_HEIGHT,
    44);
}

static void select_callback(struct MenuLayer *menu_layer, MenuIndex *cell_index, void *context) {
  // Use the row to specify which item will receive the select action
  switch (cell_index->row) {
    case 0:
      // Show the first item
      break;
    case 1:
      // Show the second item
      break;
    case 2:
      // Show the third item
      break;
  }
}

static void window_load(Window *window) {
  Layer *window_layer = window_get_root_layer(window);
  GRect bounds = layer_get_bounds(window_layer);

  s_menu_layer = menu_layer_create(bounds);
  menu_layer_set_click_config_onto_window(s_menu_layer, window);
#if defined(PBL_COLOR)
  menu_layer_set_normal_colors(s_menu_layer, GColorBlack, GColorWhite);
  menu_layer_set_highlight_colors(s_menu_layer, GColorRed, GColorWhite);
#endif
  menu_layer_set_callbacks(s_menu_layer, NULL, (MenuLayerCallbacks) {
      .get_num_rows = get_num_rows_callback,
      .draw_row = draw_row_callback,
      .get_cell_height = get_cell_height_callback,
  });
  layer_add_child(window_layer, menu_layer_get_layer(s_menu_layer));
}

static void window_unload(Window *window) {
  menu_layer_destory(s_menu_layer);
}

static void init() {
  s_main_window = window_create();
  window_set_window_handlers(s_main_window, (WindowHandlers)) {
    .load = window_load,
    .unload = window_unload,
  }
  window_stack_push(s_main_window, true);
}

static void deinit() {
  window_destroy(s_main_window);
}

int main(void) {
  init();
  app_event_loop();
  deinit();
}
