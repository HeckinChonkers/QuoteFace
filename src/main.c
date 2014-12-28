#include <pebble.h>
#define KEY_QUOTE 0
#define KEY_AUTHOR 1

static TextLayer *timeLayer;
static TextLayer *quoteLayer;
static GFont timeFont;
static GFont quoteFont;
static BitmapLayer *bgLayer;
static GBitmap *backgroundBitmap;
  
Window *my_window;
TextLayer *text_layer;

static void update_time(){
  //Get a time struct
  time_t timeVar = time(NULL);
  struct tm *clockTime = localtime(&timeVar);
  
  //Create a time string buffer
  static char buffer[] = "00:00 AM";
  
  //Write the current hours and minutes into the buffer
  if(clock_is_24h_style() == true){
    //Use 24 hour format
    strftime(buffer, sizeof("00:00"), "%H:%M", clockTime);
  } else {
    //Use 12 hour format
    strftime(buffer, sizeof("00:00AM"), "%I:%M %p", clockTime);
  }
  
  //Display this time on the TextLayer
  text_layer_set_text(timeLayer, buffer);
}

void window_load(Window *window){
  // Create background bitmap and layer
  backgroundBitmap = gbitmap_create_with_resource(RESOURCE_ID_IMAGE_QUOTE_BACKGROUND);
  bgLayer = bitmap_layer_create(GRect(0,0,144,168));
  bitmap_layer_set_bitmap(bgLayer,backgroundBitmap);
  layer_add_child(window_get_root_layer(window), bitmap_layer_get_layer(bgLayer));
  
  // Now create the time layer
  timeLayer = text_layer_create(GRect(2.5,8,139,50));
  text_layer_set_background_color(timeLayer, GColorClear);
  text_layer_set_text_color(timeLayer, GColorBlack);
  timeFont = fonts_load_custom_font(resource_get_handle(RESOURCE_ID_FONT_SANSPOSTER_BOLD_20));
  
  // And apply the font to the time layer
  text_layer_set_font(timeLayer, timeFont);
  text_layer_set_text_alignment(timeLayer, GTextAlignmentCenter);
  layer_add_child(window_get_root_layer(window), text_layer_get_layer(timeLayer));
  
  // Create the quote layer
  quoteLayer = text_layer_create(GRect(3,45,139,100));
  text_layer_set_background_color(quoteLayer, GColorClear);
  text_layer_set_text_color(quoteLayer, GColorWhite);
  quoteFont = fonts_load_custom_font(resource_get_handle(RESOURCE_ID_FONT_DJB_HOLLY_SERIF_12));
  
  // And apply the font to the quote layer
  text_layer_set_font(quoteLayer, quoteFont);
  text_layer_set_text_alignment(quoteLayer, GTextAlignmentCenter);
  text_layer_set_text(quoteLayer, "Loading Quote...");
  layer_add_child(window_get_root_layer(window), text_layer_get_layer(quoteLayer));
}

void window_unload(Window *window){
  // Destroy TextLayers
  text_layer_destroy(timeLayer);
  text_layer_destroy(quoteLayer);
  fonts_unload_custom_font(timeFont);
  fonts_unload_custom_font(quoteFont);
  
  // Destroy background bitmap and layer
  gbitmap_destroy(backgroundBitmap);
  bitmap_layer_destroy(bgLayer);
}

static void tick_handler(struct tm *clockTime, TimeUnits units_changed){
  update_time();
}

static void inbox_received_callback(DictionaryIterator *iterator, void *context){
  //Store incoming information
  static char quoteBuffer[120];
  static char authorBuffer[20];
  static char overallBuffer[140];
  
  //Read first item
  Tuple *t = dict_read_first(iterator);
  
  //For all items
  while(t != NULL){
    //Which key was received?
    switch(t->key){
      case KEY_QUOTE:
        snprintf(quoteBuffer, sizeof(quoteBuffer), "%s\n", t->value->cstring);
        break;
      case KEY_AUTHOR:
        snprintf(authorBuffer, sizeof(authorBuffer), "-%s", t->value->cstring);
        break;
      default:
        APP_LOG(APP_LOG_LEVEL_ERROR, "Key %d not recognized!", (int)t->key);
        break;
    }
    
    // Look for next item
    t = dict_read_next(iterator);
  }
  
  snprintf(overallBuffer, sizeof(overallBuffer), "%s%s", quoteBuffer, authorBuffer);
  text_layer_set_text(quoteLayer, overallBuffer);
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

void handle_init(void) {
  my_window = window_create();

  window_set_window_handlers(my_window, (WindowHandlers){
    .load = window_load,
    .unload = window_unload
  });
  
  window_stack_push(my_window, true);
  
  // Update time from the start
  update_time();
  
  // Then subscribe to event to update time every minute
  tick_timer_service_subscribe(MINUTE_UNIT, tick_handler);
  
   //Register callbacks
  app_message_register_inbox_received(inbox_received_callback);
  app_message_register_inbox_dropped(inbox_dropped_callback);
  app_message_register_outbox_failed(outbox_failed_callback);
  app_message_register_outbox_sent(outbox_sent_callback);
  
  // Open AppMessage
  app_message_open(app_message_inbox_size_maximum(), app_message_outbox_size_maximum());
}

void handle_deinit(void) {
  text_layer_destroy(text_layer);
  window_destroy(my_window);
}

int main(void) {
  handle_init();
  app_event_loop();
  handle_deinit();
}
