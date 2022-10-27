/* -*-mode:c++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*- */

#include <limits>
#include <cstring>
#include <sys/types.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <linux/ip.h>
#include <linux/tcp.h>

#include "delay_queue.hh"
#include "timestamp.hh"

using namespace std;

void DelayQueue::read_packet( const string & contents )
{
    string no_tun_contents = contents.substr(4);
 
    struct iphdr *iph = (struct iphdr *)no_tun_contents.c_str();
    struct tcphdr *tcph = (struct tcphdr *)(no_tun_contents.c_str() + sizeof(iphdr));
    struct sockaddr_in dest;
    memset(&dest, 0, sizeof(dest));
    dest.sin_addr.s_addr = iph->daddr;
    std::unordered_map<string, float>::iterator it =
        ip_delays.find(inet_ntoa(dest.sin_addr));
    float RTT_delay = 0;
    unsigned int src_port = tcph->source;
    if (it != ip_delays.end()) {
        // mapping file times are in milliseconds
        // we add all delay on uplink because dest ip
        // on downlink is client ip (note that this shouldn't matter)
        if(appeared_ports.find(src_port) == appeared_ports.end()){
            appeared_ports.insert(src_port);
            if (rand() % 100 < 50){
                prelonged_ports.insert(src_port);
            }
        }
        if(prelonged_ports.find(src_port) == appeared_ports.end()){
            RTT_delay = (it->second);
            cout <<  tcph->source << ":" << RTT_delay << endl;
        }
        else{
            RTT_delay = (it->second) + 100;
            cout <<  tcph->source << ":" << RTT_delay << endl;
        }
        
    }
    //cout << inet_ntoa(dest.sin_addr) << ":" << tcph->source << ":" << tcph->dest <<":" << RTT_delay << ":"<< rand() % 100<< endl;
    packet_queue_.emplace( timestamp() + delay_ms_ + int(RTT_delay), contents );
}

void DelayQueue::write_packets( FileDescriptor & fd )
{
    while ( (!packet_queue_.empty())
            && (packet_queue_.top().first <= timestamp()) ) {
        fd.write( packet_queue_.top().second );
        packet_queue_.pop();
    }
}

unsigned int DelayQueue::wait_time( void ) const
{
    if ( packet_queue_.empty() ) {
        return numeric_limits<uint16_t>::max();
    }

    const auto now = timestamp();

    if ( packet_queue_.top().first <= now ) {
        return 0;
    } else {
        return packet_queue_.top().first - now;
    }
}
