// #include "testtask.h"
#include <iostream>
#include <fstream>
#include <vector>
#include <sstream>
#include <limits>
#include <format>
#include <map>
#include <set>

using namespace std;

struct Time
{
    int hours;
    int minutes;
    Time(int h = 0, int m = 0) : hours(h), minutes(m) {}

    void print() const
    {
        // cout << format("{}:{}",hours,minutes) << endl;
        cout << format("{:02}:{:02}", hours, minutes) << endl;
        // printf("%02d:%02d\n", hours, minutes);
    }

    int toMinutes() const
    {
        return hours * 60 + minutes;
    }
};

struct InputData
{
    int count_of_table;
    Time time_open;
    Time time_close;
    int coast;
    vector<string> events;
};

struct InputEvent
{
    Time time;
    int id_event;
    string name_client;
    int table;
};

struct Table // Для хранения информации о столах будет использован (map)
{
    int number;
    int money;
    int work_time;

    Table(int n = 0, int m = 0, int w = 0) : number(n), money(m), work_time(w) {}
};

string checkArgument(int argc, char *argv[])
{
    if (argc > 1)
    {
        return argv[1];
    }
    else
    {
        throw "File name argument was not passed";
    }
}

// int roundToHours(){
//     // return (time in minutes) / 60;
// }

bool checkDataFormat()
{
    // Имена клиентов представляют собой комбинацию символов из алфавита a..z, 0..9, _, -
    // Время задается в 24-часовом формате с двоеточием в качестве разделителя XX:XX, незначащие нули обязательны при вводе и выводе (например 15:03 или 08:09).
    // Каждый стол имеет свой номер от 1 до N, где N – общее число столов, указанное в конфигурации.
    // Все события идут последовательно во времени. (время события N+1) ≥ (время события N).
    return true;
}

InputData readFile(string filename)
{
    InputData data;
    ifstream file(filename);
    if (!file.is_open())
    {
        throw "Error, file dont open!";
    }
    file >> data.count_of_table; // получаем количество столов
    file.ignore(numeric_limits<streamsize>::max(), '\n');

    string time_string;
    // Получаем строку со временем открытия(HH:MM) и закрытия(HH:MM)
    getline(file, time_string);
    istringstream stream(time_string);
    string time_start, time_end;
    stream >> time_start >> time_end;

    char colon; // поглащене ':' из строки
    istringstream stream_time_start(time_start);
    int start_hour, start_minute;
    stream_time_start >> start_hour >> colon >> start_minute;
    data.time_open = Time(start_hour, start_minute);

    istringstream stream_time_end(time_end);
    int end_hour, end_minute;
    stream_time_end >> end_hour >> colon >> end_minute;
    data.time_close = Time(end_hour, end_minute);

    file >> data.coast; // получаем стоимость часа
    file.ignore(numeric_limits<streamsize>::max(), '\n');

    string event;
    while (getline(file, event))
    {
        data.events.push_back(event);
    }
    return data;
}

void StartEvent(InputData data)
{
    map<int, Table> tableStats; // Будем хранить для каждого стола выручку и время использования
    set<string> ClientList;     // Будем хранить имена посетителей

    data.time_open.print();
    int time_start = data.time_open.toMinutes();
    int time_end = data.time_close.toMinutes();

    // проходим по событиям
    for (string s : data.events)
    {
        InputEvent event;
        istringstream stream(s);
        string time;
        stream >> time >> event.id_event >> event.name_client;

        if (!(stream >> event.table))
        {
            event.table = -1;
        }

        char colon; // поглащене ':' из строки
        istringstream stream_time(time);
        int hour, minute;
        stream_time >> hour >> colon >> minute;
        event.time = Time(hour, minute);
        cout << format("{:02}:{:02} {} {} {}", event.time.hours, event.time.minutes, event.id_event, event.name_client, (event.table == -1 ? " " : to_string(event.table))) << endl;

        if (event.time.toMinutes() < time_start || event.time.toMinutes() > time_end)
        {
            cout << format("{:02}:{:02} 13 NotOpenYet", event.time.hours, event.time.minutes) << endl;
            continue;
        }
        int id = event.id_event;
        string name = event.name_client;
        switch (id)
        {
        case 1: // client comming
            if (ClientList.count(name))
            {
                cout << format("{:02}:{:02} 13 YouShallNotPass", event.time.hours, event.time.minutes) << endl;
            }
            else
            {
                ClientList.insert(name);
            }
            break;
        case 2: // take table
            if (!ClientList.count(name))
            {
                cout << format("{:02}:{:02} 13 ClientUnknown", event.time.hours, event.time.minutes) << endl;
            }
            else{
                // Информация вносится в map tableStats
            }
            break;
        case 3: // waiting
            // проверка if(queue.size() > count(PC)){
            //     cout << format("{:02}:{:02} 11 {}", event.time.hours, event.time.minutes,name) << endl;
            // }
            break;
        case 4: // walk away from table
            if (!ClientList.count(name))
            {
                cout << format("{:02}:{:02} 13 ClientUnknown", event.time.hours, event.time.minutes) << endl;
            }
            // Проверка сколько времени он просидел за компьютером => вычисляем прибыль от клиента
            // Обновление информации в map tableStats
            break;
        }
    }
    data.time_close.print();
}

int main(int argc, char *argv[])
{
    try // Может не работать, пока пересматриваю структуры
    {
        string filename = checkArgument(argc, argv);
        InputData data = readFile(filename);
        StartEvent(data);
    }
    catch (const char *error_message)
    {
        cout << error_message << endl;
        return 1;
    }

    return 0;
}