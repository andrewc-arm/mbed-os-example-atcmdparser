/* ATCmdParser usage example
 * Copyright (c) 2016 ARM Limited
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

#include "mbed.h"
#include "ATCmdParser.h"
#include "UARTSerial.h"

#define   PARSER_MODE  SERVER
#define   ESP8266_DEFAULT_BAUD_RATE   115200

UARTSerial *_serial;
ATCmdParser *_parser;

int client_func()
{
    printf("\nATCmdParser Client with ESP8266 example");

    _serial = new UARTSerial(UART_TX0, UART_RX0, ESP8266_DEFAULT_BAUD_RATE);
    _parser = new ATCmdParser(_serial);
    _parser->debug_on( 1 );
    _parser->set_delimiter( "\r\n" );
    
    //Now get the FW version number of ESP8266 by sending an AT command 
    printf("\nATCmdParser: Retrieving FW version");
    _parser->send("AT+GMR");
    int version;
    if(_parser->recv("SDK version:%d", &version) && _parser->recv("OK")) {
        printf("\nATCmdParser: FW version: %d", version);
        printf("\nATCmdParser: Retrieving FW version success");
    } else { 
        printf("\nATCmdParser: Retrieving FW version failed");
        return -1;
    }

    printf("\nDone\n");
    return 0;
}

void atcmd_server_cb_test()
{
    _parser->send("%s: OK\n", __func__);
}

int server_func()
{
    _serial = new UARTSerial(UART_TX0, UART_RX0, ESP8266_DEFAULT_BAUD_RATE);
    _parser = new ATCmdParser(_serial);
    _parser->debug_on( 1 );
    _parser->set_delimiter( "\r\n" );

    // Register AT commands.
    // _parser->oob("+TEST", atcmd_server_cb_test);

    _parser->send("READY: %s:%s\n", __DATE__, __TIME__);
    for (;;)
    {
        // while (_parser->process_oob());
        _parser->send("recv start\n");
        int cT = _parser->getc();
        _parser->putc((char)cT);
        _parser->send("recv end: value = %d\n", cT);
    }

    return -1;
}

int main()
{
#if   PARSER_MODE == SERVER
    return server_func();
#elif PARSER_MODE == CLIENT
    return client_func();
#else
    return 0;
#endif
}

