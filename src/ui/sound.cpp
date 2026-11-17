/*
Changed on 18-11-2026

*/
#include "sound.h"
#include <string>
#include <windows.h>
// provide all function about sounds
#include <mmsystem.h>

namespace {
bool clickReady = false;
bool errorReady = false;

/*
How to use 
mciSendStringA(
    "name sounds + function",
    nullptr,                 // not need output answer
    0,                       // not need array 
    nullptr                  // not need announcement
);
mciSendStringA = 1 when approved
and vice vesta
*/
// alias means “an alternative name.” 
/*
We have three alias available

"play clickSound from 0" // Play from the beginning
"stop clickSound"        // Stop playback
"close clickSound"       // Close the sound

*/
bool openSound(const std::string& folder, const std::string& file, const std::string& alias) {
    std::string command = "open \"" + folder + file +
                          "\" type mpegvideo alias " + alias;
    return mciSendStringA(command.c_str(), nullptr, 0, nullptr) == 0;
}
}

bool startSound() {
    // Find both MP3 files beside the EXE.
    char executablePath[32768];
    DWORD length = GetModuleFileNameA(nullptr, executablePath, sizeof(executablePath));
    if (length == 0 || length >= sizeof(executablePath)) return false;

    std::string path(executablePath, length);
    size_t slash = path.find_last_of("\\/");
    if (slash == std::string::npos) return false;
    std::string folder = path.substr(0, slash + 1) + "sounds\\";

    // Load independently: one missing file does not disable the other sound.
    if (!clickReady) clickReady = openSound(folder, "click.mp3", "clickSound");
    if (!errorReady) errorReady = openSound(folder, "soft error.mp3", "errorSound");
    return clickReady && errorReady;
}

void playClick() {
    // When not found click sound 
    
    if (!clickReady) return;
    // found error sound, turn off error sound before turn on error sound
    if (errorReady) mciSendStringA("stop errorSound", nullptr, 0, nullptr);
    // turn off click sound
    mciSendStringA("stop clickSound", nullptr, 0, nullptr);
    // reset process
    mciSendStringA("play clickSound from 0", nullptr, 0, nullptr);
}

void playError() {
    // when not found error sound
    if (!errorReady) return;
    // found click sound, turn off click sound before turn on click sound
    if (clickReady) mciSendStringA("stop clickSound", nullptr, 0, nullptr);
    // turn off error sound
    mciSendStringA("stop errorSound", nullptr, 0, nullptr);
    // reset process
    mciSendStringA("play errorSound from 0", nullptr, 0, nullptr);
}

void finishSound() {
    // reset process

    if (clickReady) mciSendStringA("close clickSound", nullptr, 0, nullptr);
    if (errorReady) mciSendStringA("close errorSound", nullptr, 0, nullptr);
    clickReady = false;
    errorReady = false;
}
