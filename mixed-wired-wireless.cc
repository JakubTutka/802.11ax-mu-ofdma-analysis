#include "ns3/wifi-module.h"
#include "ns3/internet-module.h"
#include "ns3/config.h"
#include "ns3/string.h"
#include "ns3/command-line.h"

using namespace ns3;
NS_LOG_COMPONENT_DEFINE ("ExamplesCode");

int
main (int argc, char *argv[])
{
  /*


  MIXED-WIRED-WIRELESS.CC

  
  */
  uint32_t backboneNodes = 10;
  uint32_t stopTime = 20;

  // Payload size and data rate of application OnOff
  Config::SetDefault ("ns3::OnOffApplication::PacketSize", StringValue ("1472"));
  Config::SetDefault ("ns3::OnOffApplication::DataRate", StringValue ("100kb/s"));

  // Parsing command line args
  CommandLine cmd (__FILE__);
  cmd.AddValue ("backboneNodes", "number of backbone nodes", backboneNodes);
  cmd.AddValue ("stopTime", "simulation stop time (seconds)", stopTime);
  cmd.Parse (argc, argv);

  if (0)
    {
      // we can exit if error by:
      exit (1);
    }

  // how to create nodes?
  NodeContainer backbone;
  backbone.Create (backboneNodes);

  // create all wifi things

  WifiHelper wifi;
  WifiMacHelper mac;
  mac.SetType ("ns3::StaWifiMac");

  YansWifiChannelHelper wifiChannel = YansWifiChannelHelper::Default ();

  YansWifiPhyHelper wifiPhy;
  wifiPhy.SetChannel (wifiChannel.Create ());
  wifiPhy.SetPcapDataLinkType (WifiPhyHelper::DLT_IEEE802_11_RADIO);

  // IPv4 - Addresses
  Ipv4AddressHelper ipAddrs;
  ipAddrs.SetBase ("192.168.0.0", "255.255.255.0");
}