/**
 * @file       BlynkCC3000.h
 * @author     Volodymyr Shymanskyy
 * @license    This project is released under the MIT License (MIT)
 * @copyright  Copyright (c) 2015 Volodymyr Shymanskyy
 * @date       Mar 2015
 * @brief
 *
 */

#ifndef BlynkCC3000_h
#define BlynkCC3000_h

#define BLYNK_INFO_CONNECTION "CC3000"

#include <BlynkApiArduino.h>
#include <Blynk/BlynkProtocol.h>
#include <Adafruit_CC3000.h>
#include <utility/netapp.h>

class BlynkTransportCC3000
{
public:
    BlynkTransportCC3000(Adafruit_CC3000& cc3000)
        : cc3000(cc3000)
        , host(NULL)
        , addr(0)
        , port(0)
    {}

    void begin(uint32_t a, uint16_t p) {
        host = NULL;
        port = p;
        addr = a;
    }

    void begin(const char* h, uint16_t p) {
        host = h;
        port = p;
    }

    bool connect() {
        if (host) {
            BLYNK_LOG2(BLYNK_F("Looking for "), host);
            while (addr == 0) {
                if (!cc3000.getHostByName((char*)host, &addr)) {
                    BLYNK_LOG1(BLYNK_F("Couldn't locate server"));
                    BlynkDelay(500);
                }
            }
        }
        uint8_t* a = (uint8_t*)&addr;
        BLYNK_LOG_IP_REV("olyy28 Connecting to ", a);
        client = cc3000.connectTCP(addr, port);
        return client.connected();
    }

    void disconnect() { client.stop(); }

    size_t read(void* buf, size_t len) {
        return client.readBytes((char*)buf, len);
    }
    size_t write(const void* buf, size_t len) {
        return client.write((const uint8_t*)buf, len);
    }

    bool connected() { return client.connected(); }
    int available() { return client.available(); }

private:
    Adafruit_CC3000& cc3000;
    Adafruit_CC3000_Client client;
    const char* host;
    uint32_t    addr;
    uint16_t    port;
};

class BlynkCC3000
    : public BlynkProtocol<BlynkTransportCC3000>
{
    typedef BlynkProtocol<BlynkTransportCC3000> Base;
public:
    BlynkCC3000(Adafruit_CC3000& cc3000, BlynkTransportCC3000& transp)
        : Base(transp), cc3000(cc3000)
    {}

    void connectWiFi(const char* ssid,
                     const char* pass,
                     uint8_t     secmode)
    {
        if (!cc3000.begin())
        {
            BLYNK_FATAL("Couldn't begin()! Check your wiring?");
        }

#if !defined(CC3000_TINY_DRIVER) && defined(BLYNK_DEBUG)
        uint8_t major, minor;
        if(!cc3000.getFirmwareVersion(&major, &minor))
        {
            if(major != 0x1 || minor < 0x13) {
                BLYNK_LOG1(BLYNK_F("CC3000 upgrade needed?"));
            }
        }
#endif

        // Remove socket inactivity timeout
        /*unsigned long aucDHCP       = 14400;
        unsigned long aucARP        = 3600;
        unsigned long aucKeepalive  = 30;
        unsigned long aucInactivity = 0;
        int iRet = netapp_timeout_values(&aucDHCP, &aucARP, &aucKeepalive, &aucInactivity);
        if (iRet != 0) {
            BLYNK_FATAL("Cannot set netapp timeout!");
        }*/

        /*if (!cc3000.deleteProfiles())
        {
            BLYNK_FATAL("Fail deleting old profiles");
        }*/
        BLYNK_LOG2(BLYNK_F("olyy29Connecting to "), ssid);
        if (!cc3000.connectToAP(ssid, pass, secmode))
        {
            BLYNK_FATAL("Failed to connect to AP");
        }
        BLYNK_LOG1(BLYNK_F("Getting IP..."));
        while (!cc3000.checkDHCP())
        {
            BlynkDelay(100);
        }

        uint32_t ipAddress, netmask, gateway, dhcpserv, dnsserv;
        if(!cc3000.getIPAddress(&ipAddress, &netmask, &gateway, &dhcpserv, &dnsserv))
        {
            BLYNK_FATAL("DHCP failed.");
        }
#ifdef BLYNK_PRINT
        uint8_t* addr = (uint8_t*)&ipAddress;
        BLYNK_LOG_IP_REV("IP: ", addr);
        addr = (uint8_t*)&gateway;
        BLYNK_LOG_IP_REV("GW: ", addr);
        addr = (uint8_t*)&dnsserv;
        BLYNK_LOG_IP_REV("DNS: ", addr);
#endif
    }

    void config(const char* auth,
                const char* domain = BLYNK_DEFAULT_DOMAIN,
                uint16_t    port   = BLYNK_DEFAULT_PORT)
    {
        Base::begin(auth);

        this->conn.begin(domain, port);
    }

    void config(const char* auth,
                IPAddress   ip,
                uint16_t    port = BLYNK_DEFAULT_PORT)
    {
        Base::begin(auth);
        this->conn.begin(cc3000.IP2U32(ip[0],ip[1],ip[2],ip[3]), port);
    }

    void begin( const char* auth,
                const char* ssid,
                const char* pass,
                uint8_t     secmode,
                const char* domain = BLYNK_DEFAULT_DOMAIN,
                uint16_t    port   = BLYNK_DEFAULT_PORT)
    {
        connectWiFi(ssid, pass, secmode);
        config(auth, domain, port);
        while(this->connect() != true) {}
    }

    void begin( const char* auth,
                const char* ssid,
                const char* pass,
                uint8_t     secmode,
                IPAddress   ip,
                uint16_t    port   = BLYNK_DEFAULT_PORT)
    {
        connectWiFi(ssid, pass, secmode);
        config(auth, ip, port);
        while(this->connect() != true) {}
    }
private:
    Adafruit_CC3000& cc3000;
};

#endif
