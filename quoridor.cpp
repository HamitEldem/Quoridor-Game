//
//  main.cpp
//  quoridorGLUT
//
//  Created by Efe Eldem on
//
#define GL_SILENCE_DEPRECATION // for Macos --- Windows&GNU/Linux users can delete this line
#include <GLUT/glut.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>
#include <stdarg.h>

#define WINDOW_WIDTH  1400
#define WINDOW_HEIGHT 800

#define D2R 0.0174532
#define PI 3.1415


typedef struct{
    int move;
    int remaining;
    
    //0 is waiting state
    //1 is move state
    //2 is wall state
    int state;
    
    //coordinates
    int x;
    int y;
    
}player;

typedef struct{
    int state,flag;
}secondary;

typedef struct{
    int x;
    int y;
    int inputDone;
    
    int wallState;
    
    bool vWall;
    bool hWall;
    
    secondary sec;
    
    bool visited;
    
    
    
}grid;



typedef struct{
    int leftX;
    int rightX;
    int upY;
    int downY;
}border;


player p[2];
grid pos[10][8];
bool gameOver = false;
int winner = -1;
int pTurn = 0;
char prevKey = ' ';
int gridM , gridN;
int wallFlag = -1;
bool inputValid = true;
border bor;


//initialize the players
void initPlayers(){
    p[0].remaining = 10;
    p[1].remaining = 10;
    
    p[0].move = 0;
    p[1].move = 0;
    
    p[0].state = 0;
    p[1].state = 0;
    
    p[0].x = 5;
    p[0].y = -295;
   
    p[1].x = 5;
    p[1].y = 305;
  
}

//init grid borders
void initBorders(void){
    bor.upY = 305;
    bor.downY = -295;
    
    
    bor.rightX = 305;
    bor.leftX = -295;
    
}

// To draw a filled circle, centered at (x,y) with radius r
void circle(int x, int y, int r) {
    float angle;

    glBegin(GL_POLYGON);
    for (int i = 0; i < 100; i++) {
        angle = 2 * PI*i / 100;
        glVertex2f(x + r * cos(angle), y + r * sin(angle));
    }
    glEnd();
}

// Display text with variables
void vprint(int x, int y, void *font, const char *string, ...) {
    va_list ap;
    va_start(ap, string);
    char str[1024];
    vsprintf(str, string, ap);
    va_end(ap);

    int len, i;
    glRasterPos2f(x, y);
    len = (int)strlen(str);
    for (i = 0; i < len; i++)
        glutBitmapCharacter(font, str[i]);
}

void print(int x, int y, const char* string, void* font) {
    int len, i;

    glRasterPos2f(x, y);
    len = (int)strlen(string);
    for (i = 0; i < len; i++)
        glutBitmapCharacter(font, string[i]);
}


void drawPlayers(){
    glColor3f(1, 0, 0);
    circle(p[0].x, p[0].y, 20);
    
    glColor3f(1, 0.2, 1);
    circle(p[1].x, p[1].y, 20);
}

void resetGrid(void){
    for(int m=0; m<10; m++){
        for(int n=0; n<8; n++){
            pos[m][n].inputDone = 0;
            pos[m][n].wallState = -1;
            pos[m][n].hWall = 0;
            pos[m][n].vWall = 0;
            pos[m][n].sec.flag = 0;
        }
    }
}


void resetGame(void){
    if(gameOver){
        initPlayers();
        drawPlayers();
        resetGrid();
        gameOver = false;
    }
}

void addWall(void){
    for(int m = 0 ; m< 10; m++){
        for(int n = 0; n < 8; n++){
          
            if(pos[m][n].inputDone==1){
                if(pos[m][n].wallState){ // vertical -> x aynı kalacak y değişecek!
                    pos[m+1][n].vWall = true;
                    glColor3f(0, 0, 0);
                    glRectf(pos[m][n].x+2, pos[m][n].y -5 , pos[m][n].x+17, pos[m][n].y -127);
                }
                else if (!pos[m][n].wallState){
                    if(m>1){
                        pos[m-1][n].hWall = true;
                    }
                    glColor3f(0, 0, 0);
                    glRectf(pos[m][n].x - 53 , pos[m][n].y + 15, pos[m][n].x+70, pos[m][n].y);
                }
                
                    
            }
            
            if(pos[m][n].sec.flag == 1){
                if(pos[m][n].sec.state){ // vertical -> x aynı kalacak y değişecek!
                    glColor3f(0, 0, 0);
                    glRectf(pos[m][n].x+2, pos[m][n].y -5 , pos[m][n].x+17, pos[m][n].y -127);
                }
                else if (!pos[m][n].sec.state){
                    glColor3f(0, 0, 0);
                    glRectf(pos[m][n].x - 53 , pos[m][n].y + 15, pos[m][n].x+70, pos[m][n].y);
                }
            }
  
        }
        
    }
}

void drawGradient(int x1, int y1, int w, int h, float r, float g, float b) {
   glBegin(GL_QUADS);
       glColor3f(r, g, b);
       glVertex2f(x1, y1);
       glVertex2f(x1 + w, y1);
       glColor3f(r + 0.4, g + 0.4, b + 0.4);
       glVertex2f(x1 + w, y1 - h);
       glVertex2f(x1, y1 - h);
   glEnd();

   glColor3f(0.1, 0.1, 0.1);
   glBegin(GL_LINE_LOOP);
       glVertex2f(x1, y1);
       glVertex2f(x1 + w, y1);
       glVertex2f(x1 + w, y1 - h);
       glVertex2f(x1, y1 - h);
   glEnd();
}


void gameOverScreen(void){
    if(gameOver){
    
        
        drawGradient(-700, 500, 1400, 1000, 0.7, 0.2, 0.3);
        
        glColor3f(1,1,1);
        vprint(-100, 100, GLUT_BITMAP_TIMES_ROMAN_24, "GAME OVER!");
        vprint(-110, 60, GLUT_BITMAP_TIMES_ROMAN_24, "Winner is: Player %d", winner + 1);
    
        drawGradient(-180, 0, 320, 50, 0.4, 1, 0.3);
        vprint(-130, -30, GLUT_BITMAP_TIMES_ROMAN_24, "Click Here To Restart");

    }
}

void drawBoard(void){
    drawGradient(-700, 500, 1400, 1000, 0.5, 0.9, 0.5); // board background
    
    //board itself
    glColor3f(0.5, 0.5, 0.9);
    glRectf(-330, -330, 340, 340);
    
    
    //board grids
    int m,n;
    
    glColor3f(0.5, 0.9, 0.5);
    
    for(m=0; m<9; m++){
        for(n=0; n<9; n++){
            glRectf(2.5*(-350+30*n+10)+530, (-350 + 30*m +10)*2.5+530, (-350+30*n+30)*2.5+530, (-350 + 30*m+30)*2.5+530);
        }
    }
    
  
    
    glColor3f(1, 1, 1);
    if(p[pTurn].state == 1){
        for(m=0; m<10; m++){
            for(n=0; n<8; n++){
                //initialization of 2D struct array
                pos[m][n].x = -266 + 75 * n ;
                pos[m][n].y = 330 - 74 * m ;
                
                
                
                if(wallFlag == 1 && m<=7){
                    vprint(-266 + 75 * n, 300 - 74 * m, GLUT_BITMAP_HELVETICA_18, "%02d", m * 10 + n);
                }
                else if(wallFlag == 0 && m>= 1 && m<= 8){
                    vprint(-245 + 75 * n, 330 - 74 * m, GLUT_BITMAP_HELVETICA_18, "%02d", m * 10 + n);
                }
                
            }
        }
    }
    
}

void informationScreen(void){
    // player 1
    glColor3f(0, 0, 0);
    vprint(-560, 360, GLUT_BITMAP_TIMES_ROMAN_24, "Player 1");
    drawGradient(-650, 348, 250, 90, 0.9, 0.5, 0.2);
    vprint(-630, 320, GLUT_BITMAP_HELVETICA_18, "Walls Left: %d", p[0].remaining);
    vprint(-630, 290, GLUT_BITMAP_HELVETICA_18, "Moves Made: %d", p[0].move);

    
    // player 2
    vprint(490, 360, GLUT_BITMAP_TIMES_ROMAN_24, "Player 2");
    drawGradient(400, 348, 250, 90, 0.9, 0.5, 0.2);
    vprint(420, 320, GLUT_BITMAP_HELVETICA_18, "Walls Left: %d", p[1].remaining);
    vprint(420, 290, GLUT_BITMAP_HELVETICA_18, "Moves Made: %d", p[1].move);
    
    
    // i/o instruction screen
    switch (pTurn){
        case 0:
            drawGradient(-650, 200, 250, 100, 0.9, 0.5, 0.2);
            if(p[0].state == 0){
                vprint(-630, 160, GLUT_BITMAP_HELVETICA_18, "Press W to add a wall");
                vprint(-630, 120, GLUT_BITMAP_HELVETICA_18, "Use arrow keys to move");
            }
            else if(p[0].state == 1){
                vprint(-649, 160, GLUT_BITMAP_HELVETICA_18, "Press V for vertical position");
                vprint(-649, 120, GLUT_BITMAP_HELVETICA_18, "Press H for horizontal position");
                
                
                drawGradient(-646, 80, 250, 100, 0.9, 0.5, 0.2);
                vprint(-643, 50, GLUT_BITMAP_HELVETICA_18, "After Pressing V or H");
                vprint(-643, 20, GLUT_BITMAP_HELVETICA_18, "Press the grid position");
                vprint(-643, -10, GLUT_BITMAP_HELVETICA_18, "         (e.g. V22 or H22)");
            }
           

            break;
        case 1:
            drawGradient(400, 200, 250, 100, 0.9, 0.5, 0.2);
            if(p[1].state == 0){
                vprint(420, 160, GLUT_BITMAP_HELVETICA_18, "Press W to add a wall");
                vprint(420, 120, GLUT_BITMAP_HELVETICA_18, "Use arrow keys  to move");
            }
            else if(p[1].state == 1){
                vprint(403, 160, GLUT_BITMAP_HELVETICA_18, "Press V for vertical position");
                vprint(403, 120, GLUT_BITMAP_HELVETICA_18, "Press H for horizontal position");
        
                drawGradient(400, 80, 250, 100, 0.9, 0.5, 0.2);
                vprint(403, 50, GLUT_BITMAP_HELVETICA_18, "After Pressing V or H");
                vprint(403, 20, GLUT_BITMAP_HELVETICA_18, "Press the grid position");
                vprint(403, -10, GLUT_BITMAP_HELVETICA_18, "      (e.g. V22 or H22)");
        
            

            }
         
            
            break;
    }

    
    
}

// To display onto window using OpenGL commands
void display() {
    // Draw window
    glClearColor(0.8, 0.8, 0.8, 1.0);
    glClear(GL_COLOR_BUFFER_BIT);
    
    drawBoard();
    informationScreen();
    addWall();
    drawPlayers();
    if(!inputValid){
        glColor3f(0, 0, 0);
        vprint(-200, 360, GLUT_BITMAP_HELVETICA_18, "INPUT IS NOT VALID! INPUT AGAIN!");
    }
    gameOverScreen();
    
   


   
    glutSwapBuffers();
}



// Key function for ASCII charachters like ESC, a,b,c..,A,B,..Z
void onKeyDown(unsigned char key, int x, int y) {
    // Exit when ESC is pressed.
    if (key == 27)
        exit(0);
    
    if(!gameOver){
        if (key == 'W' || key == 'w'){
            if(p[pTurn].state == 0 && p[pTurn].remaining > 0){
                p[pTurn].state = 1;
                p[pTurn].remaining -= 1;
            }
        }
        if(key == 'H' || key == 'h'){
            if(p[pTurn].state){
                prevKey = key;
                wallFlag = 0;
            }
        }
        if(key == 'V' || key == 'v'){
            if(p[pTurn].state){
                prevKey = key;
                wallFlag = 1;
            }
        }
        if(key >= '0' && key <= '9'){
            if(prevKey == 'h' || prevKey == 'H' || prevKey == 'V' || prevKey == 'v'){
                gridM = key - '0';
                prevKey = key;
            }
            else if(prevKey >= '0' && prevKey <= '9'){
                if((pos[gridM][key - '0'].hWall == true  && wallFlag == 1) || (pos[gridM][key - '0'].vWall == true && wallFlag == 0)){
                    inputValid = false;
                }
                else{
                    if(pos[gridM][key - '0'].inputDone == 1){
                        if(pos[gridM][key - '0'].wallState != wallFlag){
                            // using a backup!
                            pos[gridM][key - '0'].sec.flag = 1;
                            pos[gridM][key - '0'].sec.state = wallFlag;
                            p[pTurn].state = 0;
                            pTurn = (pTurn + 1) % 2;
                            inputValid = true;
                        }
                        else{
                            inputValid = false;
                        }
                    }
                    else{
                        gridN = key - '0';
                        pos[gridM][gridN].inputDone = 1;
                        pos[gridM][gridN].wallState = wallFlag;
                        p[pTurn].state = 0;
                        pTurn = (pTurn + 1) % 2;
                        inputValid = true;
                    }
                }
            }
        }
        if(key == 'm' || key == 'M'){
            p[pTurn].state = 2;
        }
    }
    
    
    
    

    // To refresh the window it calls display() function
    glutPostRedisplay();
}

int findWallY(int y){
    for(int m=0; m<10; m++){
        for(int n=0; n<8; n++){
            if(y >= pos[m][n].y && y  <= pos[m][n].y + 15){
                if(pos[m][n].inputDone == 1 &&  pos[m][n].wallState == 0){
                     if(pos[m][n].x-53 < p[pTurn].x && pos[m][n].x + 70 > p[pTurn].x)
                         return 0;
                }
            }
                
        }
    }
    
    return 1;
}

int findWallX(int x){
    for(int m=0; m<10; m++){
        for(int n=0; n<8; n++){
            if(x >= pos[m][n].x+2 && x  <= pos[m][n].x + 17){
                if(pos[m][n].inputDone == 1 &&  pos[m][n].wallState == 1){
                    if(pos[m][n].y - 5  > p[pTurn].y && pos[m][n].y - 127 < p[pTurn].y)
                        return 0;
                }
            }
        }
    }
    
    return -900;
}

// Special Key like GLUT_KEY_F1, F2, F3,...
// Arrow Keys, GLUT_KEY_UP, GLUT_KEY_DOWN, GLUT_KEY_RIGHT, GLUT_KEY_RIGHT
void onSpecialKeyDown(int key, int x, int y) {
    // Write your codes here.

    if(!gameOver){
            switch (key) {
                case GLUT_KEY_UP:
                    if(p[pTurn].y == bor.upY){
                        inputValid = false;
                    }
                    else if(!findWallY(p[pTurn].y + 40))
                        inputValid = false;
                    else if(p[pTurn].y + 75 == p[(pTurn + 1) % 2].y && p[pTurn].x == p[(pTurn + 1) % 2].x){
                        if(p[pTurn].y + 150 > bor.upY){
                            inputValid = false;
                        }
                        else{
                            p[pTurn].y += 150;
                            inputValid = true;
                        }
                    }
                    else{
                        p[pTurn].y += 75;
                        inputValid = true;
                    }
                    break;
                case GLUT_KEY_DOWN:
             
                    if(p[pTurn].y == bor.downY){
                        inputValid = false;
                    }
                    else if(!findWallY(p[pTurn].y -40)){
                        inputValid = false;
                    }
                    else if(p[pTurn].y - 75 == p[(pTurn + 1) % 2].y && p[pTurn].x == p[(pTurn + 1) % 2].x){
                        if(p[pTurn].y - 150 < bor.downY){
                            inputValid = false;
                        }
                        else{
                            p[pTurn].y -= 150;
                            inputValid = true;
                        }
                    }
                    else{
                        p[pTurn].y -= 75;
                        inputValid = true;
                    }
                    break;
                case GLUT_KEY_LEFT:
                    if(!findWallX(p[pTurn].x -36)){
                        inputValid = false;
                    }
                    else if(p[pTurn].x == bor.leftX){
                        inputValid = false;
                    }
                    else if(p[pTurn].x - 75 == p[(pTurn + 1) % 2].x && p[pTurn].y == p[(pTurn + 1) % 2].y){
                        if(p[pTurn].x - 150 < bor.leftX){
                            inputValid = false;
                        }
                        else{
                            p[pTurn].x -= 150;
                            inputValid = true;
                        }
                    }
                    else{
                        p[pTurn].x -= 75;
                        inputValid = true;
                    }
                    break;
                case GLUT_KEY_RIGHT:
                    if(!findWallX(p[pTurn].x +36)){
                        inputValid = false;
                    }
                    else if(p[pTurn].x == bor.rightX){
                        inputValid = false;
                    }
                    else if(p[pTurn].x + 75 == p[(pTurn + 1) % 2].x && p[pTurn].y == p[(pTurn + 1) % 2].y){
                        if(p[pTurn].x + 150 > bor.rightX){
                            inputValid = false;
                        }
                        else{
                            p[pTurn].x += 150;
                            inputValid = true;
                        }
                    }
                    else{
                        p[pTurn].x += 75;
                        inputValid = true;
                    }
                    break;
            }
            
            if(p[0].y == bor.upY){
                gameOver = true;
                winner = 0;
            }
            else if(p[1].y == bor.downY){
                gameOver = true;
                winner = 1;
            }
            
            if(inputValid){
                p[pTurn].state = 0;
                p[pTurn].move++;
                pTurn = (pTurn + 1) % 2;
            }
        
        
        
    }
    // To refresh the window it calls display() function
    glutPostRedisplay();
}



// When a click occurs in the window,
// It provides which button
// buttons : GLUT_LEFT_BUTTON , GLUT_RIGHT_BUTTON
// states  : GLUT_UP , GLUT_DOWN
// x, y is the coordinate of the point that mouse clicked.

void onClick(int button, int stat, int x, int y) {
    // Check if the game is over
    if(gameOver) {
        if(button == GLUT_LEFT_BUTTON && stat == GLUT_DOWN) {
            // Convert window coordinates to game coordinates
            // Assuming the origin (0,0) is at the center of the window
            int gameX = x - WINDOW_WIDTH / 2;
            int gameY = WINDOW_HEIGHT / 2 - y;

            // Check if the click is within the reset button area
            if(gameX > -180 && gameX < 150 && gameY < 0 && gameY > -50) {
                resetGame();
            }
        }
    }

    // To refresh the window it calls display() function
    glutPostRedisplay();
}

// This function is called when the window size changes.
// w : is the new width of the window in pixels.
// h : is the new height of the window in pixels.
void onResize(int w, int h) {
    glViewport(0, 0, w, h);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glOrtho(-w / 2, w / 2, -h / 2, h / 2, -1, 1);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
}



void Init() {
    //Smoothing shapes
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glEnable(GL_LINE_SMOOTH);
    glEnable(GL_POLYGON_SMOOTH);
    glEnable(GL_POINT_SMOOTH);
}

int main(int argc, char *argv[]) {
    glutInit(&argc, argv );
    glutInitDisplayMode(GLUT_RGB | GLUT_DOUBLE);
    glutInitWindowSize(WINDOW_WIDTH, WINDOW_HEIGHT);
    glutCreateWindow("Quoridor Game!");
    

    // Window Events
    glutDisplayFunc(display);
    glutReshapeFunc(onResize);
    initPlayers();
    initBorders();
    
    // Keyboard Events
    glutKeyboardFunc(onKeyDown);
    glutSpecialFunc(onSpecialKeyDown);
    
    // Mouse Events
    glutMouseFunc(onClick);



    Init();
    glutMainLoop();
    return 0;
}
