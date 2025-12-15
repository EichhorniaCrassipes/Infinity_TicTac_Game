#include <SFML/Graphics.hpp>
#include <vector>
#include <cmath>
#include <iostream>
#include <algorithm>
#include <string>
#include <limits>
#include <filesystem>

namespace fs = std::filesystem;

using namespace sf;
using namespace std;

// =============== КОНСТАНТЫ И ПЕРЕЧИСЛЕНИЯ ===============
const int MAX_SIZE = 3; // размер поля для режимов 1 и 2
const int MAX_SIZE_4 = 4; // размер поля для режима 3
const int CELL_SIZE = 80; // размер клетки в пикселях
const int WINDOW_WIDTH = 800; // ширина окна
const int WINDOW_HEIGHT = 650; // высота окна

enum class GameState {
    MENU,
    GAME_MODE_1,// 3x3 человеки с распознаванием жестов
    GAME_MODE_2,// 3x3 комп и человек
    GAME_MODE_3,// 4x4 человеки и комп
    GAME_OVER
};

enum class GestureType {
    None,
    Cross,   // Крестик
    Circle   // Нолик
};

// =============== ВСПОМОГАТЕЛЬНЫЕ ФУНКЦИИ ДЛЯ ИИ 4x4 ===============
bool checkWin3InRowHelper(string arr[][MAX_SIZE_4], int size, const string& symbol) {
    // проверка строк
    for (int i = 0; i < size; ++i) {
        for (int j = 0; j <= size - 3; ++j) {
            if (arr[i][j] == symbol && arr[i][j+1] == symbol && arr[i][j+2] == symbol) {
                return true;
            }
        }
    }
    // проверка столбцов
    for (int j = 0; j < size; ++j) {
        for (int i = 0; i <= size - 3; ++i) {
            if (arr[i][j] == symbol && arr[i+1][j] == symbol && arr[i+2][j] == symbol) {
                return true;
            }
        }
    }
    // проверка диагоналей слева направо (вниз)
    for (int i = 0; i <= size - 3; ++i) {
        for (int j = 0; j <= size - 3; ++j) {
            if (arr[i][j] == symbol && arr[i+1][j+1] == symbol && arr[i+2][j+2] == symbol) {
                return true;
            }
        }
    }
    // проверка диагоналей справа налево (вверх)
    for (int i = 2; i < size; ++i) {
        for (int j = 0; j <= size - 3; ++j) {
            if (arr[i][j] == symbol && arr[i-1][j+1] == symbol && arr[i-2][j+2] == symbol) {
                return true;
            }
        }
    }
    return false;
}

// =============== СТРУКТУРЫ ДАННЫХ ===============
struct GameBoard3x3 {
    string board[MAX_SIZE][MAX_SIZE];
    int turn;
    bool gameEnded;
    string winner;
    
    GameBoard3x3() {
        reset();
    }
    
    void reset() {
        for (int i = 0; i < MAX_SIZE; ++i) {
            for (int j = 0; j < MAX_SIZE; ++j) {
                board[i][j] = " ";
            }
        }
        turn = 1;  // нечетный ход - крестик
        gameEnded = false;
        winner = "";
    }
    
    bool isCellEmpty(int row, int col) {
        return board[row][col] == " ";
    }
    
    void makeMove(int row, int col, const string& symbol) {
        if (row >= 0 && row < MAX_SIZE && col >= 0 && col < MAX_SIZE && isCellEmpty(row, col)) {
            board[row][col] = symbol;
        }
    }
    
    bool checkWin(const string& symbol) {
        // проверка строк
        for (int i = 0; i < MAX_SIZE; ++i) {
            if (board[i][0] == symbol && board[i][1] == symbol && board[i][2] == symbol) {
                return true;
            }
        }
        // проверка столбцов
        for (int j = 0; j < MAX_SIZE; ++j) {
            if (board[0][j] == symbol && board[1][j] == symbol && board[2][j] == symbol) {
                return true;
            }
        }
        // проверка диагоналей
        if (board[0][0] == symbol && board[1][1] == symbol && board[2][2] == symbol) {
            return true;
        }
        if (board[2][0] == symbol && board[1][1] == symbol && board[0][2] == symbol) {
            return true;
        }
        return false;
    }
    
    bool isBoardFull() {
        for (int i = 0; i < MAX_SIZE; ++i) {
            for (int j = 0; j < MAX_SIZE; ++j) {
                if (board[i][j] == " ") {
                    return false;
                }
            }
        }
        return true;
    }
};

struct GameBoard4x4 {
    string board[MAX_SIZE_4][MAX_SIZE_4];
    int turn;
    bool gameEnded;
    string winner;
    
    GameBoard4x4() {
        reset();
    }
    
    void reset() {
        for (int i = 0; i < MAX_SIZE_4; ++i) {
            for (int j = 0; j < MAX_SIZE_4; ++j) {
                board[i][j] = " ";
            }
        }
        turn = 1;  // 0 - x, 1 - o, 2 - c
        gameEnded = false;
        winner = "";
    }
    
    bool isCellEmpty(int row, int col) {
        return board[row][col] == " ";
    }
    
    void makeMove(int row, int col, const string& symbol) {
        if (row >= 0 && row < MAX_SIZE_4 && col >= 0 && col < MAX_SIZE_4 && isCellEmpty(row, col)) {
            board[row][col] = symbol;
        }
    }
    
    bool checkWin3InRow(const string& symbol) {
        return checkWin3InRowHelper(board, MAX_SIZE_4, symbol);
    }
    
    bool isBoardFull() {
        for (int i = 0; i < MAX_SIZE_4; ++i) {
            for (int j = 0; j < MAX_SIZE_4; ++j) {
                if (board[i][j] == " ") {
                    return false;
                }
            }
        }
        return true;
    }
};

// =============== ФУНКЦИИ ДЛЯ ИИ ===============
void computerMove3x3(string arr[][MAX_SIZE], string computerSymbol, string playerSymbol) {
    // сначала проверяем, может ли компьютер выиграть
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

void computerMove4x4(string arr[][MAX_SIZE_4], string computerSymbol, string playerSymbol1, string playerSymbol2) {
    int field_size = MAX_SIZE_4;
    // сначала проверяем, может ли компьютер выиграть
    for (int i = 0; i < field_size; ++i) {
        for (int j = 0; j < field_size; ++j) {
            if (arr[i][j] == " ") {
                arr[i][j] = computerSymbol;
                // простая проверка на победу
                if (checkWin3InRowHelper(arr, field_size, computerSymbol)) {
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
                if (checkWin3InRowHelper(arr, field_size, playerSymbol1)) {
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
                if (checkWin3InRowHelper(arr, field_size, playerSymbol2)) {
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

// =============== ФУНКЦИИ РАСПОЗНАВАНИЯ ЖЕСТОВ ===============
GestureType recognizeGesture(const std::vector<Vector2f>& points, FloatRect cellBounds) {
    if (points.size() < 10) return GestureType::None; // Слишком мало точек

    // Преобразуем координаты в локальные (от 0 до 1 внутри ячейки)
    std::vector<Vector2f> localPoints;
    for (const auto& p : points) {
        float localX = (p.x - cellBounds.left) / cellBounds.width;
        float localY = (p.y - cellBounds.top) / cellBounds.height;
        localPoints.emplace_back(localX, localY);
    }

    // === ПРОВЕРКА НА НОЛИК (CIRCLE) ===
    // 1. Проверяем, замкнута ли траектория (начало и конец близко)
    float distStartToEnd = std::hypot(
        localPoints.front().x - localPoints.back().x,
        localPoints.front().y - localPoints.back().y
    );
    bool isClosed = (distStartToEnd < 0.3f); // 30% от размера ячейки

    // 2. Проверяем круглость: точки должны быть на расстоянии от центра
    int pointsOnPerimeter = 0;
    for (const auto& p : localPoints) {
        float distToCenter = std::hypot(p.x - 0.5f, p.y - 0.5f);
        // Точки должны быть не слишком близко к центру и не за пределами
        if (distToCenter > 0.25f && distToCenter < 0.6f) {
            pointsOnPerimeter++;
        }
    }
    bool looksLikeCircle = isClosed && (pointsOnPerimeter > localPoints.size() * 0.6f);

    if (looksLikeCircle) {
        return GestureType::Circle;
    }

    // ПРОВЕРКА НА КРЕСТИК (CROSS)
    // Проверяем наличие точек в углах
    bool topLeft = false, bottomRight = false;
    bool topRight = false, bottomLeft = false;

    for (const auto& p : localPoints) {
        if (p.x < 0.3f && p.y < 0.3f) topLeft = true; // верхний левый
        if (p.x > 0.7f && p.y > 0.7f) bottomRight = true;// нижний правый
        if (p.x > 0.7f && p.y < 0.3f) topRight = true; // верхний правый
        if (p.x < 0.3f && p.y > 0.7f) bottomLeft = true;// нижний левый
    }

    // Если есть точки в противоположных углах - это крестик
    if ((topLeft && bottomRight) || (topRight && bottomLeft)) {
        if (points.size() > 15) {
            return GestureType::Cross;
        }
    }

    return GestureType::None;
}

// =============== ВСПОМОГАТЕЛЬНЫЕ ФУНКЦИИ И ТД===============
void drawGrid(RenderWindow& window, int size, int startX, int startY) {
    for (int i = 0; i <= size; ++i) {
        // вертикальные линии
        Vertex line1[] = {
            Vertex(Vector2f(startX + i * CELL_SIZE, startY), Color::Black),
            Vertex(Vector2f(startX + i * CELL_SIZE, startY + size * CELL_SIZE), Color::Black)
        };
        window.draw(line1, 2, Lines);
        
        // горизонтальные линии
        Vertex line2[] = {
            Vertex(Vector2f(startX, startY + i * CELL_SIZE), Color::Black),
            Vertex(Vector2f(startX + size * CELL_SIZE, startY + i * CELL_SIZE), Color::Black)
        };
        window.draw(line2, 2, Lines);
    }
}

void drawSymbols(RenderWindow& window, const GameBoard3x3& game, int startX, int startY) {
    for (int i = 0; i < MAX_SIZE; ++i) {
        for (int j = 0; j < MAX_SIZE; ++j) {
            if (game.board[i][j] == "x") {
                // крестик
                Vertex line1[] = {
                    Vertex(Vector2f(startX + j * CELL_SIZE + 15, startY + i * CELL_SIZE + 15), Color::Red),
                    Vertex(Vector2f(startX + j * CELL_SIZE + CELL_SIZE - 15, startY + i * CELL_SIZE + CELL_SIZE - 15), Color::Red)
                };
                window.draw(line1, 2, Lines);
                
                Vertex line2[] = {
                    Vertex(Vector2f(startX + j * CELL_SIZE + 15, startY + i * CELL_SIZE + CELL_SIZE - 15), Color::Red),
                    Vertex(Vector2f(startX + j * CELL_SIZE + CELL_SIZE - 15, startY + i * CELL_SIZE + 15), Color::Red)
                };
                window.draw(line2, 2, Lines);
            }
            else if (game.board[i][j] == "o") {
                // нолик
                CircleShape circle(CELL_SIZE / 2 - 15);
                circle.setOutlineColor(Color::Blue);
                circle.setOutlineThickness(2);
                circle.setFillColor(Color::Transparent);
                circle.setPosition(startX + j * CELL_SIZE + 15, startY + i * CELL_SIZE + 15);
                window.draw(circle);
            }
        }
    }
}

void drawSymbols4x4(RenderWindow& window, const GameBoard4x4& game, int startX, int startY) {
    for (int i = 0; i < MAX_SIZE_4; ++i) {
        for (int j = 0; j < MAX_SIZE_4; ++j) {
            if (game.board[i][j] == "x") {
                // крестик
                Vertex line1[] = {
                    Vertex(Vector2f(startX + j * CELL_SIZE + 15, startY + i * CELL_SIZE + 15), Color::Red),
                    Vertex(Vector2f(startX + j * CELL_SIZE + CELL_SIZE - 15, startY + i * CELL_SIZE + CELL_SIZE - 15), Color::Red)
                };
                window.draw(line1, 2, Lines);
                
                Vertex line2[] = {
                    Vertex(Vector2f(startX + j * CELL_SIZE + 15, startY + i * CELL_SIZE + CELL_SIZE - 15), Color::Red),
                    Vertex(Vector2f(startX + j * CELL_SIZE + CELL_SIZE - 15, startY + i * CELL_SIZE + 15), Color::Red)
                };
                window.draw(line2, 2, Lines);
            }
            else if (game.board[i][j] == "o") {
                // нолик
                CircleShape circle(CELL_SIZE / 2 - 15);
                circle.setOutlineColor(Color::Blue);
                circle.setOutlineThickness(2);
                circle.setFillColor(Color::Transparent);
                circle.setPosition(startX + j * CELL_SIZE + 15, startY + i * CELL_SIZE + 15);
                window.draw(circle);
            }
            else if (game.board[i][j] == "c") {
                // компьютер (зеленый крестик)
                Vertex line1[] = {
                    Vertex(Vector2f(startX + j * CELL_SIZE + 15, startY + i * CELL_SIZE + 15), Color::Green),
                    Vertex(Vector2f(startX + j * CELL_SIZE + CELL_SIZE - 15, startY + i * CELL_SIZE + CELL_SIZE - 15), Color::Green)
                };
                window.draw(line1, 2, Lines);
                
                Vertex line2[] = {
                    Vertex(Vector2f(startX + j * CELL_SIZE + 15, startY + i * CELL_SIZE + CELL_SIZE - 15), Color::Green),
                    Vertex(Vector2f(startX + j * CELL_SIZE + CELL_SIZE - 15, startY + i * CELL_SIZE + 15), Color::Green)
                };
                window.draw(line2, 2, Lines);
            }
        }
    }
}

// =============== КЛАСС ДЛЯ КНОПОК МЕНЮ ===============
class MenuButton {
private:
    RectangleShape rect;
    Text text;
    bool hovered = false;

public:
    MenuButton(float x, float y, float width, float height, const string& buttonText, Font& font) {
        rect.setSize(Vector2f(width, height));
        rect.setPosition(x, y);
        rect.setFillColor(Color(70, 130, 180)); // steel blue
        rect.setOutlineThickness(2);
        rect.setOutlineColor(Color(30, 70, 120));
        
        text.setFont(font);
        text.setString(buttonText);
        text.setCharacterSize(24);
        text.setFillColor(Color::White);
        text.setStyle(Text::Bold);
        
        // Центрируем текст в кнопке
        FloatRect textBounds = text.getLocalBounds();
        text.setOrigin(textBounds.left + textBounds.width / 2.0f,
                      textBounds.top + textBounds.height / 2.0f);
        text.setPosition(x + width / 2.0f, y + height / 2.0f);
    }
    
    bool isMouseOver(const Vector2i& mousePos) {
        hovered = rect.getGlobalBounds().contains(static_cast<Vector2f>(mousePos));
        return hovered;
    }
    
    void draw(RenderWindow& window) {
        // Обновляем цвета в зависимости от состояния наведения
        if (hovered) {
            rect.setFillColor(Color(90, 150, 200)); // светлее при наведении
            rect.setOutlineColor(Color(50, 90, 140));
            text.setFillColor(Color(255, 255, 220));
        } else {
            rect.setFillColor(Color(70, 130, 180));
            rect.setOutlineColor(Color(30, 70, 120));
            text.setFillColor(Color::White);
        }
        
        window.draw(rect);
        window.draw(text);
    }
    
    FloatRect getBounds() const {
        return rect.getGlobalBounds();
    }
    
    void setHovered(bool isHovered) {
        hovered = isHovered;
    }
};

// =============== ФУНКЦИЯ ЗАГРУЗКИ ШРИФТА ===============
Font loadFont() {
    Font font;
    
    // Пути к шрифтам в порядке приоритета
    vector<string> fontPaths = {
        "/System/Library/Fonts/Supplemental/Arial Unicode.ttf",  // macOS
        "/System/Library/Fonts/Helvetica.ttc",                      // macOS
        "C:/Windows/Fonts/arial.ttf",                               // Windows
        "C:/Windows/Fonts/times.ttf",                               // Windows
        "/usr/share/fonts/truetype/dejavu/DejaVuSans.ttf",          // Linux
        "arial.ttf",                                                // Текущая папка
        "font.ttf"                                                  // Резервный вариант
    };
    
    for (const auto& path : fontPaths) {
        if (fs::exists(path)) {
            if (font.loadFromFile(path)) {
                cout << "[logs]Font loaded successfully from: " << path << endl;
                return font;
            } else {
                cout << "[logs]Failed to load font from: " << path << endl;
            }
        }
    }
    
    // Если ни один шрифт не загрузился, пробуем создать минимальный шрифт
    cout << "[logs]Could not load any font files. Creating minimal font..." << endl;
    
    // Создаем минимальный шрифт из памяти (8x8 пикселей)
    unsigned char pixelData[] = {
        0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
        0xFF, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xFF,
        0xFF, 0x00, 0xFF, 0xFF, 0xFF, 0xFF, 0x00, 0xFF,
        0xFF, 0x00, 0xFF, 0x00, 0x00, 0xFF, 0x00, 0xFF,
        0xFF, 0x00, 0xFF, 0x00, 0x00, 0xFF, 0x00, 0xFF,
        0xFF, 0x00, 0xFF, 0xFF, 0xFF, 0xFF, 0x00, 0xFF,
        0xFF, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xFF,
        0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF
    };
    
    Image img;
    img.create(8, 8, pixelData);
    Texture tex;
    tex.loadFromImage(img);
    
    if (!font.loadFromMemory(pixelData, sizeof(pixelData))) {
        cerr << "[logs]Failed to create minimal font!" << endl;
        exit(1);
    }
    
    cout << "[logs]Minimal font created successfully" << endl;
    return font;
}

// =============== ОСНОВНАЯ ФУНКЦИЯ ===============
int main() {
    // Создаем окно
    RenderWindow window(VideoMode(WINDOW_WIDTH, WINDOW_HEIGHT), "Infinity Tic-Tac Game (beta 0.1)");
    window.setFramerateLimit(60);
    
    // Загружаем шрифт
    Font font = loadFont();
    
    // Состояние игры
    GameState currentState = GameState::MENU;
    
    // Игровые доски
    GameBoard3x3 game3x3;
    GameBoard4x4 game4x4;
    
    // Позиции для рисования
    const int GRID_START_X_3x3 = (WINDOW_WIDTH - MAX_SIZE * CELL_SIZE) / 2;
    const int GRID_START_Y_3x3 = 100;
    
    const int GRID_START_X_4x4 = (WINDOW_WIDTH - MAX_SIZE_4 * CELL_SIZE) / 2;
    const int GRID_START_Y_4x4 = 80;
    
    // Переменные для распознавания жестов (только для режима 1)
    bool isDrawing = false;
    std::vector<Vector2f> currentStroke;
    std::vector<Vector2f> recognizedStroke;
    GestureType lastGesture = GestureType::None;
    int currentCellRow = -1;
    int currentCellCol = -1;
    
    // Кнопки меню
    vector<MenuButton> menuButtons;
    float buttonWidth = 700;
    float buttonHeight = 60;
    float buttonX = (WINDOW_WIDTH - buttonWidth) / 2;
    
    menuButtons.push_back(MenuButton(buttonX, 200, buttonWidth, buttonHeight, "Human vs Human (drawing)", font));
    menuButtons.push_back(MenuButton(buttonX, 280, buttonWidth, buttonHeight, "Human vs AI (3x3 field)", font));
    menuButtons.push_back(MenuButton(buttonX, 360, buttonWidth, buttonHeight, "Human vs Human vs AI (4x4 field)", font));
    menuButtons.push_back(MenuButton(buttonX, 440, buttonWidth, buttonHeight, "Exit Game", font));
    
    // Основной цикл игры
    while (window.isOpen()) {
        Event event;
        while (window.pollEvent(event)) {
            if (event.type == Event::Closed) {
                window.close();
            }
            
            // Обработка событий в меню
            if (currentState == GameState::MENU) {
                if (event.type == Event::MouseButtonPressed && event.mouseButton.button == Mouse::Left) {
                    Vector2i mousePos = Mouse::getPosition(window);
                    
                    for (size_t i = 0; i < menuButtons.size(); ++i) {
                        if (menuButtons[i].isMouseOver(mousePos)) {
                            if (i == 0) {
                                currentState = GameState::GAME_MODE_1;
                                game3x3.reset();
                                cout << "[logs]Starting Mode 1: 3x3 vs Human with gesture recognition" << endl;
                            }
                            else if (i == 1) {
                                currentState = GameState::GAME_MODE_2;
                                game3x3.reset();
                                cout << "[logs]Starting Mode 2: 3x3 vs Computer" << endl;
                            }
                            else if (i == 2) {
                                currentState = GameState::GAME_MODE_3;
                                game4x4.reset();
                                cout << "[logs]Starting Mode 3: 4x4 (3 in a row)" << endl;
                            }
                            else if (i == 3) {
                                window.close();
                            }
                            break;
                        }
                    }
                }
            }
            
            // Обработка событий в игре
            if (currentState == GameState::GAME_MODE_1) {
                // Режим 1: распознавание жестов
                if (event.type == Event::MouseButtonPressed && event.mouseButton.button == Mouse::Left) {
                    Vector2i mousePos = Mouse::getPosition(window);
                    
                    // Проверяем, кликнули ли внутри игрового поля
                    FloatRect gridBounds(GRID_START_X_3x3, GRID_START_Y_3x3, MAX_SIZE * CELL_SIZE, MAX_SIZE * CELL_SIZE);
                    if (gridBounds.contains(static_cast<Vector2f>(mousePos))) {
                        // Определяем, в какой клетке кликнули
                        currentCellCol = (mousePos.x - GRID_START_X_3x3) / CELL_SIZE;
                        currentCellRow = (mousePos.y - GRID_START_Y_3x3) / CELL_SIZE;
                        
                        // Проверяем, что клетка пустая
                        if (game3x3.isCellEmpty(currentCellRow, currentCellCol)) {
                            isDrawing = true;
                            currentStroke.clear();
                            currentStroke.push_back(static_cast<Vector2f>(mousePos));
                        }
                    }
                }
                
                if (event.type == Event::MouseMoved && isDrawing) {
                    Vector2i mousePos = Mouse::getPosition(window);
                    currentStroke.push_back(static_cast<Vector2f>(mousePos));
                }
                
                if (event.type == Event::MouseButtonReleased && event.mouseButton.button == Mouse::Left) {
                    if (isDrawing && currentCellRow >= 0 && currentCellRow < MAX_SIZE && currentCellCol >= 0 && currentCellCol < MAX_SIZE) {
                        isDrawing = false;
                        
                        // Определяем границы текущей клетки
                        FloatRect cellBounds(
                            GRID_START_X_3x3 + currentCellCol * CELL_SIZE,
                            GRID_START_Y_3x3 + currentCellRow * CELL_SIZE,
                            CELL_SIZE,
                            CELL_SIZE
                        );
                        
                        // Распознаем жест
                        lastGesture = recognizeGesture(currentStroke, cellBounds);
                        recognizedStroke = currentStroke;
                        currentStroke.clear();
                        
                        // Выводим результат в консоль
                        cout << "\n[logs]=== RECOGNITION RESULT ===" << endl;
                        switch (lastGesture) {
                            case GestureType::Cross:
                                cout << "Recognized: CROSS (X)" << endl;
                                // Ход крестика (нечетный ход)
                                if (game3x3.turn % 2 == 1 && game3x3.isCellEmpty(currentCellRow, currentCellCol)) {
                                    game3x3.makeMove(currentCellRow, currentCellCol, "x");
                                    if (game3x3.checkWin("x")) {
                                        game3x3.gameEnded = true;
                                        game3x3.winner = "x";
                                        currentState = GameState::GAME_OVER;
                                    }
                                    else if (game3x3.isBoardFull()) {
                                        game3x3.gameEnded = true;
                                        game3x3.winner = "draw";
                                        currentState = GameState::GAME_OVER;
                                    }
                                    else {
                                        game3x3.turn++;
                                    }
                                }
                                break;
                            case GestureType::Circle:
                                cout << "Recognized: CIRCLE (O)" << endl;
                                // Ход нолика (четный ход)
                                if (game3x3.turn % 2 == 0 && game3x3.isCellEmpty(currentCellRow, currentCellCol)) {
                                    game3x3.makeMove(currentCellRow, currentCellCol, "o");
                                    if (game3x3.checkWin("o")) {
                                        game3x3.gameEnded = true;
                                        game3x3.winner = "o";
                                        currentState = GameState::GAME_OVER;
                                    }
                                    else if (game3x3.isBoardFull()) {
                                        game3x3.gameEnded = true;
                                        game3x3.winner = "draw";
                                        currentState = GameState::GAME_OVER;
                                    }
                                    else {
                                        game3x3.turn++;
                                    }
                                }
                                break;
                            default:
                                cout << "Recognized: UNKNOWN GESTURE" << endl;
                                break;
                        }
                        cout << "Points recorded: " << recognizedStroke.size() << endl;
                        cout << "=========================" << endl;
                        
                        currentCellRow = -1;
                        currentCellCol = -1;
                    }
                }
                
                // Клавиша R для перезапуска
                if (event.type == Event::KeyPressed && event.key.code == Keyboard::R) {
                    game3x3.reset();
                    recognizedStroke.clear();
                    lastGesture = GestureType::None;
                    cout << "[logs]Game restarted!" << endl;
                }
            }
            
            if (currentState == GameState::GAME_MODE_2) {
                // Режим 2: игра против компьютера
                if (event.type == Event::MouseButtonPressed && event.mouseButton.button == Mouse::Left) {
                    Vector2i mousePos = Mouse::getPosition(window);
                    
                    // Проверяем, кликнули ли внутри игрового поля
                    FloatRect gridBounds(GRID_START_X_3x3, GRID_START_Y_3x3, MAX_SIZE * CELL_SIZE, MAX_SIZE * CELL_SIZE);
                    if (gridBounds.contains(static_cast<Vector2f>(mousePos))) {
                        int col = (mousePos.x - GRID_START_X_3x3) / CELL_SIZE;
                        int row = (mousePos.y - GRID_START_Y_3x3) / CELL_SIZE;
                        
                        // Ход игрока (крестик, нечетный ход)
                        if (game3x3.turn % 2 == 1 && game3x3.isCellEmpty(row, col)) {
                            game3x3.makeMove(row, col, "x");
                            
                            if (game3x3.checkWin("x")) {
                                game3x3.gameEnded = true;
                                game3x3.winner = "x";
                                currentState = GameState::GAME_OVER;
                            }
                            else if (game3x3.isBoardFull()) {
                                game3x3.gameEnded = true;
                                game3x3.winner = "draw";
                                currentState = GameState::GAME_OVER;
                            }
                            else {
                                game3x3.turn++;
                                // Ход компьютера (нолик)
                                computerMove3x3(game3x3.board, "o", "x");
                                
                                if (game3x3.checkWin("o")) {
                                    game3x3.gameEnded = true;
                                    game3x3.winner = "o";
                                    currentState = GameState::GAME_OVER;
                                }
                                else if (game3x3.isBoardFull()) {
                                    game3x3.gameEnded = true;
                                    game3x3.winner = "draw";
                                    currentState = GameState::GAME_OVER;
                                }
                                else {
                                    game3x3.turn++;
                                }
                            }
                        }
                    }
                }
                
                // Клавиша R для перезапуска
                if (event.type == Event::KeyPressed && event.key.code == Keyboard::R) {
                    game3x3.reset();
                    cout << "[logs]Game restarted!" << endl;
                }
            }
            
            if (currentState == GameState::GAME_MODE_3) {
                // Режим 3: два человека и ИИ на поле 4x4
                if (event.type == Event::MouseButtonPressed && event.mouseButton.button == Mouse::Left) {
                    Vector2i mousePos = Mouse::getPosition(window);
                    
                    // Проверяем, кликнули ли внутри игрового поля
                    FloatRect gridBounds(GRID_START_X_4x4, GRID_START_Y_4x4, MAX_SIZE_4 * CELL_SIZE, MAX_SIZE_4 * CELL_SIZE);
                    if (gridBounds.contains(static_cast<Vector2f>(mousePos))) {
                        int col = (mousePos.x - GRID_START_X_4x4) / CELL_SIZE;
                        int row = (mousePos.y - GRID_START_Y_4x4) / CELL_SIZE;
                        
                        if (game4x4.isCellEmpty(row, col)) {
                            int turn = (game4x4.turn - 1) % 3; // 0 - x, 1 - o, 2 - c
                            
                            if (turn == 0) {
                                // Player 1 (x)
                                game4x4.makeMove(row, col, "x");
                                if (game4x4.checkWin3InRow("x")) {
                                    game4x4.gameEnded = true;
                                    game4x4.winner = "x";
                                    currentState = GameState::GAME_OVER;
                                }
                            }
                            else if (turn == 1) {
                                // Player 2 (o)
                                game4x4.makeMove(row, col, "o");
                                if (game4x4.checkWin3InRow("o")) {
                                    game4x4.gameEnded = true;
                                    game4x4.winner = "o";
                                    currentState = GameState::GAME_OVER;
                                }
                            }
                            
                            if (!game4x4.gameEnded && game4x4.isBoardFull()) {
                                game4x4.gameEnded = true;
                                game4x4.winner = "draw";
                                currentState = GameState::GAME_OVER;
                            }
                            
                            if (!game4x4.gameEnded) {
                                game4x4.turn++;
                            }
                            
                            // Ход компьютера (если следующий ход за ним)
                            if (!game4x4.gameEnded && ((game4x4.turn - 1) % 3 == 2)) {
                                computerMove4x4(game4x4.board, "c", "x", "o");
                                
                                if (game4x4.checkWin3InRow("c")) {
                                    game4x4.gameEnded = true;
                                    game4x4.winner = "c";
                                    currentState = GameState::GAME_OVER;
                                }
                                else if (game4x4.isBoardFull()) {
                                    game4x4.gameEnded = true;
                                    game4x4.winner = "draw";
                                    currentState = GameState::GAME_OVER;
                                }
                                else {
                                    game4x4.turn++;
                                }
                            }
                        }
                    }
                }
                
                // Клавиша R для перезапуска
                if (event.type == Event::KeyPressed && event.key.code == Keyboard::R) {
                    game4x4.reset();
                    cout << "[logs]Game restarted!" << endl;
                }
            }
            
            if (currentState == GameState::GAME_OVER) {
                // Клавиша R для возврата в меню
                if (event.type == Event::KeyPressed && event.key.code == Keyboard::R) {
                    currentState = GameState::MENU;
                    cout << "[logs]Returning to menu..." << endl;
                }
            }
        }
        
        // Обновление состояния кнопок меню
        Vector2i mousePos = Mouse::getPosition(window);
        for (auto& button : menuButtons) {
            button.isMouseOver(mousePos);
        }
        
        // Рендеринг
        window.clear(Color(240, 240, 240)); // светлый фон
        
        if (currentState == GameState::MENU) {
            // Рисуем меню
            Text title;
            title.setFont(font);
            title.setString("Infinity Tic-Tac Game");
            title.setCharacterSize(64);
            title.setFillColor(Color(50, 100, 150));
            title.setStyle(Text::Bold);
            title.setPosition(WINDOW_WIDTH / 2 - title.getGlobalBounds().width / 2, 50);
            window.draw(title);
            
            // Подзаголовок
            Text subtitle;
            subtitle.setFont(font);
            subtitle.setString("Choose game mode");
            subtitle.setCharacterSize(32);
            subtitle.setFillColor(Color(100, 130, 180));
            subtitle.setPosition(WINDOW_WIDTH / 2 - subtitle.getGlobalBounds().width / 2, 120);
            window.draw(subtitle);
            
            // Кнопки меню
            for (auto& button : menuButtons) {
                button.draw(window);
            }
            
            // Версия игры
            Text version;
            version.setFont(font);
            version.setString("beta v0.1");
            version.setCharacterSize(20);
            version.setFillColor(Color(150, 150, 180));
            version.setPosition(WINDOW_WIDTH - version.getGlobalBounds().width - 20, WINDOW_HEIGHT - 40);
            window.draw(version);
        }
        else if (currentState == GameState::GAME_MODE_1) {
            // Режим 1: 3x3 с распознаванием жестов
            drawGrid(window, MAX_SIZE, GRID_START_X_3x3, GRID_START_Y_3x3);
            drawSymbols(window, game3x3, GRID_START_X_3x3, GRID_START_Y_3x3);
            
            // Рисуем текущий штрих (если рисуем)
            if (!currentStroke.empty()) {
                VertexArray line(PrimitiveType::LineStrip, currentStroke.size());
                for (size_t i = 0; i < currentStroke.size(); ++i) {
                    line[i].position = currentStroke[i];
                    line[i].color = Color::Blue;
                }
                window.draw(line);
            }
            
            // Рисуем распознанный штрих
            if (!recognizedStroke.empty()) {
                Color strokeColor = (lastGesture == GestureType::Cross) ? Color::Red :
                                   (lastGesture == GestureType::Circle) ? Color(0, 150, 0) : Color(100, 100, 100);
                
                VertexArray line(PrimitiveType::LineStrip, recognizedStroke.size());
                for (size_t i = 0; i < recognizedStroke.size(); ++i) {
                    line[i].position = recognizedStroke[i];
                    line[i].color = strokeColor;
                }
                window.draw(line);
            }
            
            // Информация о текущем игроке
            Text currentPlayerText;
            currentPlayerText.setFont(font);
            currentPlayerText.setString((game3x3.turn % 2 == 1) ? "Player 1's Turn (X)" : "Player 2's Turn (O)");
            currentPlayerText.setCharacterSize(28);
            currentPlayerText.setFillColor((game3x3.turn % 2 == 1) ? Color::Red : Color::Blue);
            currentPlayerText.setPosition(WINDOW_WIDTH / 2 - currentPlayerText.getGlobalBounds().width / 2, 20);
            window.draw(currentPlayerText);
            
            // Инструкция
            Text instruction;
            instruction.setFont(font);
            instruction.setString("Draw X or O in the cell");
            instruction.setCharacterSize(20);
            instruction.setFillColor(Color(80, 80, 120));
            instruction.setPosition(20, WINDOW_HEIGHT - 40);
            window.draw(instruction);
        }
        else if (currentState == GameState::GAME_MODE_2) {
            // Режим 2: 3x3 против компьютера
            drawGrid(window, MAX_SIZE, GRID_START_X_3x3, GRID_START_Y_3x3);
            drawSymbols(window, game3x3, GRID_START_X_3x3, GRID_START_Y_3x3);
            
            // Информация о текущем игроке
            Text currentPlayerText;
            currentPlayerText.setFont(font);
            currentPlayerText.setString((game3x3.turn % 2 == 1) ? "Your Turn (X)" : "Computer's Turn (O)");
            currentPlayerText.setCharacterSize(28);
            currentPlayerText.setFillColor((game3x3.turn % 2 == 1) ? Color::Red : Color::Blue);
            currentPlayerText.setPosition(WINDOW_WIDTH / 2 - currentPlayerText.getGlobalBounds().width / 2, 20);
            window.draw(currentPlayerText);
        }
        else if (currentState == GameState::GAME_MODE_3) {
            // Режим 3: 4x4 два человека + ИИ
            drawGrid(window, MAX_SIZE_4, GRID_START_X_4x4, GRID_START_Y_4x4);
            drawSymbols4x4(window, game4x4, GRID_START_X_4x4, GRID_START_Y_4x4);
            
            // Информация о текущем игроке
            int turn = (game4x4.turn - 1) % 3;
            string playerText = (turn == 0) ? "Player 1's Turn (X)" :
                               (turn == 1) ? "Player 2's Turn (O)" : "Computer's Turn (C)";
            Color playerColor = (turn == 0) ? Color::Red :
                               (turn == 1) ? Color::Blue : Color::Green;
            
            Text currentPlayerText;
            currentPlayerText.setFont(font);
            currentPlayerText.setString(playerText);
            currentPlayerText.setCharacterSize(28);
            currentPlayerText.setFillColor(playerColor);
            currentPlayerText.setPosition(WINDOW_WIDTH / 2 - currentPlayerText.getGlobalBounds().width / 2, 20);
            window.draw(currentPlayerText);
        }
        else if (currentState == GameState::GAME_OVER) {
            // Экран окончания игры
            RectangleShape gameOverBg(Vector2f(WINDOW_WIDTH - 40, 150));
            gameOverBg.setPosition(20, (WINDOW_HEIGHT - 150) / 2);
            gameOverBg.setFillColor(Color(255, 255, 200));
            gameOverBg.setOutlineColor(Color::Black);
            gameOverBg.setOutlineThickness(2);
            window.draw(gameOverBg);
            
            // Результат
            string resultText = "";
            Color resultColor = Color::Black;
            
            if (game3x3.gameEnded) {
                if (game3x3.winner == "x") {
                    resultText = "Player 1 WINS!";
                    resultColor = Color::Red;
                }
                else if (game3x3.winner == "o") {
                    resultText = "Player 2 WINS!";
                    resultColor = Color::Blue;
                }
                else if (game3x3.winner == "draw") {
                    resultText = "DRAW!";
                    resultColor = Color::Black;
                }
            }
            else if (game4x4.gameEnded) {
                if (game4x4.winner == "x") {
                    resultText = "Player 1 WINS!";
                    resultColor = Color::Red;
                }
                else if (game4x4.winner == "o") {
                    resultText = "Player 2 WINS!";
                    resultColor = Color::Blue;
                }
                else if (game4x4.winner == "c") {
                    resultText = "COMPUTER WINS!";
                    resultColor = Color::Green;
                }
                else if (game4x4.winner == "draw") {
                    resultText = "DRAW!";
                    resultColor = Color::Black;
                }
            }
            
            Text result;
            result.setFont(font);
            result.setString(resultText);
            result.setCharacterSize(36);
            result.setFillColor(resultColor);
            result.setPosition(WINDOW_WIDTH / 2 - result.getGlobalBounds().width / 2, WINDOW_HEIGHT / 2 - 40);
            window.draw(result);
            
            // Подсказка
            Text restartHint;
            restartHint.setFont(font);
            restartHint.setString("Press R to restart");
            restartHint.setCharacterSize(24);
            restartHint.setFillColor(Color(80, 80, 120));
            restartHint.setPosition(WINDOW_WIDTH / 2 - restartHint.getGlobalBounds().width / 2, WINDOW_HEIGHT / 2 + 20);
            window.draw(restartHint);
        }
        
        window.display();
    }
    
    return 0;
}
