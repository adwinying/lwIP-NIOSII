/*
 * Simple lwIP 1.3.1 example program.
 *
 * Expects no Operating System (NO_SYS=1)
 *
 * lwIP initialization and optional DHCP and WEB server support can be
 * enabled by setting #defines described below.
 *
 * You must plug in your board's MAC address below before compiling!
 *
 * Contributed by: Bill Auerbach lwip@softools.com
 */

#include <stdio.h>
#include <unistd.h>
#include "system.h"
#include "sys/alt_timestamp.h"
#include "sys/alt_alarm.h"

#include "lwip/init.h"
#include "lwip/netif.h"
#include "lwip/dhcp.h"
#include "lwip/tcp.h"
#include "lwip/tcp_impl.h"
#include "lwip/stats.h"
#include "lwip/ip_frag.h"
#include "lwip/ip_addr.h"
#include "netif/etharp.h"
#include "alteraTseEthernetif.h"

#include "histogram.h"
#include "echo.h"

#define mSdelay(x) usleep(x*1000)

//  Define USE_DHCP to 1 to use DHCP.  Otherwise a static IP must be defined
#define USE_DHCP 0
#if ! LWIP_DHCP
#error LWIP_DHCP must be enabled in lwipopts.h
#endif
#if ! USE_DHCP
#define STATIC_IP IP4_ADDR(&lwipStaticIp,172,24,11,32)  // Must be in this format (change the IP addr if needed)
struct ip_addr lwipStaticIp;
#endif

//  Define BUILD_HTTPD to 1 to build the httpserver_raw contrib example
#define BUILD_HTTPD 0
#define UDP_ECHO 1

//  Alarm & timer variables - provides a 250mS counter
void        lwipProcessTimers(void);
static alt_alarm lwipAlarm;
static alt_u32  lwipProcessTimerFlag;
static alt_u32  lwipTicksPer250mS;
static alt_u32  lwip250mStimer;

//  Alarm callback function.
alt_u32 lwipAlarmCallback(void* context)
{
    lwipProcessTimerFlag = 1;          //  Set flag to process timers
    return lwipTicksPer250mS;
}

//  Define netif for lwIP
struct netif    alteraTseNetif;

int main()
{
static struct ip_addr   ip_zero = { 0 };
void httpd_init(void);
void udpecho_init(void);

// Enable/disable this in echo.h
#if TEST_HISTOGRAM
    // Get timestamp freq
    timestamp_freq = alt_timestamp_freq();
    // Init histogram
	int i;
    for (i = 1; i <= TNUM_HIST; i++){
        if(init_hist(i, MAX_HIST_VAL, histarea)){
        	//printf("%d OK\n", i);
        } else {
        	printf("%d ERR\n", i);
        }
    }
#endif /* TEST_HISTOGRAM */

    printf("Running...\n");
    lwip250mStimer = 0;
    lwipProcessTimerFlag = 0;
    lwipTicksPer250mS = alt_ticks_per_second() / 4;
    if (alt_alarm_start (&lwipAlarm, lwipTicksPer250mS, lwipAlarmCallback, NULL) < 0)
        {
        printf ("System clock is required!\n");
        for(;;);
        }
    
    //  Load platform specific MAC address into netif
    alteraTseNetif.hwaddr[0] = 0x00;
    alteraTseNetif.hwaddr[1] = 0x07;
    alteraTseNetif.hwaddr[2] = 0xab;
    alteraTseNetif.hwaddr[3] = 0xf0;
    alteraTseNetif.hwaddr[4] = 0x0d;
    alteraTseNetif.hwaddr[5] = 0xba;

    //  Initialize lwIP, Altera TSE and the ethernetif
    lwip_init();
    if(netif_add(&alteraTseNetif, &ip_zero, &ip_zero, &ip_zero, alteraTseNetif.state, ethernetif_init, ethernet_input) == NULL)
        {
        printf( "Fatal error initializing...\n" );
        for(;;);
        }
    netif_set_default(&alteraTseNetif);
    
    // Initialize Altera TSE in a loop if waiting for a link
    printf("Waiting for link...");
    while(((struct ethernetif *) alteraTseNetif.state)->link_alive!=1)
        {
        mSdelay(1000);
        putchar('.');
        tse_mac_init(0, alteraTseNetif.state);
        }
    printf("OK\n");
        
#if USE_DHCP
    dhcp_start(&alteraTseNetif);
    //  Wait for DCHP
    printf("Waiting for DHCP IP address...");
    while(! netif_is_up(&alteraTseNetif))
        {
        //  Process lwIP timer dependent code
        if(lwipProcessTimerFlag)
            lwipProcessTimers();
        //  Poll lwIP for incoming packets.
        ethernetif_input(&alteraTseNetif);
        }
#else
    STATIC_IP;
    netif_set_ipaddr(&alteraTseNetif, &lwipStaticIp);
    netif_set_up(&alteraTseNetif);
#endif
    printf("IP address: %s\n", ip_ntoa(&alteraTseNetif.ip_addr));

#if BUILD_HTTPD
    httpd_init();
#endif
    udpecho_init();
    //  This is the main loop for lwIP - other processing can be done by calling application functions.
    for(;;)
        {
        //  Process lwIP timer dependent code
        if(lwipProcessTimerFlag)
            lwipProcessTimers();
        //  Poll lwIP for incoming packets.
        ethernetif_input(&alteraTseNetif);
        }
    return 0;
}

//  Run this every 250mS to update lwIP timers
void        lwipProcessTimers(void)
{
    lwipProcessTimerFlag = 0;
    lwip250mStimer += 250;
    if( (lwip250mStimer % TCP_TMR_INTERVAL) == 0 ) { tcp_tmr(); }
    if( (lwip250mStimer % ARP_TMR_INTERVAL) == 0 ) { etharp_tmr(); }
#if IP_REASSEMBLY
    if( (lwip250mStimer % IP_TMR_INTERVAL) == 0 ) { ip_reass_tmr(); }
#endif
#if LWIP_AUTOIP
    if( (lwip250mStimer % AUTOIP_TMR_INTERVAL ) == 0 ) { autoip_tmr(); }
#endif
#if LWIP_IGMP
    if( (lwip250mStimer % IGMP_TMR_INTERVAL ) == 0 ) { igmp_tmr(); } 
#endif
#if LWIP_DHCP
    if( (lwip250mStimer % DHCP_FINE_TIMER_MSECS ) == 0 ) { dhcp_fine_tmr(); }
    if( (lwip250mStimer % (DHCP_COARSE_TIMER_SECS*1000) ) == 0 ) { dhcp_coarse_tmr(); }
#endif
#if LWIP_DNS
    if( (lwip250mStimer % DNS_TMR_INTERVAL) == 0 ) { dns_tmr(); }
#endif
}
