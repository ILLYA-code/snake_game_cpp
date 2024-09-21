#include <conio.h>
#include <iostream>
#include <cstdlib> // Для system("cls")
#include <vector>
#include <random>
#include <chrono>
#include <thread>
#include <windows.h>

using namespace std;

int snakeSize = 4;

char whereGo = 'U';

bool isEaten = false;

int score = 0;

bool isPause = false;

bool isGameOver = true;

int getWidthOrHeight(bool w) {
    // Структура для зберігання інформації про консоль
    CONSOLE_SCREEN_BUFFER_INFO csbi;

    // Отримуємо дескриптор консолі
    HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);

    if (GetConsoleScreenBufferInfo(hConsole, &csbi)) {
        if (w)
        {
            int width = csbi.srWindow.Right - csbi.srWindow.Left + 1;
            cout << width << endl;
            return width;
        }
        else {
            int height = csbi.srWindow.Bottom - csbi.srWindow.Top + 1;
            cout << height << endl;
            return height;
        }
    }
    else {
        return 10;
    }
}

void gotoXY(int x, int y) {
    HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
    COORD coord;
    coord.X = x;
    coord.Y = y;
    SetConsoleCursorPosition(hConsole, coord);
}

char getCharAtXY(int x, int y) {
    HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE); // Отримуємо хендл консолі
    char ch; // Символ, який буде зчитано
    COORD coord; // Координати
    DWORD read; // Змінна для збереження кількості прочитаних символів

    coord.X = x;
    coord.Y = y;

    // Читаємо символ з буфера консолі
    if (!ReadConsoleOutputCharacterA(hConsole, &ch, 1, coord, &read)) {
        std::cerr << "Failed to read console output character." << std::endl;
        return '\0'; // Повертаємо порожній символ у разі помилки
    }

    return ch; // Повертаємо символ
}


// Функція для генерації випадкового числа в заданому діапазоні
int randNum(int min, int max) {
    // Ініціалізація генератора випадкових чисел
    std::random_device rd; // Для отримання випадкового насіння
    std::mt19937 gen(rd()); // Мерсенський твірний генератор

    // Встановлення рівномірного розподілу
    std::uniform_int_distribution<> dis(min, max); // Рівномірний розподіл

    // Генерація і повернення випадкового числа
    return dis(gen);
}

void cls() {
    system("cls");
}

void spawnFood(int height, int width, char foodSym, char snakeSym) {

    int x = randNum(0, width - 1);
    int y = randNum(0, height - 1);

    char chXY = getCharAtXY(x, y);

    if (chXY == snakeSym || chXY == foodSym)
    {
        spawnFood(height, width, foodSym, snakeSym);
    }
    else {
        gotoXY(x, y);
        cout << foodSym;
    }
}

bool eat(int height, int width, char foodSym, char snakeSym, int availableSyms, int sandwiches) {
    isEaten = true;
    score++;

    if (sandwiches < (availableSyms + 5))
    {
        spawnFood(height, width, foodSym, snakeSym);
        return true;
    }
    else if (sandwiches >= (availableSyms + 5)) {
        return true;
    }
    else {
        return false;
    }

    
}

class Game {
public:

    void Start(int height, int width, char fieldSymbol, char snakeSymbol, char foodSymbol, int sandwiches, int speed, int fixedField) {

        isGameOver = false;
        whereGo = 'U';
        score = 0;

        std::vector<std::vector<char>> fieldElements(height, std::vector<char>(width, fieldSymbol));

        cls();

        for (auto& h : fieldElements) {
            for (char c : h) {
                std::cout << c;
            }
            std::cout << std::endl;
        }

        std::vector<std::vector<int>> snakeArr(4, std::vector<int>(2, 0));

        snakeArr[3][0] = width / 2;
        snakeArr[3][1] = height / 2;

        snakeArr[2][0] = width / 2;
        snakeArr[2][1] = (height / 2) + 1;

        snakeArr[1][0] = width / 2;
        snakeArr[1][1] = (height / 2) + 2;

        snakeArr[0][0] = width / 2;
        snakeArr[0][1] = (height / 2) + 3;

        for (int i = snakeArr.size() - 1; i >= 0; --i) {
            int x;
            int y;

            x = snakeArr[i][0];
            y = snakeArr[i][1];

            fieldElements[y][x] = snakeSymbol;

        }

        //cout << snakeArr.size() << endl;
        //cout << fieldElements.size() * fieldElements[0].size() << endl;

        updateField(snakeArr[snakeArr.size() - 1][0], snakeArr[snakeArr.size() - 1][1], snakeArr[0][0], snakeArr[0][1], snakeSymbol, fieldSymbol, foodSymbol, height, width, sandwiches, speed, fixedField, ((fieldElements[0].size() * fieldElements.size()) - snakeArr.size()));

        for (int i = 0; i < sandwiches; ++i) {
            spawnFood(height, width, foodSymbol, snakeSymbol);
        }

        char ch1;

        while (!isGameOver) {
            if (_kbhit()) {
                // Забираємо всі натискання клавіш з буферу, залишаючи тільки останню
                while (_kbhit()) {
                    ch1 = _getch(); // Остання натиснута клавіша
                }
                // Обробляємо тільки останню команду
                switch (ch1) {
                case 'H': 
                    if (whereGo != 'D')
                    {
                        whereGo = 'U';
                    }
                    break; // Вверх
                case 'P': 
                    if (whereGo != 'U')
                    {
                        whereGo = 'D';
                    }
                    break;
                case 'K': 
                    if (whereGo != 'R')
                    {
                        whereGo = 'L';
                    }
                    break;
                case 'M': 
                    if (whereGo != 'L')
                    {
                        whereGo = 'R';
                    }
                    break;
                case 'p':
                    whereGo = 'P';
                    break;
                default: break;
                }
            }

            // Рух змійки відповідно до напрямку
            switch (whereGo) {
                //case !'p': isPause = false; continue;
            case 'L': // Ліворуч
                isPause = false;
                snakeArr.push_back(std::vector<int>{(snakeArr[snakeArr.size() - 1][0] - 1), (snakeArr[snakeArr.size() - 1][1])});
                break;
            case 'R': // Праворуч
                isPause = false;
                snakeArr.push_back(std::vector<int>{(snakeArr[snakeArr.size() - 1][0] + 1), (snakeArr[snakeArr.size() - 1][1])});
                break;
            case 'U': // Вгору
                isPause = false;
                snakeArr.push_back(std::vector<int>{(snakeArr[snakeArr.size() - 1][0]), (snakeArr[snakeArr.size() - 1][1] - 1)});
                break;
            case 'D': // Вниз
                isPause = false;
                snakeArr.push_back(std::vector<int>{(snakeArr[snakeArr.size() - 1][0]), (snakeArr[snakeArr.size() - 1][1] + 1)});
                break;
            case 'P':
                isPause = true;
                break;
            default: break;
            }

            if (!isEaten && !isPause) {
                snakeArr.erase(snakeArr.begin()); // Видаляємо хвіст 
            }
            else {
                isEaten = false;
            }

            //cout << snakeArr.size() << endl;
            //cout << fieldElements.size() * fieldElements[0].size() << endl;

            if (!isPause)
            {
                updateField(snakeArr[snakeArr.size() - 1][0], snakeArr[snakeArr.size() - 1][1], snakeArr[0][0], snakeArr[0][1], snakeSymbol, fieldSymbol, foodSymbol, height, width, sandwiches, speed, fixedField, ((fieldElements[0].size() * fieldElements.size()) - snakeArr.size()));
            }

            std::this_thread::sleep_for(std::chrono::milliseconds(800 / speed)); // Затримка
        }


    }

    void updateField(int hx, int hy, int sx, int sy, char snakeSym, char fieldSym, char foodSym, int height, int width, int sandwiches, int speed, int fixedField, int availableSyms) {

        char nextCh = getCharAtXY(hx, hy);

        if (fixedField >= 1) {
            if (nextCh == snakeSym || hx < 0 || hx >= width || hy < 0 || hy >= height)
            {
                //spawnFood(height, width, foodSym, snakeSym);
                End(score, height, width, fieldSym, snakeSym, foodSym, sandwiches, speed, fixedField);
            }
            else if (nextCh == foodSym) {
                //--availableSyms;

                if (!eat(height, width, foodSym, snakeSym, availableSyms, sandwiches))
                {
                    End(score, height, width, fieldSym, snakeSym, foodSym, sandwiches, speed, fixedField);
                }
            }
            else {
                gotoXY(hx, hy);
                cout << foodSym;
            }
        }
        else {
            if (nextCh == snakeSym)
            {
                //spawnFood(height, width, foodSym, snakeSym);
                End(score, height, width, fieldSym, snakeSym, foodSym, sandwiches, speed, fixedField);
            }
            else if (nextCh == foodSym) {
                if (!eat(height, width, foodSym, snakeSym, availableSyms, sandwiches))
                {
                    End(score, height, width, fieldSym, snakeSym, foodSym, sandwiches, speed, fixedField);
                }
            }
            else {
                gotoXY(hx, hy);
                cout << foodSym;
            }
        }

        if (!isGameOver)
        {
            gotoXY(hx, hy);
            cout << snakeSym;

            gotoXY(sx, sy);
            cout << fieldSym;
        }
    }

    void End(int score, int height, int width, char fieldSym, char snakeSym, char foodSym, int sandwiches, int speed, int fixedField) {
        isGameOver = true;
        cls();
        cout << "score is: " << score << endl << endl;
        cout << "Press R to restart";

        char ch;
        ch = _getch();
        
        if (ch == 'r' || ch == 'R')
        {
            Game game;
            game.Start(height, width, fieldSym, snakeSym, foodSym, sandwiches, speed, fixedField);
        }
        else {
            cout << endl << endl << "Press any key to quit" << endl;
            cout << endl << "Press R to restart";

            ch = _getch();

            if (ch == 'r' || ch == 'R')
            {
                Game game;
                game.Start(height, width, fieldSym, snakeSym, foodSym, sandwiches, speed, fixedField);
            }
        }
    }
};

void checkSpeed(int speed) {
    if (speed > 10)
    {
        cout << endl << "too long speed";
        cout << endl << "speed: 1 - 10: ";
        cin >> speed;
        checkSpeed(speed);
    }
    else if (speed < 1) {
        cout << endl << "too slow speed";
        cout << endl << "speed: 1 - 10: ";
        cin >> speed;
        checkSpeed(speed);
    }
}

void startGame() {
    int height, width, sandwiches, speed, fixedField;
    char fieldSymbol = '.', snakeSymbol = '#', foodSymbol = '*';

    cout << "type height: ";
    cin >> height;
    int maxHeight = getWidthOrHeight(false);

    if (height > maxHeight)
    {
        height = maxHeight;
    }
    else if (height < 10) {
        height = 10;
    }

    cout << endl << "type width: ";
    cin >> width;
    int maxWidth = getWidthOrHeight(true);

    if (width > maxWidth)
    {
        width = maxWidth;
    }
    else if (width < 10) {
        width = 10;
    }

    int maxSandwiches = ((maxWidth * maxHeight) - 5);

    cout << endl << "sandwiches: ";
    cin >> sandwiches;

    if (sandwiches > maxSandwiches)
    {
        sandwiches = maxSandwiches;
    }

    cout << endl << "speed: 1 - 10: ";
    cin >> speed;
    cout << endl << "fixed field? (1/0): ";
    cin >> fixedField;

    checkSpeed(speed);

    Game game;

    game.Start(height, width, fieldSymbol, snakeSymbol, foodSymbol, sandwiches, speed, fixedField);
}

int main() {
    setlocale(LC_ALL, "uk_UA");

     startGame();

    /*while (true) {
        char el = _getch();
        cout << el << endl;
    }*/

    return 0;
}
