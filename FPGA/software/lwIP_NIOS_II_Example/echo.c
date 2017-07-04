/*
 * Copyright (c) 2001-2003 Swedish Institute of Computer Science.
 * All rights reserved. 
 * 
 * Redistribution and use in source and binary forms, with or without modification, 
 * are permitted provided that the following conditions are met:
 *
 * 1. Redistributions of source code must retain the above copyright notice,
 *    this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright notice,
 *    this list of conditions and the following disclaimer in the documentation
 *    and/or other materials provided with the distribution.
 * 3. The name of the author may not be used to endorse or promote products
 *    derived from this software without specific prior written permission. 
 *
 * THIS SOFTWARE IS PROVIDED BY THE AUTHOR ``AS IS'' AND ANY EXPRESS OR IMPLIED 
 * WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF 
 * MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT 
 * SHALL THE AUTHOR BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, 
 * EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT 
 * OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS 
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN 
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING 
 * IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY 
 * OF SUCH DAMAGE.
 *
 * This file is part of the lwIP TCP/IP stack.
 * 
 * Author: Adam Dunkels <adam@sics.se>
 *
 */

#include "lwip/udp.h"
#include "lwip/debug.h"
#include "histogram.h"
#include "echo.h"

/*
 * Histogram variables
 */
#if TEST_HISTOGRAM
uint32          histarea[1001] = { 0 };
ObjectIDType    histid = 1;
boolean         measuring_flg = FALSE;
boolean         printed_flg = FALSE;
#endif

void udpecho_recv(void *arg, struct udp_pcb *pcb, struct pbuf *p, struct 
ip_addr *addr, u16_t port)
{
//#if TEST_HISTOGRAM
#if 0
    if (histid <= TNUM_HIST && measuring_flg == FALSE) {
        printf("[histid: %d] Histogram measuring starting...\n", histid);
        begin_measure(histid);
        measuring_flg = TRUE;
    }
#endif

    if (p != NULL) {
        /* send received packet back to sender */
        udp_sendto(pcb, p, addr, port);
        /* free the pbuf */
        pbuf_free(p);
    }

#if TEST_HISTOGRAM
    // End histogram measure
    if (histid <= TNUM_HIST && measuring_flg == TRUE) {
        //printf("[histid: %d] Histogram measuring ending...\n", histid);
        if (end_measure(histid++)) {
            //printf("%d OK\n", histid);
        } else {
            //printf("%d ERR\n", histid);
        }
        measuring_flg = FALSE;
    }

    // Print histogram results
    if (histid > TNUM_HIST && printed_flg == FALSE) {
        printf("[histid: %d] Printing...\n", histid);
        printed_flg = TRUE;
        print_hist(TNUM_HIST);
    }
#endif
}


void udpecho_init(void)
{
    struct udp_pcb * pcb;
    printf("Initializing udp_init()...\n");

    /* get new pcb */
    pcb = udp_new();
    if (pcb == NULL) {
        printf("udp_new failed!\n");
        return;
    }

    /* bind to any IP address on port 7 */
    if (udp_bind(pcb, IP_ADDR_ANY, 7) != ERR_OK) {
        printf("udp_bind failed!\n");
        return;
    }

    /* set udpecho_recv() as callback function
       for received packets */
    printf("Ready to accept UDP packets on port 7\n");
    udp_recv(pcb, udpecho_recv, NULL);
}
