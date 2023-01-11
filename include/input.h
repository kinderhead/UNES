#ifndef __INPUT_H__
#define __INPUT_H__

#include <SDL.h>
#include <stdio.h>
#include <stdbool.h>
#include <string.h>

typedef enum {
    BUTTONS_A = 0,
    BUTTONS_B,
    BUTTONS_START,
    BUTTONS_SELECT,
    BUTTONS_LEFT,
    BUTTONS_RIGHT,
    BUTTONS_UP,
    BUTTONS_DOWN,
    _Buttons_SIZE
} Buttons;

/**
 * @brief Internal input struct
 */
typedef struct {
    bool buttons[_Buttons_SIZE];
    SDL_KeyCode controls[_Buttons_SIZE];
} _UNES_INPUT;

/**
 * @brief Internal init function
 */
void _UNES_INPUT_init();

/**
 * @brief Internal exit function
 */
void _UNES_INPUT_free();

/**
 * @brief Internal input function
 * 
 * @param ev Event
 */
void _unes_input_update(SDL_Event ev);

/**
 * @brief Binds a key to a control
 * 
 * @param button Button
 * @param key SDL key
 */
void unes_set_keybind(Buttons button, SDL_KeyCode key);

/**
 * @brief Checks if the button is pressed
 * 
 * @param button Button
 * @return If the button is pressed
 */
bool unes_is_pressed(Buttons button);

#endif // __INPUT_H__