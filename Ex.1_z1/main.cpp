#include "common.h"
#include "Image.h"
#include "Player.h"

#define GLFW_DLL
#include <GLFW/glfw3.h>

constexpr GLsizei WINDOW_WIDTH = 1024, WINDOW_HEIGHT = 1024;

struct InputState
{
    bool keys[1024]{}; //массив состояний кнопок - нажата/не нажата
    GLfloat lastX = 400, lastY = 300; //исходное положение мыши
    bool firstMouse = true;
    bool captureMouse         = true;  // Мышка захвачена нашим приложением или нет?
    bool capturedMouseJustNow = false;
} static Input;


GLfloat deltaTime = 0.0f;
GLfloat lastFrame = 0.0f;

bool fog = false;
bool effect = false;
int te = -1;

enum Status
{
    WALL,
    DOOR,
    GAME_OVER,
    YOU_WIN,
    COIN,
    OK
};



void OnKeyboardPressed(GLFWwindow* window, int key, int scancode, int action, int mode)
{
    switch (key)
    {
    case GLFW_KEY_ESCAPE:
        if (action == GLFW_PRESS)
            glfwSetWindowShouldClose(window, GL_TRUE);
        break;
    case GLFW_KEY_1:
        glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
        break;
    case GLFW_KEY_2:
        glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
        break;
    default:
        if (action == GLFW_PRESS)
            Input.keys[key] = true;
        else if (action == GLFW_RELEASE)
            Input.keys[key] = false;
    }
}

int processPlayerMovement(Player &player)
{
    // usleep(100);
    if (Input.keys[GLFW_KEY_S]) {
        player.ProcessInput(MovementDir::UP);
        return 5;
    }
    else if (Input.keys[GLFW_KEY_W]) {
        player.ProcessInput(MovementDir::DOWN);
        return 4;
    }
    else if (Input.keys[GLFW_KEY_A]) {
        player.ProcessInput(MovementDir::LEFT);
        return 6;
    }
    else if (Input.keys[GLFW_KEY_D]) {
        player.ProcessInput(MovementDir::RIGHT);
        return 3;
    }
    if (Input.keys[GLFW_KEY_F]) {
        fog = true;
        return 1;
    }
    return 0;
}

void OnMouseButtonClicked(GLFWwindow* window, int button, int action, int mods)
{
    if (button == GLFW_MOUSE_BUTTON_RIGHT && action == GLFW_RELEASE)
        Input.captureMouse = !Input.captureMouse;

    if (Input.captureMouse)
    {
        glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
        Input.capturedMouseJustNow = true;
    }
    else
        glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);

}

void OnMouseMove(GLFWwindow* window, double xpos, double ypos)
{
    if (Input.firstMouse)
    {
        Input.lastX = float(xpos);
        Input.lastY = float(ypos);
        Input.firstMouse = false;
    }

    GLfloat xoffset = float(xpos) - Input.lastX;
    GLfloat yoffset = Input.lastY - float(ypos);

    Input.lastX = float(xpos);
    Input.lastY = float(ypos);
}


void OnMouseScroll(GLFWwindow* window, double xoffset, double yoffset)
{
    // ...
}


int initGL()
{
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        std::cout << "Failed to initialize OpenGL context" << std::endl;
        return -1;
    }

    std::cout << "Vendor: " << glGetString(GL_VENDOR) << std::endl;
    std::cout << "Renderer: " << glGetString(GL_RENDERER) << std::endl;
    std::cout << "Version: " << glGetString(GL_VERSION) << std::endl;
    std::cout << "GLSL: " << glGetString(GL_SHADING_LANGUAGE_VERSION) << std::endl;

    std::cout << "Controls: "<< std::endl;
    std::cout << "press right mouse button to capture/release mouse cursor  "<< std::endl;
    std::cout << "W, A, S, D - movement  "<< std::endl;
    std::cout << "press ESC to exit" << std::endl;

    return 0;
}

Status is_valid(char (&a)[32][32], int y, int x)
{
    if (a[y][x] == '#' || a[y][x] == 'C') {
        return WALL;
    }
    else if (a[y][x] == 'L' || a[y][x] == 'R' || a[y][x] == 'U' || a[y][x] == 'D') {
        return DOOR;
    }
    else if (a[y][x] == 'G') {
        return COIN;
    }
    else if (a[y][x] == 'Q') {
        return YOU_WIN;
    }
    else if (a[y][x] == 'f' || a[y][x] == 'E' || a[y][x] == 'H' || a[y][x] == 'T') {
        return GAME_OVER;
    }
    return OK;
}

int main(int argc, char** argv)
{
    if(!glfwInit())
        return -1;

    //	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    //	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    //	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_RESIZABLE, GL_FALSE);

    GLFWwindow*  window = glfwCreateWindow(WINDOW_WIDTH, WINDOW_HEIGHT, "Welcome to the Game!", nullptr, nullptr);
    if (window == nullptr)
    {
        std::cout << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return -1;
    }

    glfwMakeContextCurrent(window);

    glfwSetKeyCallback        (window, OnKeyboardPressed);
    glfwSetCursorPosCallback  (window, OnMouseMove);
    glfwSetMouseButtonCallback(window, OnMouseButtonClicked);
    glfwSetScrollCallback     (window, OnMouseScroll);

    if(initGL() != 0)
    return -1;

    //Reset any OpenGL errors which could be present for some reason
    GLenum gl_error = glGetError();
    while (gl_error != GL_NO_ERROR)
        gl_error = glGetError();
    int q = 0;



    //Image img("../resources/tex.png");
    Image screenBuffer(WINDOW_WIDTH, WINDOW_HEIGHT, 4);

    glViewport(0, 0, WINDOW_WIDTH, WINDOW_HEIGHT);  GL_CHECK_ERRORS;
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f); GL_CHECK_ERRORS;

    char RoomMap[5][4] = {0};
    char CurrentRoom[32][32];

    //game loop
    std::ifstream fin;
    fin.open("../resources/RoomMap.txt");
    char *buff = new char[5];
    buff[4] = 0;
    for (int i = 0; i < 5; i++) {
        fin.getline(buff, 5);
        for (int j = 0; j < 4; j++) {
            RoomMap[i][j] = buff[j];
        }
    }
    fin.close();

    int RoomCoordX = 0, RoomCoordY = 0;

    std::string path = "../resources/Room?.txt";
    path[17] = RoomMap[RoomCoordY][RoomCoordX];

    int cl = 0;
    int coins = 0;
    int carr[10][2];
    int ghostX = 0, ghostY = 0;
    int enemyX = 0, enemyY = 0;
    int tornadoX = 0, tornadoY = 0;

    bool is_ghost = false;
    bool is_enemy = false;
    bool is_tornado = false;

    int c;
    fin.open(path);
    int k = 0, l = 0;
    while ((c = fin.get()) != EOF) {
        if (c == '\n') {
            l = 0;
            k++;
        }
        else {
            CurrentRoom[k][l] = (char)c;
            if ((char)c == 'C')
                cl++;
            if ((char)c == 'G') {
                carr[coins][0] = k;
                carr[coins][1] = l;
                coins++;
            }
            if ((char)c == 'H') {
                is_ghost = true;
                ghostY = k;
                ghostX = l;
            }
            if ((char)c == 'T') {
                is_tornado = true;
                tornadoY = k;
                tornadoX = l;
            }
            if ((char)c == 'E') {
                is_enemy = true;
                enemyY = k;
                enemyX = l;
            }
                //coins++;
            l++;
        }
    }
    fin.close();

    int xx, yy;
    for (int i = 0; i < tileSize; ++i) {
        for (int j = 0; j < tileSize; ++j) {
            if (CurrentRoom[i][j] == 'L' && CurrentRoom[i - 1][j] == '#') {
                CurrentRoom[i][j] = '@';
                xx = j; //* tileSize;
                yy = i; // * tileSize;
                i = tileSize;
                j = tileSize;
            }
        }
    }

    Point starting_pos{.x = xx, .y = yy};
    Player player{starting_pos};
    player.DrawMap(screenBuffer, CurrentRoom, cl);
    //xx /= tileSize;
    //yy /= tileSize;


    int time = 0;
    char prev = 'L';
    int prevx = 0, prevy = 0;
    int ef = 0;

    while (!glfwWindowShouldClose(window))
    {
        GLfloat currentFrame = glfwGetTime();
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;

        glfwPollEvents();

        //processPlayerMovement(player);
        //player.Draw(screenBuffer);
        if (time % 6 == 0) {
            switch (processPlayerMovement(player)) {
                /*case 1:
                    player.Fog(screenBuffer);
                    break;*/
                case 5:
                {
                    Status t = is_valid(CurrentRoom, yy + 1, xx);
                    if (t == OK) {
                        CurrentRoom[yy][xx] = prev;
                        q = 0;
                        player.DrawMap(screenBuffer, CurrentRoom, cl);
                        if (fog && q < 1) {
                            q = 1;
                            player.Fog(screenBuffer);
                        }
                        prev = CurrentRoom[yy + 1][xx];
                        CurrentRoom[yy + 1][xx] = '@';
                        yy++;
                        q = 0;
                        player.DrawMap(screenBuffer, CurrentRoom, cl);
                        if (fog && q < 1) {
                            q = 1;
                            player.Fog(screenBuffer);
                        }
                    }
                    if (t == YOU_WIN) {
                        effect = true;
                        te = 0;
                        for (int i = 0; i < tileSize; i++)
                            for (int j = 0; j < tileSize; j++)
                                CurrentRoom[i][j] = ' ';
                        //player.DrawYouWin(screenBuffer);
                    }
                    if (t == GAME_OVER) {
                        effect = true;
                        te = 1;
                        for (int i = 0; i < tileSize; i++)
                            for (int j = 0; j < tileSize; j++)
                                CurrentRoom[i][j] = ' ';
                        //player.DrawGameOver(screenBuffer);
                    }
                    if (t == COIN) {
                        CurrentRoom[yy][xx] = prev;
                        prev = '.';
                        CurrentRoom[yy + 1][xx] = '@';
                        yy++;
                        q = 0;
                        player.DrawMap(screenBuffer, CurrentRoom, cl);
                        if (fog && q < 1) {
                            q = 1;
                            player.Fog(screenBuffer);
                        }
                        Image smile("../resources/smile.png");
                        for (int i = 0; i < tileSize; i++) {
                            for (int j = 0; j < tileSize; j++) {
                                if (smile.GetPixel(j, i).a != 0)
                                    screenBuffer.PutPixel(xx * tileSize + j, yy * tileSize + i, smile.GetPixel(j, i));
                            }
                        }
                        //coin disappear function!!!
                    }
                    if (t == WALL) {
                        player.ProcessInput(MovementDir::DOWN);
                    }
                    if (t == DOOR) {
                        RoomCoordY++;
                        path[17] = RoomMap[RoomCoordY][RoomCoordX];

                        cl = 0;
                        fin.open(path);
                        k = 0;
                        l = 0;
                        coins = 0;
                        ghostX = 0;
                        ghostY = 0;
                        enemyX = 0;
                        enemyY = 0;
                        tornadoX = 0;
                        tornadoY = 0;
                        is_ghost = false;
                        is_enemy = false;
                        is_tornado = false;
                        prevx = 0;
                        prevy = 0;
                        fog = false;

                        while ((c = fin.get()) != EOF) {
                            if (c == '\n') {
                                l = 0;
                                k++;
                            }
                            else {
                                CurrentRoom[k][l] = (char)c;
                                if ((char)c == 'C')
                                    cl++;
                                if ((char)c == 'G') {
                                    carr[coins][0] = k;
                                    carr[coins][1] = l;
                                    coins++;
                                }
                                if ((char)c == 'H') {
                                    is_ghost = true;
                                    ghostY = k;
                                    ghostX = l;
                                }
                                if ((char)c == 'T') {
                                    is_tornado = true;
                                    tornadoY = k;
                                    tornadoX = l;
                                }
                                if ((char)c == 'E') {
                                    is_enemy = true;
                                    enemyY = k;
                                    enemyX = l;
                                }
                                l++;
                            }
                        }
                        fin.close();
                        for (int i = 0; i < tileSize; ++i) {
                            for (int j = 0; j < tileSize; ++j) {
                                if (CurrentRoom[i][j] == 'U' && CurrentRoom[i][j - 1] == '#') {
                                    CurrentRoom[i][j] = '@';
                                    xx = j; //* tileSize;
                                    yy = i; // * tileSize;
                                    i = tileSize;
                                    j = tileSize;
                                }
                            }
                        }
                        prev = 'D';
                        player.NewRoom(xx, yy);
                        q = 0;
                        effect = true;
                        te = 2;
                        //player.DrawMap(screenBuffer, CurrentRoom, cl);
                    }
                    break;
                }

                case 4:
                {
                    Status t = is_valid(CurrentRoom, yy - 1, xx);
                    if (t == OK) {
                        CurrentRoom[yy][xx] = prev;
                        q = 0;
                        player.DrawMap(screenBuffer, CurrentRoom, cl);
                        if (fog && q < 1) {
                            q = 1;
                            player.Fog(screenBuffer);
                        }
                        prev = CurrentRoom[yy - 1][xx];
                        CurrentRoom[yy - 1][xx] = '@';
                        yy--;
                        q = 0;
                        player.DrawMap(screenBuffer, CurrentRoom, cl);
                        if (fog && q < 1) {
                            q = 1;
                            player.Fog(screenBuffer);
                        }
                    }
                    if (t == YOU_WIN) {
                        effect = true;
                        te = 0;
                        for (int i = 0; i < tileSize; i++)
                            for (int j = 0; j < tileSize; j++)
                                CurrentRoom[i][j] = ' ';
                        //player.DrawYouWin(screenBuffer);
                    }
                    if (t == GAME_OVER) {
                        effect = true;
                        te = 1;
                        for (int i = 0; i < tileSize; i++)
                            for (int j = 0; j < tileSize; j++)
                                CurrentRoom[i][j] = ' ';
                        //player.DrawGameOver(screenBuffer);
                    }
                    if (t == COIN) {
                        CurrentRoom[yy][xx] = prev;
                        prev = '.';
                        CurrentRoom[yy - 1][xx] = '@';
                        yy--;
                        q = 0;
                        player.DrawMap(screenBuffer, CurrentRoom, cl);
                        if (fog && q < 1) {
                            q = 1;
                            player.Fog(screenBuffer);
                        }
                        Image smile("../resources/smile.png");
                        for (int i = 0; i < tileSize; i++) {
                            for (int j = 0; j < tileSize; j++) {
                                if (smile.GetPixel(j, i).a != 0)
                                    screenBuffer.PutPixel(xx * tileSize + j, yy * tileSize + i, smile.GetPixel(j, i));
                            }
                        }
                        //coin disappear function!!!
                    }
                    if (t == WALL) {
                        player.ProcessInput(MovementDir::UP);
                    }
                    if (t == DOOR) {
                        RoomCoordY--;
                        path[17] = RoomMap[RoomCoordY][RoomCoordX];

                        cl = 0;
                        fin.open(path);
                        k = 0;
                        l = 0;
                        coins = 0;
                        ghostX = 0;
                        ghostY = 0;
                        enemyX = 0;
                        enemyY = 0;
                        tornadoX = 0;
                        tornadoY = 0;
                        is_ghost = false;
                        is_enemy = false;
                        is_tornado = false;
                        prevx = 0;
                        prevy = 0;
                        fog = false;

                        while ((c = fin.get()) != EOF) {
                            if (c == '\n') {
                                l = 0;
                                k++;
                            }
                            else {
                                CurrentRoom[k][l] = (char)c;
                                if ((char)c == 'C')
                                    cl++;
                                if ((char)c == 'G') {
                                    carr[coins][0] = k;
                                    carr[coins][1] = l;
                                    coins++;
                                }
                                l++;
                            }
                        }
                        fin.close();
                        for (int i = 0; i < tileSize; ++i) {
                            for (int j = 0; j < tileSize; ++j) {
                                if (CurrentRoom[i][j] == 'D' && CurrentRoom[i][j - 1] == '#') {
                                    CurrentRoom[i][j] = '@';
                                    xx = j; //* tileSize;
                                    yy = i; // * tileSize;
                                    i = tileSize;
                                    j = tileSize;
                                }
                            }
                        }
                        prev = 'U';
                        player.NewRoom(xx, yy);
                        q = 0;
                        effect = true;
                        te = 2;
                        //player.DrawMap(screenBuffer, CurrentRoom, cl);
                    }
                    break;
                }

                case 6:
                {
                    Status t = is_valid(CurrentRoom, yy, xx - 1);
                    if (t == OK) {
                        CurrentRoom[yy][xx] = prev;
                        q = 0;
                        player.DrawMap(screenBuffer, CurrentRoom, cl);
                        if (fog && q < 1) {
                            q = 1;
                            player.Fog(screenBuffer);
                        }
                        prev = CurrentRoom[yy][xx - 1];
                        CurrentRoom[yy][xx - 1] = '@';
                        xx--;
                        q = 0;
                        player.DrawMap(screenBuffer, CurrentRoom, cl);
                        if (fog && q < 1) {
                            q = 1;
                            player.Fog(screenBuffer);
                        }
                    }
                    if (t == YOU_WIN) {
                        effect = true;
                        te = 0;
                        for (int i = 0; i < tileSize; i++)
                            for (int j = 0; j < tileSize; j++)
                                CurrentRoom[i][j] = ' ';
                        //player.DrawYouWin(screenBuffer);
                    }
                    if (t == GAME_OVER) {
                        effect = true;
                        te = 1;
                        for (int i = 0; i < tileSize; i++)
                            for (int j = 0; j < tileSize; j++)
                                CurrentRoom[i][j] = ' ';
                        //player.DrawGameOver(screenBuffer);
                    }
                    if (t == COIN) {
                        CurrentRoom[yy][xx] = prev;
                        prev = '.';
                        CurrentRoom[yy][xx - 1] = '@';
                        xx--;
                        q = 0;
                        player.DrawMap(screenBuffer, CurrentRoom, cl);
                        if (fog && q < 1) {
                            q = 1;
                            player.Fog(screenBuffer);
                        }
                        Image smile("../resources/smile.png");
                        for (int i = 0; i < tileSize; i++) {
                            for (int j = 0; j < tileSize; j++) {
                                if (smile.GetPixel(j, i).a != 0)
                                    screenBuffer.PutPixel(xx * tileSize + j, yy * tileSize + i, smile.GetPixel(j, i));
                            }
                        }
                        //coin disappear function!!!
                    }
                    if (t == WALL) {
                        player.ProcessInput(MovementDir::RIGHT);
                    }
                    if (t == DOOR) {
                        RoomCoordX--;
                        path[17] = RoomMap[RoomCoordY][RoomCoordX];

                        cl = 0;
                        fin.open(path);
                        k = 0;
                        l = 0;
                        coins = 0;
                        ghostX = 0;
                        ghostY = 0;
                        enemyX = 0;
                        enemyY = 0;
                        tornadoX = 0;
                        tornadoY = 0;
                        is_ghost = false;
                        is_enemy = false;
                        is_tornado = false;
                        prevx = 0;
                        prevy = 0;
                        fog = false;

                        while ((c = fin.get()) != EOF) {
                            if (c == '\n') {
                                l = 0;
                                k++;
                            }
                            else {
                                CurrentRoom[k][l] = (char)c;
                                if ((char)c == 'C')
                                    cl++;
                                if ((char)c == 'G') {
                                    carr[coins][0] = k;
                                    carr[coins][1] = l;
                                    coins++;
                                }
                                if ((char)c == 'H') {
                                    is_ghost = true;
                                    ghostY = k;
                                    ghostX = l;
                                }
                                if ((char)c == 'T') {
                                    is_tornado = true;
                                    tornadoY = k;
                                    tornadoX = l;
                                }
                                if ((char)c == 'E') {
                                    is_enemy = true;
                                    enemyY = k;
                                    enemyX = l;
                                }
                                l++;
                            }
                        }
                        fin.close();
                        for (int i = 0; i < tileSize; ++i) {
                            for (int j = 0; j < tileSize; ++j) {
                                if (CurrentRoom[i][j] == 'R' && CurrentRoom[i + 1][j] == '#') {
                                    CurrentRoom[i][j] = '@';
                                    xx = j; //* tileSize;
                                    yy = i; // * tileSize;
                                    i = tileSize;
                                    j = tileSize;
                                }
                            }
                        }
                        prev = 'L';
                        player.NewRoom(xx, yy);
                        q = 0;
                        effect = true;
                        te = 2;
                        //player.DrawMap(screenBuffer, CurrentRoom, cl);
                    }
                    break;
                }

                case 3:
                {
                    Status t = is_valid(CurrentRoom, yy, xx + 1);
                    if (t == OK) {
                        CurrentRoom[yy][xx] = prev;
                        q = 0;
                        player.DrawMap(screenBuffer, CurrentRoom, cl);
                        if (fog && q < 1) {
                            q = 1;
                            player.Fog(screenBuffer);
                        }
                        prev = CurrentRoom[yy][xx + 1];
                        CurrentRoom[yy][xx + 1] = '@';
                        xx++;
                        q = 0;
                        player.DrawMap(screenBuffer, CurrentRoom, cl);
                        if (fog && q < 1) {
                            q = 1;
                            player.Fog(screenBuffer);
                        }
                    }
                    if (t == YOU_WIN) {
                        effect = true;
                        te = 0;
                        for (int i = 0; i < tileSize; i++)
                            for (int j = 0; j < tileSize; j++)
                                CurrentRoom[i][j] = ' ';
                        //player.DrawYouWin(screenBuffer);
                    }
                    if (t == GAME_OVER) {
                        effect = true;
                        te = 1;
                        for (int i = 0; i < tileSize; i++)
                            for (int j = 0; j < tileSize; j++)
                                CurrentRoom[i][j] = ' ';
                        //player.DrawGameOver(screenBuffer);
                    }
                    if (t == COIN) {
                        CurrentRoom[yy][xx] = prev;
                        prev = '.';
                        CurrentRoom[yy][xx + 1] = '@';
                        xx++;
                        q = 0;
                        player.DrawMap(screenBuffer, CurrentRoom, cl);
                        if (fog && q < 1) {
                            q = 1;
                            player.Fog(screenBuffer);
                        }
                        Image smile("../resources/smile.png");
                        for (int i = 0; i < tileSize; i++) {
                            for (int j = 0; j < tileSize; j++) {
                                if (smile.GetPixel(j, i).a != 0)
                                    screenBuffer.PutPixel(xx * tileSize + j, yy * tileSize + i, smile.GetPixel(j, i));
                            }
                        }
                        //coin disappear function!!!
                    }
                    if (t == WALL) {
                        player.ProcessInput(MovementDir::LEFT);
                    }
                    if (t == DOOR) {
                        RoomCoordX++;
                        path[17] = RoomMap[RoomCoordY][RoomCoordX];

                        cl = 0;
                        fin.open(path);
                        k = 0;
                        l = 0;
                        coins = 0;
                        ghostX = 0;
                        ghostY = 0;
                        enemyX = 0;
                        enemyY = 0;
                        tornadoX = 0;
                        tornadoY = 0;
                        is_ghost = false;
                        is_enemy = false;
                        is_tornado = false;
                        prevx = 0;
                        prevy = 0;
                        fog = false;

                        while ((c = fin.get()) != EOF) {
                            if (c == '\n') {
                                l = 0;
                                k++;
                            }
                            else {
                                CurrentRoom[k][l] = (char)c;
                                if ((char)c == 'C')
                                    cl++;
                                if ((char)c == 'G') {
                                    carr[coins][0] = k;
                                    carr[coins][1] = l;
                                    coins++;
                                }
                                if ((char)c == 'H') {
                                    is_ghost = true;
                                    ghostY = k;
                                    ghostX = l;
                                }
                                if ((char)c == 'T') {
                                    is_tornado = true;
                                    tornadoY = k;
                                    tornadoX = l;
                                }
                                if ((char)c == 'E') {
                                    is_enemy = true;
                                    enemyY = k;
                                    enemyX = l;
                                }
                                l++;
                            }
                        }
                        fin.close();
                        for (int i = 0; i < tileSize; ++i) {
                            for (int j = 0; j < tileSize; ++j) {
                                if (CurrentRoom[i][j] == 'L' && CurrentRoom[i + 1][j] == '#') {
                                    CurrentRoom[i][j] = '@';
                                    xx = j; //* tileSize;
                                    yy = i; // * tileSize;
                                    i = tileSize;
                                    j = tileSize;
                                }
                            }
                        }
                        prev = 'R';
                        player.NewRoom(xx, yy);
                        q = 0;
                        effect = true;
                        te = 2;
                        //player.DrawMap(screenBuffer, CurrentRoom, cl);
                    }
                    break;
                }
            }
            /*for (int i = 0; i < 32; i++) {
                for (int j = 0; j < 32; j++) {
                    std::cout << CurrentRoom[i][j];
                }
                std::cout << std::endl;
            }*/

            //time = 0;
        }
        time++;

        if (time % 6 == 0) {
            for (int i = 0; i < coins; i++) {
                if (CurrentRoom[carr[i][0]][carr[i][1]] == 'G')
                    player.Coin(screenBuffer, carr[i][1], carr[i][0], time % 8 + 1);
            }
        }
        if (time % 8 == 0) {
            if (is_ghost) {
                if (CurrentRoom[ghostY][ghostX + 1] == '@' || CurrentRoom[ghostY][ghostX - 1] == '@'
                || CurrentRoom[ghostY - 1][ghostX] == '@' || CurrentRoom[ghostY + 1][ghostX] == '@') {
                    for (int i = 0; i < tileSize; i++)
                        for (int j = 0; j < tileSize; j++)
                            CurrentRoom[i][j] = ' ';
                    player.DrawGameOver(screenBuffer);
                }
                else if (CurrentRoom[ghostY][ghostX + 1] == 'h' && (prevx != ghostX + 1)) {
                    prevx = ghostX;
                    prevy = ghostY;
                    player.Ghost(screenBuffer, ghostX, ghostY, 1, 0);
                    CurrentRoom[ghostY][ghostX] = 'h';
                    CurrentRoom[ghostY][ghostX + 1] = 'H';
                    ghostX++;
                } else if (CurrentRoom[ghostY + 1][ghostX] == 'h' && (prevy != ghostY + 1)) {
                    prevx = ghostX;
                    prevy = ghostY;
                    player.Ghost(screenBuffer, ghostX, ghostY, 0, 1);
                    CurrentRoom[ghostY][ghostX] = 'h';
                    CurrentRoom[ghostY + 1][ghostX] = 'H';
                    ghostY++;
                } else if (CurrentRoom[ghostY][ghostX - 1] == 'h' && (prevx != ghostX - 1)) {
                    prevx = ghostX;
                    prevy = ghostY;
                    player.Ghost(screenBuffer, ghostX, ghostY, -1, 0);
                    CurrentRoom[ghostY][ghostX] = 'h';
                    CurrentRoom[ghostY][ghostX - 1] = 'H';
                    ghostX--;
                } else if (CurrentRoom[ghostY - 1][ghostX] == 'h' && (prevy != ghostY - 1)) {
                    prevx = ghostX;
                    prevy = ghostY;
                    player.Ghost(screenBuffer, ghostX, ghostY, 0, -1);
                    CurrentRoom[ghostY][ghostX] = 'h';
                    CurrentRoom[ghostY - 1][ghostX] = 'H';
                    ghostY--;
                }
            }

            if (is_enemy) {
                if (CurrentRoom[enemyY][enemyX + 1] == '@' || CurrentRoom[enemyY][enemyX - 1] == '@'
                    || CurrentRoom[enemyY - 1][enemyX] == '@' || CurrentRoom[enemyY + 1][enemyX] == '@') {
                    for (int i = 0; i < tileSize; i++)
                        for (int j = 0; j < tileSize; j++)
                            CurrentRoom[i][j] = ' ';
                    player.DrawGameOver(screenBuffer);
                }
                else if (CurrentRoom[enemyY][enemyX + 1] == 'e'  && (prevx != enemyX + 1)) {
                    prevx = enemyX;
                    prevy = enemyY;
                    player.Enemy(screenBuffer, enemyX, enemyY, 1, 0);
                    CurrentRoom[enemyY][enemyX] = 'e';
                    CurrentRoom[enemyY][enemyX + 1] = 'E';
                    enemyX++;
                }
                else if (CurrentRoom[enemyY + 1][enemyX] == 'e' && (prevy != enemyY + 1)) {
                    prevx = enemyX;
                    prevy = enemyY;
                    player.Enemy(screenBuffer, enemyX, enemyY, 0, 1);
                    CurrentRoom[enemyY][enemyX] = 'e';
                    CurrentRoom[enemyY + 1][enemyX] = 'E';
                    enemyY++;
                }
                else if (CurrentRoom[enemyY][enemyX - 1] == 'e' && (prevx != enemyX - 1)) {
                    prevx = enemyX;
                    prevy = enemyY;
                    player.Enemy(screenBuffer, enemyX, enemyY, -1, 0);
                    CurrentRoom[enemyY][enemyX] = 'e';
                    CurrentRoom[enemyY][enemyX - 1] = 'E';
                    enemyX--;
                }
                else if (CurrentRoom[enemyY - 1][enemyX] == 'e' && (prevy != enemyY - 1)) {
                    prevx = enemyX;
                    prevy = enemyY;
                    player.Enemy(screenBuffer, enemyX, enemyY, 0, -1);
                    CurrentRoom[enemyY][enemyX] = 'e';
                    CurrentRoom[enemyY - 1][enemyX] = 'E';
                    enemyY--;
                }

            }

            if (is_tornado) {
                if (CurrentRoom[tornadoY][tornadoX + 1] == '@' || CurrentRoom[tornadoY][tornadoX - 1] == '@'
                    || CurrentRoom[tornadoY - 1][tornadoX] == '@' || CurrentRoom[tornadoY + 1][tornadoX] == '@') {
                    for (int i = 0; i < tileSize; i++)
                        for (int j = 0; j < tileSize; j++)
                            CurrentRoom[i][j] = ' ';
                    player.DrawGameOver(screenBuffer);
                }
                else if (CurrentRoom[tornadoY][tornadoX + 1] == 't' && (prevx != tornadoX + 1)) {
                    prevx = tornadoX;
                    prevy = tornadoY;
                    player.Tornado(screenBuffer, tornadoX, tornadoY, 1, 0);
                    CurrentRoom[tornadoY][tornadoX] = 't';
                    CurrentRoom[tornadoY][tornadoX + 1] = 'T';
                    tornadoX++;
                } else if (CurrentRoom[tornadoY + 1][tornadoX] == 't' && (prevy != tornadoY + 1)) {
                    prevx = tornadoX;
                    prevy = tornadoY;
                    player.Tornado(screenBuffer, tornadoX, tornadoY, 0, 1);
                    CurrentRoom[tornadoY][tornadoX] = 't';
                    CurrentRoom[tornadoY + 1][tornadoX] = 'T';
                    tornadoY++;
                } else if (CurrentRoom[tornadoY][tornadoX - 1] == 't' && (prevx != tornadoX - 1)) {
                    prevx = tornadoX;
                    prevy = tornadoY;
                    player.Tornado(screenBuffer, tornadoX, tornadoY, -1, 0);
                    CurrentRoom[tornadoY][tornadoX] = 't';
                    CurrentRoom[tornadoY][tornadoX - 1] = 'T';
                    tornadoX--;
                } else if (CurrentRoom[tornadoY - 1][tornadoX] == 't' && (prevy != tornadoY - 1)) {
                    prevx = tornadoX;
                    prevy = tornadoY;
                    player.Tornado(screenBuffer, tornadoX, tornadoY, 0, -1);
                    CurrentRoom[tornadoY][tornadoX] = 't';
                    CurrentRoom[tornadoY - 1][tornadoX] = 'T';
                    tornadoY--;
                }
            }
        }
        if (fog && q < 1) {
            q = 1;
            player.Fog(screenBuffer);
        }
        if (effect && ef < 10) {
            ef++;
            player.Effect(screenBuffer);
            //player.DrawGameOver(screenBuffer);
        }
        if (ef == 10) {
            effect = false;
            ef = 0;
            if (te == 0)
                player.DrawYouWin(screenBuffer);
            if (te == 1)
                player.DrawGameOver(screenBuffer);
            if (te == 2)
                player.DrawMap(screenBuffer, CurrentRoom, cl);
        }
        

        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); GL_CHECK_ERRORS;
        glRasterPos2f(-1, 1);
        glPixelZoom(1, -1);
        glDrawPixels (WINDOW_WIDTH, WINDOW_HEIGHT, GL_RGBA, GL_UNSIGNED_BYTE, screenBuffer.Data()); GL_CHECK_ERRORS;

        glfwSwapBuffers(window);
    }

    glfwTerminate();
    return 0;
}
