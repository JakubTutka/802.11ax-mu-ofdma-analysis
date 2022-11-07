#include "ns3/node-container.h"
#include "ns3/yans-wifi-helper.h"
#include "ns3/wifi-helper.h"
#include "ns3/string.h"
#include "ns3/boolean.h"
#include "ns3/ssid.h"
#include "ns3/config.h"
#include "ns3/uinteger.h"
#include "ns3/enum.h"
#include "ns3/wifi-acknowledgment.h"
#include "ns3/internet-module.h"
#include "ns3/applications-module.h"
#include "ns3/wifi-module.h"

using namespace ns3;

int
main ()
{
  NodeContainer wifiStaNodes;
  wifiStaNodes.Create (4);

  NodeContainer wifiApNode;
  wifiApNode.Create (1);

  // Create a channel helper and phy helper, and then create the channel
  YansWifiChannelHelper channel = YansWifiChannelHelper::Default ();
  YansWifiPhyHelper phy;
  phy.SetChannel (channel.Create ());

  // Create a WifiMacHelper, wich is reused across STA and AP config.
  WifiMacHelper mac;

  // Create a WifiHelper, which will use the above helpers to create
  // and install Wifi devices. Configure a Wifi standard to use, which
  // will align various parameters in the Phy and Mac to standard defaults.
  WifiHelper wifi;
  wifi.SetStandard (WIFI_STANDARD_80211ax);

  // Declare NetDeviceContainers to hold the container returned by the helper
  NetDeviceContainer wifiStaDevices;
  NetDeviceContainer wifiApDevice;

  // Perform the instlattation
  mac.SetType ("ns3::StaWifiMac");
  wifiStaDevices = wifi.Install (phy, mac, wifiStaNodes);

  mac.SetType ("ns3::ApWifiMac");
  wifiApDevice = wifi.Install (phy, mac, wifiApNode);

  /*
    YANS WIFI CHANNEL HELPER

    With default PropagationLoss and PropagationDelay model.
    Propragation delay: ns3::ConstantSpeedPropagationDelayModel
    Propagation loss: ns3::LogDistancePropagationLossModel
    Reference loss changed if 2.4 GHz

    YansWifiChannelHelper::AddPropagationLoss
    YansWifiChannelHelper::SetPropagationDelay
  */

  YansWifiChannelHelper wifiChannelHelper = YansWifiChannelHelper::Default ();
  Ptr<Channel> wifiChannel = wifiChannelHelper.Create ();

  /*
    YANS WIFI PHY HELPER
    ns3::WifiPhy -> ns3::YansWifiChannel

    create instance of YansWifiPhy, add possibly ErrorRateModel and MobilityModel
    Default: ns3::TableBasedErrorRateModel, change it by YansWifiPhyHelper::SetErrorRateModel.
    Enable pcap: YansWifiPhyHelper::SetPcapDataLinkType (enum SupportedPcapDataLinkTypes dlt)

    MIMO:
    wifiPhyHelper.Set ("Antennas", UintegerValue (2));
    wifiPhyHelper.Set ("MaxSupportedTxSpatialStreams", UintegerValue (2));
    wifiPhyHelper.Set ("MaxSupportedRxSpatialStreams", UintegerValue (2));
  */

  YansWifiPhyHelper wifiPhyHelper;
  wifiPhyHelper.SetChannel (wifiChannel);

  /*
  Setting Frequency, channel width, channel number
 */

  WifiHelper wifi;
  wifi.SetStandard (WIFI_STANDARD_80211ax);
  wifi.SetRemoteStationManager ("ns3::ConstantRateWifiManager", "DataMode", StringValue ("HeMcs8"),
                                "ControlMode", StringValue ("HeMcs3"));

  // Install PHY and MAC
  Ssid ssid = Ssid ("ns3-wifi");
  WifiMacHelper mac;

  mac.SetType ("ns3::StaWifiMac", "Ssid", SsidValue (ssid), "ActiveProbing", BooleanValue (false));

  NetDeviceContainer staDevice;
  staDevice = wifi.Install (phy, mac, wifiStaNodes);

  mac.SetType ("ns3::ApWifiMac", "Ssid", SsidValue (ssid));

  NetDeviceContainer apDevice;
  apDevice = wifi.Install (phy, mac, wifiApNode);

  /*
   Channel, frequency, channel width, and band configuration

   ns3::WifiPhy -> ChannelSettings (channel number, width, freq band)
   ChannelSettings attribiute can be set in a number of ways.
  */

  StringValue value ("{38, 40, BAND_5GHZ, 0}"); // channel 38, 5GHz, 40 MHz - how to set it?
  Config::SetDefault ("ns3::WifiPhy::ChannelSettings", value);

  // or

  wifiPhyHelper.Set ("ChannelSettings", value);

  // or

  Config::Set ("/NodeList/0/DeviceList/*/$ns3::WifiNetDevice/Phy/$ns3::WifiPhy/ChannelSettings",
               value);

  /*
    WiFiHelper::SetStandard()
    check channel settings, values:
    WIFI_STANDARD_80211a,
    WIFI_STANDARD_80211b,
    WIFI_STANDARD_80211g,
    WIFI_STANDARD_80211p,
    WIFI_STANDARD_80211n,
    WIFI_STANDARD_80211ac,
    WIFI_STANDARD_80211ax

    default: ax

    Defalut settings for the operating channel
    if the band unspecifed: 5 Ghz in our case
    if channel width and number unspecified: 80 MHz
    if channel width unspecified, but channel number valid: width of the valid channel
    if channel number unspecified: first on the list set
  */

  WifiHelper wifi;
  wifi.SetStandard (WIFI_STANDARD_80211ac);
  YansWifiPhyHelper phyHelper;
  phyHelper.Set ("ChannelSettings", StringValue ("{58, 0, BAND_5GHZ, 0}"));
  // channel width unspecified
  // -> it is set to 80 MHz (width of channel 58)

  WifiHelper wifi;
  wifi.SetStandard (WIFI_STANDARD_80211n);
  YansWifiPhyHelper phyHelper;
  phyHelper.Set ("ChannelSettings", StringValue ("{0, 40, BAND_5GHZ, 0}"));
  // channel number unspecified
  // -> it is set to channel 38 (first 40 MHz channel in the 5GHz band)

  WifiHelper wifi;
  wifi.SetStandard (WIFI_STANDARD_80211ax);
  YansWifiPhyHelper phyHelper;
  phyHelper.Set ("ChannelSettings", StringValue ("{0, 0, BAND_2_4GHZ, 0}"));
  // both channel number and width unspecified
  // -> width set to 20 MHz (default width for 802.11ax in the 2.4 GHZ band)
  // -> channel number set to 1 (first 20 MHz channel in the 2.4 GHz band)

  WifiHelper wifi;
  wifi.SetStandard (WIFI_STANDARD_80211a);
  YansWifiPhyHelper phyHelper;
  phyHelper.Set ("ChannelSettings", StringValue ("{0, 0, BAND_UNSPECIFIED, 0}"));
  // band, channel number and width unspecified
  // -> band is set to WIFI_PHY_BAND_5GHZ (default band for 802.11a)
  // -> width set to 20 MHz (default width for 802.11a in the 5 GHZ band)
  // -> channel number set to 36 (first 20 MHz channel in the 5 GHz band)

  /*
    WifiPhy::Frequency [MHz]
    WifiPhy::ChannelWidth [MHz]
    WifiPhy::ChannelNumber [567 str model]

    The wifi standard can be configured only once, i.e., it is not possible to change standard during a simulation.
    It is instead possible to change the channel settings at any time.
  */

  /*
    WifiMacHelper
    takes care of low and high model.
    config things like: type of mac, ht/vht/he support
  */

  // non-Qos, non-Ht/non-Vht/non-He MAC, non-AP STA
  WifiMacHelper wifiMacHelper;
  Ssid ssid = Ssid ("ns-3-ssid");
  wifiMacHelper.SetType ("ns3::StaWifiMac", "Ssid", SsidValue (ssid), "ActiveProbing",
                         BooleanValue (false));

  // AP with QoS-enabled
  WifiMacHelper wifiMacHelper;
  wifiMacHelper.SetType ("ns3::ApWifiMac", "Ssid", SsidValue (ssid), "QosSupported",
                         BooleanValue (true), "BeaconGeneration", BooleanValue (true),
                         "BeaconInterval", TimeValue (Seconds (2.5)));

  /*
   With QoS-enabled 4 diffrent ACs: AC_VO, AC_VI, AC_BE, AC_BK
   And when selecting 802.11ax as the desired wifi standard, 802.11e/WMM-style QoS, 802.11n-style High Throughput
   (HT), 802.11ac-style Very High Throughput (VHT) and 802.11ax-style High Efficiency (HE) support gets enabled.

   For MAC with QoS, we can set block ack treshold (number of packet for which block ack mechanism should be used)
   and block ack inactivity timeout
  */

  //  For example the following user code configures a MAC that will be a non-AP STA with QoS enabled and a block ack
  // threshold for AC_BE set to 2 packets, in an infrastructure network where the AP has SSID ns-3-ssid :
  // WifiMacHelper wifiMacHelper;

  Ssid ssid = Ssid ("ns-3-ssid");
  wifiMacHelper.SetType ("ns3::StaWifiMac", "Ssid", SsidValue (ssid), "QosSupported",
                         BooleanValue (true), "BE_BlockAckThreshold", UintegerValue (2),
                         "ActiveProbing", BooleanValue (false));

  /*
   In our case of 802.11ax we can set: MSDU aggregation for a particular AC (A-MSDU feature), MPDU aggregation for a particular AC (A-MPDU feature)
   By default, MSDU aggregation feature is disabled for all ACs and MPDU aggregation is enabled for AC_VI and
   AC_BE, with a maximum aggregation size of 65535 bytes
  */

  //  For example the following user code configures a MAC that will be a non-AP STA with HT and QoS enabled, MPDU
  // aggregation enabled for AC_VO with a maximum aggregation size of 65535 bytes, and MSDU aggregation enabled
  // for AC_BE with a maximum aggregation size of 7935 bytes, in an infrastructure network where the AP has SSID
  // ns-3-ssid :
  WifiHelper wifi;
  wifi.SetStandard (WIFI_STANDARD_80211n);
  WifiMacHelper wifiMacHelper;
  Ssid ssid = Ssid ("ns-3-ssid");
  wifiMacHelper.SetType ("ns3::StaWifiMac", "Ssid", SsidValue (ssid), "VO_MaxAmpduSize",
                         UintegerValue (65535), "BE_MaxAmsduSize", UintegerValue (7935),
                         "ActiveProbing", BooleanValue (false));

  /*
  
    IMPORTANT!!!
    Multi-user frames, DL/UL OFDMA
  */

  WifiMacHelper wifiMacHelper;
  wifiMacHelper.SetMultiUserScheduler ("ns3::RrMultiUserScheduler", "EnableUlOfdma",
                                       BooleanValue (true), "EnableBsrp", BooleanValue (false));

  // ACK MANAGER FOR 802.11ax, three available
  Config::SetDefault ("ns3::WifiDefaultAckManager::DlMuAckSequenceType",
                      EnumValue (WifiAcknowledgment::DL_MU_AGGREGATE_TF));

  // How to select Access Category AC?
  /*
   ToS field in Ipv4
  */

  // InetSocketAddress destAddress (ipv4Address, udpPort);
  // destAddress.SetTos (tos);
  // OnOffHelper onoff ("ns3::UdpSocketFactory", destAddress);

  /*
   WifiHelper
   - settin standard (default 802.11a) WifiHelper::SetStandard
   - setting RemoteStationManager (default ns3::ArfWifiManager) WifiHelper::SetRemoteStationManager
   - setting timing slot,sifs,pifs below
   - set the attributes of the default ack policy selector
    (ConstantWifiAckPolicySelector ) or to select a different (user provided) ack policy selector, for each of
    the available Access Categories. As an example, the following code can be used to set the BaThreshold attribute of
    the default ack policy selector associated with BE AC to 0.5:

    WifiHelper wifi;
    wifi.SetAckPolicySelectorForAc (AC_BE, "ns3::ConstantWifiAckPolicySelector","BaThreshold", DoubleValue (0.5));

    - OBSS PD spatial reuse 802.11ax
    WifiHelper wifi;
    wifi.SetObssPdAlgorithm ("ns3::ConstantObssPdAlgorithm",
    "ObssPdLevel", DoubleValue (-72.0));
  */

  // NetDeviceContainer wifiContainer = WifiHelper::Install(wifiPhyHelper, wifiMacHelper, c)
  //Once install is done, we overwrite the standard timing values
  // Config::Set ("/NodeList/*/DeviceList/*/$ns3::WifiNetDevice/Phy/Slot",
  //              TimeValue,(MicroSeconds (slot)));
  // Config::Set ("/NodeList/*/DeviceList/*/$ns3::WifiNetDevice/Phy/Sifs",
  //              TimeValue,(MicroSeconds (sifs)));
  // Config::Set ("/NodeList/*/DeviceList/*/$ns3::WifiNetDevice/Phy/Pifs",
  //              TimeValue,(MicroSeconds (pifs)));

  // HT CONFIG

  Ptr<NetDevice> nd = wifiStaNodes.Get (0);
  Ptr<WifiNetDevice> wnd = nd->GetObject<WifiNetDevice> ();
  Ptr<HtConfiguration> htConfiguration = wnd->GetHtConfiguration ();
  htConfiguration->SetShortGuardIntervalSupported (true);

  // for all stations..
  Config::Set (
      "/NodeList//DeviceList/ /$ns3::WifiNetDevice/HtConfiguration/ShortGuardIntervalSupported",
      BooleanValue (true));

  // VHT CONFIG - nothing
  // HE CONFIG
  /*
    - various GI (3200, 1600, 800 ns)
  */

  Ptr<NetDevice> nd = wifiStaNodes.Get (0);
  Ptr<WifiNetDevice> wnd = nd->GetObject<WifiNetDevice> ();
  Ptr<HeConfiguration> heConfiguration = wnd->GetHeConfiguration ();
  heConfiguration->SetGuardInterval (NanoSeconds (1600));

  //   802.11ax allows extended compressed Block ACKs containing a 256-bits bitmap, making possible transmissions of
  // A-MPDUs containing up to 256 MPDUs, depending on the negotiated buffer size. In order to configure the buffer size
  // of an 802.11ax device, the following line of code could be used:
  heConfiguration->SetMpduBufferSize (256);

  //   When using UL MU transmissions, solicited TB PPDUs can arrive at the AP with a different delay, due to
  // the different propagation delay from the various stations. In real systems, late TB PPDUs cause a variable
  // amount of interference depending on the receiver’s sensitivity. This phenomenon can be modeled through the
  // ns3::HeConfiguration::MaxTbPpduDelay attribute, which defines the maximum delay with which a TB PPDU
  // can arrive with respect to the first TB PPDU in order to be decoded properly. TB PPDUs arriving after more than
  // MaxTbPpduDelay since the first TB PPDU are discarded and considered as interference.

  
  return 0;
}