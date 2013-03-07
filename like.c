/*-
 * Public platform independent Near Field Communication (NFC) library examples
 * 
 * Copyright (C) 2010, Romuald Conty
 * Copyright (C) 2011, Romain Tartiere, Romuald Conty
 * 
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *  1) Redistributions of source code must retain the above copyright notice,
 *  this list of conditions and the following disclaimer. 
 *  2 )Redistributions in binary form must reproduce the above copyright
 *  notice, this list of conditions and the following disclaimer in the
 *  documentation and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE
 * LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 * 
 * Note that this license only applies on the examples, NFC library itself is under LGPL
 *
 */


#include <err.h>
#include <signal.h>
#include <stdio.h>
#include <stddef.h>
#include <stdlib.h>
#include <string.h>

#include <nfc/nfc.h>
#include <nfc/nfc-types.h>

#include "nfc-utils.h"

#define MAX_DEVICE_COUNT 16

static nfc_device *pnd = NULL;

void stop_polling (int sig)
{
  (void) sig;
  if (pnd)
    nfc_abort_command (pnd);
  else
    exit (EXIT_FAILURE);
}

int nfc_poll(nfc_target *pnt) 
{
  const uint8_t uiPollNr = 20;
  const uint8_t uiPeriod = 2;
  const nfc_modulation nmModulations[5] = {
    { .nmt = NMT_ISO14443A, .nbr = NBR_106 },
    { .nmt = NMT_ISO14443B, .nbr = NBR_106 },
    { .nmt = NMT_FELICA, .nbr = NBR_212 },
    { .nmt = NMT_FELICA, .nbr = NBR_424 },
    { .nmt = NMT_JEWEL, .nbr = NBR_106 },
  };
  const size_t szModulations = 5;
  printf ("NFC device will poll during %ld ms (%u pollings of %lu ms for %zd modulations)\n", (unsigned long) uiPollNr * szModulations * uiPeriod * 150, uiPollNr, (unsigned long) uiPeriod * 150, szModulations);
  int res = 0;
  if ((res = nfc_initiator_poll_target (pnd, nmModulations, szModulations, uiPollNr, uiPeriod, pnt))  < 0) {
    // TODO move that code out of here
    nfc_perror (pnd, "nfc_initiator_poll_target");
    nfc_close (pnd);
    nfc_exit (NULL);
    exit (EXIT_FAILURE);
  }

  return res;
}


void to_hex_string(const uint8_t *buff, char *str)
{
  // TODO assert the size of str ... 

  const char * hex = "0123456789abcdef";
  const unsigned char * pin = buff;
  char * pout = str;
  int j = 0;
  for(; j < sizeof(buff); ++j){
    *pout++ = hex[(*pin>>4)&0xF];
    *pout++ = hex[(*pin++)&0xF];
  }
  *pout = 0;
}



int main (int argc, const char *argv[])
{

  signal (SIGINT, stop_polling);

  // Display libnfc version
  const char *acLibnfcVersion = nfc_version ();
  printf ("%s uses libnfc %s\n", argv[0], acLibnfcVersion);

  
  nfc_init (NULL);

  pnd = nfc_open (NULL, NULL);

  if (pnd == NULL) {
    ERR ("%s", "Unable to open NFC device.");
    exit (EXIT_FAILURE);
  }

  if (nfc_initiator_init (pnd) < 0) {
    nfc_perror (pnd, "nfc_initiator_init");
    exit (EXIT_FAILURE);    
  }

  printf ("NFC reader: %s opened\n", nfc_device_get_name (pnd));
 
  while(1) {
    nfc_target nt;
    int res = nfc_poll(&nt);
   

    if (res > 0) {
      bool verbose = false;
      print_nfc_target ( nt, verbose );


      // To finish ... 
      if(nt.nm.nmt == NMT_ISO14443A)
      {        
        nfc_iso14443a_info nai = nt.nti.nai;
        char hex_uid[21];
        to_hex_string(nai.abtUid, hex_uid);

        printf("nico_uid=%s\n", hex_uid);
      }

    } else {
      printf ("No target found.\n");
    }
  }


  nfc_close (pnd);
  nfc_exit (NULL);
  exit (EXIT_SUCCESS);
}
