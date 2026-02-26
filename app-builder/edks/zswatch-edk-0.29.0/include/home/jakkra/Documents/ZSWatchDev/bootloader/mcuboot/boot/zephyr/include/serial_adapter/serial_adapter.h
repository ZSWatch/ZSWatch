/*
 * Copyright (c) 2017 Nordic Semiconductor ASA
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#ifndef H_SERIAL_ADAPTER
#define H_SERIAL_ADAPTER

int
console_out(int c);

void
console_write(const char *str, int cnt);

int
boot_console_init(void);

int
console_read(char *str, int str_cnt, int *newline);

#endif // SERIAL_ADAPTER
