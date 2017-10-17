#include <stdint.h>
#include <stdbool.h>

#include "song.h"
#include "efm32gg.h"
#include "interrupt_handlers.h"


void playTone()
{
	sample_counter++;

	if(sample_counter < current_song_pointer->tone_samples[tone_indice])
	{
		*DAC0_CH0DATA = 200;
		*DAC0_CH1DATA = 200;
	}
	else
	{
		*DAC0_CH0DATA = 0;
		*DAC0_CH1DATA = 0;
	}

	if(sample_counter >= current_song_pointer->tone_samples[tone_indice])
	{
		sample_counter = 0;
	}
}


void playSong()
{
	song_counter++;

	if(song_counter < current_song_pointer->tone_length[tone_indice])
	{
		playTone();
	}
	else
	{
		song_counter = 0;	//reset song counter
		tone_indice++;		//play next note

		if(current_song_pointer->tone_samples[tone_indice] == -1) 
		{
			stopTimer();
			start_song = 0;
			tone_indice = 0;
			*SCR = 0x6;
			deactivateDAC();
		}
	}
}

void selectSong(uint16_t buttons_pressed)
{
	switch(buttons_pressed)
	{
		case BUTTON_1:	
			initializeSong(startup_song_pointer);
			break;

		case BUTTON_2:
			initializeSong(got_song_pointer);
			break;

		case BUTTON_3:
			initializeSong(still_alive_song_pointer);
			break;

		case BUTTON_4:
			initializeSong(lisa_song_pointer);
			break;

		default:
			break;
	}
		
}

void initializeSong(struct song *song_pointer)
{
	start_song = 1;
	current_song_pointer = song_pointer;
	tone_indice = 0;	//reset tone indice so the song starts at the first tone
	song_counter = 0;	//reset song counter so that the song plays for the correct amount of time
	sample_counter = 0;	//reset tone counter so that the tone plays for the correct amount of time

}

//Structs should only be DEFINED in header file, but actual DECLARATION should happen in one source file
//Otherwise we'll have our beloved multiple definition errors.

//Also putting pointers in here
void makeSongs()
{
	struct song startup_song = {
		{
			261, 
			522, 
			108, 
			1022,
			-1
		}, 
		{
			(500*NUM_SAMPLES)/1000,
			(500*NUM_SAMPLES)/1000, 
			(500*NUM_SAMPLES)/1000, 
			(500*NUM_SAMPLES)/1000,
			0
		},
	};

	struct song GoT_song = {
		{
			NUM_SAMPLES/G4, //Should be 44100/391.995 =~112.5
			NUM_SAMPLES/C4, 
			NUM_SAMPLES/Eb4, 
			NUM_SAMPLES/F4,
			NUM_SAMPLES/G4,
			NUM_SAMPLES/C4, 
			NUM_SAMPLES/Eb4, 
			NUM_SAMPLES/F4,	
			NUM_SAMPLES/G4,
			NUM_SAMPLES/C4, 
			NUM_SAMPLES/Eb4, 
			NUM_SAMPLES/F4,	
			NUM_SAMPLES/G4,
			NUM_SAMPLES/C4, 
			NUM_SAMPLES/Eb4, 
			NUM_SAMPLES/F4,	
			NUM_SAMPLES/G4,
			NUM_SAMPLES/C4,
			NUM_SAMPLES/E4,
			NUM_SAMPLES/F4, 
			NUM_SAMPLES/G4,
			NUM_SAMPLES/C4,
			NUM_SAMPLES/E4,
			NUM_SAMPLES/F4,	
			NUM_SAMPLES/G4,
			NUM_SAMPLES/C4,
			NUM_SAMPLES/E4,
			NUM_SAMPLES/F4, 
			NUM_SAMPLES/G4,
			NUM_SAMPLES/C4,
			NUM_SAMPLES/E4,
			NUM_SAMPLES/F4, 
			NUM_SAMPLES/G4, 
			NUM_SAMPLES/C4, 
			NUM_SAMPLES/Eb4,
			NUM_SAMPLES/F4, 
			NUM_SAMPLES/G4, 
			NUM_SAMPLES/C4, 
			NUM_SAMPLES/Eb4,
			NUM_SAMPLES/F4, 
			NUM_SAMPLES/D4,
			NUM_SAMPLES/G3,
			NUM_SAMPLES/Bb3,
			NUM_SAMPLES/C4,	
			NUM_SAMPLES/D4,
			NUM_SAMPLES/G3,
			NUM_SAMPLES/Bb3,
			NUM_SAMPLES/C4,	
			NUM_SAMPLES/D4,
			NUM_SAMPLES/G3,
			NUM_SAMPLES/Bb3,
			NUM_SAMPLES/C4,	
			NUM_SAMPLES/D4,
			NUM_SAMPLES/G3,
			NUM_SAMPLES/Bb3,
			NUM_SAMPLES/F4,	
			NUM_SAMPLES/Bb3,
			-1,
		},
		{
			(GoT_takt1*NUM_SAMPLES)/1000, 
			(GoT_takt1*NUM_SAMPLES)/1000, 
			(GoT_takt2*NUM_SAMPLES)/1000, 
			(GoT_takt2*NUM_SAMPLES)/1000,
			(GoT_takt1*NUM_SAMPLES)/1000, 
			(GoT_takt1*NUM_SAMPLES)/1000, 
			(GoT_takt2*NUM_SAMPLES)/1000, 
			(GoT_takt2*NUM_SAMPLES)/1000,
			(GoT_takt1*NUM_SAMPLES)/1000, 
			(GoT_takt1*NUM_SAMPLES)/1000, 
			(GoT_takt2*NUM_SAMPLES)/1000, 
			(GoT_takt2*NUM_SAMPLES)/1000,
			(GoT_takt1*NUM_SAMPLES)/1000, 
			(GoT_takt1*NUM_SAMPLES)/1000, 
			(GoT_takt2*NUM_SAMPLES)/1000, 
			(GoT_takt2*NUM_SAMPLES)/1000,
			(GoT_takt1*NUM_SAMPLES)/1000, 
			(GoT_takt1*NUM_SAMPLES)/1000, 
			(GoT_takt2*NUM_SAMPLES)/1000, 
			(GoT_takt2*NUM_SAMPLES)/1000,
			(GoT_takt1*NUM_SAMPLES)/1000, 
			(GoT_takt1*NUM_SAMPLES)/1000, 
			(GoT_takt2*NUM_SAMPLES)/1000, 
			(GoT_takt2*NUM_SAMPLES)/1000,
			(GoT_takt1*NUM_SAMPLES)/1000, 
			(GoT_takt1*NUM_SAMPLES)/1000, 
			(GoT_takt2*NUM_SAMPLES)/1000, 
			(GoT_takt2*NUM_SAMPLES)/1000,
			(GoT_takt1*NUM_SAMPLES)/1000, 
			(GoT_takt1*NUM_SAMPLES)/1000, 
			(GoT_takt2*NUM_SAMPLES)/1000, 
			(GoT_takt2*NUM_SAMPLES)/1000,
			(GoT_hel*NUM_SAMPLES)/1000, 
			(GoT_hel*NUM_SAMPLES)/1000, 
			(GoT_takt2*NUM_SAMPLES)/1000,
			(GoT_takt2*NUM_SAMPLES)/1000,
			(GoT_halv*NUM_SAMPLES)/1000, 
			(GoT_halv*NUM_SAMPLES)/1000,
			(GoT_takt2*NUM_SAMPLES)/1000,
			(GoT_takt2*NUM_SAMPLES)/1000,
			(GoT_takt1*NUM_SAMPLES)/1000, 
			(GoT_takt1*NUM_SAMPLES)/1000, 
			(GoT_takt2*NUM_SAMPLES)/1000, 
			(GoT_takt2*NUM_SAMPLES)/1000,
			(GoT_takt1*NUM_SAMPLES)/1000, 
			(GoT_takt1*NUM_SAMPLES)/1000, 
			(GoT_takt2*NUM_SAMPLES)/1000, 
			(GoT_takt2*NUM_SAMPLES)/1000,
			(GoT_takt1*NUM_SAMPLES)/1000, 
			(GoT_takt1*NUM_SAMPLES)/1000, 
			(GoT_takt2*NUM_SAMPLES)/1000, 
			(GoT_takt2*NUM_SAMPLES)/1000,
			(GoT_takt1*NUM_SAMPLES)/1000, 
			(GoT_takt1*NUM_SAMPLES)/1000, 
			(GoT_takt1*NUM_SAMPLES)/1000, 
			(GoT_hel*NUM_SAMPLES)/1000,
			(GoT_hel*NUM_SAMPLES)/1000,
			0
		},
	};

	struct song still_alive_song = {
		{
			NUM_SAMPLES/784, //This
			NUM_SAMPLES/740, //was
			NUM_SAMPLES/659, //a
			NUM_SAMPLES/659, //triumph
			NUM_SAMPLES/740, 
			0,
			NUM_SAMPLES/784, //I'm making
			NUM_SAMPLES/740,
			NUM_SAMPLES/659, //a
			NUM_SAMPLES/659, //note
			NUM_SAMPLES/740, //here
			0,
			NUM_SAMPLES/587, //huge
			0,
			NUM_SAMPLES/659, //success
			NUM_SAMPLES/440,
			0,
			NUM_SAMPLES/440, //It's
			NUM_SAMPLES/659, //hard
			NUM_SAMPLES/740, //to
			NUM_SAMPLES/784, //overstate
			NUM_SAMPLES/659,
			NUM_SAMPLES/554,
			NUM_SAMPLES/587, //my
			NUM_SAMPLES/659, //satisfaction
			NUM_SAMPLES/440,
			NUM_SAMPLES/440,
			NUM_SAMPLES/740,
			-1
		}, 
		{
			(250*NUM_SAMPLES)/1000, //This
			(250*NUM_SAMPLES)/1000, //was
			(250*NUM_SAMPLES)/1000, //a
			(250*NUM_SAMPLES)/1000, //triumph
			(400*NUM_SAMPLES)/1000, 
			(1200*NUM_SAMPLES)/1000, //0
			(250*NUM_SAMPLES)/1000, //I'm making
			(250*NUM_SAMPLES)/1000,
			(250*NUM_SAMPLES)/1000, //a
			(250*NUM_SAMPLES)/1000, //note
			(500*NUM_SAMPLES)/1000, //here
			(200*NUM_SAMPLES)/1000, //0
			(400*NUM_SAMPLES)/1000,	//huge
			(150*NUM_SAMPLES)/1000, //0
			(250*NUM_SAMPLES)/1000, //success
			(250*NUM_SAMPLES)/1000,
			(1200*NUM_SAMPLES)/1000,	//0
			(250*NUM_SAMPLES)/1000, //It's
			(400*NUM_SAMPLES)/1000, //hard
			(250*NUM_SAMPLES)/1000, //to
			(350*NUM_SAMPLES)/1000, //overstate
			(350*NUM_SAMPLES)/1000,
			(350*NUM_SAMPLES)/1000,
			(250*NUM_SAMPLES)/1000, //my
			(500*NUM_SAMPLES)/1000, //satisfaction
			(500*NUM_SAMPLES)/1000,
			(250*NUM_SAMPLES)/1000,
			(250*NUM_SAMPLES)/1000,
			0
		},
	};

	struct song lisa_song = {
		{
			NUM_SAMPLES/C4, 
			NUM_SAMPLES/D4, 
			NUM_SAMPLES/E4, 
			NUM_SAMPLES/F4,
			NUM_SAMPLES/G4,
			NUM_SAMPLES/G4,
			NUM_SAMPLES/A4,
			NUM_SAMPLES/A4,
			NUM_SAMPLES/A4,
			NUM_SAMPLES/A4,
			NUM_SAMPLES/G4,
			0,
			NUM_SAMPLES/F4,
			NUM_SAMPLES/F4,
			NUM_SAMPLES/F4,
			NUM_SAMPLES/F4,
			NUM_SAMPLES/E4, 
			NUM_SAMPLES/E4, 
			NUM_SAMPLES/D4, 
			NUM_SAMPLES/D4, 
			NUM_SAMPLES/D4, 
			NUM_SAMPLES/D4, 
			NUM_SAMPLES/C4, 
			-1
		}, 
		{
			(250*NUM_SAMPLES)/1000, 
			(250*NUM_SAMPLES)/1000, 
			(250*NUM_SAMPLES)/1000, 
			(250*NUM_SAMPLES)/1000,
			(500*NUM_SAMPLES)/1000,
			(500*NUM_SAMPLES)/1000,
			(250*NUM_SAMPLES)/1000,
			(250*NUM_SAMPLES)/1000,
			(250*NUM_SAMPLES)/1000,
			(250*NUM_SAMPLES)/1000,
			(500*NUM_SAMPLES)/1000,
			(500*NUM_SAMPLES)/1000, //0
			(250*NUM_SAMPLES)/1000,
			(250*NUM_SAMPLES)/1000,
			(250*NUM_SAMPLES)/1000,
			(250*NUM_SAMPLES)/1000,
			(500*NUM_SAMPLES)/1000,
			(500*NUM_SAMPLES)/1000, 
			(250*NUM_SAMPLES)/1000,
			(250*NUM_SAMPLES)/1000,
			(250*NUM_SAMPLES)/1000,
			(250*NUM_SAMPLES)/1000, 
			(500*NUM_SAMPLES)/1000,
			0
		},
	};

	current_song_pointer = &startup_song; //Just need to initialize it as something
	got_song_pointer = &GoT_song;
	startup_song_pointer = &startup_song;
	still_alive_song_pointer = &still_alive_song;
	lisa_song_pointer = &lisa_song;
}