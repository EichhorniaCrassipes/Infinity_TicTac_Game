#include <iostream>
#include <string>
#include <limits>

using namespace std;

// максимальный размер поля 3х3
const int MAX_SIZE = 3;
// размер поля для третьего режима (4х4)
const int MAX_SIZE_4 = MAX_SIZE + 1;

// функция для вывода помощи
void print_help() {
    cout << endl << "=== HELP FOR THE GAME ===" << endl;
    cout << "Enter coordinates in the format: row column" << endl;
    cout << "Coordinates from 1 to " << MAX_SIZE << endl;
    cout << "Example: 1 2 - put the symbol in the first row, second column" << endl;
    cout << "Enter 0 0 to exit the game" << endl;
    cout << "Enter h for this help" << endl;
    cout << "========================" << endl << endl;
}

// функция для вывода помощи c произвольным размером поля
void print_help(int field_size) {
    cout << endl << "=== HELP FOR THE GAME ===" << endl;
    cout << "Enter coordinates in the format: row column" << endl;
    cout << "Coordinates from 1 to " << field_size << endl;
    cout << "Example: 1 2 - put the symbol in the first row, second column" << endl;
    cout << "Enter 0 0 to exit the game" << endl;
    cout << "Enter h for this help" << endl;
    cout << "========================" << endl << endl;
}

// функция для хода компьютера
void computerMove(string arr[][MAX_SIZE], string computerSymbol, string playerSymbol) {
    // cначала проверяем, может ли компьютер выиграть
    for (int i = 0; i < MAX_SIZE; ++i) {
        for (int j = 0; j < MAX_SIZE; ++j) {
            if (arr[i][j] == " ") {
                arr[i][j] = computerSymbol;
                // проверяем победу
                bool win = false;
                // проверка строк
                for (int k = 0; k < MAX_SIZE; ++k) {
                    if (arr[k][0] == arr[k][1] && arr[k][0] == arr[k][2] && arr[k][0] == computerSymbol) {
                        win = true;
                    }
                }
                // проверка столбцов
                for (int k = 0; k < MAX_SIZE; ++k) {
                    if (arr[0][k] == arr[1][k] && arr[0][k] == arr[2][k] && arr[0][k] == computerSymbol) {
                        win = true;
                    }
                }
                // проверка диагоналей
                if (arr[0][0] == arr[1][1] && arr[1][1] == arr[2][2] && arr[2][2] == computerSymbol) {
                    win = true;
                }
                if (arr[2][0] == arr[1][1] && arr[1][1] == arr[0][2] && arr[1][1] == computerSymbol) {
                    win = true;
                }
                if (win) {
                    return;
                }
                arr[i][j] = " ";
            }
        }
    }
    
    // потом проверяем, нужно ли блокировать игрока
    for (int i = 0; i < MAX_SIZE; ++i) {
        for (int j = 0; j < MAX_SIZE; ++j) {
            if (arr[i][j] == " ") {
                arr[i][j] = playerSymbol;
                // проверяем победу игрока
                bool block = false;
                // проверка строк
                for (int k = 0; k < MAX_SIZE; ++k) {
                    if (arr[k][0] == arr[k][1] && arr[k][0] == arr[k][2] && arr[k][0] == playerSymbol) {
                        block = true;
                    }
                }
                // проверка столбцов
                for (int k = 0; k < MAX_SIZE; ++k) {
                    if (arr[0][k] == arr[1][k] && arr[0][k] == arr[2][k] && arr[0][k] == playerSymbol) {
                        block = true;
                    }
                }
                // проверка диагоналей
                if (arr[0][0] == arr[1][1] && arr[1][1] == arr[2][2] && arr[2][2] == playerSymbol) {
                    block = true;
                }
                if (arr[2][0] == arr[1][1] && arr[1][1] == arr[0][2] && arr[1][1] == playerSymbol) {
                    block = true;
                }
                if (block) {
                    arr[i][j] = computerSymbol;
                    return;
                }
                arr[i][j] = " ";
            }
        }
    }
    
    // иначе выбираем лучшую позицию: центр, углы, края
    if (arr[1][1] == " ") {
        arr[1][1] = computerSymbol;
        return;
    }
    // углы
    if (arr[0][0] == " ") {
        arr[0][0] = computerSymbol;
        return;
    }
    if (arr[0][2] == " ") {
        arr[0][2] = computerSymbol;
        return;
    }
    if (arr[2][0] == " ") {
        arr[2][0] = computerSymbol;
        return;
    }
    if (arr[2][2] == " ") {
        arr[2][2] = computerSymbol;
        return;
    }
    // края
    for (int i = 0; i < MAX_SIZE; ++i) {
        for (int j = 0; j < MAX_SIZE; ++j) {
            if (arr[i][j] == " ") {
                arr[i][j] = computerSymbol;
                return;
            }
        }
    }
}

// функция проверки победы (3 в ряд) на поле 4х4
bool check_win_3_in_row_4(string arr[][MAX_SIZE_4], int field_size, const string &symbol) {
    // проверка строк
    for (int i = 0; i < field_size; ++i) {
        for (int j = 0; j <= field_size - 3; ++j) {
            if (arr[i][j] == symbol &&
                arr[i][j+1] == symbol &&
                arr[i][j+2] == symbol) {
                return true;
            }
        }
    }
    // проверка столбцов
    for (int j = 0; j < field_size; ++j) {
        for (int i = 0; i <= field_size - 3; ++i) {
            if (arr[i][j] == symbol &&
                arr[i+1][j] == symbol &&
                arr[i+2][j] == symbol) {
                return true;
            }
        }
    }
    // проверка диагоналей слева направо (вниз)
    for (int i = 0; i <= field_size - 3; ++i) {
        for (int j = 0; j <= field_size - 3; ++j) {
            if (arr[i][j] == symbol &&
                arr[i+1][j+1] == symbol &&
                arr[i+2][j+2] == symbol) {
                return true;
            }
        }
    }
    // проверка диагоналей справа налево (вверх)
    for (int i = 2; i < field_size; ++i) {
        for (int j = 0; j <= field_size - 3; ++j) {
            if (arr[i][j] == symbol &&
                arr[i-1][j+1] == symbol &&
                arr[i-2][j+2] == symbol) {
                return true;
            }
        }
    }
    return false;
}

// функция для хода компьютера на поле 4х4 (3 в ряд)
void computerMove4(string arr[][MAX_SIZE_4], string computerSymbol, string playerSymbol1, string playerSymbol2) {
    int field_size = MAX_SIZE_4;
    // cначала проверяем, может ли компьютер выиграть
    for (int i = 0; i < field_size; ++i) {
        for (int j = 0; j < field_size; ++j) {
            if (arr[i][j] == " ") {
                arr[i][j] = computerSymbol;
                if (check_win_3_in_row_4(arr, field_size, computerSymbol)) {
                    return;
                }
                arr[i][j] = " ";
            }
        }
    }
    
    // потом проверяем, нужно ли блокировать первого игрока
    for (int i = 0; i < field_size; ++i) {
        for (int j = 0; j < field_size; ++j) {
            if (arr[i][j] == " ") {
                arr[i][j] = playerSymbol1;
                if (check_win_3_in_row_4(arr, field_size, playerSymbol1)) {
                    arr[i][j] = computerSymbol;
                    return;
                }
                arr[i][j] = " ";
            }
        }
    }

    // потом проверяем, нужно ли блокировать второго игрока
    for (int i = 0; i < field_size; ++i) {
        for (int j = 0; j < field_size; ++j) {
            if (arr[i][j] == " ") {
                arr[i][j] = playerSymbol2;
                if (check_win_3_in_row_4(arr, field_size, playerSymbol2)) {
                    arr[i][j] = computerSymbol;
                    return;
                }
                arr[i][j] = " ";
            }
        }
    }

    // иначе ставим в первую свободную клетку
    for (int i = 0; i < field_size; ++i) {
        for (int j = 0; j < field_size; ++j) {
            if (arr[i][j] == " ") {
                arr[i][j] = computerSymbol;
                return;
            }
        }
    }
}

int main () {
    // выбор режима игры
    cout << "Choose game mode:" << endl;
    cout << "1 - Game against a person" << endl;
    cout << "2 - Game against the computer" << endl;
    cout << "3 - Two persons and AI on 4x4 field (3 in a row wins)" << endl;
    int gameMode;
    cin >> gameMode;
    while (gameMode < 1 || gameMode > 3) {
        cout << "Invalid choice. Enter 1, 2 or 3: ";
        cin.clear();
        cin.ignore(numeric_limits<streamsize>::max(), '\n');
        cin >> gameMode;
    }

    // третий режим: два человека и ИИ на поле 4х4
    if (gameMode == 3) {
        int field_size = MAX_SIZE_4;
        // создаем и заполняем массив 4х4 пустыми клетками
        string arr4[MAX_SIZE_4][MAX_SIZE_4];
        for (int i = 0; i < field_size; ++i) {
            for (int j = 0; j < field_size; ++j) {
                arr4[i][j] = " ";
            }
        }
        // инициализация поля, первый вывод
        for (int i = 0; i < field_size; ++i) {
            for (int j = 0; j < field_size; ++j) {
                cout << arr4[i][j];
                if (j != field_size - 1) {
                    cout << " | ";
                }
            }
            if (i != field_size - 1) {
                cout << endl << "____________" << endl;
            }
        }
        // переменная затычка для выхода
        char symbol3 = '?';
        // переменная для хранения шага игры
        int hod3 = 1;

        // ЦИКЛ ИГРЫ ДЛЯ РЕЖИМА 3
        while (symbol3 != '!') {
            int turn = (hod3 - 1) % 3; // 0 - первый игрок (x), 1 - второй игрок (o), 2 - ИИ (c)

            if (turn == 0) {
                cout << endl << "Step: " << hod3 << " Input place for x (player 1): " << endl << "Print 0 0 to break, h for help...";
                string input;
                cin >> input;
                if (input == "h" || input == "H") {
                    print_help(field_size);
                    continue;
                }
                int stroka, stolbec;
                try {
                    stroka = stoi(input);
                } catch (...) {
                    cout << endl << "Invalid input. Enter two integers or h for help." << endl;
                    cin.clear();
                    cin.ignore(numeric_limits<streamsize>::max(), '\n');
                    continue;
                }
                if (!(cin >> stolbec)) {
                    cout << endl << "Invalid input. Enter two integers." << endl;
                    cin.clear();
                    cin.ignore(numeric_limits<streamsize>::max(), '\n');
                    continue;
                }
                cout << endl;
                if (stroka == 0 && stolbec == 0) {
                    symbol3 = '!';
                    break;
                }
                if (stroka < 1 || stroka > field_size || stolbec < 1 || stolbec > field_size) {
                    cout << "Invalid coordinates. You need to enter a number from 1 to " << field_size << "." << endl;
                    continue;
                }
                if (arr4[stroka-1][stolbec-1] != " ") {
                    cout << "Cell is already taken, try again." << endl;
                    continue;
                }
                else {
                    arr4[stroka-1][stolbec-1] = "x";
                }
            }
            else if (turn == 1) {
                cout << endl << "Step: " << hod3 << " Input place for o (player 2): " << endl << "Print 0 0 to break, h for help...";
                string input;
                cin >> input;
                if (input == "h" || input == "H") {
                    print_help(field_size);
                    continue;
                }
                int stroka, stolbec;
                try {
                    stroka = stoi(input);
                } catch (...) {
                    cout << endl << "Invalid input. Enter two integers or h for help." << endl;
                    cin.clear();
                    cin.ignore(numeric_limits<streamsize>::max(), '\n');
                    continue;
                }
                if (!(cin >> stolbec)) {
                    cout << endl << "Invalid input. Enter two integers." << endl;
                    cin.clear();
                    cin.ignore(numeric_limits<streamsize>::max(), '\n');
                    continue;
                }
                cout << endl;
                if (stroka == 0 && stolbec == 0) {
                    symbol3 = '!';
                    break;
                }
                if (stroka < 1 || stroka > field_size || stolbec < 1 || stolbec > field_size) {
                    cout << "Invalid coordinates. You need to enter a number from 1 to " << field_size << "." << endl;
                    continue;
                }
                if (arr4[stroka-1][stolbec-1] != " ") {
                    cout << "Cell is already taken, try again." << endl;
                    continue;
                }
                else {
                    arr4[stroka-1][stolbec-1] = "o";
                }
            }
            else {
                cout << endl << "Step: " << hod3 << " Computer (c) is making a move..." << endl;
                computerMove4(arr4, "c", "x", "o");
            }

            // проверка победы для всех трех участников
            bool flag3 = false;
            if (check_win_3_in_row_4(arr4, field_size, "x")) {
                cout << endl << "X WON IN " << hod3 << " STEPS!" << endl;
                flag3 = true;
            }
            else if (check_win_3_in_row_4(arr4, field_size, "o")) {
                cout << endl << "O WON IN " << hod3 << " STEPS!" << endl;
                flag3 = true;
            }
            else if (check_win_3_in_row_4(arr4, field_size, "c")) {
                cout << endl << "C WON IN " << hod3 << " STEPS!" << endl;
                flag3 = true;
            }

            if (flag3) {
                symbol3 = '!';
                break;
            }

            if (hod3 == field_size * field_size) {
                cout << endl << "Draw!" << endl;
                symbol3 = '!';
                break;
            }

            // увеличиваем ход на 1
            ++hod3;
            // выводим поле
            for (int i = 0; i < field_size; ++i) {
                for (int j = 0; j < field_size; ++j) {
                    cout << arr4[i][j];
                    if (j != field_size - 1) {
                        cout << " | ";
                    }
                }
                if (i != field_size - 1) {
                    cout << endl << "____________" << endl;
                }
            }
        }
        // выводим поле в финале
        cout << endl;
        for (int i = 0; i < field_size; ++i) {
            for (int j = 0; j < field_size; ++j) {
                cout << arr4[i][j];
                if (j != field_size - 1) {
                    cout << " | ";
                }
            }
            if (i != field_size - 1) {
                cout << endl << "____________" << endl;
            }
        }
        return 0;
    }
    
    // создаем и заполняем массив 3х3 пустыми клетками
    string arr[MAX_SIZE][MAX_SIZE];
    for(int i = 0; i < MAX_SIZE; ++i) {
        for (int j = 0; j < MAX_SIZE; ++j) {
            arr[i][j] = " ";
        }
    }
    // инициализация поля, первый вывод
    for(int i = 0; i < MAX_SIZE; ++i) {
        for (int j = 0; j < MAX_SIZE; ++j) {
            cout << arr[i][j];
            if (j!=2){
                cout << " | ";
            }
        }
        if(i!=2){
            cout << endl << "__________" << endl;
        }
        
    }
    // переменная затычка надо ее на что-то поменять, потом обыграть как-то
    char symbol = '?';
    // переменная для хранения шага игры
    int hod = 1;
    
    // ЦИКЛ ИГРЫ
    while (symbol != '!') {

        // если четный ход - то ходит нолик
        if (hod%2==0) {
            // в режиме против компьютера компьютер играет ноликами
            if (gameMode == 2) {
                cout << endl << "Step: " << hod << " Computer (o) is making a move..." << endl;
                computerMove(arr, "o", "x");
            }
            else {
                cout << endl << "Step: " << hod << " Input place for o: " << endl << "Print 0 0 to break, h for help...";
                string input;
                cin >> input;
                if (input == "h" || input == "H") {
                    print_help();
                    continue;
                }
                int stroka, stolbec;
                try {
                    stroka = stoi(input);
                } catch (...) {
                    cout << endl << "Invalid input. Enter two integers or h for help." << endl;
                    cin.clear();
                    cin.ignore(numeric_limits<streamsize>::max(), '\n');
                    continue;
                }
                if (!(cin >> stolbec)) {
                    cout << endl << "Invalid input. Enter two integers." << endl;
                    cin.clear();
                    cin.ignore(numeric_limits<streamsize>::max(), '\n');
                    continue;
                }
                cout << endl;
                if (stroka == 0 && stolbec == 0) {
                    symbol = '!';
                    break;
                }
                if (stroka < 1 || stroka > MAX_SIZE || stolbec < 1 || stolbec > MAX_SIZE) {
                    cout << "Invalid coordinates. You need to enter a number from 1 to " << MAX_SIZE << "." << endl;
                    continue;
                }
                if (arr[stroka-1][stolbec-1] != " ") {
                    cout << "Cell is already taken, try again." << endl;
                    continue;
                }
                else {
                    arr[stroka-1][stolbec-1] = "o";
                }
            }
        }
        // нечетный ход - ходит крестик
        else {
            cout << endl << "Step: " << hod << " Input place for x: " << endl << "Print 0 0 to break, h for help...";
            string input;
            cin >> input;
            if (input == "h" || input == "H") {
                print_help();
                continue;
            }
            int stroka, stolbec;
            try {
                stroka = stoi(input);
            } catch (...) {
                cout << endl << "Invalid input. Enter two integers or h for help." << endl;
                cin.clear();
                cin.ignore(numeric_limits<streamsize>::max(), '\n');
                continue;
            }
            if (!(cin >> stolbec)) {
                cout << endl << "Invalid input. Enter two integers." << endl;
                cin.clear();
                cin.ignore(numeric_limits<streamsize>::max(), '\n');
                continue;
            }
            cout << endl;
            if (stroka == 0 && stolbec == 0) {
                symbol = '!';
                break;
            }
            if (stroka < 1 || stroka > MAX_SIZE || stolbec < 1 || stolbec > MAX_SIZE) {
                cout << "Invalid coordinates. You need to enter a number from 1 to " << MAX_SIZE << "." << endl;
                continue;
            }
            if (arr[stroka-1][stolbec-1] != " ") {
                cout << "Cell is already taken, try again." << endl;
                continue;
            }
            else {
                arr[stroka-1][stolbec-1] = "x";
            }
        }

        bool flag = false;
        for (int i = 0; i < 3; ++i) {

            if (arr[i][0] == arr[i][1] && arr[i][0] == arr[i][2] && arr[i][0] == "x"){
                cout << endl << "X WON IN " << hod << " STEPS!" << endl;
                flag = true;
            }
            else if (arr[i][0] == arr[i][1] && arr[i][0] == arr[i][2] && arr[i][0] == "o"){
                cout << endl << "O WON IN " << hod << " STEPS!" << endl;
                flag = true;
            }
        }
        for (int i = 0; i < 3; ++i) {
            if (arr[0][i] == arr[1][i] && arr[0][i] == arr[2][i] && arr[0][i] == "x"){
                cout << endl << "X WON IN " << hod << " STEPS!" << endl;
                flag = true;
            }
            else if (arr[0][i] == arr[1][i] && arr[0][i] == arr[2][i] && arr[0][i] == "o"){
                cout << endl << "O WON IN " << hod << " STEPS!" << endl;
                flag = true;
            }
        }
            if (arr[0][0] == arr[1][1] && arr[1][1] == arr[2][2] && arr[2][2] == "x"){
                cout << endl << "X WON IN " << hod << " STEPS!" << endl;
                flag = true;
            }
            else if (arr[0][0] == arr[1][1] && arr[1][1] == arr[2][2] && arr[2][2] == "o"){
                cout << endl << "O WON IN " << hod << " STEPS!" << endl;
                flag = true;
            }
        
            if (arr[2][0] == arr[1][1] && arr[1][1] == arr[0][2] && arr[1][1] == "x"){
                cout << endl << "X WON IN " << hod << " STEPS!" << endl;
                flag = true;
            }
            else if (arr[2][0] == arr[1][1] && arr[1][1] == arr[0][2] && arr[1][1] == "o"){
                cout << endl << "O WON IN " << hod << " STEPS!" << endl;
                flag = true;
            }
        if (flag) {
            symbol = '!';
            break;
        }

        if (hod == MAX_SIZE * MAX_SIZE) {
            cout << endl << "Draw!" << endl;
            symbol = '!';
            break;
        }
        // увеличиваем ход на 1
        ++hod;
        // выводим поле
        for(int i = 0; i < MAX_SIZE; ++i) {
            for (int j = 0; j < MAX_SIZE; ++j) {
                cout << arr[i][j];
                if (j!=2){
                    cout << " | ";
                }
            }
            if(i!=2){
                cout << endl << "__________" << endl;
            }
        
        }
    }
    // Выводим поле в финале
    for(int i = 0; i < MAX_SIZE; ++i) {
        for (int j = 0; j < MAX_SIZE; ++j) {
            cout << arr[i][j];
            if (j!=2){
                cout << " | ";
            }
        }
        if(i!=2){
            cout << endl << "__________" << endl;
        }
    }
    return 0;
}