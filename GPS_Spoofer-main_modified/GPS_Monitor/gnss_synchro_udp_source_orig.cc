#include "gnss_synchro_udp_source.h"
#include "gnss_synchro.pb.h"
#include <sstream>
#include <ncurses.h>
#include <iostream>
#include <string>
#include <fstream>
#include <bits/stdc++.h>
#include <chrono>

using namespace std;

Gnss_Synchro_Udp_Source::Gnss_Synchro_Udp_Source(const unsigned short port) :
    socket{io_service},
    endpoint{boost::asio::ip::udp::v4(), port}
{
    socket.open(endpoint.protocol(), error);  // Open socket.
    socket.bind(endpoint, error);             // Bind the socket to the given local endpoint.
    
    std::string hostname{"192.168.0.2"};

    // Socket for PRN data (port 7535)
    sock = ::socket(AF_INET, SOCK_DGRAM, 0);
    destination.sin_family = AF_INET;
    destination.sin_port = htons(7535);
    destination.sin_addr.s_addr = inet_addr(hostname.c_str()); 
    
    // Socket for TOW data (port 1000)
    sock_tow = ::socket(AF_INET, SOCK_DGRAM, 0);
    destination_tow.sin_family = AF_INET;
    destination_tow.sin_port = htons(1000);
    destination_tow.sin_addr.s_addr = inet_addr(hostname.c_str());

    // Initialize variables
    old_tow = 0;
    index = 0;
    sent_tow = false;
    MAX_CHAN = 29;
}

bool Gnss_Synchro_Udp_Source::read_gnss_synchro(gnss_sdr::Observables& stocks)
{
    std::cerr << "Usage: monitoring-client <port>\n";
    char buff[1500];  // Buffer for storing the received data.

    // Block until data is received
    int bytes = socket.receive(boost::asio::buffer(buff));
    std::string data(&buff[0], bytes);
    
    // Deserialize the data
    return stocks.ParseFromString(data);
}

void Gnss_Synchro_Udp_Source::populate_channels(gnss_sdr::Observables& stocks)
{
    for (std::size_t i = 0; i < stocks.observable_size(); i++)
    {
        gnss_sdr::GnssSynchro ch = stocks.observable(i);
        if (ch.fs() != 0)  // Channel is valid.
        {
            channels[ch.channel_id()] = ch;
        }
    }
}

bool Gnss_Synchro_Udp_Source::print_table()
{
    if (read_gnss_synchro(stocks))
    {
        populate_channels(stocks);
        clear();  // Clear the screen.

        // Print table header
        attron(A_REVERSE);
        printw("%3s%6s%14s%17s%21s%25s%32s\n", 
               "CH", "PRN", "CN0 [dB-Hz]", "Doppler [Hz]", 
               "prompt_i", "rx_time", "TOW_at_current_symbol_ms");
        attroff(A_REVERSE);

        double new_tow;
        double diff;
        double bits[MAX_CHAN];
        memset(bits, 0, sizeof(bits));  // Initialize all bits to 0

        int bit = 0;
        int main_bit = 0;

        // Process each channel
        for (auto const& ch : channels)
        {
            int channel_id = ch.first;
            gnss_sdr::GnssSynchro data = ch.second;

            printw("%3d%6d%14f%17f%21f%25f%32lu\n", 
                   channel_id, data.prn(), data.cn0_db_hz(), 
                   data.carrier_doppler_hz(), data.prompt_i(), 
                   data.rx_time(), data.tow_at_current_symbol_ms());

            // Determine bit value
            main_bit = (data.prompt_i() > 0) ? 1 : 0;
            bit = (data.prn() * 10) + main_bit;
            printw("%3d \n", bit);

            bits[channel_id] = static_cast<double>(bit);
            new_tow = data.tow_at_current_symbol_ms();
            diff = data.rx_time();
        }

        // Send data when TOW changes
        if (old_tow != new_tow)
        {  
            // Send PRN data to port 7535
            sendto(sock, bits, MAX_CHAN * sizeof(double), 0, 
                  reinterpret_cast<sockaddr*>(&destination), sizeof(destination));

            // Send TOW to port 1000
            double TOW_r = new_tow / 1000;
            sendto(sock_tow, &TOW_r, sizeof(double), 0,
                  reinterpret_cast<sockaddr*>(&destination_tow), sizeof(destination_tow));

            old_tow = new_tow;  // Update for next iteration
        }

        refresh();  // Update the screen.
        return true;
    }
    else
    {
        return false;
    }
}
