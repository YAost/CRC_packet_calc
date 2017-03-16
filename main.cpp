#include <iostream>
#include <string.h>
#include <sstream>
#include <iomanip>
#include <bitset>

using namespace std;

void calculateSUM(int *, int , int , int &);
void calculateCRC(int &, string );
void outputValue(int *, int , int , bool , string );

int main()
{
    int i = 0;
    int j = 0;
    stringstream ss;
    int *inputMass;         // massive with packet's bytes
    int sizeMass;           // size of it
    int ihl = 0;            // internet header length, for IPv4 = real length, IPv6 doesn't support
    int crcIP = 0;          // checksum IP
    int crcUDP = 0;         // checksum UDP
    int crcTMP = 0;             // temporary variable to calculate
    string inputStr;        // input string with spaces between bytes
    cout << "Protocols and services of IP networks" << endl;
    cout << "CRC calculate of IP, TCP, UDP headers" << endl;
    cout << "V1.0" << endl << endl;
    cout << "Enter the packet in HEX, put space between bytes" << endl;
    cout << "If you need to calculate checksum, put 00 00 in desired bytes" << endl;
    cout << "Packet: " << endl;
    getline(cin >> hex, inputStr);     // read string with spaces
    cout << endl << "Size of packet = " << inputStr.length() << " symbols include spaces << endl;    // Whole number of chars in string
    sizeMass =  ( inputStr.length() + 1 ) / 3 ;     // size of mass = number of chars in string without spaces, two chars in one masive's variable
    // for string 14 E8 number of chars is 5, add 1 char for very last byte at the end of string, devided by 3 = 2 bytes
    inputMass = new int [ sizeMass ];      // allocate memory of needed number of bytes
    cout << "Êîëè÷åñòâî áàéò = " << ( inputStr.length() + 1 ) / 3 << endl << endl;
    for ( i=0; i < (inputStr.length()+1); i = i + 3)    // sort string's chars: zero first, then every third char (whic is byte)
        if ( j < sizeMass )         // fill our actuall massive with chars from string
        {
            ss << hex << inputStr[ i ];     // put the value of first char from string into temp stream
            ss << hex << inputStr[ i + 1 ]; // then the second one
            ss >> inputMass[ j ];       // fill the actual massive 
            j++;                // itr
            ss.clear();         // flush the stream
        }
    for ( j = 0, i = 0; j < sizeMass; j++ )    {        // display the elements
        if ( i == 16 )  {
            i = 0;
            cout << endl;
        }
        i++;
        cout << hex << setfill('0') << setw(2) << uppercase <<  inputMass[ j ] << " ";      // formated output of HEX-elements with leading zeroes in capitals
    }
    if (inputMass[12] == 0x08 && inputMass[13] == 0x00) {   //
        cout << endl << endl << "Ethernet header:" << endl << endl;
        outputValue(inputMass, 0, 6, 0, "Destination MAC");
        outputValue(inputMass, 6, 12, 0, "Sender MAC");
        outputValue(inputMass, 12, 14, 0, "IPv4 over Ethernet");
        cout << endl;

        if ((0b11110000 & inputMass[14]) == 0b01000000) {    // check IP version
            cout << "IP header:" << endl;
            outputValue(inputMass, 14, 16, 0, "IPv4 header length (4byte words); type of service");
            ihl = (0b00001111 & inputMass[14])*4; // calculate length in bytes, last 4 bits
            cout << "IP header length = " << dec << ihl << " byte" << endl;
            ihl = 14 + ihl;     // ip header ends up at ihl address
            outputValue(inputMass, 16, 18, 1, "- length of IP packet (bytes)");

            cout << endl << "Calculate checksum of IP header:" << endl;
            if (inputMass[24]==0 && inputMass[25] == 0) {
                calculateSUM(inputMass,14,ihl,crcIP);
                calculateCRC(crcIP, "IP");
            }
            else
                cout << "CRC is in packet and it's " << hex << setfill('0') << setw(2) << inputMass[24] << " " << setfill('0') << setw(2) << inputMass[25] << endl;

            if (inputMass[23]==0x11) {
                cout << "UDP header" << endl;
                if (inputMass[ihl+6]==0 && inputMass[ihl+7]==0)    {
                    cout << "Calculate checksum of UDP header:" << endl;
                    calculateSUM(inputMass, 26, 33, crcUDP);

                    cout << hex << setfill('0') << setw(4) << crcUDP << " + 0011 = ";
                    crcUDP += 0x0011;
                    cout << crcUDP << endl;
                    ss << hex << setfill('0') << setw(2) << inputMass[ihl+4] << setfill('0') << setw(2) << inputMass[ihl+5];      //add psuedoheader: protocol udp 00 11
                    ss >> crcTMP;
                    cout << hex << setfill('0') << setw(4) << crcUDP << " + " << setfill('0') << setw(4) << crcTMP;
                    crcUDP += crcTMP;
                    cout << " = " << crcUDP << endl;
                    ss.clear();
                    crcTMP = 0;

                    calculateSUM(inputMass, ihl, sizeMass, crcUDP);
                    calculateCRC(crcUDP, "UDP");
                }
                else cout << "CRC is in packet and it's  " << hex << setfill('0') << setw(2) << inputMass[ihl+6] << setfill('0') << setw(2) << inputMass[ihl+7] << endl;
            }



            if (inputMass[23]==0x06) cout << "TCP";
            if (inputMass[23]==0x01) cout << "ICMP";
        }
        /*should be about IPv6 */
    }
    /*cout << endl << "Çàâåðøåíèå ïðîãðàììû" << endl;
    cin.get();*/
    return 0;
}

void calculateSUM(int *MASS, int MASS_START, int MASS_END, int &CRC_NAME)   {
    int it = 0;
    int CRC_TMP;
    stringstream stream;
    for (it = MASS_START; it < MASS_END; it = it + 2)   {
            stream << hex << setfill('0') << setw(2) << MASS[it] << setfill('0') << setw(2) << MASS[it+1];
            stream >> CRC_TMP;
            cout << hex << setfill('0') << setw(4) << CRC_NAME << " + " << setfill('0') << setw(4) << CRC_TMP;
            CRC_NAME += CRC_TMP;
            cout << " = " << CRC_NAME << endl;
            stream.clear();
            CRC_TMP = 0;
    }

}

void calculateCRC(int &CRC_NAME, string proto)    {
    int CRC_TMP = CRC_NAME >> 16;
    CRC_NAME = 0b00001111111111111111&CRC_NAME;
    cout << CRC_NAME << " + " << CRC_TMP << " = ";
    CRC_NAME = (0b00001111111111111111&CRC_NAME) + CRC_TMP;
    cout << CRC_NAME << endl;
    cout << "FFFF - " << CRC_NAME << " = ";
    CRC_NAME = 0xFFFF-CRC_NAME;
    cout << CRC_NAME << " - checksum of header " << proto << endl;
}

void outputValue(int *MASS, int MASS_START, int MASS_END, bool decimal, string text) {
    int it = 0;
    for (it = MASS_START; it < MASS_END; it++) {
        cout << hex << setfill('0') << setw(2) << MASS[it] << " ";
    }
    if (decimal)    {
        cout << "Decimal: ";
        for (it = MASS_START; it < MASS_END; it++)  {
        cout << dec << setfill('0') << setw(2) << MASS[it] << " ";
        }
    }
    cout << " " << text << endl;
}
