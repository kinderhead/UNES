#include "input.h"

static _UNES_INPUT* input;

void _UNES_INPUT_init()
{
    input = malloc(sizeof(_UNES_INPUT));
    memset(input, 0, sizeof(_UNES_INPUT));

    input->controls[BUTTONS_A] = SDLK_z;
    input->controls[BUTTONS_B] = SDLK_x;
    input->controls[BUTTONS_SELECT] = SDLK_s;
    input->controls[BUTTONS_START] = SDLK_KP_ENTER;
    input->controls[BUTTONS_LEFT] = SDLK_LEFT;
    input->controls[BUTTONS_UP] = SDLK_UP;
    input->controls[BUTTONS_DOWN] = SDLK_DOWN;
    input->controls[BUTTONS_RIGHT] = SDLK_RIGHT;
}

void _UNES_INPUT_free()
{
    free(input);
}

void _unes_input_update(SDL_Event ev)
{
    switch (ev.type)
    {
    case SDL_KEYUP:
        for (int i = 0; i < _Buttons_SIZE; i++)
        {
            if (input->controls[i] == ev.key.keysym.sym) {
                input->buttons[i] = false;
                break;
            }
        }
        break;
    
    case SDL_KEYDOWN:
        for (int i = 0; i < _Buttons_SIZE; i++)
        {
            if (input->controls[i] == ev.key.keysym.sym) {
                input->buttons[i] = true;
                break;
            }
        }
        break;

    default:
        break;
    }
}

void unes_set_keybind(Buttons button, SDL_KeyCode key)
{
    if (button >= _Buttons_SIZE) return;
    input->controls[button] = key;
}

bool unes_is_pressed(Buttons button)
{
    if (button >= _Buttons_SIZE) return false;
    return input->buttons[button];
}
