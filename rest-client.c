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


#include <stdio.h>
#include <curl/curl.h>
#include <assert.h>

void rest_client_initialize()
{
  curl_global_init(CURL_GLOBAL_ALL);
}

void rest_client_cleanup()
{
  curl_global_cleanup();
}

const char* post_like_event(char *hex_uid)
{
  char post_fiels[100];
  const char *uid_param_name = "user_uid";
  assert( sizeof(hex_uid) + sizeof(uid_param_name) < sizeof(post_fiels)+1 );
  sprintf(post_fiels, "%s=%s", uid_param_name, hex_uid);

  CURL *curl = curl_easy_init();
  if(curl) {
    curl_easy_setopt(curl, CURLOPT_URL, "http://localhost:5000/dump/body");
    curl_easy_setopt(curl, CURLOPT_POSTFIELDS, post_fiels);
    
    CURLcode res = curl_easy_perform(curl);
    
    curl_easy_cleanup(curl);

    if(res != CURLE_OK) {
      return curl_easy_strerror(res);
    } else {
      return NULL;
    }
  } else {
    return "Impossible initialize CURL";
  }
}