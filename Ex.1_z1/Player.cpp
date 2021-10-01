#include "Player.h"
//#include <iostream>
//#include <unistd.h>



bool Player::Moved() const
{
    if(coords.x == old_coords.x || coords.y == old_coords.y)
        return false;
    else
        return true;
}

void Player::ProcessInput(MovementDir dir)
{
    //int move_dist = move_speed * 32;
    switch(dir)
    {
        case MovementDir::UP:
            old_coords.y = coords.y;
            coords.y++;
            break;
        case MovementDir::DOWN:
            old_coords.y = coords.y;
            coords.y--;
            break;
        case MovementDir::LEFT:
            old_coords.x = coords.x;
            coords.x--;
            break;
        case MovementDir::RIGHT:
            old_coords.x = coords.x;
            coords.x++;
            break;
        default:
            break;
    }
}

void Player::Draw(Image &screen)
{
    Image img("../resources/tex.png");
    if(Moved())
    {

        for(int y = old_coords.y; y <= old_coords.y + tileSize; ++y)
        {
            for(int x = old_coords.x; x <= old_coords.x + tileSize; ++x)
            {
                screen.PutPixel(x, y, img.GetPixel(x % img.Height(), y % img.Width()));
            }
        }
        old_coords = coords;
    }
    Image img1("../resources/g7134.png");
    for(int y = coords.y; y <= coords.y + tileSize; ++y)
    {
        for(int x = coords.x; x <= coords.x + tileSize; ++x)
        {
            Pixel tmp = img1.GetPixel(x % img1.Height(), y % img1.Width());
            Pixel t = img.GetPixel(x % img.Height(), y % img.Width());
            screen.PutPixel(x, y, {.r = tmp.r * (tmp.a / 255) + (1 - tmp.a / 255) * t.r,
                                   .g = tmp.g * (tmp.a / 255) + (1 - tmp.a / 255) * t.g,
                                   .b = tmp.b * (tmp.a / 255) + (1 - tmp.a / 255) * t.b,
                                   .a = 255});
            //screen.PutPixel(x, y, {.r = tmp.r * tmp.a + (255 - tmp.a) * t.r, .g = tmp.g * tmp.a + (255 - tmp.a) * t.g, .b = tmp.b * tmp.a + (255 - tmp.a) * t.b, .a = 255});
        }
    }
}

void Player::DrawYouWin(Image &screen)
{

    Image img("../resources/You_win.jpg");
    for(int y = 0; y <= 1024 + tileSize; ++y)
    {
        for(int x = 0; x <= 1024 + tileSize; ++x)
        {
            screen.PutPixel(x, y, backgroundColor);
        }
    }

    for(int y = 0; y < img.Height(); ++y)
    {
        for(int x = 0; x < img.Width(); ++x)
        {
            screen.PutPixel(x, y, img.GetPixel(x, y));
        }
    }
}

void Player::DrawGameOver(Image &screen)
{

    Image img("../resources/Game_over.jpeg");
    for (int y = 0; y <= 1024 + tileSize; ++y)
    {
        for (int x = 0; x <= 1024 + tileSize; ++x)
        {
            screen.PutPixel(x, y, backgroundColor);
        }
    }

    for (int y = 0; y < img.Height(); ++y)
    {
        for (int x = 0; x < img.Width(); ++x)
        {
            screen.PutPixel(x, y, img.GetPixel(x, y));
        }
    }
}

void Player::Light(Image &screen, int x, int y)
{
    //usleep(1000);
    Pixel col{.r = 255, .g = 219, .b = 139, .a = 150};
    //Pixel col{.r = 250, .g = 128, .b = 114, .a = 150};
    //Pixel col{.r = 0, .g = 128, .b = 128, .a = 150};
    //Pixel col{.r = 255, .g = 99, .b = 71, .a = 150};
    double a = 150.0;
    double res = 0.0;
    int rr, gg, bb;
    int n = 100;
    double param = 100;
    for (int i = y - n; i <= y + n; i++) {
        for (int j = x - n; j <= x + n; j++) {
            if ((i - y) * (i - y) + (j - x) * (j - x) <= n * n) {
                Pixel tmp = screen.GetPixel(j, i);
                res = (int)col.r * (a / 255.0) + (1.0 - a / 255.0) * (int)tmp.r;
                rr = floor(res);
                //std::cout << (a / 255.0) << "   " << res << " " << (unsigned int)rr << std::endl;
                res = (int)col.g * (a / 255.0) + (1.0 - a / 255.0) * (int)tmp.g;
                gg = floor(res);
                //std::cout << res << " " << gg << std::endl;
                res = (int)col.b * (a / 255.0) + (1.0 - a / 255.0) * (int)tmp.b;
                bb = floor(res);
                //std::cout << res << " " << bb << std::endl;
                screen.PutPixel(j, i, {.r = (unsigned int)rr, .g = (unsigned int)gg, .b = (unsigned int)bb, .a = 255});
                a = param - param * ((i - y) * (i - y) + (j - x) * (j - x)) / (n * n);
            }
        }
    }
}

void Player::DrawMap(Image &screen, char (&a)[32][32], int cl)
{
    Image wall("../resources/wall.png");
    Image empty("../resources/grass.png");
    Image door("../resources/door.png");
    Image player("../resources/player.png");
    Image flame("../resources/sticky_flame.png");
    Image floor("../resources/floor7.png");
    Image lava("../resources/lava3.png");
    Image ghost("../resources/ghost.png");
    Image coin("../resources/coin4.png");
    Image tornado("../resources/air_elemental.png");
    Image enemy("../resources/enemy.png");

    int lights[cl][2];
    int t = 0;
    if(!Moved())
    {
        for (int i = 0; i < tileSize; i++) {
            for (int j = 0; j < tileSize; j++) {
                switch (a[i][j]) {
                    case '#':
                        for (int y = 0; y < tileSize; y++) {
                            for (int x = 0; x < tileSize; x++) {
                                screen.PutPixel(j * tileSize + x, i * tileSize + y, wall.GetPixel(x, y));
                            }
                        }
                        break;

                    case ' ':
                        for (int y = 0; y < tileSize; y++) {
                            for (int x = 0; x < tileSize; x++) {
                                screen.PutPixel(j * tileSize + x, i * tileSize + y, backgroundColor);
                            }
                        }
                        break;

                    case '.':
                        for (int y = 0; y < tileSize; y++) {
                            for (int x = 0; x < tileSize; x++) {
                                screen.PutPixel(j * tileSize + x, i * tileSize + y, floor.GetPixel(x, y));
                            }
                        }
                        break;

                    case 'f':
                        for (int y = 0; y < tileSize; y++) {
                            for (int x = 0; x < tileSize; x++) {
                                screen.PutPixel(j * tileSize + x, i * tileSize + y, lava.GetPixel(x, y));
                            }
                        }
                        break;

                    case 'e':
                        for (int y = 0; y < tileSize; y++) {
                            for (int x = 0; x < tileSize; x++) {
                                screen.PutPixel(j * tileSize + x, i * tileSize + y, floor.GetPixel(x, y));
                            }
                        }
                        break;

                    case 'h':
                        for (int y = 0; y < tileSize; y++) {
                            for (int x = 0; x < tileSize; x++) {
                                screen.PutPixel(j * tileSize + x, i * tileSize + y, floor.GetPixel(x, y));
                            }
                        }
                        break;

                    case 't':
                        for (int y = 0; y < tileSize; y++) {
                            for (int x = 0; x < tileSize; x++) {
                                screen.PutPixel(j * tileSize + x, i * tileSize + y, floor.GetPixel(x, y));
                            }
                        }
                        break;

                    case 'R':
                        for (int y = 0; y < tileSize; y++) {
                            for (int x = 0; x < tileSize; x++) {
                                screen.PutPixel(j * tileSize + x, i * tileSize + y, door.GetPixel(x, y));
                            }
                        }
                        break;

                    case 'L':
                        for (int y = 0; y < tileSize; y++) {
                            for (int x = 0; x < tileSize; x++) {
                                screen.PutPixel(j * tileSize + x, i * tileSize + y, door.GetPixel(x, y));
                            }
                        }
                        break;

                    case 'U':
                        for (int y = 0; y < tileSize; y++) {
                            for (int x = 0; x < tileSize; x++) {
                                screen.PutPixel(j * tileSize + x, i * tileSize + y, door.GetPixel(x, y));
                            }
                        }
                        break;

                    case 'D':
                        for (int y = 0; y < tileSize; y++) {
                            for (int x = 0; x < tileSize; x++) {
                                screen.PutPixel(j * tileSize + x, i * tileSize + y, door.GetPixel(x, y));
                            }
                        }
                        break;

                    case 'Q':
                        for (int y = 0; y < tileSize; y++) {
                            for (int x = 0; x < tileSize; x++) {
                                screen.PutPixel(j * tileSize + x, i * tileSize + y, door.GetPixel(x, y));
                            }
                        }
                        break;

                    case '@':
                        for (int y = 0; y < tileSize; y++) {
                            for (int x = 0; x < tileSize; x++) {
                                Pixel tmp = player.GetPixel(x, y);
                                Pixel fl = floor.GetPixel(x, y);
                                screen.PutPixel(j * tileSize + x, i * tileSize + y,
                                                {.r = tmp.r * (tmp.a / 255) + (1 - tmp.a / 255) * fl.r,
                                                 .g = tmp.g * (tmp.a / 255) + (1 - tmp.a / 255) * fl.g,
                                                 .b = tmp.b * (tmp.a / 255) + (1 - tmp.a / 255) * fl.b,
                                                 .a = 255});
                            }
                        }
                        break;

                    case 'T':
                        for (int y = 0; y < tileSize; y++) {
                            for (int x = 0; x < tileSize; x++) {
                                Pixel tmp = tornado.GetPixel(x, y);
                                Pixel fl = floor.GetPixel(x, y);
                                screen.PutPixel(j * tileSize + x, i * tileSize + y,
                                                {.r = tmp.r * (tmp.a / 255) + (1 - tmp.a / 255) * fl.r,
                                                        .g = tmp.g * (tmp.a / 255) + (1 - tmp.a / 255) * fl.g,
                                                        .b = tmp.b * (tmp.a / 255) + (1 - tmp.a / 255) * fl.b,
                                                        .a = 255});
                            }
                        }
                        break;

                    case 'E':
                        for (int y = 0; y < tileSize; y++) {
                            for (int x = 0; x < tileSize; x++) {
                                Pixel tmp = enemy.GetPixel(x, y);
                                Pixel fl = floor.GetPixel(x, y);
                                screen.PutPixel(j * tileSize + x, i * tileSize + y,
                                                {.r = tmp.r * (tmp.a / 255) + (1 - tmp.a / 255) * fl.r,
                                                        .g = tmp.g * (tmp.a / 255) + (1 - tmp.a / 255) * fl.g,
                                                        .b = tmp.b * (tmp.a / 255) + (1 - tmp.a / 255) * fl.b,
                                                        .a = 255});
                            }
                        }
                        break;

                    case 'H':
                        for (int y = 0; y < tileSize; y++) {
                            for (int x = 0; x < tileSize; x++) {
                                Pixel tmp = ghost.GetPixel(x, y);
                                Pixel fl = floor.GetPixel(x, y);
                                screen.PutPixel(j * tileSize + x, i * tileSize + y,
                                                {.r = tmp.r * (tmp.a / 255) + (1 - tmp.a / 255) * fl.r,
                                                        .g = tmp.g * (tmp.a / 255) + (1 - tmp.a / 255) * fl.g,
                                                        .b = tmp.b * (tmp.a / 255) + (1 - tmp.a / 255) * fl.b,
                                                        .a = 255});
                            }
                        }
                        break;

                    case 'G':
                        for (int y = 0; y < tileSize; y++) {
                            for (int x = 0; x < tileSize; x++) {
                                Pixel tmp = coin.GetPixel(x, y);
                                Pixel fl = floor.GetPixel(x, y);
                                screen.PutPixel(j * tileSize + x, i * tileSize + y,
                                                {.r = tmp.r * (tmp.a / 255) + (1 - tmp.a / 255) * fl.r,
                                                        .g = tmp.g * (tmp.a / 255) + (1 - tmp.a / 255) * fl.g,
                                                        .b = tmp.b * (tmp.a / 255) + (1 - tmp.a / 255) * fl.b,
                                                        .a = 255});
                            }
                        }

                        break;

                    case 'C':
                        for (int y = 0; y < tileSize; y++) {
                            for (int x = 0; x < tileSize; x++) {
                                Pixel tmp = flame.GetPixel(x, y);
                                Pixel fl = floor.GetPixel(x, y);
                                screen.PutPixel(j * tileSize + x, i * tileSize + y,
                                                {.r = tmp.r * (tmp.a / 255) + (1 - tmp.a / 255) * fl.r,
                                                        .g = tmp.g * (tmp.a / 255) + (1 - tmp.a / 255) * fl.g,
                                                        .b = tmp.b * (tmp.a / 255) + (1 - tmp.a / 255) * fl.b,
                                                        .a = 255});
                            }
                        }
                        lights[t][0] = j * tileSize + tileSize / 2;
                        lights[t][1] = i * tileSize + tileSize / 2;
                        t++;
                        //Light(screen, j * tileSize + tileSize / 2, i * tileSize + tileSize / 2);
                        break;
                }
            }
        }

        for (int i = 0; i < t; i++) {
            Light(screen, lights[i][0], lights[i][1]);
        }
    }
    else
    {
        switch (a[old_coords.y][old_coords.x]) {
            case '#':
                for (int y = 0; y < tileSize; y++) {
                    for (int x = 0; x < tileSize; x++) {
                        screen.PutPixel(old_coords.x * tileSize + x, old_coords.y * tileSize + y, wall.GetPixel(x, y));
                    }
                }
                break;

            case ' ':
                for (int y = 0; y < tileSize; y++) {
                    for (int x = 0; x < tileSize; x++) {
                        screen.PutPixel(old_coords.x * tileSize + x, old_coords.y * tileSize + y, backgroundColor);
                    }
                }
                break;

            case '.':
                for (int y = 0; y < tileSize; y++) {
                    for (int x = 0; x < tileSize; x++) {
                        screen.PutPixel(old_coords.x * tileSize + x, old_coords.y * tileSize + y, floor.GetPixel(x, y));
                    }
                }
                break;

            case 'f':
                for (int y = 0; y < tileSize; y++) {
                    for (int x = 0; x < tileSize; x++) {
                        screen.PutPixel(old_coords.x * tileSize + x, old_coords.y * tileSize + y, lava.GetPixel(x, y));
                    }
                }
                break;

            case 'e':
                for (int y = 0; y < tileSize; y++) {
                    for (int x = 0; x < tileSize; x++) {
                        screen.PutPixel(old_coords.x * tileSize + x, old_coords.y * tileSize + y, ghost.GetPixel(x, y));
                    }
                }
                break;

            case 'R':
                for (int y = 0; y < tileSize; y++) {
                    for (int x = 0; x < tileSize; x++) {
                        screen.PutPixel(old_coords.x * tileSize + x, old_coords.y * tileSize + y, door.GetPixel(x, y));
                    }
                }
                break;

            case 'L':
                for (int y = 0; y < tileSize; y++) {
                    for (int x = 0; x < tileSize; x++) {
                        screen.PutPixel(old_coords.x * tileSize + x, old_coords.y * tileSize + y, door.GetPixel(x, y));
                    }
                }
                break;

            case 'U':
                for (int y = 0; y < tileSize; y++) {
                    for (int x = 0; x < tileSize; x++) {
                        screen.PutPixel(old_coords.x * tileSize + x, old_coords.y * tileSize + y, door.GetPixel(x, y));
                    }
                }
                break;

            case 'D':
                for (int y = 0; y < tileSize; y++) {
                    for (int x = 0; x < tileSize; x++) {
                        screen.PutPixel(old_coords.x * tileSize + x, old_coords.y * tileSize + y, door.GetPixel(x, y));
                    }
                }
                break;

            case 'Q':
                for (int y = 0; y < tileSize; y++) {
                    for (int x = 0; x < tileSize; x++) {
                        screen.PutPixel(old_coords.x * tileSize + x, old_coords.y * tileSize + y, door.GetPixel(x, y));
                    }
                }
                break;

            /*case '@':
                for (int y = 0; y < tileSize; y++) {
                    for (int x = 0; x < tileSize; x++) {
                        Pixel tmp = player.GetPixel(x, y);
                        Pixel fl = screen.GetPixel(x, y);
                        screen.PutPixel(old_coords.x * tileSize + x, old_coords.y * tileSize + y,
                                        {.r = tmp.r * (tmp.a / 255) + (1 - tmp.a / 255) * fl.r,
                                                .g = tmp.g * (tmp.a / 255) + (1 - tmp.a / 255) * fl.g,
                                                .b = tmp.b * (tmp.a / 255) + (1 - tmp.a / 255) * fl.b,
                                                .a = 255});
                    }
                }
                break;*/

            case 'G':
                for (int y = 0; y < tileSize; y++) {
                    for (int x = 0; x < tileSize; x++) {
                        Pixel tmp = coin.GetPixel(x, y);
                        Pixel fl = floor.GetPixel(x, y);
                        screen.PutPixel(old_coords.x * tileSize + x, old_coords.y * tileSize + y,
                                        {.r = tmp.r * (tmp.a / 255) + (1 - tmp.a / 255) * fl.r,
                                                .g = tmp.g * (tmp.a / 255) + (1 - tmp.a / 255) * fl.g,
                                                .b = tmp.b * (tmp.a / 255) + (1 - tmp.a / 255) * fl.b,
                                                .a = 255});
                    }
                }
                break;

            case 'C':
                for (int y = 0; y < tileSize; y++) {
                    for (int x = 0; x < tileSize; x++) {
                        Pixel tmp = flame.GetPixel(x, y);
                        Pixel fl = floor.GetPixel(x, y);
                        screen.PutPixel(old_coords.x * tileSize + x, old_coords.y * tileSize + y,
                                        {.r = tmp.r * (tmp.a / 255) + (1 - tmp.a / 255) * fl.r,
                                                .g = tmp.g * (tmp.a / 255) + (1 - tmp.a / 255) * fl.g,
                                                .b = tmp.b * (tmp.a / 255) + (1 - tmp.a / 255) * fl.b,
                                                .a = 255});
                    }
                }
                lights[t][0] = old_coords.x * tileSize + tileSize / 2;
                lights[t][1] = old_coords.y * tileSize + tileSize / 2;
                t++;
                //Light(screen, j * tileSize + tileSize / 2, i * tileSize + tileSize / 2);
                break;
        }
        /*for (int y = 0; y < tileSize; y++) {
            for (int x = 0; x < tileSize; x++) {
                screen.PutPixel(old_coords.x * tileSize + x, old_coords.y * tileSize + y, floor.GetPixel(x, y));
            }
        }*/
        old_coords = coords;
        for (int y = 0; y < tileSize; y++) {
            for (int x = 0; x < tileSize; x++) {
                Pixel tmp = player.GetPixel(x, y);
                Pixel fl = floor.GetPixel(x, y);
                screen.PutPixel(coords.x * tileSize + x, coords.y * tileSize + y,
                                {.r = tmp.r * (tmp.a / 255) + (1 - tmp.a / 255) * fl.r,
                                        .g = tmp.g * (tmp.a / 255) + (1 - tmp.a / 255) * fl.g,
                                        .b = tmp.b * (tmp.a / 255) + (1 - tmp.a / 255) * fl.b,
                                        .a = 255});
            }
        }
        for (int i = 0; i < t; i++) {
            Light(screen, lights[i][0], lights[i][1]);
        }
    }
}

void Player::Coin(Image &screen, int x, int y, int t)
{
    std::string path = "../resources/coin?.png";
    switch (t) {
        case 1:
            path[17] = '1';
            break;

        case 2:
            path[17] = '2';
            break;

        case 3:
            path[17] = '3';
            break;

        case 4:
            path[17] = '4';
            break;

        case 5:
            path[17] = '5';
            break;

        case 6:
            path[17] = '6';
            break;

        case 7:
            path[17] = '7';
            break;

        case 8:
            path[17] = '8';
            break;

    }
    Image coins(path);
    Image floors("../resources/floor7.png");
    for (int i = 0; i < tileSize; i++) {
        for (int j = 0; j < tileSize; j++) {
            Pixel coin = coins.GetPixel(j, i);
            Pixel floor = floors.GetPixel(j, i);
            screen.PutPixel(x * tileSize + j, y * tileSize + i, {.r = coin.r * (coin.a / 255) + (1 - coin.a / 255) * floor.r,
                    .g = coin.g * (coin.a / 255) + (1 - coin.a / 255) * floor.g,
                    .b = coin.b * (coin.a / 255) + (1 - coin.a / 255) * floor.b,
                    .a = 255});
        }
    }
}

void Player::Ghost(Image &screen, int x, int y, int dx, int dy)
{
    Image ghost("../resources/ghost.png");
    Image floor("../resources/floor7.png");
    for (int i = 0; i < tileSize; i++) {
        for (int j = 0; j < tileSize; j++) {
            screen.PutPixel(x * tileSize + j, y * tileSize + i, floor.GetPixel(j, i));
        }
    }
    x += dx;
    y += dy;
    for (int i = 0; i < tileSize; i++) {
        for (int j = 0; j < tileSize; j++) {
            Pixel tmp = ghost.GetPixel(j, i);
            Pixel fl = floor.GetPixel(j, i);
            screen.PutPixel(x * tileSize + j, y * tileSize + i,
                            {.r = tmp.r * (tmp.a / 255) + (1 - tmp.a / 255) * fl.r,
                                    .g = tmp.g * (tmp.a / 255) + (1 - tmp.a / 255) * fl.g,
                                    .b = tmp.b * (tmp.a / 255) + (1 - tmp.a / 255) * fl.b,
                                    .a = 255});
        }
    }
}

void Player::Enemy(Image &screen, int x, int y, int dx, int dy)
{
    Image enemy("../resources/enemy.png");
    Image floor("../resources/floor7.png");
    for (int i = 0; i < tileSize; i++) {
        for (int j = 0; j < tileSize; j++) {
            screen.PutPixel(x * tileSize + j, y * tileSize + i, floor.GetPixel(j, i));
        }
    }
    x += dx;
    y += dy;
    for (int i = 0; i < tileSize; i++) {
        for (int j = 0; j < tileSize; j++) {
            Pixel tmp = enemy.GetPixel(j, i);
            Pixel fl = floor.GetPixel(j, i);
            screen.PutPixel(x * tileSize + j, y * tileSize + i,
                            {.r = tmp.r * (tmp.a / 255) + (1 - tmp.a / 255) * fl.r,
                                    .g = tmp.g * (tmp.a / 255) + (1 - tmp.a / 255) * fl.g,
                                    .b = tmp.b * (tmp.a / 255) + (1 - tmp.a / 255) * fl.b,
                                    .a = 255});
        }
    }
}

void Player::Tornado(Image &screen, int x, int y, int dx, int dy)
{
    Image tornado("../resources/air_elemental.png");
    Image floor("../resources/floor7.png");
    for (int i = 0; i < tileSize; i++) {
        for (int j = 0; j < tileSize; j++) {
            screen.PutPixel(x * tileSize + j, y * tileSize + i, floor.GetPixel(j, i));
        }
    }
    x += dx;
    y += dy;
    for (int i = 0; i < tileSize; i++) {
        for (int j = 0; j < tileSize; j++) {
            Pixel tmp = tornado.GetPixel(j, i);
            Pixel fl = floor.GetPixel(j, i);
            screen.PutPixel(x * tileSize + j, y * tileSize + i,
                            {.r = tmp.r * (tmp.a / 255) + (1 - tmp.a / 255) * fl.r,
                                    .g = tmp.g * (tmp.a / 255) + (1 - tmp.a / 255) * fl.g,
                                    .b = tmp.b * (tmp.a / 255) + (1 - tmp.a / 255) * fl.b,
                                    .a = 255});
        }
    }
}

void Player::Fog(Image &screen)
{
    Pixel col{.r = 123, .g = 104, .b = 238, .a = 150};
    double a = 50, res;
    int rr, gg, bb;
    for (int i = 0; i <= 1024; i++) {
        for (int j = 0; j <= 1024; j++) {
            Pixel tmp = screen.GetPixel(j, i);
            res = (int)col.r * (a / 255.0) + (1.0 - a / 255.0) * (int)tmp.r;
            rr = floor(res);
            //std::cout << (a / 255.0) << "   " << res << " " << (unsigned int)rr << std::endl;
            res = (int)col.g * (a / 255.0) + (1.0 - a / 255.0) * (int)tmp.g;
            gg = floor(res);
            //std::cout << res << " " << gg << std::endl;
            res = (int)col.b * (a / 255.0) + (1.0 - a / 255.0) * (int)tmp.b;
            bb = floor(res);
            //std::cout << res << " " << bb << std::endl;
            screen.PutPixel(j, i, {.r = (unsigned int)rr, .g = (unsigned int)gg, .b = (unsigned int)bb, .a = 255});
            //a = param - param * ((i - y) * (i - y) + (j - x) * (j - x)) / (n * n);
        }
    }
}

void Player::Effect(Image &screen)
{
    Pixel col{.r = 0, .g = 0, .b = 0, .a = 0};
    double a = 75, res;
    int rr, gg, bb;
    for (int i = 0; i <= 1024; i++) {
        for (int j = 0; j <= 1024; j++) {
            Pixel tmp = screen.GetPixel(j, i);
            res = (int)col.r * (a / 255.0) + (1.0 - a / 255.0) * (int)tmp.r;
            rr = floor(res);
            //std::cout << (a / 255.0) << "   " << res << " " << (unsigned int)rr << std::endl;
            res = (int)col.g * (a / 255.0) + (1.0 - a / 255.0) * (int)tmp.g;
            gg = floor(res);
            //std::cout << res << " " << gg << std::endl;
            res = (int)col.b * (a / 255.0) + (1.0 - a / 255.0) * (int)tmp.b;
            bb = floor(res);
            //std::cout << res << " " << bb << std::endl;
            screen.PutPixel(j, i, {.r = (unsigned int)rr, .g = (unsigned int)gg, .b = (unsigned int)bb, .a = 255});
            //a = param - param * ((i - y) * (i - y) + (j - x) * (j - x)) / (n * n);
        }
    }
}