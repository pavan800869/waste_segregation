#include <ESP32Servo.h>
#include "edge-impulse-sdk/classifier/ei_run_classifier.h"
#include "esp_camera.h"

// Define servo motors
Servo leftServo;  // For dry waste
Servo rightServo; // For wet waste

// Define GPIO pins for servos
#define LEFT_SERVO_PIN 14   // Adjust as per your wiring
#define RIGHT_SERVO_PIN 15  // Adjust as per your wiring

// Camera configuration
#define PWDN_GPIO -1
#define RESET_GPIO -1
#define XCLK_GPIO 0
#define SIOD_GPIO 26
#define SIOC_GPIO 27
#define Y9_GPIO 35
#define Y8_GPIO 34
#define Y7_GPIO 39
#define Y6_GPIO 36
#define Y5_GPIO 21
#define Y4_GPIO 19
#define Y3_GPIO 18
#define Y2_GPIO 5
#define VSYNC_GPIO 25
#define HREF_GPIO 23
#define PCLK_GPIO 22

void setup() {
    Serial.begin(115200);

    // Attach servos to their respective pins
    leftServo.attach(LEFT_SERVO_PIN);
    rightServo.attach(RIGHT_SERVO_PIN);

    // Initialize camera
    camera_config_t config;
    config.ledc_channel = LEDC_CHANNEL_0;
    config.ledc_timer = LEDC_TIMER_0;
    config.pin_d0 = Y2_GPIO;
    config.pin_d1 = Y3_GPIO;
    config.pin_d2 = Y4_GPIO;
    config.pin_d3 = Y5_GPIO;
    config.pin_d4 = Y6_GPIO;
    config.pin_d5 = Y7_GPIO;
    config.pin_d6 = Y8_GPIO;
    config.pin_d7 = Y9_GPIO;
    config.pin_xclk = XCLK_GPIO;
    config.pin_pclk = PCLK_GPIO;
    config.pin_vsync = VSYNC_GPIO;
    config.pin_href = HREF_GPIO;
    config.pin_sccb_sda = SIOD_GPIO;
    config.pin_sccb_scl = SIOC_GPIO;
    config.pin_pwdn = PWDN_GPIO;
    config.pin_reset = RESET_GPIO;
    config.xclk_freq_hz = 20000000;
    config.pixel_format = PIXFORMAT_JPEG;

    if (psramFound()) {
        config.frame_size = FRAMESIZE_QVGA;
        config.jpeg_quality = 10;
        config.fb_count = 2;
    } else {
        config.frame_size = FRAMESIZE_QVGA;
        config.jpeg_quality = 12;
        config.fb_count = 1;
    }

    esp_err_t err = esp_camera_init(&config);
    if (err != ESP_OK) {
        Serial.printf("Camera initialization failed with error 0x%x", err);
        return;
    }

    Serial.println("Camera Ready!");
}

void loop() {
    camera_fb_t *fb = esp_camera_fb_get();
    if (!fb) {
        Serial.println("Failed to capture image");
        return;
    }

    ei::signal_t signal;
    int res = numpy::signal_from_buffer(fb->buf, fb->len, &signal);
    if (res != EI_IMPULSE_OK) {
        Serial.println("Failed to create signal from buffer");
        return;
    }

    ei_impulse_result_t result;
    res = run_classifier(&signal, &result, false);
    if (res != EI_IMPULSE_OK) {
        Serial.println("Failed to run classifier");
        return;
    }

    Serial.println("Inference result:");
    for (size_t i = 0; i < EI_CLASSIFIER_LABEL_COUNT; i++) {
        Serial.print(result.classification[i].label);
        Serial.print(": ");
        Serial.println(result.classification[i].value);
    }

    // Identify waste type
    float dryWasteScore = 0.0, wetWasteScore = 0.0;
    for (size_t i = 0; i < EI_CLASSIFIER_LABEL_COUNT; i++) {
        if (strcmp(result.classification[i].label, "dry_waste") == 0) {
            dryWasteScore = result.classification[i].value;
        } else if (strcmp(result.classification[i].label, "wet_waste") == 0) {
            wetWasteScore = result.classification[i].value;
        }
    }

    // Threshold-based decision-making
    if (dryWasteScore > wetWasteScore && dryWasteScore > 0.6) {
        Serial.println("Dry waste detected! Rotating left servo...");
        leftServo.write(90);  // Rotate left servo
        delay(1000);
        leftServo.write(0);   // Reset position
    } else if (wetWasteScore > dryWasteScore && wetWasteScore > 0.6) {
        Serial.println("Wet waste detected! Rotating right servo...");
        rightServo.write(90);  // Rotate right servo
        delay(1000);
        rightServo.write(0);   // Reset position
    } else {
        Serial.println("Uncertain classification, not operating servos.");
    }

    esp_camera_fb_return(fb);
    delay(2000);  // Small delay before the next iteration
}
