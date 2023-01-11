#include "apu.h"

static _UNES_APU* apu;

void _UNES_APU_init()
{
    apu = malloc(sizeof(_UNES_APU));
    memset(apu, 0, sizeof(_UNES_APU));

    apu->spec.freq = 44100;
    apu->spec.format = AUDIO_U16;
    apu->spec.channels = 2;
    apu->spec.samples = 512;
    apu->spec.callback = _UNES_APU_run;

    SDL_AudioSpec have;
    apu->device = SDL_OpenAudioDevice(NULL, 0, &apu->spec, &have, SDL_AUDIO_ALLOW_ANY_CHANGE);
    if (!apu->device) {
        printf("Error setting up audio %s\nAPU is disabled\n", SDL_GetError());
    }
    apu->spec = have;

    SDL_PauseAudioDevice(apu->device, 0);
}

void _UNES_APU_free()
{
    SDL_CloseAudioDevice(apu->device);
    free(apu);
}

void _UNES_APU_run(void* userdata, uint8_t* stream, int len)
{
    if (apu->enabled) {
        unsigned int duty = 0;

        // Pulse 1
        float samples_per_period = (1/apu->square1.freq) / (1/apu->spec.freq);
        bool high = true;

        for (int i = 0; i < len; i += 2)
        {
            if (high) {
                stream[i] = apu->square1.volume;
                stream[i+1] = apu->square1.volume;
            } else {
                stream[i] = 0;
                stream[i+1] = 0;
            }

            float duty_cycle = 0;
            switch (apu->square1.cycle)
            {
            case DUTY_12_5:
                duty_cycle = apu->spec.freq/(apu->square1.freq*8);
            case DUTY_25:
                duty_cycle = apu->spec.freq/(apu->square1.freq*4);
            case DUTY_50:
                duty_cycle = apu->spec.freq/(apu->square1.freq*2);
                break;
            
            default:
                break;
            }
            duty++;
            if (duty >= duty_cycle) {
                high = !high;
                duty = 0;
            }
        }
    } else {
        memset(stream, 0, len);
    }
}

void unes_apu_enable()
{
    apu->enabled = true;
}

void unes_apu_disable()
{
    apu->enabled = false;
}

void unes_set_pulse_data(APU_CHANNELS channel, float frequency, uint16_t volume, DUTY_CYCLE cycle)
{
    if (channel == SQUARE1) {
        apu->square1 = (PulseData){frequency, volume, cycle};
    } else if (channel == SQUARE2) {
        apu->square2 = (PulseData){frequency, volume, cycle};
    } else {
        printf("Invalid channel\n");
    }
}

void unes_set_triangle_data(bool muted, float frequency)
{
    apu->triangle = (TriangleData){muted, frequency};
}
