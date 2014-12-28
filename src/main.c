#include <pebble.h>

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
  text_layer_set_text(quoteLayer, "Only two things are infinite, the universe and human stupidity, and I'm not sure about the former.\n-Albert Einstein");
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
