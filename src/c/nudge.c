#include <pebble.h>

#define NUM_CONTACTS 3

typedef enum {
  MessageTypeEmoji,
  MessageTypeCanned,
  MessageTypeVoice,
} MessageType;

typedef struct {
  MessageType type;
} Context;

static MessageType s_current_type;

static Window *s_main_window;
static MenuLayer *s_menu_layer;

static ActionMenu *s_action_menu;
static ActionMenuLevel *s_root_level, *s_emoji_layer;

static DictationSession *s_dictation_session;
static char s_last_text[512];

static char emojis[80][10] = {
  "\U0001F601",
  "\U0001F602",
  "\U0001F603",
  "\U0001F604",
  "\U0001F605",
  "\U0001F606",
  "\U0001F609",
  "\U0001F60A",
  "\U0001F60B",
  "\U0001F60C",
  "\U0001F60D",
  "\U0001F60F",
  "\U0001F612",
  "\U0001F613",
  "\U0001F614",
  "\U0001F616",
  "\U0001F618",
  "\U0001F61A",
  "\U0001F61C",
  "\U0000263A",
  "\U0001F607",
  "\U0001F608",
  "\U0001F60E",
  "\U0001F610",
  "\U0001F611",
  "\U0001F615",
  "\U0001F617",
  "\U0001F619",
  "\U0001F61B",
  "\U0001F61F",
  "\U0001F626",
  "\U0001F627",
  "\U0001F62C",
  "\U0001F62E",
  "\U0001F62F",
  "\U0001F634",
  "\U0001F636",
  "\U0001F425",
  "\U00002764",
  "\U0001F493",
  "\U0001F61D",
  "\U0001F61E",
  "\U0001F620",
  "\U0001F621",
  "\U0001F622",
  "\U0001F623",
  "\U0001F624",
  "\U0001F625",
  "\U0001F628",
  "\U0001F629",
  "\U0001F62A",
  "\U0001F62B",
  "\U0001F630",
  "\U0001F631",
  "\U0001F632",
  "\U0001F633",
  "\U0001F635",
  "\U0001F637",
  "\U0001F600",
  "\U0001F494",
  "\U0001F495",
  "\U0001F496",
  "\U0001F497",
  "\U0001F498",
  "\U0001F499",
  "\U0001F49A",
  "\U0001F49B",
  "\U0001F49C",
  "\U0001F49D",
  "\U0001F49E",
  "\U0001F49F",
  "\U0001F37A",
  "\U0001F37B",
  "\U0001F389",
  "\U0000270B",
  "\U0000270C",
  "\U0001F44D",
  "\U0001F44E",
  "\U0001F64F",
  "\U0001F4A9"
};

static void dictation_session_callback(DictationSession *session, DictationSessionStatus status, char *transcription, void *context) {
  APP_LOG(APP_LOG_LEVEL_INFO, "Dictation status: %d", (int)status);
}

static void action_performed_callback(ActionMenu *action_menu, const ActionMenuItem *action, void *context) {
  s_current_type = (MessageType)action_menu_item_get_action_data(action);

  switch(s_current_type) {
    case MessageTypeEmoji:
      break;
    case MessageTypeCanned:
      break;
    case MessageTypeVoice:
      s_dictation_session = dictation_session_create(sizeof(s_last_text), dictation_session_callback, NULL);
      dictation_session_start(s_dictation_session);
      break;
  }
}

static void init_action_menu() {
  s_root_level = action_menu_level_create(3);

  s_emoji_layer = action_menu_level_create(80);
  action_menu_level_add_child(s_root_level, s_emoji_layer, "Emoji");
  for (int i = 0; i < 80; i++) {
    action_menu_level_add_action(s_emoji_layer, emojis[i], action_performed_callback, (void *)MessageTypeEmoji);
  }

  action_menu_level_add_action(s_root_level, "Canned", action_performed_callback, (void *)MessageTypeCanned);
  
#if defined(PBL_MICROPHONE)
  action_menu_level_add_action(s_root_level, "Voice", action_performed_callback, (void *)MessageTypeVoice);
#endif

}

static void select_click_handler(struct MenuLayer *menu_layer, MenuIndex *cell_index, void *context) {
  ActionMenuConfig config = (ActionMenuConfig) {
    .root_level = s_root_level,
    .colors = {
      .background = PBL_IF_COLOR_ELSE(GColorRed, GColorWhite),
      .foreground = GColorBlack,
    },
    .align = ActionMenuAlignTop,
  };

  action_menu_level_set_display_mode(s_root_level, ActionMenuLevelDisplayModeWide);
  action_menu_level_set_display_mode(s_emoji_layer, ActionMenuLevelDisplayModeThin);
  
  
  s_action_menu = action_menu_open(&config);
}

static uint16_t get_num_rows_callback(MenuLayer *menu_layer, uint16_t section_index, void *data) {
  return NUM_CONTACTS;
}

static void draw_row_callback(GContext *ctx, const Layer *cell_layer, MenuIndex *cell_index, void *context) {
  // Determine which section we"re going to draw in
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
