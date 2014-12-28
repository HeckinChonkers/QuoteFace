#include <pebble.h>

static TextLayer *timeLayer;
static TextLayer *quoteLayer;
static GFont timeFont;
static GFont quoteFont;
static BitmapLayer *bgLayer;
static GBitmap *backgroundBitmap;
  
Window *my_window;
TextLayer *text_layer;

void window_load(Window *window){
  // Create background bitmap and layer
  backgroundBitmap = gbitmap_create_with_resource(RESOURCE_ID_IMAGE_QUOTE_BACKGROUND);
  bgLayer = bitmap_layer_create(GRect(0,0,144,168));
  bitmap_layer_set_bitmap(bgLayer,backgroundBitmap);
  layer_add_child(window_get_root_layer(window), bitmap_layer_get_layer(bgLayer));
  
  // Now create the time layer
  timeLayer = text_layer_create(GRect(2.5,20,139,50));
  text_layer_set_background_color(timeLayer, GColorClear);
  text_layer_set_text_color(timeLayer, GColorBlack);
  timeFont = fonts_load_custom_font(resource_get_handle(RESOURCE_ID_FONT_SANSPOSTER_BOLD_20));
  
  // And apply the font to the time layer
  text_layer_set_font(timeLayer, timeFont);
  text_layer_set_text_alignment(timeLayer, GTextAlignmentCenter);
  text_layer_set_text(timeLayer, "00:00AM");
  layer_add_child(window_get_root_layer(window), text_layer_get_layer(timeLayer));
  
  // Create the quote layer
  quoteLayer = text_layer_create(GRect(5,65,139,100));
  text_layer_set_background_color(quoteLayer, GColorClear);
  text_layer_set_text_color(quoteLayer, GColorWhite);
  quoteFont = fonts_load_custom_font(resource_get_handle(RESOURCE_ID_FONT_DJB_HOLLY_SERIF_14));
  
  // And apply the font to the quote layer
  text_layer_set_font(quoteLayer, quoteFont);
  text_layer_set_text_alignment(quoteLayer, GTextAlignmentCenter);
  text_layer_set_text(quoteLayer, "Only two things are infinite, the universe and human stupidity, and I'm not sure about the former.\n-Albert Einstein");
  layer_add_child(window_get_root_layer(window), text_layer_get_layer(quoteLayer));
}

void window_unload(Window *window){
  // Destroy background bitmap and layer
  gbitmap_destroy(backgroundBitmap);
  bitmap_layer_destroy(bgLayer);
}

void handle_init(void) {
  my_window = window_create();

  window_set_window_handlers(my_window, (WindowHandlers){
    .load = window_load,
    .unload = window_unload
  });
  
  window_stack_push(my_window, true);
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
