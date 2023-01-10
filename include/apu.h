#ifndef __APU_H__
#define __APU_H__

#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdlib.h>
#include <SDL.h>

#define UNES_SQUARE_PERIOD(freq) 111860.8/freq - 1
#define UNES_TRIANGLE_PERIOD(freq) 55930.4/freq - 1

/**
 * @brief APU channels
 * @note Square and pulse channels are the same
 */
enum APU_CHANNELS {
    SQUARE1 = 0,
    SQUARE2,
    TRIANGLE,
    NOISE
};

/**
 * @brief Duty cycle options
 */
enum DUTY_CYCLE {
    DUTY_12_5 = 0,
    DUTY_25,
    DUTY_50,
    DUTY_75
};

/**
 * @brief Pulse/square channel data
 */
typedef struct {
    /**
     * @brief Frequency in hertz
     */
    float freq;
    
    /**
     * @brief Volume
     */
    uint16_t volume;

    /**
     * @brief Duty cycle
     */
    int cycle;
} PulseData;

/**
 * @brief Triangle channel data
 */
typedef struct {
    /**
     * @brief Muted or not
     */
    bool muted;

    /**
     * @brief Frequency in hertz
     */
    float freq;
} TriangleData;

/**
 * @brief Internal audio struct
 */
typedef struct {
    SDL_AudioSpec spec;
    SDL_AudioDeviceID device;

    bool enabled;

    PulseData square1;
    PulseData square2;
    TriangleData triangle;
} _UNES_APU;

/**
 * @brief Internal init function
 */
void _UNES_APU_init();

/**
 * @brief Internal cleanup function
 */
void _UNES_APU_free();

/**
 * @brief Internal execution function
 */
void _UNES_APU_run(void* userdata, uint8_t* stream, int len);

/**
 * @brief Enable APU
 */
void unes_apu_enable();

/**
 * @brief Disable APU
 */
void unes_apu_disable();

/**
 * @brief Sets a channel's data for the frame
 * 
 * @param channel Channel from APU_CHANNELS
 * @param frequency Frequency in hertz
 * @param volume Volume
 * @param cycle Duty cycle from DUTY_CYCLE
 */
void unes_set_pulse_data(int channel, float frequency, uint16_t volume, int cycle);

/**
 * @brief Sets the triangle's channel data for the frame
 * 
 * @param muted Muted
 * @param frequency Frequency in hertz
 */
void unes_set_triangle_data(bool muted, float frequency);

#endif // __APU_H__