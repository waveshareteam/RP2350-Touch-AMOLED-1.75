/*****************************************************************************
* | File      	:   main.c
* | Author      :   Waveshare Team
* | Info        :
*----------------
* |	This version:   V1.0
* | Date        :   2025-02-26
* | Info        :   
#
# Permission is hereby granted, free of charge, to any person obtaining a copy
# of this software and associated documnetation files (the "Software"), to deal
# in the Software without restriction, including without limitation the rights
# to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
# copies of the Software, and to permit persons to  whom the Software is
# furished to do so, subject to the following conditions:
#
# The above copyright notice and this permission notice shall be included in
# all copies or substantial portions of the Software.
#
# THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
# IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
# FITNESS OR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
# AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
# LIABILITY WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
# OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
# THE SOFTWARE.
#
******************************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "DEV_Config.h"
#include "audio_pio.h"
#include "es8311.h"

int main() 
{  
    DEV_Module_Init();
    es8311_init(pico_audio);
    es8311_sample_frequency_config(pico_audio.mclk_freq, pico_audio.sample_freq);
    es8311_microphone_config();
    es8311_voice_volume_set(pico_audio.volume);
    es8311_microphone_gain_set(pico_audio.mic_gain);

    uint16_t chip_id = es8311_read_id();
    printf("Chip ID:0x%x", chip_id);

    // 1.Output 440HZ sine wave test
    // Sine_440hz_out();
    
    // 2.Play Happy Birthday
    // Happy_birthday_out();

    // 3.Recording and playback loopback test
    Loopback_test();

    // 4.Play music
    // Music_out();

    return 0;
}
