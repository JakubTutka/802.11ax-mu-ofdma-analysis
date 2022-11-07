#include "ns3/internet-module.h"
#include <iostream>

using namespace ns3;

int
main ()
{
  Ptr<Packet> pkt = Create<Packet> ();
  uint32_t uid = pkt->GetUid ();

  Ptr<Packet> pkt2 = Create<Packet>(5); // 5 bytes of size

  std::cout << pkt2->GetSize() << std::endl;
  std::cout << uid << std::endl;

}