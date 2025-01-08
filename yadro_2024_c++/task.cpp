// #include "testtask.h"
#include <iostream>
#include <fstream>
#include <vector>
#include <sstream>
#include <limits>
#include <format>
#include <map>
#include <set>
#include <queue>
#include <regex>

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
    // int number;
    bool occupied;
    int money;
    int work_time;
    // Time start;
    // Time end;

    Table(bool o = false, int m = 0, int w = 0) : occupied(o), money(m), work_time(w) {}

    friend ostream &operator<<(ostream &os, const Table &table)
    {
        os << format("Occupied: {}, Money: {}, Work time: {}",
                     table.occupied ? "Yes" : "No",
                     table.money,
                     table.work_time);
        return os;
    }
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

int roundToHours(int minutes)
{
    return (minutes / 60);
}

bool checkTimeFormat(string time){
    regex time_regex (R"(^([01][0-9]|2[0-3]):[0-5][0-9]$)");
    return regex_match(time,time_regex);
}

bool checkEventFormat(string event)
{
    regex event_regex(R"(^([01][0-9]|2[0-3]):([0-5][0-9]) ([1-9]\d*) ([a-z0-9_-]+)(?: ([1-9]\d*))?$)");
    return regex_match(event,event_regex);
}

// Получаем данные из файла в структуру InputData для дальнейшего взаимодествия
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
    
    if (!checkTimeFormat(time_start) || !checkTimeFormat(time_end))
    { // Проверка строк на формат времени
        throw time_string;
    }

    char colon; // поглащене ':' из строки
    istringstream stream_time_start(time_start);
    int start_hour, start_minute;
    stream_time_start >> start_hour >> colon >> start_minute;
    data.time_open = Time(start_hour, start_minute); // получаем время открытия

    istringstream stream_time_end(time_end);
    int end_hour, end_minute;
    stream_time_end >> end_hour >> colon >> end_minute;
    data.time_close = Time(end_hour, end_minute); // получаем время закрытия

    file >> data.coast; // получаем стоимость часа
    // проверка на время
    file.ignore(numeric_limits<streamsize>::max(), '\n');

    string event;
    Time swap_time = Time(0, 0);
    while (getline(file, event))
    {
        // Проверка event на формат
        // if(!checkEventFormat(event)){
        //     throw event;
        // }
        // else{
        //     data.events.push_back(event);
        // }
        data.events.push_back(event);
    }
    return data;
}

void StartEvent(InputData data)
{
    map<int, Table> tableStats; // Будем хранить для каждого стола выручку и время использования
    set<string> ClientList;     // Будем хранить имена посетителей
    queue<string> fifo;         // Очередь людей

    // Заполняем информацию о столах
    for (int i = 1; i <= data.count_of_table; i++)
    {
        tableStats[i] = Table(false, 0, 0);
    }

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

        int id = event.id_event;
        string name = event.name_client;
        int event_time = event.time.toMinutes();
        switch (id)
        {
        case 1:                                                   // client comming
            if (event_time < time_start || event_time > time_end) // Проверка, что клиент пришёл в рабочее время
            {
                cout << format("{:02}:{:02} 13 NotOpenYet", event.time.hours, event.time.minutes) << endl;
                continue;
            }
            else if (ClientList.count(name)) // Проверка на наличие клиента в ПК клубе
            {
                cout << format("{:02}:{:02} 13 YouShallNotPass", event.time.hours, event.time.minutes) << endl;
            }
            else
            {
                ClientList.insert(name); // Добавление имени клиента в список присутствующих
            }
            break;
        case 2:                          // take table
            if (!ClientList.count(name)) // Проверка на наличие клиента в ПК клубе
            {
                cout << format("{:02}:{:02} 13 ClientUnknown", event.time.hours, event.time.minutes) << endl;
            }
            else if (tableStats[event.table].occupied) // Проверка свободен ли стол
            {
                cout << format("{:02}:{:02} 13 PlaceIsBusy", event.time.hours, event.time.minutes) << endl;
            }
            else // Если стол свободен, то клиент садится
            {
                // Информация вносится в map tableStats
                tableStats[event.table].occupied = true;
                tableStats[event.table].work_time = event.time.toMinutes();
            }
            break;
        case 3: // waiting
            // Проверка на свободные столы, если их нету, то клиент уходит
            // if( условием ){ проверять все столы, пока не occupied = FALSE}
            if (fifo.size() + 1 > tableStats.size())
            { // сменить IF на ELSE IF, когда сделаю норм IF на проверку занятости всех столов
                cout << format("{:02}:{:02} 11 {}", event.time.hours, event.time.minutes, name) << endl;
            }
            break;
        case 4: // walk away from table
            if (!ClientList.count(name))
            {
                cout << format("{:02}:{:02} 13 ClientUnknown", event.time.hours, event.time.minutes) << endl;
            }

            // Наступило время закрытия? Да, то событие 11.
            // cout << format("{:02}:{:02} 11 {}", event.time.hours, event.time.minutes, event.n) << endl;

            // Проверка сколько времени он просидел за компьютером (От "переменной - время сейчас")
            // Вычисляем прибыль от клиента (через while делим минуты на 60, пока остаток не равен 0)
            // Обновление информации в map tableStats

            // Проверка на наличие очереди:
            // Если в ней есть клиент, то генерируется событие 12, и за свободный стол, садится первый клиент из очереди, удаляется из очереди
            break;
        }
    }

    data.time_close.print(); // Вызвать событие 11, для всех клиентов, которые сейчас в ПК клубе ДО вывода времени закрытия

    // Выводим информацию о столах
    for (const auto &[key, value] : tableStats)
    {
        cout << "Table " << key << ": " << value << endl;
    }

    // Очистка памяти
    ClientList.clear();
    tableStats.clear();
    while (!fifo.empty())
    {
        fifo.pop();
    }
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
    catch (const string error_message)
    {
        cout << error_message << endl;
        return 1;
    }

    return 0;
}