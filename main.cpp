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
    int *inputMass;         // массив байт пакета
    int sizeMass;           // размер массива байтов
    int ihl = 0;            // для IPv4 - размер заголовка, для IPv6 = 0
    int crcIP = 0;          // значение чексуммы заголовка IP
    int crcUDP = 0;         // значение чексуммы заголовка UDP
    int crcTMP = 0;             // временная переменная для рассчетов CRC
    string inputStr;        // исходная строка с пробелами
    setlocale( LC_ALL, "Russian" );
    cout << "Протоколы, сервисы и услуги в сетях IP" << endl;
    cout << "Подсчет контрольной суммы заголовков IP, TCP, UDP" << endl;
    cout << "Версия 1.0" << endl << endl;
    cout << "Введите содержимое пакета в шестнадцатиричном виде, отделяя байты пробелами." << endl;
    cout << "Необходимые для вычисления поля контрольной суммы заменить нулями." << endl;
    cout << "Пакет: " << endl;
    getline(cin >> hex, inputStr);     // читаем строку с пробелами
    cout << endl << "Размер пакета = " << inputStr.length() << " символ с пробелами" << endl;    // общее количество символов в строке
    sizeMass =  ( inputStr.length() + 1 ) / 3 ;     // размер массива = количеству символов в строке без пробелов, по два символа в ячейку массива
    // для строки 14 E8 количество символов = 5, прибавляем 1 символ под "недостающий" пробел в конце строки, делим на 3 = получаем 2 байта
    inputMass = new int [ sizeMass ];      // выделяем память под количество байт пакета
    cout << "Количество байт = " << ( inputStr.length() + 1 ) / 3 << endl << endl;
    for ( i=0; i < (inputStr.length()+1); i = i + 3)    // перебираем символы строки: нулевой, а после каждый третий символ - начало байта (двухсимвольный элемент)
        if ( j < sizeMass )         // заполняем массив байт пакета
        {
            ss << hex << inputStr[ i ];     // помещаем значение первого символа байта из строки в хранилище-поток в HEX-виде
            ss << hex << inputStr[ i + 1 ]; // помещаем значение второго символа байта
            ss >> inputMass[ j ];       // выводим значения из хранилища-потока в ячейку массива байтов
            j++;                // итератор
            ss.clear();         // очищаем хранилище-поток
        }
    for ( j = 0, i = 0; j < sizeMass; j++ )    {        // вывод элементов массива байтов
        if ( i == 16 )  {
            i = 0;
            cout << endl;
        }
        i++;
        cout << hex << setfill('0') << setw(2) << uppercase <<  inputMass[ j ] << " ";      // форматированный вывод HEX-элементов с ведущими нулями и в верхнем регистре
    }
    if (inputMass[12] == 0x08 && inputMass[13] == 0x00) {   //
        cout << endl << endl << "Заголовок Ethernet:" << endl << endl;
        outputValue(inputMass, 0, 6, 0, "MAC получателя");
        outputValue(inputMass, 6, 12, 0, "MAC отправителя");
        outputValue(inputMass, 12, 14, 0, "IPv4 over Ethernet");
        cout << endl;

        if ((0b11110000 & inputMass[14]) == 0b01000000) {    // проверка первых 4х бит IP-пакета - версии IP
            cout << "Заголовок IP:" << endl;
            outputValue(inputMass, 14, 16, 0, "IPv4, длина заголовка (4-х байтных слов); тип трафика");
            ihl = (0b00001111 & inputMass[14])*4; // вычисляем длину заголовка в байтах, вторые 4 бит
            cout << "Длина заголовка IP (IHL) = " << dec << ihl << " байт" << endl;
            ihl = 14 + ihl;     // заголовок IP заканчивается по адресу ihl
            outputValue(inputMass, 16, 18, 1, "- Длина пакета IP в байтах");

            cout << endl << "Расчет контрольной суммы заголовка IP:" << endl;
            if (inputMass[24]==0 && inputMass[25] == 0) {
                calculateSUM(inputMass,14,ihl,crcIP);
                calculateCRC(crcIP, "IP");
            }
            else
                cout << "Контрольная сумма указана в пакете, равна " << hex << setfill('0') << setw(2) << inputMass[24] << " " << setfill('0') << setw(2) << inputMass[25] << endl;

            if (inputMass[23]==0x11) {
                cout << "Заголовок UDP" << endl;
                if (inputMass[ihl+6]==0 && inputMass[ihl+7]==0)    {
                    cout << "Расчет контрольной суммы заголовка UDP:" << endl;
                    calculateSUM(inputMass, 26, 33, crcUDP);

                    cout << hex << setfill('0') << setw(4) << crcUDP << " + 0011 = ";
                    crcUDP += 0x0011;
                    cout << crcUDP << endl;
                    ss << hex << setfill('0') << setw(2) << inputMass[ihl+4] << setfill('0') << setw(2) << inputMass[ihl+5];      // прибавляем псевдозаголовок: протокол (UDP) - 0011
                    ss >> crcTMP;
                    cout << hex << setfill('0') << setw(4) << crcUDP << " + " << setfill('0') << setw(4) << crcTMP;
                    crcUDP += crcTMP;
                    cout << " = " << crcUDP << endl;
                    ss.clear();
                    crcTMP = 0;

                    calculateSUM(inputMass, ihl, sizeMass, crcUDP);
                    calculateCRC(crcUDP, "UDP");
                }
                else cout << "Контрольная сумма указана в пакете, равна " << hex << setfill('0') << setw(2) << inputMass[ihl+6] << setfill('0') << setw(2) << inputMass[ihl+7] << endl;
            }



            if (inputMass[23]==0x06) cout << "TCP";
            if (inputMass[23]==0x01) cout << "ICMP";
        }
        /* тут должно быть про IPv6 */
    }
    /*cout << endl << "Завершение программы" << endl;
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
    cout << CRC_NAME << " - контрольная сумма заголовка " << proto << endl;
}

void outputValue(int *MASS, int MASS_START, int MASS_END, bool decimal, string text) {
    int it = 0;
    for (it = MASS_START; it < MASS_END; it++) {
        cout << hex << setfill('0') << setw(2) << MASS[it] << " ";
    }
    if (decimal)    {
        cout << "Десятичный: ";
        for (it = MASS_START; it < MASS_END; it++)  {
        cout << dec << setfill('0') << setw(2) << MASS[it] << " ";
        }
    }
    cout << " " << text << endl;
}
