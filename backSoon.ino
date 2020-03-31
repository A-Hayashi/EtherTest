// Present a "Will be back soon web page", as stand-in webserver.
// 2011-01-30 <jc@wippler.nl>
//
// License: GPLv2

#include "EtherCard.h"

#define STATIC 0  // set to 1 to disable DHCP (adjust myip/gwip values below)

#define PAYLOAD_LEN (1024)

#if STATIC
// ethernet interface ip address
static byte myip[] = { 192, 168, 11, 3 };
// gateway ip address
static byte gwip[] = { 192, 168, 11, 1 };
#endif

// ethernet mac address - must be unique on your network
static byte mymac[] = { 0x74, 0x69, 0x69, 0x2D, 0x30, 0x31 };

byte Ethernet::buffer[PAYLOAD_LEN + 100]; // tcp/ip send and receive buffer

uint8_t ipDestinationAddress[IP_LEN];

void setup() {
  Serial.begin(57600);
  Serial.println("\n[backSoon]");

  // Change 'SS' to your Slave Select pin, if you arn't using the default pin
  if (ether.begin(sizeof Ethernet::buffer, mymac, 8) == 0)
    Serial.println( "Failed to access Ethernet controller");
#if STATIC
  ether.staticSetup(myip, gwip);
#else
  if (!ether.dhcpSetup())
    Serial.println("DHCP failed");
#endif

  ether.printIp("IP:  ", ether.myip);
  ether.printIp("GW:  ", ether.gwip);
  ether.printIp("DNS: ", ether.dnsip);

  ether.parseIp(ipDestinationAddress, "192.168.11.2");
}

char payload[1];
uint16_t nSourcePort = 1234;
uint16_t nDestinationPort = 5678;

#define SEND_DATA_SIZE (10000)
void loop() {
  uint16_t time = millis();
  for (uint16_t i = 0; i < SEND_DATA_SIZE; i++) {
    ether.packetLoop(ether.packetReceive());
    ether.sendUdp(payload, PAYLOAD_LEN, nSourcePort, ipDestinationAddress, nDestinationPort);
    Serial.println(i);
  }
  uint16_t elapsed = millis() - time;
  Serial.print("send ");
  Serial.print(SEND_DATA_SIZE);
  Serial.println(" KB");

  Serial.print(elapsed);
  Serial.println(" ms");

  float bps = (float)SEND_DATA_SIZE * 8 / (elapsed / SEND_DATA_SIZE);
  Serial.print(bps);
  Serial.println(" kbps");
  while (1);
}
