#ifndef SOUND_H
#define SOUND_H

#include <stdint.h>

// Frecuency table, taken from
// https://blogger.googleusercontent.com/img/b/R29vZ2xl/AVvXsEgs4ia4Rd7FI71hw910jjOkROUdr9jSI57eebJ7q1CI9ib1p8WXOi9gvwtGYc_V-cDuALghLxkufDOqc2twadxB4S42259NsnxWIGBdnOxQz11iohkT0YIPphWLie47ADNzfhh3dvrt0AU/s1600/FRECUENCIA_NOTAS_MUSICALES.jpg

#define DO      261
#define RE      293
#define MI      329
#define FA      349
#define FASharp 370
#define SOL     392
#define LA      440
#define SI      493

#define DO2  523
#define RE2  587
#define MI2  659
#define FA2  698
#define SOL2 784
#define LA2  880
#define SI2  987
#define DO3  1046

/*
    @brief Plays a sound
    @param nFrequence The frequency of the sound
*/
void play_sound(uint32_t nFrequence);

/*
    @brief Stops the sound
*/
void no_sound();

/*
    @brief Plays a beep
*/
void beep();

/*
    @brief Plays a note sound
    @param freq The frequency of the note
    @param duration The duration of the note
    @param waitAfter The time to wait after the note
*/
void play_note_sound(int freq, int duration, int waitAfter);

/*
    @brief Plays a lovely melody
*/
void play_melody();

/*
    @brief Plays the Better Call Saul theme
*/
void play_better_call_saul();

#endif /* SOUND_H */