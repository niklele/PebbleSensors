#include <pebble.h>

static Window *s_main_window;
static Layer *s_canvas_layer;
static TextLayer *s_output_layer;

AccelData accel_data;

static void canvas_update_proc(Layer *this_layer, GContext *ctx) {
    GRect bounds = layer_get_bounds(this_layer);

    // Get the center of the screen (non full-screen)
//     GPoint center = GPoint(bounds.size.w / 2, (bounds.size.h / 2));

    // Draw the 'loop' of the 'P'
//     graphics_context_set_fill_color(ctx, GColorBlack);
//     graphics_fill_circle(ctx, center, 40);
//     graphics_context_set_fill_color(ctx, GColorWhite);
//     graphics_fill_circle(ctx, center, 35);

    // Draw the 'stalk'
//     graphics_context_set_fill_color(ctx, GColorBlack);
//     graphics_fill_rect(ctx, GRect(32, 40, 5, 100), 0, GCornerNone);
    
    graphics_context_set_fill_color(ctx, GColorBlack);
    
    int16_t x = accel_data.x / 4;
    graphics_fill_rect(ctx, GRect(10, 20, x, 10), 0, GCornerNone);
    
    int16_t y = accel_data.y / 4;
    graphics_fill_rect(ctx, GRect(10, 60, y, 10), 0, GCornerNone);

    int16_t z = accel_data.z / 4;
    graphics_fill_rect(ctx, GRect(10, 80, z, 10), 0, GCornerNone);
}

static void data_handler(AccelData *data, uint32_t num_samples) {
    // Long lived buffer
    static char s_buffer[128];

    // Compose string of all data for 3 samples
    snprintf(s_buffer, sizeof(s_buffer), 
             "N X,Y,Z\n0 %d,%d,%d\n1 %d,%d,%d\n2 %d,%d,%d", 
             data[0].x, data[0].y, data[0].z, 
             data[1].x, data[1].y, data[1].z, 
             data[2].x, data[2].y, data[2].z
            );

    //Show the data
    text_layer_set_text(s_output_layer, s_buffer);
    
    if (!data->did_vibrate) {
        accel_data.timestamp = data->timestamp;
        accel_data.x = data->x;
        accel_data.y = data->y;
        accel_data.z = data->z;
    }
}


static void main_window_load(Window *window) {
    Layer *window_layer = window_get_root_layer(window);
    GRect window_bounds = layer_get_bounds(window_layer);
    
    // Create output TextLayer
    s_output_layer = text_layer_create(GRect(5, 0, window_bounds.size.w - 10, window_bounds.size.h));
    text_layer_set_font(s_output_layer, fonts_get_system_font(FONT_KEY_GOTHIC_24));
    text_layer_set_text(s_output_layer, "No data yet.");
    text_layer_set_overflow_mode(s_output_layer, GTextOverflowModeWordWrap);
    layer_add_child(window_layer, text_layer_get_layer(s_output_layer));

    // Create Layer
//     s_canvas_layer = layer_create(GRect(0, 0, window_bounds.size.w, window_bounds.size.h));
//     layer_add_child(window_layer, s_canvas_layer);

    // Set the update_proc
//     layer_set_update_proc(s_canvas_layer, canvas_update_proc);
}

static void main_window_unload(Window *window) {
    // Destroy Layer
    layer_destroy(s_canvas_layer);
}

static void init(void) {
    accel_data.timestamp = 0;
    accel_data.did_vibrate = false;
    accel_data.x = 0;
    accel_data.y = 0;
    accel_data.z = 0;
    
    // Create main Window
    s_main_window = window_create();
    window_set_window_handlers(s_main_window, (WindowHandlers) {
        .load = main_window_load,
        .unload = main_window_unload,
    });
    window_stack_push(s_main_window, true);
    
    // Subscribe to the accelerometer data service
    int num_samples = 3;
    accel_data_service_subscribe(num_samples, data_handler);

    // Choose update rate
    accel_service_set_sampling_rate(ACCEL_SAMPLING_100HZ);
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