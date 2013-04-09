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

#include "stdio.h"
#include "fcntl.h"
#include "unistd.h"

#include "led.h"

const char *GPIO_ON = "1";
const char *GPIO_OFF = "0";

const int LED_PIN = 27;

/* TODO : we should be fully equivalent of : 

sudo su
cd /sys/class/gpio
echo 27 > export
cd gpio27
echo out > direction
echo 1 > value
*/

int write_gpio(const int pin, const char *value)
{
	char gpio_name[128];
	sprintf(gpio_name, "/sys/class/gpio/gpio%d/value", pin) ;
	const int fd = open(gpio_name, O_WRONLY);
	write(fd, value, 1); /* TODO check the write values ? */
	return close(fd);
}

int turn_led_on() 
{
	return write_gpio(LED_PIN, GPIO_ON);
}

int turn_led_off() 
{
	return write_gpio(LED_PIN, GPIO_OFF);
}