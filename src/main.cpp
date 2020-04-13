#include <iostream>
#include <SFML/Graphics.hpp>
#define SCREEN_WIDTH 1024
#define SCREEN_HEIGHT 768

int main() {
    sf::RenderWindow window(sf::VideoMode(SCREEN_WIDTH, SCREEN_HEIGHT), "SFML Test");
    while (window.isOpen()) {
        sf::Event event;
        while (window.pollEvent(event)) {
            switch (event.type) {
            case sf::Event::Closed:
                window.close();
                break;
            default:
                break;
            }
        }
        window.clear();
        window.display();
    }
}