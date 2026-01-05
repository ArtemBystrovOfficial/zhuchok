#include "GameRunner.hpp"
#include <atomic>
namespace game_runner {

    struct Position {
        int x, y;
        Position(int x = 0, int y = 0) : x(x), y(y) {}
    };

    enum Direction {
        DOWN = 0,
        RIGHT = 1,
        UP = 2,
        LEFT = 3
    };

    int runGame(common::BaseMap & mp) {
        // Ищем начальную позицию жука (B)
        Position bugPos(0, 0);
        bool found = false;
        for (unsigned int y = 0; y < mp.GetHeight(); y++) {
            for (unsigned int x = 0; x < mp.GetWidth(); x++) {
                if (mp.getCell(x, y).getCellType() == common::CellType::Bug) {
                    bugPos.x = x;
                    bugPos.y = y;
                    found = true;
                    break;
                }
            }
            if (found) break;
        }

        if (!found) {
            return 0; // Жук не найден
        }

        Direction currentDir = DOWN;
        int stepCounter = 0;

        // Создаем матрицу для подсчета посещений ячеек
        // Размер соответствует карте
        std::vector<std::vector<int>> visitCounts(
            mp.GetHeight(),
            std::vector<int>(mp.GetWidth(), 0)
        );

        // Бесконечный цикл движения, пока не достигнем цели или не застрянем
        while (true) {
            // Проверяем, достигли ли целевой точки (28,6) как в оригинальной функции
            if (bugPos.x == mp.GetWidth() - 1 && bugPos.y == mp.GetHeight() - 1) {
                for(int i=0;i<mp.GetHeight();i++)
                    for(int j=0;j<mp.GetWidth();j++)
                        mp.cell(j,i).setVisitCount(visitCounts[i][j]);
                return stepCounter;
            }

            // Проверяем, не застряли ли мы (например, если жук в тупике)
            stepCounter++;
            if (stepCounter > 10000) { // Защита от бесконечного цикла
                for(int i=0;i<mp.GetHeight();i++)
                    for(int j=0;j<mp.GetWidth();j++)
                        mp.cell(j,i).setVisitCount(visitCounts[i][j]);
                return stepCounter;
            }

            // Получаем значения посещений соседних ячеек
            int downCount = INT_MAX, rightCount = INT_MAX, upCount = INT_MAX, leftCount = INT_MAX;
            bool canMoveDown = false, canMoveRight = false, canMoveUp = false, canMoveLeft = false;

            // Проверяем нижнюю ячейку
            if (bugPos.y + 1 < static_cast<int>(mp.GetHeight())) {
                const auto& cell = mp.getCell(bugPos.x, bugPos.y + 1);
                if (!cell.isWall()) {
                    downCount = visitCounts[bugPos.y + 1][bugPos.x];
                    canMoveDown = true;
                }
            }

            // Проверяем правую ячейку
            if (bugPos.x + 1 < static_cast<int>(mp.GetWidth())) {
                const auto& cell = mp.getCell(bugPos.x + 1, bugPos.y);
                if (!cell.isWall()) {
                    rightCount = visitCounts[bugPos.y][bugPos.x + 1];
                    canMoveRight = true;
                }
            }

            // Проверяем верхнюю ячейку
            if (bugPos.y - 1 >= 0) {
                const auto& cell = mp.getCell(bugPos.x, bugPos.y - 1);
                if (!cell.isWall()) {
                    upCount = visitCounts[bugPos.y - 1][bugPos.x];
                    canMoveUp = true;
                }
            }

            // Проверяем левую ячейку
            if (bugPos.x - 1 >= 0) {
                const auto& cell = mp.getCell(bugPos.x - 1, bugPos.y);
                if (!cell.isWall()) {
                    leftCount = visitCounts[bugPos.y][bugPos.x - 1];
                    canMoveLeft = true;
                }
            }

            // Определяем текущее направление
            int curCount = INT_MAX;
            switch (currentDir) {
                case DOWN: curCount = canMoveDown ? downCount : INT_MAX; break;
                case RIGHT: curCount = canMoveRight ? rightCount : INT_MAX; break;
                case UP: curCount = canMoveUp ? upCount : INT_MAX; break;
                case LEFT: curCount = canMoveLeft ? leftCount : INT_MAX; break;
            }

            // Выбираем направление движения по алгоритму оригинальной функции
            Position newPos = bugPos;
            Direction newDir = currentDir;

            // Логика выбора направления из оригинальной функции
            if (curCount <= downCount && curCount <= rightCount &&
                curCount <= upCount && curCount <= leftCount) {
                // Двигаемся в текущем направлении
                switch (currentDir) {
                    case DOWN:
                        if (canMoveDown) {
                            newPos.y++;
                        }
                        break;
                    case RIGHT:
                        if (canMoveRight) {
                            newPos.x++;
                        }
                        break;
                    case UP:
                        if (canMoveUp) {
                            newPos.y--;
                        }
                        break;
                    case LEFT:
                        if (canMoveLeft) {
                            newPos.x--;
                        }
                        break;
                }
            } else if (downCount <= rightCount && downCount <= upCount && downCount <= leftCount) {
                if (canMoveDown) {
                    newPos.y++;
                    newDir = DOWN;
                }
            } else if (rightCount <= downCount && rightCount <= upCount && rightCount <= leftCount) {
                if (canMoveRight) {
                    newPos.x++;
                    newDir = RIGHT;
                }
            } else if (upCount <= rightCount && upCount <= downCount && upCount <= leftCount) {
                if (canMoveUp) {
                    newPos.y--;
                    newDir = UP;
                }
            } else if (leftCount <= rightCount && leftCount <= downCount && leftCount <= upCount) {
                if (canMoveLeft) {
                    newPos.x--;
                    newDir = LEFT;
                }
            }

            // Увеличиваем счетчик посещений текущей ячейки перед уходом
            visitCounts[bugPos.y][bugPos.x]++;

            // Проверяем, можем ли двигаться
            if (newPos.x == bugPos.x && newPos.y == bugPos.y) {
                // Не можем двигаться - вероятно, тупик
                // Пытаемся найти любое доступное направление
                if (canMoveDown) {
                    newPos.y++;
                    newDir = DOWN;
                } else if (canMoveRight) {
                    newPos.x++;
                    newDir = RIGHT;
                } else if (canMoveUp) {
                    newPos.y--;
                    newDir = UP;
                } else if (canMoveLeft) {
                    newPos.x--;
                    newDir = LEFT;
                } else {
                    // Полный тупик - возвращаем текущий счетчик
                    for(int i=0;i<mp.GetHeight();i++)
                        for(int j=0;j<mp.GetWidth();j++)
                            mp.cell(j,i).setVisitCount(visitCounts[i][j]);
                    return stepCounter;
                }
            }

            // Обновляем позицию жука
            bugPos = newPos;
            currentDir = newDir;
        }


        for(int i=0;i<mp.GetHeight();i++)
            for(int j=0;j<mp.GetWidth();j++)
                mp.cell(j,i).setVisitCount(visitCounts[i][j]);
        return stepCounter;
    }
}
