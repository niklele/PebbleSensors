#include <pebble.h>

#define CMD_START 1
#define CMD_DATA 2
#define CMD_STOP 3

#define KEY_DATA_TIME 10
#define KEY_DATA_X 11
#define KEY_DATA_Y 12
#define KEY_DATA_Z 13
#define KEY_DATA_VIBRATE 14

#define KEY_PB_COMMAND 20
#define KEY_PB_DATA 21
#define KEY_JS_STATUS 22

#define NUM_SAMPLES 1

static Window *s_main_window;
static TextLayer *s_output_layer;
bool g_start;

static void inbox_received_callback(DictionaryIterator *iterator, void *context) {
    APP_LOG(APP_LOG_LEVEL_INFO, "Message received!");

    // Get the first pair
    Tuple *t = dict_read_first(iterator);

    // Process all pairs present
    while(t != NULL) {
        // Process this pair's key
        switch (t->key) {
            case KEY_JS_STATUS:
                APP_LOG(APP_LOG_LEVEL_INFO, "KEY_JS_STATUS received with value %d", (int)t->value->int32);
                break;
        }

        // Get next pair, if any
        t = dict_read_next(iterator);
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

static void data_handler(AccelData *data, uint32_t num_samples) {
    // Long lived buffer
//     static char s_buffer[128];

    // Compose string of all data for 3 samples
//     snprintf(s_buffer, sizeof(s_buffer), 
//              "N X,Y,Z\n0 %d,%d,%d\n1 %d,%d,%d\n2 %d,%d,%d", 
//              data[0].x, data[0].y, data[0].z ,
//              data[1].x, data[1].y, data[1].z,
//              data[2].x, data[2].y, data[2].z
//             );

    //Show the data
//     text_layer_set_text(s_output_layer, s_buffer);
    
    //timestamp 

    DictionaryIterator *iter;
    app_message_outbox_begin(&iter);

    if (iter == NULL) {
        return;
    }

//   dict_write_tuplet(iter, &value);
//   dict_write_end(iter);
    
    if (g_start) {
        dict_write_data(iter, KEY_PB_DATA, (const uint8_t *const) data, sizeof(data));

        app_message_outbox_send();
    }
}

static void main_window_load(Window *window) {
    Layer *window_layer = window_get_root_layer(window);
    GRect window_bounds = layer_get_bounds(window_layer);
    
    // Create output TextLayer
    s_output_layer = text_layer_create(GRect(5, 0, window_bounds.size.w - 10, window_bounds.size.h));
    text_layer_set_font(s_output_layer, fonts_get_system_font(FONT_KEY_GOTHIC_24));
    text_layer_set_text(s_output_layer, "Waiting for SELECT");
    text_layer_set_overflow_mode(s_output_layer, GTextOverflowModeWordWrap);
    layer_add_child(window_layer, text_layer_get_layer(s_output_layer));
}

static void main_window_unload(Window *window) {
}

void select_single_click_handler(ClickRecognizerRef recognizer, void *context) {
//   ... called on single click, and every 1000ms of being held ...
    Window *window = (Window *) context;
    
    text_layer_set_text(s_output_layer, "Sending msg START");
    
    DictionaryIterator *iter;
    app_message_outbox_begin(&iter);

    if (iter == NULL) {
        return;
    }

//   dict_write_tuplet(iter, &value);
//   dict_write_end(iter);
    
//     dict_write_data(iter, KEY_DATA, (const uint8_t *const) data, sizeof(data));
    
    g_start = !g_start;
    
    int val = (g_start) ? CMD_START : CMD_STOP;
    dict_write_int(iter, KEY_PB_COMMAND, &val, sizeof(val), true);

    app_message_outbox_send();
}

void config_provider(Window *window) {
    window_single_click_subscribe(BUTTON_ID_SELECT, select_single_click_handler);
}

static void init(void) {
    g_start = false;
    
    // Create main Window
    s_main_window = window_create();
    window_set_window_handlers(s_main_window, (WindowHandlers) {
        .load = main_window_load,
        .unload = main_window_unload,
    });
    window_stack_push(s_main_window, true);
    
    // Register button clicks
    window_set_click_config_provider(s_main_window, (ClickConfigProvider) config_provider);
    
    // Register AppMessage callbacks
    app_message_register_inbox_received(inbox_received_callback);
    app_message_register_inbox_dropped(inbox_dropped_callback);
    app_message_register_outbox_failed(outbox_failed_callback);
    app_message_register_outbox_sent(outbox_sent_callback);
    
    // Open AppMessage
    app_message_open(app_message_inbox_size_maximum(), app_message_outbox_size_maximum());
    
    // Subscribe to the accelerometer data service
    accel_data_service_subscribe(NUM_SAMPLES, data_handler);

    // Choose update rate
    accel_service_set_sampling_rate(ACCEL_SAMPLING_10HZ);
}

static void deinit(void) {
    // Destroy main Window
    window_destroy(s_main_window);
    
    // Unsubscribe from Accelerometer
    accel_data_service_unsubscribe();
}

int main(void) {
    init();
    app_event_loop();
    deinit();
}