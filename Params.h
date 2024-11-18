#pragma once
#include <bitset>
#define MAX_COMPONENTS 32
#define APP_COLOR sf::Color(0, 255, 51)
#define APP_COLOR_TRANSP sf::Color(0, 255, 51, 50)
#define BGD_COLOR sf::Color(20, 20, 20)
#define OVERLAY_COLOR sf::Color(0, 0, 0, 150)
#define M_PI 3.14159265358979323846f
#define M_GRAVITY 9.8f
#define M_DOWN sf::Vector2f(0, 1)
//#define DEBUG

using Bitmask = std::bitset<MAX_COMPONENTS>;
