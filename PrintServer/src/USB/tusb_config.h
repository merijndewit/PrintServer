#pragma once
#define TUSB_OPT_HOST_MODE        1

// Enable CDC-ACM (serial communication) support on USB Host
#define TUSB_OPT_HOST_CDC_ACM     1

// Optional: Set USB host speed (Full Speed for most devices, can also be High Speed if supported)
#define TUSB_OPT_HOST_SPEED       TUSB_SPEED_FULL

// Optional: Set buffer size for USB host operations (larger buffer sizes may help with performance)
#define TUSB_OPT_HOST_BUF_SIZE    512  // Default is 64, you can increase if necessary

// Optional: Host Debounce time (in ms) before resetting the USB device after disconnection
#define TUSB_OPT_HOST_DEBOUNCE_MS 100

// === Optional Configurations ===
// Optional: Enable USB Host events and callback support for things like device connection/disconnection
// #define TUSB_OPT_HOST_ENABLE_ENUM_FILTER_CALLBACK   1

// === USB Power and Resource Configuration ===
#define TUSB_OPT_USB_PWR_PIN      0   // If using an external power pin for USB VBUS, set to the correct GPIO number
#define TUSB_OPT_USB_PWR_VBUS     1   // Enable VBUS power detection if necessary for the USB host

// Optional: Configure endpoint size (should typically work with defaults, but can be adjusted if necessary)
#define TUSB_OPT_MAX_ENDPOINT_SIZE 64  // The default endpoint size is usually fine for most devices

// Optional: Include debugging for USB events (you can enable this if you want additional logs)
#define TUSB_OPT_DEBUG            2   // Set to 1 for debugging logs