// #include "testtask.h"
#include <iostream>
#include <fstream>
#include <vector>
#include <sstream>

using namespace std;

struct Time_w
{
    string time_start;
    string time_end;
};

struct InputData
{
    int count_of_table;
    Time_w time_work;
    int coast;
    vector<string> events;
};

string checkArgument(int argc, char *argv[])
{
    if (argc > 1)
    {
        return argv[1];
    }
    else
    {
        throw "Не передан аргумент с именем файла";
    }
}

InputData readFile(string filename){
    InputData data;
    ifstream file(filename);
    if(!file.is_open()){
        throw "Ошибка в открытие файла";
    }
    file >> data.count_of_table; // получаем количество столов
    file.ignore();

    string time_string;
    getline(file,time_string);
    istringstream stream(time_string);
    stream >> data.time_work.time_start >> data.time_work.time_end;

    file >> data.coast; // получаем стоимость часа
    file.ignore();

    string events;
    while(getline(file,events)){
        data.events.push_back(events);
    }
    return data;
}

bool checkDataFormat(){
    // Имена клиентов представляют собой комбинацию символов из алфавита a..z, 0..9, _, -
    // Время задается в 24-часовом формате с двоеточием в качестве разделителя XX:XX, незначащие нули обязательны при вводе и выводе (например 15:03 или 08:09).
    // Каждый стол имеет свой номер от 1 до N, где N – общее число столов, указанное в конфигурации.
    // Все события идут последовательно во времени. (время события N+1) ≥ (время события N).
    return true;
}

int main(int argc, char *argv[])
{
    try
    {
        string filename = checkArgument(argc, argv);
        InputData data = readFile(filename);
    }
    catch (const char *error_message)
    {
        cout << error_message << endl;
        return 1;
    }

    return 0;
}