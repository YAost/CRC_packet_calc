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
        cout << hex << setfill('0') << setw(2) << uppercase <<  inputMass[ j ] << " ";      // ôîðìàòèðîâàííûé âûâîä HEX-ýëåìåíòîâ ñ âåäóùèìè íóëÿìè è â âåðõíåì ðåãèñòðå
    }
    if (inputMass[12] == 0x08 && inputMass[13] == 0x00) {   //
        cout << endl << endl << "Çàãîëîâîê Ethernet:" << endl << endl;
        outputValue(inputMass, 0, 6, 0, "MAC ïîëó÷àòåëÿ");
        outputValue(inputMass, 6, 12, 0, "MAC îòïðàâèòåëÿ");
        outputValue(inputMass, 12, 14, 0, "IPv4 over Ethernet");
        cout << endl;

        if ((0b11110000 & inputMass[14]) == 0b01000000) {    // ïðîâåðêà ïåðâûõ 4õ áèò IP-ïàêåòà - âåðñèè IP
            cout << "Çàãîëîâîê IP:" << endl;
            outputValue(inputMass, 14, 16, 0, "IPv4, äëèíà çàãîëîâêà (4-õ áàéòíûõ ñëîâ); òèï òðàôèêà");
            ihl = (0b00001111 & inputMass[14])*4; // âû÷èñëÿåì äëèíó çàãîëîâêà â áàéòàõ, âòîðûå 4 áèò
            cout << "Äëèíà çàãîëîâêà IP (IHL) = " << dec << ihl << " áàéò" << endl;
            ihl = 14 + ihl;     // çàãîëîâîê IP çàêàí÷èâàåòñÿ ïî àäðåñó ihl
            outputValue(inputMass, 16, 18, 1, "- Äëèíà ïàêåòà IP â áàéòàõ");

            cout << endl << "Ðàñ÷åò êîíòðîëüíîé ñóììû çàãîëîâêà IP:" << endl;
            if (inputMass[24]==0 && inputMass[25] == 0) {
                calculateSUM(inputMass,14,ihl,crcIP);
                calculateCRC(crcIP, "IP");
            }
            else
                cout << "Êîíòðîëüíàÿ ñóììà óêàçàíà â ïàêåòå, ðàâíà " << hex << setfill('0') << setw(2) << inputMass[24] << " " << setfill('0') << setw(2) << inputMass[25] << endl;

            if (inputMass[23]==0x11) {
                cout << "Çàãîëîâîê UDP" << endl;
                if (inputMass[ihl+6]==0 && inputMass[ihl+7]==0)    {
                    cout << "Ðàñ÷åò êîíòðîëüíîé ñóììû çàãîëîâêà UDP:" << endl;
                    calculateSUM(inputMass, 26, 33, crcUDP);

                    cout << hex << setfill('0') << setw(4) << crcUDP << " + 0011 = ";
                    crcUDP += 0x0011;
                    cout << crcUDP << endl;
                    ss << hex << setfill('0') << setw(2) << inputMass[ihl+4] << setfill('0') << setw(2) << inputMass[ihl+5];      // ïðèáàâëÿåì ïñåâäîçàãîëîâîê: ïðîòîêîë (UDP) - 0011
                    ss >> crcTMP;
                    cout << hex << setfill('0') << setw(4) << crcUDP << " + " << setfill('0') << setw(4) << crcTMP;
                    crcUDP += crcTMP;
                    cout << " = " << crcUDP << endl;
                    ss.clear();
                    crcTMP = 0;

                    calculateSUM(inputMass, ihl, sizeMass, crcUDP);
                    calculateCRC(crcUDP, "UDP");
                }
                else cout << "Êîíòðîëüíàÿ ñóììà óêàçàíà â ïàêåòå, ðàâíà " << hex << setfill('0') << setw(2) << inputMass[ihl+6] << setfill('0') << setw(2) << inputMass[ihl+7] << endl;
            }



            if (inputMass[23]==0x06) cout << "TCP";
            if (inputMass[23]==0x01) cout << "ICMP";
        }
        /* òóò äîëæíî áûòü ïðî IPv6 */
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
    cout << CRC_NAME << " - êîíòðîëüíàÿ ñóììà çàãîëîâêà " << proto << endl;
}

void outputValue(int *MASS, int MASS_START, int MASS_END, bool decimal, string text) {
    int it = 0;
    for (it = MASS_START; it < MASS_END; it++) {
        cout << hex << setfill('0') << setw(2) << MASS[it] << " ";
    }
    if (decimal)    {
        cout << "Äåñÿòè÷íûé: ";
        for (it = MASS_START; it < MASS_END; it++)  {
        cout << dec << setfill('0') << setw(2) << MASS[it] << " ";
        }
    }
    cout << " " << text << endl;
}
