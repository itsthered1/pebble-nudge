#include <pebble.h>

#define NUM_CONTACTS 3

static Window *s_main_window;
static MenuLayer *s_menu_layer;
static ActionMenu *s_action_menu;
static ActionMenuLevel *s_root_level;

static void action_performed_callback(ActionMenu *action_menu, const ActionMenuItem *action, void *context) {
  //what
}

static void init_action_menu() {
  s_root_level = action_menu_level_create(3);

  action_menu_level_add_action(s_root_level, "Emoji", action_performed_callback, NULL);
  action_menu_level_add_action(s_root_level, "Canned", action_performed_callback, NULL);
  action_menu_level_add_action(s_root_level, "Voice", action_performed_callback, NULL);
}

static void select_click_handler(struct MenuLayer *menu_layer, MenuIndex *cell_index, void *context) {
  ActionMenuConfig config = (ActionMenuConfig) {
    .root_level = s_root_level,
    .colors = {
      .background = PBL_IF_COLOR_ELSE(GColorRed, GColorWhite),
      .foreground = GColorBlack,
    },
    .align = ActionMenuAlignCenter
  };
  
  switch (cell_index->row) {
    case 0:
      s_action_menu = action_menu_open(&config);
      break;
    case 1:
      s_action_menu = action_menu_open(&config);
      break;
    case 2:
      s_action_menu = action_menu_open(&config);
      break;
  }
}

static uint16_t get_num_rows_callback(MenuLayer *menu_layer, uint16_t section_index, void *data) {
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
      .select_click = select_click_handler,
  });
  layer_add_child(window_layer, menu_layer_get_layer(s_menu_layer));
}

static void window_unload(Window *window) {
  menu_layer_destroy(s_menu_layer);
  action_menu_hierarchy_destroy(s_root_level, NULL, NULL);
}

static void init() {
  s_main_window = window_create();
  window_set_window_handlers(s_main_window, (WindowHandlers) {
    .load = window_load,
    .unload = window_unload,
  });
  window_stack_push(s_main_window, true);

  init_action_menu();
}

static void deinit() {
  window_destroy(s_main_window);
}

int main(void) {
  init();
  app_event_loop();
  deinit();
}
