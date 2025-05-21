/**
 * Traffic Control System for STM32F4
 * 
 * This program implements a traffic control system for a 4-way intersection
 * with adaptive timing based on traffic load.
 */

#include "CLOCK.h"
#include "SYS_INIT.h"
#include "GPIO.h"
#include <stdlib.h>

// Define traffic directions
#define NORTH_SOUTH 0
#define EAST_WEST   1

// Define traffic light states
#define RED     0
#define YELLOW  1
#define GREEN   2

// Define traffic load levels
#define TRAFFIC_LOW  5
#define TRAFFIC_HIGH 15
#define MAX_TRAFFIC  30

// Define timing parameters (in milliseconds)
#define GREEN_TIME       5000
#define YELLOW_TIME      2000
#define RED_TIME         7000  // RED_TIME = GREEN_TIME + YELLOW_TIME
#define EXTENDED_TIME    15000

// Define GPIO pins for traffic lights (12 LEDs total, grouped as 3 lights x 4 directions)
// NORTH traffic lights
#define N_RED_PIN        GPIO_PIN_0  // PA0 - Red light for North
#define N_YELLOW_PIN     GPIO_PIN_1  // PA1 - Yellow light for North
#define N_GREEN_PIN      GPIO_PIN_2  // PA2 - Green light for North

// SOUTH traffic lights
#define S_RED_PIN        GPIO_PIN_3  // PA3 - Red light for South
#define S_YELLOW_PIN     GPIO_PIN_4  // PA4 - Yellow light for South
#define S_GREEN_PIN      GPIO_PIN_5  // PA5 - Green light for South

// EAST traffic lights
#define E_RED_PIN        GPIO_PIN_6  // PA6 - Red light for East
#define E_YELLOW_PIN     GPIO_PIN_7  // PA7 - Yellow light for East
#define E_GREEN_PIN      GPIO_PIN_8  // PA8 - Green light for East

// WEST traffic lights
#define W_RED_PIN        GPIO_PIN_9  // PA9 - Red light for West
#define W_YELLOW_PIN     GPIO_PIN_10 // PA10 - Yellow light for West
#define W_GREEN_PIN      GPIO_PIN_11 // PA11 - Green light for West

// Grouped pins for North-South and East-West directions
#define NS_RED_PINS      (N_RED_PIN | S_RED_PIN)
#define NS_YELLOW_PINS   (N_YELLOW_PIN | S_YELLOW_PIN)
#define NS_GREEN_PINS    (N_GREEN_PIN | S_GREEN_PIN)

#define EW_RED_PINS      (E_RED_PIN | W_RED_PIN)
#define EW_YELLOW_PINS   (E_YELLOW_PIN | W_YELLOW_PIN)
#define EW_GREEN_PINS    (E_GREEN_PIN | W_GREEN_PIN)

#define NS_LOAD_LED_PIN  GPIO_PIN_12  // PA12 - North-South load indicator
#define EW_LOAD_LED_PIN  GPIO_PIN_13  // PA13 - East-West load indicator
#define BOTH_LOAD_LED_PIN GPIO_PIN_14 // PA14 - Both directions high load indicator


// Traffic counters
uint8_t ns_traffic_count = 0;
uint8_t ew_traffic_count = 0;

// Function prototypes
void setup_gpio(void);
void set_traffic_light(int direction, int color);
void generate_traffic(void);
void process_traffic_movement(int direction);
uint8_t is_traffic_low(int direction);
void clear_all_lights(void);
void update_load_indicators(void);

int main(void)
{
    // Initialize system
    initClock();
    sysInit();
    
    // Initialize GPIO pins
    setup_gpio();
    
    // Initialize random number generator
    srand(getmsTick());
    
    // Main loop
    while (1) {
        // NORTH-SOUTH gets GREEN, EAST-WEST gets RED
        set_traffic_light(NORTH_SOUTH, GREEN);
        set_traffic_light(EAST_WEST, RED);
        
        // Process traffic for standard green time
        for (uint32_t time = 0; time < GREEN_TIME; time += 500) {
            generate_traffic();
            update_load_indicators();
            process_traffic_movement(NORTH_SOUTH);
            ms_delay(500);
        }
        
        // Check if traffic is low on NORTH-SOUTH, extend green time if needed
        if (!is_traffic_low(NORTH_SOUTH)) {
            for (uint32_t time = 0; time < EXTENDED_TIME; time += 500) {
                generate_traffic();
                update_load_indicators();
                process_traffic_movement(NORTH_SOUTH);
                ms_delay(500);
            }
        }
        
        // NORTH-SOUTH gets YELLOW
        set_traffic_light(NORTH_SOUTH, YELLOW);
        for (uint32_t time = 0; time < YELLOW_TIME; time += 500) {
            generate_traffic();
            update_load_indicators();
            ms_delay(500);
        }
        
        // NORTH-SOUTH gets RED, EAST-WEST gets GREEN
        set_traffic_light(NORTH_SOUTH, RED);
        set_traffic_light(EAST_WEST, GREEN);
        
        // Process traffic for standard green time
        for (uint32_t time = 0; time < GREEN_TIME; time += 500) {
            generate_traffic();
            update_load_indicators();
            process_traffic_movement(EAST_WEST);
            ms_delay(500);
        }
        
        // Check if traffic is low on EAST-WEST, extend green time if needed
        if (!is_traffic_low(EAST_WEST)) {
            for (uint32_t time = 0; time < EXTENDED_TIME; time += 500) {
                generate_traffic();
                update_load_indicators();
                process_traffic_movement(EAST_WEST);
                ms_delay(500);
            }
        }
        
        // EAST-WEST gets YELLOW
        set_traffic_light(EAST_WEST, YELLOW);
        for (uint32_t time = 0; time < YELLOW_TIME; time += 500) {
            generate_traffic();
            update_load_indicators();
            ms_delay(500);
        }
    }
}

/**
 * Configure all GPIO pins needed for the traffic system
 */
void setup_gpio(void)
{
    // Initialize GPIO struct
    GPIO_InitTypeDef gpio = {
        .Mode      = GPIO_MODE_OUTPUT_PP,
        .Pull      = GPIO_NOPULL,
        .Speed     = GPIO_SPEED_FREQ_LOW,
        .Alternate = 0
    };
    
    // Configure all traffic light pins on GPIOA
    gpio.Pin = N_RED_PIN | N_YELLOW_PIN | N_GREEN_PIN |
               S_RED_PIN | S_YELLOW_PIN | S_GREEN_PIN |
               E_RED_PIN | E_YELLOW_PIN | E_GREEN_PIN |
               W_RED_PIN | W_YELLOW_PIN | W_GREEN_PIN |
               NS_LOAD_LED_PIN | EW_LOAD_LED_PIN | BOTH_LOAD_LED_PIN;  // Add load indicators
    GPIO_Init(GPIOA, &gpio);
    
    // Initially, all lights are off
    clear_all_lights();
}

/**
 * Turn off all traffic lights
 */
void clear_all_lights(void)
{
    GPIO_WritePin(GPIOA, N_RED_PIN | N_YELLOW_PIN | N_GREEN_PIN | 
                         S_RED_PIN | S_YELLOW_PIN | S_GREEN_PIN |
                         E_RED_PIN | E_YELLOW_PIN | E_GREEN_PIN |
                         W_RED_PIN | W_YELLOW_PIN | W_GREEN_PIN |
                         NS_LOAD_LED_PIN | EW_LOAD_LED_PIN | BOTH_LOAD_LED_PIN, GPIO_PIN_RESET);
}

/**
 * Set a specific traffic light for a direction
 *
 * @param direction NORTH_SOUTH or EAST_WEST
 * @param color RED, YELLOW, or GREEN
 */
void set_traffic_light(int direction, int color)
{
    uint16_t red_pins, yellow_pins, green_pins;
    
    // Select pins based on direction
    if (direction == NORTH_SOUTH) {
        red_pins = NS_RED_PINS;
        yellow_pins = NS_YELLOW_PINS;
        green_pins = NS_GREEN_PINS;
    } else { // EAST_WEST
        red_pins = EW_RED_PINS;
        yellow_pins = EW_YELLOW_PINS;
        green_pins = EW_GREEN_PINS;
    }
    
    // Turn off all lights for this direction first
    GPIO_WritePin(GPIOA, red_pins | yellow_pins | green_pins, GPIO_PIN_RESET);
    
    // Turn on the appropriate light
    if (color == RED) {
        GPIO_WritePin(GPIOA, red_pins, GPIO_PIN_SET);
    } else if (color == YELLOW) {
        GPIO_WritePin(GPIOA, yellow_pins, GPIO_PIN_SET);
    } else if (color == GREEN) {
        GPIO_WritePin(GPIOA, green_pins, GPIO_PIN_SET);
    }
}

/**
 * Generate random traffic for both directions
 * This function is called periodically to simulate vehicles arriving
 */
void generate_traffic(void)
{
    // Generate random traffic for NORTH-SOUTH (0-2 cars per cycle)
    int ns_new_traffic = rand() % 3;
    ns_traffic_count += ns_new_traffic;
    if (ns_traffic_count > MAX_TRAFFIC) {
        ns_traffic_count = MAX_TRAFFIC;
    }
    
    // Generate random traffic for EAST-WEST (0-2 cars per cycle)
    int ew_new_traffic = rand() % 3;
    ew_traffic_count += ew_new_traffic;
    if (ew_traffic_count > MAX_TRAFFIC) {
        ew_traffic_count = MAX_TRAFFIC;
    }
}

/**
 * Process traffic movement when light is green
 * Randomly reduces traffic count to simulate cars moving forward or turning left
 *
 * @param direction NORTH_SOUTH or EAST_WEST
 */
void process_traffic_movement(int direction)
{
    int cars_moving;
    
    if (direction == NORTH_SOUTH && ns_traffic_count > 0) {
        // Determine how many cars move (up to 3 per cycle)
        cars_moving = rand() % 4;
        if (cars_moving > ns_traffic_count) {
            cars_moving = ns_traffic_count;
        }
        
        // Reduce traffic count based on movement
        ns_traffic_count -= cars_moving;
    }
    else if (direction == EAST_WEST && ew_traffic_count > 0) {
        // Determine how many cars move (up to 3 per cycle)
        cars_moving = rand() % 4;
        if (cars_moving > ew_traffic_count) {
            cars_moving = ew_traffic_count;
        }
        
        // Reduce traffic count based on movement
        ew_traffic_count -= cars_moving;
    }
}

/**
 * Check if traffic is low for a given direction
 *
 * @param direction NORTH_SOUTH or EAST_WEST
 * @return 1 if traffic is low, 0 otherwise
 */
uint8_t is_traffic_low(int direction)
{
    if (direction == NORTH_SOUTH) {
        return (ns_traffic_count <= TRAFFIC_LOW) ? 1 : 0;
    } else { // EAST_WEST
        return (ew_traffic_count <= TRAFFIC_LOW) ? 1 : 0;
    }
}


// Function to update load indicator LEDs
void update_load_indicators(void) {
    // Check North-South load
    if (ns_traffic_count > TRAFFIC_LOW) {
        GPIO_WritePin(GPIOA, NS_LOAD_LED_PIN, GPIO_PIN_SET);
    } else {
        GPIO_WritePin(GPIOA, NS_LOAD_LED_PIN, GPIO_PIN_RESET);
    }
    
    // Check East-West load
    if (ew_traffic_count > TRAFFIC_LOW) {
        GPIO_WritePin(GPIOA, EW_LOAD_LED_PIN, GPIO_PIN_SET);
    } else {
        GPIO_WritePin(GPIOA, EW_LOAD_LED_PIN, GPIO_PIN_RESET);
    }
    
    // Check if both directions have high load
    if (ns_traffic_count > TRAFFIC_LOW && ew_traffic_count > TRAFFIC_LOW) {
        GPIO_WritePin(GPIOA, BOTH_LOAD_LED_PIN, GPIO_PIN_SET);
    } else {
        GPIO_WritePin(GPIOA, BOTH_LOAD_LED_PIN, GPIO_PIN_RESET);
    }
}
