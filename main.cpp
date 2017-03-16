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
    int *inputMass;         // ������ ���� ������
    int sizeMass;           // ������ ������� ������
    int ihl = 0;            // ��� IPv4 - ������ ���������, ��� IPv6 = 0
    int crcIP = 0;          // �������� �������� ��������� IP
    int crcUDP = 0;         // �������� �������� ��������� UDP
    int crcTMP = 0;             // ��������� ���������� ��� ��������� CRC
    string inputStr;        // �������� ������ � ���������
    setlocale( LC_ALL, "Russian" );
    cout << "���������, ������� � ������ � ����� IP" << endl;
    cout << "������� ����������� ����� ���������� IP, TCP, UDP" << endl;
    cout << "������ 1.0" << endl << endl;
    cout << "������� ���������� ������ � ����������������� ����, ������� ����� ���������." << endl;
    cout << "����������� ��� ���������� ���� ����������� ����� �������� ������." << endl;
    cout << "�����: " << endl;
    getline(cin >> hex, inputStr);     // ������ ������ � ���������
    cout << endl << "������ ������ = " << inputStr.length() << " ������ � ���������" << endl;    // ����� ���������� �������� � ������
    sizeMass =  ( inputStr.length() + 1 ) / 3 ;     // ������ ������� = ���������� �������� � ������ ��� ��������, �� ��� ������� � ������ �������
    // ��� ������ 14 E8 ���������� �������� = 5, ���������� 1 ������ ��� "�����������" ������ � ����� ������, ����� �� 3 = �������� 2 �����
    inputMass = new int [ sizeMass ];      // �������� ������ ��� ���������� ���� ������
    cout << "���������� ���� = " << ( inputStr.length() + 1 ) / 3 << endl << endl;
    for ( i=0; i < (inputStr.length()+1); i = i + 3)    // ���������� ������� ������: �������, � ����� ������ ������ ������ - ������ ����� (�������������� �������)
        if ( j < sizeMass )         // ��������� ������ ���� ������
        {
            ss << hex << inputStr[ i ];     // �������� �������� ������� ������� ����� �� ������ � ���������-����� � HEX-����
            ss << hex << inputStr[ i + 1 ]; // �������� �������� ������� ������� �����
            ss >> inputMass[ j ];       // ������� �������� �� ���������-������ � ������ ������� ������
            j++;                // ��������
            ss.clear();         // ������� ���������-�����
        }
    for ( j = 0, i = 0; j < sizeMass; j++ )    {        // ����� ��������� ������� ������
        if ( i == 16 )  {
            i = 0;
            cout << endl;
        }
        i++;
        cout << hex << setfill('0') << setw(2) << uppercase <<  inputMass[ j ] << " ";      // ��������������� ����� HEX-��������� � �������� ������ � � ������� ��������
    }
    if (inputMass[12] == 0x08 && inputMass[13] == 0x00) {   //
        cout << endl << endl << "��������� Ethernet:" << endl << endl;
        outputValue(inputMass, 0, 6, 0, "MAC ����������");
        outputValue(inputMass, 6, 12, 0, "MAC �����������");
        outputValue(inputMass, 12, 14, 0, "IPv4 over Ethernet");
        cout << endl;

        if ((0b11110000 & inputMass[14]) == 0b01000000) {    // �������� ������ 4� ��� IP-������ - ������ IP
            cout << "��������� IP:" << endl;
            outputValue(inputMass, 14, 16, 0, "IPv4, ����� ��������� (4-� ������� ����); ��� �������");
            ihl = (0b00001111 & inputMass[14])*4; // ��������� ����� ��������� � ������, ������ 4 ���
            cout << "����� ��������� IP (IHL) = " << dec << ihl << " ����" << endl;
            ihl = 14 + ihl;     // ��������� IP ������������� �� ������ ihl
            outputValue(inputMass, 16, 18, 1, "- ����� ������ IP � ������");

            cout << endl << "������ ����������� ����� ��������� IP:" << endl;
            if (inputMass[24]==0 && inputMass[25] == 0) {
                calculateSUM(inputMass,14,ihl,crcIP);
                calculateCRC(crcIP, "IP");
            }
            else
                cout << "����������� ����� ������� � ������, ����� " << hex << setfill('0') << setw(2) << inputMass[24] << " " << setfill('0') << setw(2) << inputMass[25] << endl;

            if (inputMass[23]==0x11) {
                cout << "��������� UDP" << endl;
                if (inputMass[ihl+6]==0 && inputMass[ihl+7]==0)    {
                    cout << "������ ����������� ����� ��������� UDP:" << endl;
                    calculateSUM(inputMass, 26, 33, crcUDP);

                    cout << hex << setfill('0') << setw(4) << crcUDP << " + 0011 = ";
                    crcUDP += 0x0011;
                    cout << crcUDP << endl;
                    ss << hex << setfill('0') << setw(2) << inputMass[ihl+4] << setfill('0') << setw(2) << inputMass[ihl+5];      // ���������� ���������������: �������� (UDP) - 0011
                    ss >> crcTMP;
                    cout << hex << setfill('0') << setw(4) << crcUDP << " + " << setfill('0') << setw(4) << crcTMP;
                    crcUDP += crcTMP;
                    cout << " = " << crcUDP << endl;
                    ss.clear();
                    crcTMP = 0;

                    calculateSUM(inputMass, ihl, sizeMass, crcUDP);
                    calculateCRC(crcUDP, "UDP");
                }
                else cout << "����������� ����� ������� � ������, ����� " << hex << setfill('0') << setw(2) << inputMass[ihl+6] << setfill('0') << setw(2) << inputMass[ihl+7] << endl;
            }



            if (inputMass[23]==0x06) cout << "TCP";
            if (inputMass[23]==0x01) cout << "ICMP";
        }
        /* ��� ������ ���� ��� IPv6 */
    }
    /*cout << endl << "���������� ���������" << endl;
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
    cout << CRC_NAME << " - ����������� ����� ��������� " << proto << endl;
}

void outputValue(int *MASS, int MASS_START, int MASS_END, bool decimal, string text) {
    int it = 0;
    for (it = MASS_START; it < MASS_END; it++) {
        cout << hex << setfill('0') << setw(2) << MASS[it] << " ";
    }
    if (decimal)    {
        cout << "����������: ";
        for (it = MASS_START; it < MASS_END; it++)  {
        cout << dec << setfill('0') << setw(2) << MASS[it] << " ";
        }
    }
    cout << " " << text << endl;
}
