#ifndef SONG_H
#define SONG_H

#include <stdint.h>

#define C8	4186.01

#define B7	3951.07
#define As7	3729.31
#define Bb7	3729.31
#define A7	3520
#define Gs7	3322.44
#define Ab7	3322.44
#define G7	3135.96
#define Fs7	2959.96
#define Gb7	2959.96
#define F7	2793.83
#define E7	2637.02
#define Ds7	2489.02
#define Eb7	2489.02
#define D7	2349.32
#define Cs7	2217.46
#define Db7	2217.46
#define C7	2093

#define B6	1975.53
#define As6	1864.66
#define Bb6	1864.66
#define A6	1760
#define Gs6	1661.22
#define Ab6	1661.22
#define G6	1567.98
#define Fs6	1479.98
#define Gb6	1479.98
#define F6	1396.91
#define E6	1318.51
#define Ds6	1244.51
#define Eb6	1244.51
#define D6	1174.66
#define Cs6	1108.73
#define Db6	1108.73
#define C6	1046.5

#define B5	987.767
#define As5	932.328
#define Bb5 932.328
#define A5	880
#define Gs5	830.609
#define Ab5	830.609
#define G5	783.991
#define Fs5	739.989
#define Gb5	739.989
#define F5	698.456
#define E5	659.255
#define Ds5	622.254
#define Eb5	622.254
#define D5	587.33
#define Cs5	554.365
#define Db5	554.365
#define C5	523.251

#define B4	493.883
#define As4	466.164
#define Bb4	466.164
#define A4	440
#define Gs4	415.305
#define Ab4	415.305
#define G4	391.995
#define Fs4	369.994
#define Gb4	369.994
#define F4	349.228
#define E4	329.628
#define Ds4	311.127
#define Eb4	311.127
#define D4	293.665
#define Cs4	277.183
#define Db4	277.183
#define C4	261.626

#define B3	246.942
#define As3	233.082
#define Bb3	233.082
#define A3	220
#define Gs3	207.652
#define Ab3	207.652
#define G3	195.998
#define Fs3	184.997
#define Gb3	184.997
#define F3	174.614
#define E3	164.814
#define Ds3	155.563
#define Eb3	155.563
#define D3	146.832
#define Cs3	138.591
#define Db3	138.591
#define C3	130.813

#define B2	123.471
#define As2	116.541
#define Bb2	116.541
#define A2	110
#define Gs2	103.826
#define Ab2	103.826
#define G2	97.9989
#define Fs2	92.4986
#define Gb2	92.4986
#define F2	87.4069
#define E2	82.4069
#define Ds2	77.7817
#define Eb2	77.7817
#define D2	73.4162
#define Cs2	69.2957
#define Db2	69.2957
#define C2	65.4064

#define B1	61.7354
#define As1	58.2705
#define Bb1	58.2705
#define A1	55
#define Gs1	51.9131
#define Ab1	51.9131
#define G1	48.9994
#define Fs1	46.2493
#define Gb1	46.2493
#define F1	43.6535
#define E1	41.2034
#define Ds1	38.8909
#define Eb1	38.8909
#define D1	36.7081
#define Cs1	34.6478
#define Db1	34.6478
#define C1	32.7032

#define B0	30.8677
#define As0	29.1352
#define Bb0	29.1352
#define A0	27.0

#define NUM_SAMPLES 44100

#define GoT_takt1 600
#define GoT_hel 1700
#define GoT_halv 1200
#define GoT_takt2 300


int start_song;

void playTone();
void playSong();
void selectSong(uint16_t btn);

struct song{
	uint32_t tone_samples[57];
	uint64_t tone_length[57];
};


void initializeSong(struct song *song_pointer);

extern struct song GoT_song;
extern struct song startup_song;
extern struct song current_song;
extern struct song still_alive_song;
extern struct song lisa_song;

uint32_t song_counter;	// playSong() length
uint32_t sample_counter;	// playNote() samples
uint32_t tone_indice;	

void makeSongs();

//Pointers to song structs
struct song *current_song_pointer;
struct song *got_song_pointer;
struct song *startup_song_pointer;
struct song *still_alive_song_pointer;
struct song *lisa_song_pointer;


#endif