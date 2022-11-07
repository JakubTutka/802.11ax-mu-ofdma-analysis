#include "ns3/mobility-module.h"
#include "ns3/double.h"
#include "ns3/uinteger.h"
#include "ns3/string.h"

using namespace ns3;

int
main ()
{
  // current cartesian position, speed of an obj.
  // course change notifier
  // helper classes, place nodes, setup mobility models

  /*
    DESIGN

    - mobility models, position allocators, helper functions
    
    a) mobility models
    - evolution of postition in cartesian coordinate system
    - GetObject<MobilityModel> on Node

    b) position allocators
    - initial position of object

    c) helper classes
    - MobilityHelper -> {MobilityModel, PositionAllocator} on NodeContainer
  */

  /*
    Coordinate system
    - cartesian only

    Coodinates
    - ns3::Vector
    - also Rectangle, Box, Waypoint

    MobilityModel
    - GetPosition(), GetDistanceFrom()
    - position/velocity attribiutes
    - CourseChange notifi.

    Subclasses of MobilityModel:
    -ConstantPosition/Velocity/Acceleration
    -RandomDirection2D/Walk2D
    - ... more

    PositionAllocators:
    - ListPositionAllocator
    - GridPositionAllocator
    - ...

  */

  MobilityHelper mobility;
  mobility.SetPositionAllocator ("ns3::GridPositionAllocator", "MinX", DoubleValue (0.0), "MinY",
                                 DoubleValue (0.0), "DeltaX", DoubleValue (5.0), "DeltaY",
                                 DoubleValue (10.0), "GridWidth", UintegerValue (3), "LayoutType",
                                 StringValue ("RowFirst"));

  mobility.SetMobilityModel ("ns3::RandomWalk2dMobilityModel", "Bounds",
                             RectangleValue (Rectangle (-50, 50, -50, 50)));
  
  NodeContainer wifiStaNodes;
  wifiStaNodes.Create (10);

  mobility.Install (wifiStaNodes);
  return 0;
}