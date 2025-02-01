#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>
#include <random>
#include <ctime>

using namespace std;

vector<vector<int>> ReadFile(string filepath)
{
    vector<vector<int>> matrix;
    string line;
    ifstream file(filepath);

    if (!file.is_open())
    {
        cout << "Error, file dont open!" << endl;
    }
    while (getline(file, line))
    {
        istringstream ss(line);
        vector<int> row;
        int num;
        while (ss >> num)
            row.push_back(num);
        if (!row.empty())
            matrix.push_back(row);
    }
    file.close();
    return matrix;
}

vector<vector<int>> GetGeneratingMatrix(vector<vector<int>> matrix, int coloms)
{
    vector<vector<int>> generating_matrix;

    for (const auto &row : matrix)
    {
        vector<int> line;
        int count = 0;
        for (int num : row)
        {
            if (count >= coloms)
                continue;
            line.push_back(num);
            count++;
        }
        if (!line.empty())
            generating_matrix.push_back(line);
    }
    return generating_matrix;
}

// Useless func, needs only for test and visual
// ---------------------------------------------------------
void PrintMatrix(const vector<vector<int>> &matrix)
{
    for (const auto &row : matrix)
    {
        for (int num : row)
        {
            cout << num << " ";
        }
        cout << endl;
    }
}

void PrintVector(const vector<int> &vector)
{
    for (int num = 0; num < vector.size(); num++)
        cout << vector[num] << " ";
    cout << endl;
}
// ---------------------------------------------------------

vector<int> GenerateData(int length)
{
    vector<int> data(length);
    for (int num = 0; num < length; num++)
        data[num] = rand() % 2;
    return data;
}

// AddNoise()
// EncodeData()
// DecodeData()

int main()
{
    srand(time(nullptr));
    string filepath = "matrix_A.txt";
    vector<vector<int>> matrix_A = ReadFile(filepath);
    auto matrix_B = GetGeneratingMatrix(matrix_A, 4);
    auto di = GenerateData(20);

    PrintMatrix(matrix_B);
    PrintVector(di);

    return 0;
}