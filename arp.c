#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <stdbool.h>
#include <winsock2.h>
#include <ws2ipdef.h>
#include <iphlpapi.h>
#include <ws2tcpip.h>

#include "arp.h"

static bool ping = true;

static bool handler(DWORD type){
    if(CTRL_C_EVENT == type){
        ping = false;
        return true;
    }
    return false;
}

static void get_ip(IPAddr *src, int index){
    PIP_ADAPTER_INFO adapterInfo, adapter;
    ULONG buffer = sizeof(IP_ADAPTER_INFO);
    GetAdaptersInfo(NULL, &buffer);
    if((adapterInfo = (IP_ADAPTER_INFO*)malloc(buffer)) != NULL){
        if(GetAdaptersInfo(adapterInfo, &buffer) == NO_ERROR){
            adapter = adapterInfo;
            while(adapter){
                if(adapter->Index == index)
                    *src = inet_addr(adapter->IpAddressList.IpAddress.String);
                adapter = adapter->Next;
            }
        }
        free(adapterInfo);
    }
}

static void print(const char *ipaddr, const BYTE *mac, const double rtt){
    printf("Replay from %s %02X:%02X:%02X:%02X:%02X:%02X time=%.3lf ms\n", ipaddr, mac[0], mac[1], mac[2], mac[3], mac[4], mac[5], rtt);
}

void arping(const char *ipaddr, LONGLONG count){
    IPAddr dst = inet_addr(ipaddr);
    IPAddr src;
    ULONG len = 6;
    double rtt = 0;
    ULONG mac[2];
    DWORD adapter = 0;
    LONGLONG count_pckt = 0;
    ULONGLONG send_count = 0;
    ULONGLONG recv_count = 0;
    LARGE_INTEGER start_time;
	LARGE_INTEGER end_time;
	LARGE_INTEGER cpu_frq;
    GetBestInterface(dst, &adapter);
    get_ip(&src, adapter);
    SetConsoleCtrlHandler((PHANDLER_ROUTINE)handler, true);
    while(ping != false && count_pckt != count){
        memset(mac, 0xff, sizeof(mac));
        QueryPerformanceFrequency((LARGE_INTEGER *)&cpu_frq);
        QueryPerformanceCounter((LARGE_INTEGER *)&start_time);
        if(SendARP(dst, src, &mac, &len) == NO_ERROR){
            QueryPerformanceCounter((LARGE_INTEGER *)&end_time);
            rtt = ((double)((end_time.QuadPart - start_time.QuadPart) * (double) 1000.0 / (double)cpu_frq.QuadPart));
            print(ipaddr, (BYTE*)mac, rtt);
            recv_count++;
        }
        count_pckt++;
        send_count++;
        Sleep(1000);
    }
    printf("---- Statistics %s ----\n", ipaddr);
    printf("Sent packets %llu, Received packets %llu\n", send_count, recv_count);
}
