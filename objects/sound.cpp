#include "sound.h"
#include <windows.h>
#include <mmsystem.h>

void playSound() {
    waveOutSetVolume(nullptr, 0x0CCC0CCC);
    PlaySound(TEXT("../Sounds/Main.wav"), nullptr, SND_FILENAME | SND_ASYNC | SND_LOOP);
}

void playsound_clicked() {
    PlaySound(TEXT("../Sounds/Click.wav"), nullptr, SND_FILENAME | SND_ASYNC);
}
