#include <GL/glut.h>
#include <cstdlib>
#include <ctime>
#include <cmath>
#include <cstring>
#include <iostream>

enum Level { NONE, EASY, MEDIUM, HARD };

class Game {
public:
    static const int MAX_SIZE = 20;
    int grid[MAX_SIZE][MAX_SIZE];
    int ROWS = 10, COLS = 12;
    int TILE_SIZE = 80;
    int BOMB_RATIO = 10;
    const int BORDER_SIZE = 80;
    const int TOP_SPACE = 70;
    int selectedRow = -1, selectedCol = -1;
    bool gameOver = false;
    int score = 0;
    Level currentLevel = NONE;

    void setLevelConfig(Level level) {
        currentLevel = level;
        switch(level) {
            case EASY: ROWS = 6; COLS = 8; BOMB_RATIO = 5; break;
            case MEDIUM: ROWS = 8; COLS = 10; BOMB_RATIO = 10; break;
            case HARD: ROWS = 10; COLS = 12; BOMB_RATIO = 20; break;
            default: break;
        }
    }

    void drawEllipse(float cx, float cy, float rx, float ry, int segments = 100) {
        glBegin(GL_TRIANGLE_FAN);
        for (int i = 0; i <= segments; i++) {
            float theta = 2.0f * 3.1415926f * float(i) / float(segments);
            float x = rx * cosf(theta);
            float y = ry * sinf(theta);
            glVertex2f(cx + x, cy + y);
        }
        glEnd();
    }

    void drawChocolateRound(float x, float y) {
        glColor3f(0.55f, 0.27f, 0.07f);//round chocolate color
        drawEllipse(x + TILE_SIZE/2, y + TILE_SIZE/2, TILE_SIZE/3, TILE_SIZE/3);
        glColor4f(1, 1, 1, 0.4f);//round chocolate top little circle color
        glBegin(GL_TRIANGLE_FAN);
        glVertex2f(x + TILE_SIZE/2 - 12, y + TILE_SIZE/2 - 12);
        for (int i = 0; i <= 20; i++) {
            float angle = 2 * 3.14159f * i / 20;
            glVertex2f(x + TILE_SIZE/2 - 12 + 8*cos(angle), y + TILE_SIZE/2 - 12 + 8*sin(angle));
        }
        glEnd();
    }

    void drawChocolateTriangle(float x, float y) {
        glColor3f(0.4f, 0.2f, 0.05f);//triangle chocolate inside color
        glBegin(GL_TRIANGLES);
        glVertex2f(x + TILE_SIZE/2, y + TILE_SIZE/4);
        glVertex2f(x + 3*TILE_SIZE/4, y + 3*TILE_SIZE/4);
        glVertex2f(x + TILE_SIZE/4, y + 3*TILE_SIZE/4);
        glEnd();
        glColor3f(0.25f, 0.1f, 0.05f);//triangle chocolate inside circle color
        drawEllipse(x + TILE_SIZE/2, y + TILE_SIZE/2, 5, 5);
    }

    void drawChocolateDiamond(float x, float y) {
        glColor3f(0.6f, 0.35f, 0.1f);//diamond chocolate inside color
        glBegin(GL_POLYGON);
        glVertex2f(x + TILE_SIZE/2, y + TILE_SIZE/6);
        glVertex2f(x + 5*TILE_SIZE/6, y + TILE_SIZE/2);
        glVertex2f(x + TILE_SIZE/2, y + 5*TILE_SIZE/6);
        glVertex2f(x + TILE_SIZE/6, y + TILE_SIZE/2);
        glEnd();
        glColor3f(0.4f, 0.2f, 0.05f);//diamond chocolate inside border color
        glLineWidth(2);
        glBegin(GL_LINES);
        glVertex2f(x + TILE_SIZE/2, y + TILE_SIZE/6);
        glVertex2f(x + TILE_SIZE/2, y + 5*TILE_SIZE/6);
        glVertex2f(x + TILE_SIZE/6, y + TILE_SIZE/2);
        glVertex2f(x + 5*TILE_SIZE/6, y + TILE_SIZE/2);
        glEnd();
    }

    void drawChocolateStar(float x, float y) {
        glColor3f(0.0f, 0.15f, 0.15f);//star chocolate color
        float cx = x + TILE_SIZE/2;
        float cy = y + TILE_SIZE/2;
        float r_outer = TILE_SIZE/3;
        float r_inner = r_outer / 2.5f;
        glBegin(GL_TRIANGLE_FAN);
        for (int i=0; i<=10; i++) {
            float angle = i * 3.14159f / 5;
            float r = (i % 2 == 0) ? r_outer : r_inner;
            glVertex2f(cx + r * sin(angle), cy + r * cos(angle));
        }
        glEnd();
    }

    void drawBomb(float x, float y) {
        glColor3f(0.0f, 0.0f, 0.0f);//bomb  inside color
        drawEllipse(x + TILE_SIZE/2, y + TILE_SIZE/2, TILE_SIZE/3, TILE_SIZE/3);
        glColor3f(0,0,0);//bomb outside color
        glLineWidth(3);
        glBegin(GL_LINE_LOOP);
        for (int i = 0; i < 100; i++) {
            float theta = 2 * 3.14159f * i / 100;
            float rx = TILE_SIZE/3;
            float ry = TILE_SIZE/3;
            glVertex2f(x + TILE_SIZE/2 + rx * cos(theta), y + TILE_SIZE/2 + ry * sin(theta));
        }
        glEnd();
        glBegin(GL_QUADS);
        glVertex2f(x + TILE_SIZE/2 - 5, y + TILE_SIZE/2 - TILE_SIZE/3 - 10);
        glVertex2f(x + TILE_SIZE/2 + 5, y + TILE_SIZE/2 - TILE_SIZE/3 - 10);
        glVertex2f(x + TILE_SIZE/2 + 5, y + TILE_SIZE/2 - TILE_SIZE/3);
        glVertex2f(x + TILE_SIZE/2 - 5, y + TILE_SIZE/2 - TILE_SIZE/3);
        glEnd();
    }

    void drawTile(int row, int col) {
        int x = col * TILE_SIZE;
        int y = row * TILE_SIZE;
        glColor3f(0.9,0.8,0.5);//game inside display color
        glBegin(GL_QUADS);
        glVertex2i(x, y); glVertex2i(x + TILE_SIZE, y);
        glVertex2i(x + TILE_SIZE, y + TILE_SIZE); glVertex2i(x, y + TILE_SIZE);
        glEnd();
        switch(grid[row][col]) {
            case 0: drawChocolateRound(x,y); break;
            case 1: drawChocolateTriangle(x,y); break;
            case 2: drawChocolateDiamond(x,y); break;
            case 3: drawChocolateStar(x,y); break;
            case 5: drawBomb(x,y); break;
        }
        glColor3f((row == selectedRow && col == selectedCol) ? 1 : 0, (row == selectedRow && col == selectedCol) ? 1 : 0, 0);
        glLineWidth((row == selectedRow && col == selectedCol) ? 4 : 2);
        glBegin(GL_LINE_LOOP);
        glVertex2i(x, y); glVertex2i(x + TILE_SIZE, y);
        glVertex2i(x + TILE_SIZE, y + TILE_SIZE); glVertex2i(x, y + TILE_SIZE);
        glEnd();
    }

    void drawText(float x, float y, const char* text, float r=0, float g=0, float b=0) {
        glColor3f(r,g,b);
        glRasterPos2f(x,y);
        for(const char* c = text; *c != '\0'; c++)
            glutBitmapCharacter(GLUT_BITMAP_HELVETICA_18, *c);
    }

    void displayGameOverScreen() {
        glClear(GL_COLOR_BUFFER_BIT);
        char finalScore[20];
        sprintf(finalScore, "%d", score);
        int winWidth = COLS * TILE_SIZE + 2*BORDER_SIZE;
        int winHeight = ROWS * TILE_SIZE + TOP_SPACE + BORDER_SIZE;
        drawText(winWidth/2 - 60, winHeight/2, "GAME OVER!", 0, 0, 0);
        drawText(winWidth/2 - 70, winHeight/2 + 30, "Final Score :", 0, 0, 0);
        drawText(winWidth/2 + 50, winHeight/2 + 30, finalScore, 0, 0, 0);
        glutSwapBuffers();
    }

    void initGrid() {
        for (int i = 0; i < ROWS; i++)
            for (int j = 0; j < COLS; j++)
                grid[i][j] = (rand() % 100 < BOMB_RATIO) ? 5 : rand() % 4;
    }

    void resetGame() {
        score = 0; gameOver = false;
        selectedRow = selectedCol = -1;
        initGrid();
        while (checkMatches(false)) collapseGrid();
    }

    bool checkMatches(bool updateScore);
    void collapseGrid();
    void mouseClick(int button, int state, int x, int y);
    void mouseClickOnLevel(int button, int state, int x, int y);
    void display();
};

Game game;

bool Game::checkMatches(bool updateScore) {
    bool foundMatch = false;
    bool toClear[20][20] = {false};
    int matched = 0, bombMatchCount = 0;
    for (int i=0; i<ROWS; i++)
        for (int j=0; j<COLS-2; j++)
            if (grid[i][j] >=0 && grid[i][j] <6 &&
                grid[i][j] == grid[i][j+1] && grid[i][j] == grid[i][j+2])
                toClear[i][j] = toClear[i][j+1] = toClear[i][j+2] = true, foundMatch = true;
    for (int j=0; j<COLS; j++)
        for (int i=0; i<ROWS-2; i++)
            if (grid[i][j] >=0 && grid[i][j] <6 &&
                grid[i][j] == grid[i+1][j] && grid[i][j] == grid[i+2][j])
                toClear[i][j] = toClear[i+1][j] = toClear[i+2][j] = true, foundMatch = true;
    if (foundMatch) {
        for (int i=0; i<ROWS; i++)
            for (int j=0; j<COLS; j++)
                if (toClear[i][j]) {
                    if (grid[i][j] == 5) bombMatchCount++;
                    grid[i][j] = -1, matched++;
                }
        if (updateScore && bombMatchCount >= 3) gameOver = true;
        if (updateScore && !gameOver) score += matched * 10;//score increase
    }
    return foundMatch;
}

void Game::collapseGrid() {
    for (int j=0; j<COLS; j++) {
        int emptyRow = ROWS-1;
        for (int i=ROWS-1; i>=0; i--)
            if (grid[i][j] != -1)
                grid[emptyRow--][j] = grid[i][j];
        for (int i=emptyRow; i>=0; i--)
            grid[i][j] = (rand()%100 < BOMB_RATIO) ? 5 : rand()%4;
    }
}

void Game::mouseClick(int button, int state, int x, int y) {
    if (gameOver || button != GLUT_LEFT_BUTTON || state != GLUT_DOWN) return;
    x -= BORDER_SIZE;
    y -= TOP_SPACE;
    int col = x / TILE_SIZE;
    int row = y / TILE_SIZE;
    if (row<0 || row>=ROWS || col<0 || col>=COLS) return;
    if (grid[row][col] == 5) {
        gameOver = true;
        glutPostRedisplay();
        return;
    }
    if (selectedRow == -1) {
        selectedRow = row; selectedCol = col;
    } else {
        int dr = abs(selectedRow - row);
        int dc = abs(selectedCol - col);
        if ((dr==1 && dc==0) || (dr==0 && dc==1)) {
            if (grid[row][col]==5 || grid[selectedRow][selectedCol]==5) {
                gameOver = true; glutPostRedisplay(); return;
            }
            std::swap(grid[selectedRow][selectedCol], grid[row][col]);
            if (!checkMatches(true))
                std::swap(grid[selectedRow][selectedCol], grid[row][col]);
            else collapseGrid();
        }
        selectedRow = selectedCol = -1;
        glutPostRedisplay();
    }
}

void Game::mouseClickOnLevel(int button, int state, int x, int y) {
    if (button != GLUT_LEFT_BUTTON || state != GLUT_DOWN) return;
    int winH = glutGet(GLUT_WINDOW_HEIGHT);
    int optionY = winH / 2;
    if (y > optionY - 30 && y < optionY - 10) currentLevel = EASY;
    else if (y > optionY + 0 && y < optionY + 20) currentLevel = MEDIUM;
    else if (y > optionY + 30 && y < optionY + 50) currentLevel = HARD;
    if (currentLevel != NONE) {
        setLevelConfig(currentLevel);
        glutReshapeWindow(COLS * TILE_SIZE + 2*BORDER_SIZE, ROWS * TILE_SIZE + TOP_SPACE + BORDER_SIZE);
        resetGame();
        glutPostRedisplay();
    }
}

void Game::display() {
    if (currentLevel == NONE) {
        glClear(GL_COLOR_BUFFER_BIT);
        int winW = 800, winH = 600;
        drawText(winW/2 - 60, winH/2 - 60, "Select Level", 0,0,0);
        drawText(winW/2 - 50, winH/2 - 20, "1. Easy", 0,0.0,0);
        drawText(winW/2 - 50, winH/2 + 10, "2. Medium", 0.0,0.0,0);
        drawText(winW/2 - 50, winH/2 + 40, "3. Hard", 0.0,0,0);
        glutSwapBuffers();
    } else if (gameOver) {
        displayGameOverScreen();
    } else {
        glClear(GL_COLOR_BUFFER_BIT);
        glPushMatrix();
        glTranslatef(BORDER_SIZE, TOP_SPACE, 0);
        for (int i=0; i<ROWS; i++)
            for (int j=0; j<COLS; j++)
                drawTile(i,j);
        glPopMatrix();
        glColor3f(0,0,0);//outside border color
        glLineWidth(3);
        glBegin(GL_LINE_LOOP);
        glVertex2i(BORDER_SIZE, TOP_SPACE);
        glVertex2i(BORDER_SIZE + COLS*TILE_SIZE, TOP_SPACE);
        glVertex2i(BORDER_SIZE + COLS*TILE_SIZE, TOP_SPACE + ROWS*TILE_SIZE);
        glVertex2i(BORDER_SIZE, TOP_SPACE + ROWS*TILE_SIZE);
        glEnd();
        char scoreText[30];
        sprintf(scoreText, "Score:%d", score);
        drawText(BORDER_SIZE + 10, TOP_SPACE / 2 + 10, scoreText, 0, 0, 0);//score text color
        glutSwapBuffers();
    }
}

void timer(int) {
    if (!game.gameOver && game.checkMatches(true)) {
        game.collapseGrid();
        glutPostRedisplay();
    }
    glutTimerFunc(300, timer, 0);
}

void reshape(int w, int h) {
    glViewport(0,0,w,h);
    glMatrixMode(GL_PROJECTION); glLoadIdentity();
    gluOrtho2D(0, w, h, 0);
    glMatrixMode(GL_MODELVIEW); glLoadIdentity();
}

int main(int argc, char** argv) {
    srand(time(0));
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB);
    glutInitWindowSize(800, 600);//display size
    glutCreateWindow("Chocolate Match-3 (OOP)");

    glClearColor(0.82, 0.41, 0.12, 1);//display color

    glutDisplayFunc([](){ game.display(); });
    glutMouseFunc([](int b, int s, int x, int y){
        if (game.currentLevel == NONE) game.mouseClickOnLevel(b, s, x, y);
        else game.mouseClick(b, s, x, y);
    });

    glutReshapeFunc(reshape);
    glutTimerFunc(300, timer, 0);

    glutMainLoop();
    return 0;
}
