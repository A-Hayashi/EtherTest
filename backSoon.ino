// Present a "Will be back soon web page", as stand-in webserver.
// 2011-01-30 <jc@wippler.nl>
//
// License: GPLv2

#include "EtherCard/EtherCard.h"
#include "Xcp.h"

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
uint16_t nSourcePort = 1234;
uint16_t nDestinationPort = 5678;

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
  ether.udpServerListenOnPort(&udpSerialPrint, nSourcePort);
}


void speedTest()
{
  #define SEND_DATA_SIZE (1000)
  char payload[1];

  uint16_t startTime = millis();
  for (uint16_t i = 0; i < SEND_DATA_SIZE; i++) {
    ether.packetLoop(ether.packetReceive());
    ether.sendUdp(payload, PAYLOAD_LEN, nSourcePort, ipDestinationAddress, nDestinationPort);

    Serial.print(i);
    Serial.print(":");
    Serial.println(millis());
  }
  uint16_t elapsed = millis() - startTime;
  Serial.print("send ");
  Serial.print(SEND_DATA_SIZE);
  Serial.println(" KB");

  Serial.print(elapsed);
  Serial.println(" ms");

  float bps = (float)SEND_DATA_SIZE * 8 / ((float)elapsed / 1000);
  Serial.print(bps);
  Serial.println(" kbps");

  while (1);
}

void udpSerialPrint(uint16_t dest_port, uint8_t src_ip[IP_LEN], uint16_t src_port, const char *data, uint16_t len){
  Serial.print("dest_port: ");
  Serial.println(dest_port);
  Serial.print("src_port: ");
  Serial.println(src_port);


  Serial.print("src_ip: ");
  ether.printIp(src_ip);
  Serial.println("");

  Serial.print("len: ");
  Serial.println(len);

  Serial.print("data: ");
  Serial.println(data);

  Xcp_SoAdIfRxIndication((uint8*)data, len);
}

Std_ReturnType SoAdIf_Transmit(uint8* data, uint16 len)
{
    ether.sendUdp((const char *)data, len, nSourcePort, ipDestinationAddress, nDestinationPort);

    return E_OK;
}

void loop()
{
    ether.packetLoop(ether.packetReceive());
//  speedTest();
}
