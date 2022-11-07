/*
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation;
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 *
 * Author: Jakub Tutka <jtutka@student.agh.edu.pl>
 */

#include <iostream>
#include <string>
#include "ns3/command-line.h"
#include "ns3/core-module.h"
#include "ns3/wifi-acknowledgment.h"
#include "ns3/node-container.h"
#include "ns3/wifi-mac-helper.h"
#include "ns3/wifi-helper.h"
#include "ns3/yans-wifi-helper.h"
#include "ns3/ssid.h"
#include "ns3/mobility-module.h"

// TODO; DESCRIPTION OF SIMULATION

using namespace ns3;

NS_LOG_COMPONENT_DEFINE ("SCENARIO #1");

int
main (int argc, char *argv[])
{
  //   uint32_t payloadSize = 1472;
  //   double simulationTime = 20.0;
  uint32_t nSta = 4;
  uint32_t nAp = 1;
  std::string dlAckSeqType = "ACK-SU";
  int mcs = 1;
  int gi = 800; // ns

  CommandLine cmd (__FILE__);
  cmd.AddValue ("dlAckType", "Ack sequence type for DL OFDMA (ACK-SU, MU-BAR, AGGR-MU-BAR)",
                dlAckSeqType);
  cmd.AddValue ("mcs", "MCS (0-11) for simulation", mcs);
  cmd.AddValue ("gi", "Guard Interval value (800ns, 1600ns or 3200ns)", gi);

  if (dlAckSeqType == "ACK-SU")
    {
      Config::SetDefault ("ns3::WifiDefaultAckManager::DlMuAckSequenceType",
                          EnumValue (WifiAcknowledgment::DL_MU_BAR_BA_SEQUENCE));
    }
  else if (dlAckSeqType == "MU-BAR")
    {
      Config::SetDefault ("ns3::WifiDefaultAckManager::DlMuAckSequenceType",
                          EnumValue (WifiAcknowledgment::DL_MU_TF_MU_BAR));
    }
  else if (dlAckSeqType == "AGGR-MU-BAR")
    {
      Config::SetDefault ("ns3::WifiDefaultAckManager::DlMuAckSequenceType",
                          EnumValue (WifiAcknowledgment::DL_MU_AGGREGATE_TF));
    }

  NodeContainer wifiStaNodes;
  NodeContainer wifiApNode;

  NetDeviceContainer staDevices;
  NetDeviceContainer apDevice;

  wifiStaNodes.Create (nSta);
  wifiApNode.Create (nAp);

  WifiHelper wifi;
  WifiMacHelper mac;
  YansWifiPhyHelper phy;
  YansWifiChannelHelper channel = YansWifiChannelHelper::Default ();

  std::ostringstream oss;
  oss << "HeMcs" << mcs;

  wifi.SetStandard (WIFI_STANDARD_80211ax);
  wifi.SetRemoteStationManager ("ns3::ConstantRateWifiManager", "DataMode",
                                StringValue (oss.str ()), "ControlMode", StringValue (oss.str ()));
  phy.SetChannel (channel.Create ());
  phy.Set ("ChannelSettings", StringValue ("{0, 20, BAND_5GHZ, 0}"));
  phy.SetPcapDataLinkType (WifiPhyHelper::DLT_IEEE802_11_RADIO);

  Ssid ssid = Ssid ("scenario-1");

  mac.SetType ("ns3::StaWifiMac", "Ssid", SsidValue (ssid));
  staDevices = wifi.Install (phy, mac, wifiStaNodes);

  mac.SetType ("ns3::ApWifiMac", "EnableBeaconJitter", BooleanValue (false), "Ssid",
               SsidValue (ssid));
  apDevice = wifi.Install (phy, mac, wifiApNode);

  Config::Set ("/NodeList/*/DeviceList/*/$ns3::WifiNetDevice/HeConfiguration/GuardInterval",
               TimeValue (NanoSeconds (gi)));

  MobilityHelper mobility;
  Ptr<ListPositionAllocator> positionAlloc = CreateObject<ListPositionAllocator> ();

  positionAlloc->Add (Vector (0.0, 0.0, 0.0)); // STA1
  positionAlloc->Add (Vector (0.0, 6.0, 0.0)); // STA2
  positionAlloc->Add (Vector (6.0, 0.0, 0.0)); // STA3
  positionAlloc->Add (Vector (6.0, 6.0, 0.0)); // STA4
  positionAlloc->Add (Vector (3.0, 3.0, 0.0)); // AP

  return 0;
}