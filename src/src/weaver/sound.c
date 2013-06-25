/*
  Copyright (C) 2009 Thiago Leucz Astrizi

 This file is part of Weaver API.

 Weaver API is free software: you can redistribute it and/or modify
 it under the terms of the GNU General Public License as published by
 the Free Software Foundation, either version 3 of the License, or
 (at your option) any later version.

 Weaver API is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 GNU General Public License for more details.

 You should have received a copy of the GNU General Public License
 along with Weaver API.  If not, see <http://www.gnu.org/licenses/>.
*/

#define _GNU_SOURCE
#include <pthread.h>
#include <sched.h>
#include <vorbis/vorbisfile.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <signal.h>
#include "weaver.h"
#include "sound.h"
#include "display.h"

pthread_t _music;
pthread_t _sound;


int _handle_vorbis_error(int status){
  if(status < 0){
    fprintf(stderr, "Unknown error...\n");
    return 1;
  }
  else if(status == OV_HOLE){
    fprintf(stderr, "Error: A hole in vorbis file.\n");
    exit(1);
  }
  else if(status == OV_EBADLINK){
    fprintf(stderr, "Error: Invalid stream.\n");
    exit(1);
  }
  else if(status == OV_EINVAL){
    fprintf(stderr, "Error: Corrupted headers.\n");
    exit(1);
  }
  return 0;
}

void handle_pcm_error(int status, snd_pcm_uframes_t frames, snd_pcm_t **handle){
  if(status == -EPIPE){
    snd_pcm_prepare(*handle);

  }
  else{
    if(status < 0){
      fprintf(stderr, "error from writei: %s\n", snd_strerror(status));
    }
    else if (status != (int) frames) {
      //fprintf(stderr, "short write, write %d frames\n", status);
    }
  }
}

// This functions opens an Ogg Vorbis file and play it
void play_sound(char *file){
  int status;
  char *path = (char *) malloc(sizeof(char) * (strlen(file) + 6));
  path[0] = '\0';
  strcat(path, "sound/");
  strcat(path, file);
  if(_sound)
    pthread_cancel(_sound);
  status = pthread_create(&_sound, NULL, _play_soundfile, (void *) path);
  if(status)
    fprintf(stderr, "Warning: Error creating thread. Error status: %d\n", status);
}

void play_music(char *file){
  int status;
  char *path = (char *) malloc(sizeof(char) * (strlen(file) + 6));
  path[0] = '\0';
  strcat(path, "music/");
  strcat(path, file);
  if(_music)
    pthread_cancel(_music);
  status = pthread_create(&_music, NULL, _play_soundfile, (void *) path);
  if(status)
    fprintf(stderr, "Warning: Error creating thread. Error status: %d\n", status); 
}

void stop_music(void){
  if(_music)
    pthread_cancel(_music);
}

void *_play_soundfile(void *file){
  int status;
  snd_pcm_t *handle;
  snd_pcm_hw_params_t *params;
  snd_pcm_uframes_t frames;
  int size;
  char *buffer;
  OggVorbis_File vf;
  FILE *fp;
  int current_section;
  struct timespec req = {0, 10000000}; // 0.01 seconds
  char *path;
  int infinite_loop = 0;

  if((* (char *) file) == 'm'){
    infinite_loop = 1;
  }
  pthread_setcancelstate(PTHREAD_CANCEL_DISABLE, NULL);
  path = (char *) malloc(strlen((char *) file) + 60);
  path[0] = '\0';

  // Opening file passed as argument
  strcat(path, "/usr/share/games/DUMMY/");
  strcat(path, (char *) file);
  fp = fopen(path, "rb");
  if(!fp){
    path[0] = '\0';
    strcat(path, (char *) file);
    fp = fopen(path, "rb");
  }

  // Opening PCM device
  status = snd_pcm_open(&handle, "default", SND_PCM_STREAM_PLAYBACK, 0);
  if(status < 0){
    fprintf(stderr, "unable to open PCM device: %s\n", snd_strerror(status));
    return (void *) 1;
  }

  // Allocate hardware parameters and set them to default
  snd_pcm_hw_params_alloca(&params);
  snd_pcm_hw_params_any(handle, params);

  // Initializing Ogg Vorbis decoding
  status = ov_open(fp, &vf, NULL, 0);
  if(status == OV_EREAD){
    fprintf(stderr, "Warning: Error reading from %s.\n", path);
    return (void *) 1;
  }
  else if(status == OV_ENOTVORBIS){
    fprintf(stderr, "Warning: %s is not an Ogg Vorbis file.\n", path);
    return (void *) 1;
  }
  else if(status == OV_EVERSION){
    fprintf(stderr, "Warning: Vorbis version mismatch in %s.\n", path);
    return (void *) 1;
  }
  else if(status == OV_EBADHEADER){
    fprintf(stderr, "Warning: Invalid Vorbis header in %s.\n", path);
    return (void *) 1;
  }
  else if(status == OV_EFAULT){
    fprintf(stderr, "Warning: Internal logic fault decoding file %s.\n", path);
    return (void *) 1;
  }

  // Getting information from the file
  {
    //char **ptr=ov_comment(&vf,-1)->user_comments;
    vorbis_info *vi=ov_info(&vf,-1);
    //while(*ptr){
    // fprintf(stderr,"%s\n",*ptr);
    // ++ptr;
    //  }

    // A frame contains 1 sample in Mono and 2 in Stereo
    frames = 1024;
    size = 4096;
    // Adjusting some informations in PCM according with what we read
    snd_pcm_hw_params_set_access(handle, params, SND_PCM_ACCESS_RW_INTERLEAVED);
    snd_pcm_hw_params_set_format(handle, params, SND_PCM_FORMAT_S16_LE);
    snd_pcm_hw_params_set_channels(handle, params, vi -> channels);
    snd_pcm_hw_params_set_rate(handle, params, (unsigned) vi -> rate, 0);
    //snd_pcm_hw_params_set_rate_near(handle, params, (unsigned *) &(vi -> rate), &dir);
    snd_pcm_hw_params_set_period_size(handle, params, frames, 0);
    //snd_pcm_hw_params_set_period_size_near(handle, params, &frames, &dir);
    snd_pcm_hw_params_set_buffer_size(handle, params, size);
    //snd_pcm_hw_params_set_buffer_size_near(handle, params, (snd_pcm_uframes_t *) &size);
    status = snd_pcm_hw_params(handle, params);
    if(status < 0){
      fprintf(stderr, "unable to set hw parameters: %s\n", snd_strerror(status));
      return (void *) 1;
    }
  }

  buffer = (char *) malloc(size);

  snd_pcm_prepare(handle);

  pthread_cleanup_push(free, file);
  pthread_cleanup_push(free, (void*) path);
  pthread_cleanup_push(free, (void *) buffer);
  pthread_cleanup_push(_close_vorbis_file, (void *) &vf);
  pthread_cleanup_push(_close_pcm, (void *) handle);
  pthread_setcancelstate(PTHREAD_CANCEL_ENABLE, NULL);

  do{
    while(1){
      nanosleep(&req, NULL);
      int ret;
      status = 0;
      do{
	ret = ov_read(&vf, &buffer[status], size - status, 0, 2, 1,
		      &current_section);
	if(ret == 0)
	  break;
	if(!_handle_vorbis_error(ret))
	  status += ret;
      } while(status < size);

      if(ret == 0){
	if(status > 0){
	  snd_pcm_hw_params_set_buffer_size(handle, params, size);
	  snd_pcm_hw_params(handle, params);
	  status = snd_pcm_writei(handle, buffer, frames);
	  handle_pcm_error(status, frames, &handle);
	}
	break;
      }
      if(status > 0){
	status = snd_pcm_writei(handle, buffer, frames);
	handle_pcm_error(status, frames, &handle);
      }
    }
    if(infinite_loop){
      pthread_setcancelstate(PTHREAD_CANCEL_DISABLE, NULL);
      ov_clear(&vf);
      fp = fopen(path, "rb");
      ov_open(fp, &vf, NULL, 0);
      pthread_setcancelstate(PTHREAD_CANCEL_ENABLE, NULL);
    }
  } while(infinite_loop);
  // Finishing
  pthread_cleanup_pop(1);
  pthread_cleanup_pop(1);
  pthread_cleanup_pop(1);
  pthread_cleanup_pop(1);
  pthread_cleanup_pop(1);
  pthread_exit(NULL);
  return NULL;
}

void _close_vorbis_file(void *vf){
  ov_clear((OggVorbis_File *) vf);
  return;
}

void _close_pcm(void *pcm){
  snd_pcm_drain((snd_pcm_t *) pcm);
  snd_pcm_close((snd_pcm_t *) pcm);
  return;
}
