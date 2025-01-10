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

struct InputEvent
{
    Time time;
    int id_event;
    string name_client;
    int table_number;
};

struct InputData
{
    int count_of_table;
    Time time_open;
    Time time_close;
    int coast;
    vector<InputEvent> events;
};

struct Table // Для хранения информации о столах будет использован (map)
{
    // int number;
    string occupied;
    int money;
    int work_time;
    int time_table_occupied;

    Table(string o = "None", int m = 0, int w = 0, int t = 0) : occupied(o), money(m), work_time(w), time_table_occupied(t) {}

    friend ostream &operator<<(ostream &os, const Table &table)
    {
        os << format("{} {}",
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

bool checkTimeFormat(string time)
{
    regex time_regex(R"(^([01][0-9]|2[0-3]):[0-5][0-9]$)");
    return regex_match(time, time_regex);
}

bool checkEventFormat(string event)
{
    regex event_regex(R"(^([01][0-9]|2[0-3]):([0-5][0-9]) ([1-9]\d*) ([a-z0-9_-]+)(?:\s([1-9]\d*))?\s*$)");
    return regex_match(event, event_regex);
}

int SpendHours(int minutes)
{
    int n = minutes / 60;
    if (n % 60 > 0)
    {
        n++;
    }
    return n;
}

// Округрения времени до часов
int roundToHours(int minutes)
{
    return (minutes / 60);
}

int findTable(map<int, Table> tableStats, string name)
{
    int n = 0;
    for (int table = 1; table < tableStats.size(); table++)
    {
        if (tableStats[table].occupied == name)
        {
            n = table;
        }
    }
    return n;
}

bool checkTableOccupied(map<int, Table> tableStats)
{
    for (int table = 1; table < tableStats.size(); table++)
    {
        if (tableStats[table].occupied == "None")
        {
            return true;
        }
    }
    return false;
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
    int intermediate_time = swap_time.toMinutes();
    while (getline(file, event))
    {
        // Проверка event на формат
        if (!checkEventFormat(event)) // Надо решить проблему связанную с регулярным выражением для проверки
        {
            cout << "CheckEventFormat" << endl;
            throw event;
        }
        else
        {
            InputEvent object;
            istringstream stream(event);
            string time;
            stream >> time >> object.id_event >> object.name_client;

            if (!(stream >> object.table_number))
            {
                object.table_number = -1;
            }

            char colon; // поглащене ':' из строки
            istringstream stream_time(time);
            int hour, minute;
            stream_time >> hour >> colon >> minute;
            object.time = Time(hour, minute);

            if (intermediate_time > object.time.toMinutes() || (object.table_number != -1 && (object.table_number < 1 || object.table_number > data.count_of_table)))
            {
                cout << "Time or Number of table" << endl;
                throw format("{:02}:{:02} {} {} {}", object.time.hours, object.time.minutes, object.id_event, object.name_client, (object.table_number == -1 ? " " : to_string(object.table_number)));
            }
            else
            {
                intermediate_time = object.time.toMinutes();
                data.events.push_back(object);
            }
        }
        // data.events.push_back(event);
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
        tableStats[i] = Table("None", 0, 0, 0);
    }

    data.time_open.print();
    int time_start = data.time_open.toMinutes();
    int time_end = data.time_close.toMinutes();

    // проходим по событиям
    for (InputEvent event : data.events)
    {
        cout << format("{:02}:{:02} {} {} {}", event.time.hours, event.time.minutes, event.id_event, event.name_client, (event.table_number == -1 ? " " : to_string(event.table_number))) << endl;

        int id = event.id_event;
        string name = event.name_client;
        int time_event = event.time.toMinutes();
        switch (id)
        {
        case 1:                                                   // client comming
            if (time_event < time_start || time_event > time_end) // Проверка, что клиент пришёл в рабочее время
            {
                cout << format("{:02}:{:02} 13 NotOpenYet", event.time.hours, event.time.minutes) << endl;
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
            else if (tableStats[event.table_number].occupied != "None") // Проверка свободен ли стол
            {
                cout << format("{:02}:{:02} 13 PlaceIsBusy", event.time.hours, event.time.minutes) << endl;
            }
            else // Если стол свободен, то клиент садится
            {
                // А как обработать событие, что он пересаживается за свободный стол...
                // Тогда нужен поиск стола, за которым он сидел, вычисление платы (событие id 4)
                int table_number = findTable(tableStats, name); // Поиск номера стола по имени клиента в map tableStats
                if (table_number > 0)
                {
                    tableStats[table_number].work_time += (time_event - tableStats[table_number].time_table_occupied);
                    tableStats[table_number].money += data.coast * SpendHours(time_event - tableStats[table_number].time_table_occupied);
                    tableStats[table_number].occupied = "None";
                }
                else
                {
                    // Информация вносится в map tableStats
                    tableStats[event.table_number].occupied = name;
                    tableStats[event.table_number].time_table_occupied = event.time.toMinutes();
                }
            }
            break;
        case 3:                                 // waiting
            if (checkTableOccupied(tableStats)) // Свободные столы есть?
            {
                cout << format("{:02}:{:02} 13 ICanWaitNoLonger!", event.time.hours, event.time.minutes) << endl;
            }
            else if (fifo.size() + 1 > tableStats.size()) // Если я встану в очередь, она будет больше, чем количество столов
            {
                cout << format("{:02}:{:02} 11 {}", event.time.hours, event.time.minutes, name) << endl;
            }
            else // Добавляется в очередь
            {
                fifo.push(name);
            }
            break;
        case 4: // walk away from table
            if (!ClientList.count(name))
            {
                cout << format("{:02}:{:02} 13 ClientUnknown", event.time.hours, event.time.minutes) << endl;
            }
            else
            {
                int table_number = findTable(tableStats, name); // Поиск номера стола по имени клиента в map tableStats
                tableStats[table_number].work_time += (time_event - tableStats[table_number].time_table_occupied);
                tableStats[table_number].money += data.coast * SpendHours(time_event - tableStats[table_number].time_table_occupied);
                if (fifo.empty())
                {
                    tableStats[table_number].occupied = "None";
                }
                else
                {
                    tableStats[table_number].occupied = fifo.front();
                    tableStats[table_number].time_table_occupied = time_event;
                    cout << format("{:02}:{:02} 12 {} {}", event.time.hours, event.time.minutes, fifo.front(), table_number) << endl;
                    fifo.pop();
                }
            }
            break;
        }
    }
    // Вызвать событие 11, для всех клиентов, которые сейчас в ПК клубе ДО вывода времени закрытия
    int time_event = data.time_close.toMinutes();
    for (int table_number = 1; table_number <= data.count_of_table; table_number++)
    {
        if (tableStats[table_number].occupied != "None")
        {
            tableStats[table_number].work_time += (time_event - tableStats[table_number].time_table_occupied);
            tableStats[table_number].money += data.coast * SpendHours(time_event - tableStats[table_number].time_table_occupied);
            cout << format("{:02}:{:02} 11 {}", data.time_close.hours, data.time_close.minutes, tableStats[table_number].occupied) << endl;
            tableStats[table_number].occupied = "None";
        }
    }
    data.time_close.print();

    // Выводим информацию о столах
    for (const auto &[key, value] : tableStats)
    {
        cout << format("{} {} {:02}:{:02}", key, value.money, value.work_time / 60, value.work_time - (value.work_time / 60) * 60) << endl;
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