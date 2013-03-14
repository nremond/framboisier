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

#include "assert.h"
#include "stdio.h"
#include "string.h"
#include "curl/curl.h"

void rest_client_initialize()
{
  curl_global_init(CURL_GLOBAL_ALL);
}

void rest_client_cleanup()
{
  curl_global_cleanup();
}

size_t http_write(void* buf, size_t size, size_t nmemb, void* userp)
{
  printf("body=%s\n", (char*)buf);
  return nmemb;
}

const char* post_like_event(const char *server_host, const char *hex_uid)
{
  char post_fiels[128];
  const char *uid_param_name = "user_uid";
  assert( strlen(hex_uid) + strlen(uid_param_name) < sizeof(post_fiels)+1 );
  snprintf(post_fiels, 128, "%s=%s", uid_param_name, hex_uid);

  CURL *curl = curl_easy_init();
  if(curl) {
    char url[256];
    snprintf(url, 256, "http://%s/dump/post_form", server_host);

    curl_easy_setopt(curl, CURLOPT_URL, url);
    curl_easy_setopt(curl, CURLOPT_POSTFIELDS, post_fiels);
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, http_write);

    CURLcode res = curl_easy_perform(curl);
    
    curl_easy_cleanup(curl);

    if(res != CURLE_OK) {
      return curl_easy_strerror(res);
    } else {
      return NULL;
    }
  } else {
    return "Impossible to initialize CURL";
  }
}