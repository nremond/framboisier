/**
 * Copyright 2013 Nicolas Rémond (@nremond)
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *    http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include <signal.h>
#include <string.h>
#include <assert.h>
#include <unistd.h>

#include "nfc/nfc.h"
#include "nfc/nfc-types.h"

#include "led.h"
#include "nfc-utils.h"
#include "rest-client.h"

#define MAX_DEVICE_COUNT 16

static nfc_device *pnd = NULL;

void to_hex_string(const uint8_t *buff, size_t buff_size, char *str, size_t str_size)
{
  assert(buff != NULL);
  assert(str != NULL);
  assert( buff_size*2 < str_size);

  const char * hex = "0123456789abcdef";
  const uint8_t * pbuff = buff;
  char * pout = str;
  for(int j = 0; j < buff_size; ++j){
    *pout++ = hex[(*pbuff>>4) & 0xF];
    *pout++ = hex[(*pbuff++) & 0xF];
  }
  *pout = 0;
}

void initialize_ressources()
{
  /* NFC Client */
  nfc_init(NULL);
  
  /* REST client */
  rest_client_initialize();

  /* The control LED should be turned off */
  turn_led_off();
}

void cleanup_ressources()
{
  nfc_close(pnd);
  nfc_exit(NULL);

  rest_client_cleanup();
}

void stop_polling(int sig)
{
  (void) sig;
  if (pnd)
    nfc_abort_command(pnd);
  else
    exit(EXIT_FAILURE);
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
    // TODO That should be a "for(;;)" loop !!
    nfc_perror (pnd, "nfc_initiator_poll_target");
    cleanup_ressources();
    exit (EXIT_FAILURE);
  }

  return res;
}

void event_loop(const char *server_host)
{
  for(;;) {
    nfc_target nt;
    const int res = nfc_poll(&nt);
   
    if (res > 0) { 
      // TODO remove that afterwards
      // bool verbose = false;
      // print_nfc_target ( nt, verbose );


      // To finish ... 
      if(nt.nm.nmt == NMT_ISO14443A)
      {        
        const nfc_iso14443a_info nai = nt.nti.nai;
        char hex_uid[32];
        to_hex_string(nai.abtUid, nai.szUidLen, hex_uid, sizeof hex_uid);

        printf("nico_uid=%s\n", hex_uid);
        post_like_event(server_host, hex_uid);

        turn_led_on();
        sleep(2); /* in seconds */
        turn_led_off();

      }

    } else {
      printf ("No target found.\n");
    }
  }
}

const char* parse_arguments(const int argc, const char *argv[])
{
  const char *host_arg = "--server_host";
  if(argc!=3 || strcmp(argv[1], host_arg)!=0) 
  {
    printf("usage: %s %s foo.local\n", argv[0], host_arg);
    return NULL;
  } 
  else 
  {
    return argv[2];
  }
}


int main(const int argc, const char *argv[])
{
  const char *server_host = parse_arguments(argc, argv);
  if(server_host == NULL) {
    exit(EXIT_FAILURE);
  }

  signal(SIGINT, stop_polling);
 
  initialize_ressources();

  pnd = nfc_open (NULL, NULL);

  if(pnd == NULL) {
    ERR ("%s", "Unable to open NFC device.");
    exit (EXIT_FAILURE);
  }

  if(nfc_initiator_init (pnd) < 0) {
    nfc_perror (pnd, "nfc_initiator_init");
    exit (EXIT_FAILURE);    
  }

  printf("Events will be sent to 'http://%s' and using libnfc %s\n", server_host, nfc_version());
  printf("NFC reader: %s opened\n", nfc_device_get_name (pnd));
 
  event_loop(server_host);

  cleanup_ressources();
  exit (EXIT_SUCCESS);
}




