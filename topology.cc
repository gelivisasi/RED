


#include "ns3/core-module.h"
#include "ns3/network-module.h"
#include "ns3/internet-module.h"
#include "ns3/point-to-point-module.h" 
#include "ns3/netanim-module.h"
#include "ns3/applications-module.h" 
#include "ns3/animation-interface.h" 
#include "ns3/point-to-point-layout-module.h" 
#include "ns3/ipv4-static-routing-helper.h"
#include "ns3/ipv4-list-routing-helper.h" 
#include "ns3/ipv4-global-routing-helper.h" 
#include "ns3/flow-monitor.h"
#include "ns3/flow-monitor-helper.h"
#include "ns3/flow-monitor-module.h" 
#include <iostream>
#include <fstream> 
#include <vector> 
#include <string>
#include <cstdlib>


using namespace ns3;
//using namespace std;

/*
 * 
 */

class MyApp : public Application
{
public:
  MyApp ();
  virtual ~MyApp ();

  void Setup (Ptr<Socket> socket, Address address, uint32_t packetSize, uint32_t nPackets, DataRate dataRate);

private:
  virtual void StartApplication (void);
  virtual void StopApplication (void);

  void ScheduleTx (void);
  void SendPacket (void);

  Ptr<Socket>     m_socket;
  Address         m_peer;
  uint32_t        m_packetSize;
  uint32_t        m_nPackets;
  DataRate        m_dataRate;
  EventId         m_sendEvent;
  bool            m_running;
  uint32_t        m_packetsSent;
};

MyApp::MyApp ()
  : m_socket (0),
    m_peer (),
    m_packetSize (0),
    m_nPackets (0),
    m_dataRate (0),
    m_sendEvent (),
    m_running (false),
    m_packetsSent (0)
{
}

MyApp::~MyApp ()
{
  m_socket = 0;
}

void
MyApp::Setup (Ptr<Socket> socket, Address address, uint32_t packetSize, uint32_t nPackets, DataRate dataRate)
{
  m_socket = socket;
  m_peer = address;
  m_packetSize = packetSize;
  m_nPackets = nPackets;
  m_dataRate = dataRate;
}

void
MyApp::StartApplication (void)
{
  m_running = true;
  m_packetsSent = 0;
  m_socket->Bind ();
  m_socket->Connect (m_peer);
  SendPacket ();
}

void
MyApp::StopApplication (void)
{
  m_running = false;

  if (m_sendEvent.IsRunning ())
    {
      Simulator::Cancel (m_sendEvent);
    }

  if (m_socket)
    {
      m_socket->Close ();
    }
}

void
MyApp::SendPacket (void)
{
  Ptr<Packet> packet = Create<Packet> (m_packetSize);
  m_socket->Send (packet);

  if (++m_packetsSent < m_nPackets)
    {
      ScheduleTx ();
    }
}

void
MyApp::ScheduleTx (void)
{
  if (m_running)
    {
      Time tNext (Seconds (m_packetSize * 8 / static_cast<double> (m_dataRate.GetBitRate ())));
      m_sendEvent = Simulator::Schedule (tNext, &MyApp::SendPacket, this);
    }
}

static void
CwndChange (Ptr<OutputStreamWrapper> stream, uint32_t oldCwnd, uint32_t newCwnd)
{
  //NS_LOG_UNCOND (Simulator::Now ().GetSeconds () << "\t" << newCwnd);
  *stream->GetStream () << Simulator::Now ().GetSeconds () << "\t" << oldCwnd << "\t" << newCwnd << std::endl;
}

int main(int argc, char* argv[]) {

/* default parameters start */


  std::string tcp_variant = "TcpNewReno";
  std::string bandwidth = "5Mbps";
 std::string delay = "2ms";
  double error_rate = 0.000001;
 //int queuesize = 10; //packets
  int simulation_time = 20; //seconds
  bool tracing = true;



/* default parameters end */
	


// Select TCP variant
  /*if (tcp_variant.compare("TcpTahoe") == 0)
    Config::SetDefault("ns3::TcpL4Protocol::SocketType", TypeIdValue (TcpTahoe::GetTypeId()));
  else if (tcp_variant.compare("TcpReno") == 0)
    Config::SetDefault("ns3::TcpL4Protocol::SocketType", TypeIdValue (TcpReno::GetTypeId()));
  else if (tcp_variant.compare("TcpNewReno") == 0)*/
    Config::SetDefault("ns3::TcpL4Protocol::SocketType", TypeIdValue (TcpNewReno::GetTypeId()));
  /*else
    {
      fprintf (stderr, "Invalid TCP version\n");
      exit (1);
    }
	

  Config::SetDefault ("ns3::DropTailQueue::MaxPackets", UintegerValue(uint32_t(queuesize)));
*/
    NS_LOG_COMPONENT_DEFINE("net_layer");
    uint16_t num_Nodes = 12;
	
 /** configuration parameter for UDP flow only **/
   	
	uint32_t PacketSize = 1460; // bytes 
	std::string UDPRate ("5Mbps"); 


/** setting up default configuration **/

    CommandLine cmd;
   cmd.AddValue("PacketSize", "size of application packet sent", PacketSize);
    cmd.AddValue("DataRate", "rate of pacekts sent", UDPRate);
    cmd.AddValue("tracing", "turn on ascii and pcap tracing", tracing);
    cmd.Parse(argc, argv);


    Config::SetDefault("ns3::OnOffApplication::PacketSize", UintegerValue(PacketSize));
   Config::SetDefault("ns3::OnOffApplication::DataRate", StringValue(UDPRate));
    Config::SetDefault("ns3::Ipv4GlobalRouting::RespondToInterfaceEvents", BooleanValue(true));

    /* configuration for NETAnim Animator **/
    ns3::PacketMetadata::Enable();
    std::string animFile = "topology12.xml";

    

	
/** Network Topology Creation **/    
    NodeContainer nodes;
    NodeContainer link[num_Nodes-1];
    Ipv4AddressHelper ipv4;
    NetDeviceContainer d_link[num_Nodes-1];
    Ipv4InterfaceContainer iic[num_Nodes-1];
    int i, num_nodes_on_one_side;
    Ipv4Address address[num_Nodes-1];
    nodes.Create(num_Nodes);
    num_nodes_on_one_side = num_Nodes/2;
    address[0] = "10.1.1.0";
    address[1] = "10.1.2.0";
    address[2] = "10.1.3.0";
    address[3] = "10.1.4.0";
    address[4] = "10.1.5.0";
    address[5] = "10.1.6.0";
    address[6] = "10.1.7.0";
    address[7] = "10.1.8.0";
    address[8] = "10.1.9.0";
    address[9] = "10.1.10.0";
    address[10] = "10.1.11.0";
    for(i=0; i < num_nodes_on_one_side-1 ; i++)
    {
    	link[i] = NodeContainer(nodes.Get(i), nodes.Get(num_nodes_on_one_side-1));
    }
   
    link[i] = NodeContainer(nodes.Get(num_nodes_on_one_side-1), nodes.Get(num_Nodes-1));
    
    for(i = num_nodes_on_one_side; i < num_Nodes-1 ; i++)
    {
    	link[i] = NodeContainer(nodes.Get(i), nodes.Get(num_Nodes-1));

    }


/** Point to Point Link Helper which sets data rate and delay value for Link **/
    PointToPointHelper p2p;
    p2p.SetDeviceAttribute ("DataRate", StringValue (bandwidth));
    p2p.SetChannelAttribute ("Delay", StringValue (delay));

 /** Setting up each link between two nodes **/
    
    for(i=0; i < num_Nodes-1 ; i++)
    {
	d_link[i] = p2p.Install(link[i]);
    }
	    	



  /*  Error Model at Reciever end  */
 Ptr<RateErrorModel> em = CreateObject<RateErrorModel> ();
 em->SetAttribute ("ErrorRate", DoubleValue (error_rate));
 d_link[6].Get (1)->SetAttribute ("ReceiveErrorModel", PointerValue (em));
 d_link[7].Get (1)->SetAttribute ("ReceiveErrorModel", PointerValue (em)); 


    NS_LOG_INFO("Flow Monitor");
    Ipv4StaticRoutingHelper staticRouting;
    Ipv4GlobalRoutingHelper globalRouting;
    Ipv4ListRoutingHelper list;
    list.Add(staticRouting, 0);
    list.Add(globalRouting, 10);
    // Install network stacks on the nodes 
    InternetStackHelper internet;
    internet.SetRoutingHelper(list);
    internet.Install(nodes);

/** Assigning Network Base Address to Each node **/

    for(i=0; i < num_Nodes-1 ; i++)
    {
	ipv4.SetBase(address[i], "255.255.255.0");
	//address  = htonl(ntohl(address) + 1);
 	iic[i] = ipv4.Assign(d_link[i]);
    	iic[i].SetMetric(0, 2);
	iic[i].SetMetric(1, 2);
		
    }
	
     Ipv4GlobalRoutingHelper::PopulateRoutingTables();





/** Flow number 1 starts here**/


//Reciver setup
  uint16_t sinkPort = 8080;
  Address sinkAddress (InetSocketAddress (iic[num_nodes_on_one_side-1].GetAddress (1), sinkPort));
  PacketSinkHelper packetSinkHelper ("ns3::TcpSocketFactory", InetSocketAddress (Ipv4Address::GetAny (), sinkPort));
  ApplicationContainer sinkApps = packetSinkHelper.Install (nodes.Get(11));
  sinkApps.Start (Seconds (0.));
  sinkApps.Stop (Seconds (simulation_time));

//Sender Setup 
  Ptr<Socket> ns3TcpSocket = Socket::CreateSocket (nodes.Get (3), TcpSocketFactory::GetTypeId ());

  Ptr<MyApp> app = CreateObject<MyApp> ();

  //configuration parameter for TCP Flow 
  app->Setup (ns3TcpSocket, sinkAddress, 1460, 1000000, DataRate("5Mbps"));
  nodes.Get (0)->AddApplication (app);
  app->SetStartTime (Seconds (1.));
  app->SetStopTime (Seconds (simulation_time));

  //trace cwnd
  AsciiTraceHelper asciiTraceHelper;
  Ptr<OutputStreamWrapper> stream = asciiTraceHelper.CreateFileStream ("topology.cwnd");
  ns3TcpSocket->TraceConnectWithoutContext ("CongestionWindow", MakeBoundCallback (&CwndChange, stream));


/** flow number 1 ends here **/


//flow number 2 starts here
//Reciver setup 
/*uint16_t sinkPortf2 = 8081;
  Address sinkAddressf2 (InetSocketAddress (iHD.GetAddress (1), sinkPortf2));
  PacketSinkHelper packetSinkHelperf2 ("ns3::TcpSocketFactory", InetSocketAddress (Ipv4Address::GetAny (), sinkPortf2));
  ApplicationContainer sinkAppsf2 = packetSinkHelperf2.Install (nodes.Get(2));
  sinkAppsf2.Start (Seconds (0.));
  sinkAppsf2.Stop (Seconds (simulation_time));



//Sender Setup 
  Ptr<Socket> ns3TcpSocketf2 = Socket::CreateSocket (nodes.Get (5), TcpSocketFactory::GetTypeId ());

  Ptr<MyApp> appf2 = CreateObject<MyApp> ();
  appf2->Setup ( ns3TcpSocketf2,sinkAddressf2,1460, 1000000, DataRate("5Mbps"));
  nodes.Get (0)->AddApplication (appf2);
  appf2->SetStartTime (Seconds (1.));
  appf2->SetStopTime (Seconds (simulation_time));

  //trace cwnd
 // AsciiTraceHelper asciiTraceHelper;
  Ptr<OutputStreamWrapper> streamf2 = asciiTraceHelper.CreateFileStream ("tcp-netf2.cwnd");
  ns3TcpSocketf2->TraceConnectWithoutContext ("CongestionWindow", MakeBoundCallback (&CwndChange,  streamf2 ));
*/
/** flow number 2 ends here **/





//Flow number 3 is UDP flow starts here 
/*  uint16_t UDPport = 9;
   	 
   PacketSinkHelper UDPsink("ns3::UdpSocketFactory", InetSocketAddress(Ipv4Address::GetAny(), UDPport));
    ApplicationContainer App;
    NodeContainer SourceNode = NodeContainer(nodes.Get(3));
    NodeContainer SinkNode = NodeContainer(nodes.Get(6));

    App = UDPsink.Install(SinkNode);
    App.Start(Seconds(0.0));
    App.Stop(Seconds(10.0));
    Address D_Address(InetSocketAddress(iHD.GetAddress(1), UDPport));


    OnOffHelper UDPsource("ns3::UdpSocketFactory", D_Address);
	UDPsource.SetAttribute ("OnTime", StringValue ("ns3::ConstantRandomVariable[Constant=1]"));
	UDPsource.SetAttribute ("OffTime", StringValue ("ns3::ConstantRandomVariable[Constant=0]"));    
	App = UDPsource.Install(SourceNode);
    App.Start(Seconds(1.0));
    App.Stop(Seconds (simulation_time)); 
*/
/**Flow number 3 is UDP flow ends here **/

/** NetAnim Animator Setup **/

    AnimationInterface anim(animFile);
    Ptr<Node> n = nodes.Get(0);
    anim.SetConstantPosition(n, 0, 0);
    n = nodes.Get(1);
    anim.SetConstantPosition(n, 0, 2);
    n = nodes.Get(2);
    anim.SetConstantPosition(n, 0, 4);
    n = nodes.Get(3);
    anim.SetConstantPosition(n, 0, 6);
    n = nodes.Get(4);
    anim.SetConstantPosition(n, 0, 8);
    n = nodes.Get(5);
    anim.SetConstantPosition(n, 4, 4);
    n = nodes.Get(6);
    anim.SetConstantPosition(n, 10, 0);
    n = nodes.Get(7);
    anim.SetConstantPosition(n, 10, 2);
    n = nodes.Get(8);
    anim.SetConstantPosition(n, 10, 4);
    n = nodes.Get(9);
    anim.SetConstantPosition(n, 10, 6);
    n = nodes.Get(10);
    anim.SetConstantPosition(n, 10, 8);
    n = nodes.Get(11);
    anim.SetConstantPosition(n, 6, 4);


    if (tracing == true) {
        AsciiTraceHelper ascii;
        p2p.EnableAsciiAll(ascii.CreateFileStream("topology.tr"));
        p2p.EnablePcapAll("topology");
    }




Ptr<OutputStreamWrapper> stream1 = Create<OutputStreamWrapper> ("Table22", std::ios::out); 
Ipv4GlobalRoutingHelper helper2;
helper2.PrintRoutingTableAllAt(Seconds(2.0),stream1);





/**flow monitor **/


// 1. Install FlowMonitor on all nodes
FlowMonitorHelper flowmon;
Ptr<FlowMonitor> monitor = flowmon.InstallAll ();

// 2. Run simulation for 60 seconds
Simulator::Stop (Seconds (60));
Simulator::Run ();
// 3. Print per flow statistics
monitor->CheckForLostPackets ();
Ptr<Ipv4FlowClassifier> classifier = DynamicCast<Ipv4FlowClassifier> (flowmon.GetClassifier ());
std::map<FlowId, FlowMonitor::FlowStats> stats = monitor->GetFlowStats ();
for (std::map<FlowId, FlowMonitor::FlowStats>::const_iterator i = stats.begin (); i != stats.end (); ++i)
{
Ipv4FlowClassifier::FiveTuple t = classifier->FindFlow (i->first);
std::cout << "Flow " << i->first - 1 << " (" << t.sourceAddress << ":"<<t.sourcePort<< " -> " << t.destinationAddress << ":"<<t.destinationPort << ")\n";std::cout << " Tx Bytes: " << i->second.txBytes << "\n";
std::cout << " Rx Bytes: " << i->second.rxBytes << "\n";
std::cout << " Throughput: " << i->second.rxBytes * 8.0 /(i->second.timeLastRxPacket.GetSeconds()- i->second.timeFirstTxPacket.GetSeconds()) / 1024 / 1024 << " Mbps \n"; 
}

Simulator::Destroy ();
return 0;
}

