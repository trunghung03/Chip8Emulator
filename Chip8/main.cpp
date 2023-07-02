#define _CRT_SECURE_NO_WARNINGS

#include <SFML/Graphics.hpp>
#include <SFML/Config.hpp>
#include <SFML/System.hpp>

#include <iostream>
#include <array>

#include "chip8.hpp"

struct Chip8 chip8;

sf::RenderWindow window(sf::VideoMode(1200, 600), "Chip8 VM");
sf::Texture texture;
sf::Sprite sprite;

void SetPixelsToTexture(std::array<uint8_t, 2048Ui64>& states);
void render();
void inputHandling();

void initGraphics();

int main(int argc, char* argv[]) {
	initGraphics();
	LoadROM(argv[1], chip8);

	while (window.isOpen()) {
		inputHandling();

		emulator(chip8);

		SetPixelsToTexture(chip8.graphics);
		sprite.setTexture(texture);

		render();
	}

	return 0;
}

void initGraphics() {
	window.setFramerateLimit(60);
	texture.create(WIDTH, HEIGHT);
	sprite.setScale(18.0, 18.0);
}

void inputHandling() {
	sf::Event event;
	while (window.pollEvent(event)) {
		if (event.type == sf::Event::Closed)
			window.close();
	}
}

void render() {
	window.clear();
	window.draw(sprite);
	window.display();
}

void SetPixelsToTexture(std::array<uint8_t, 2048Ui64>& states) {
	sf::Uint8* pixels = new sf::Uint8[WIDTH * HEIGHT * 4]; // * 4 because pixels have 4 components (RGBA)
	for (int i = 0; i < states.size(); i++) {
		if (states[i]) {
			pixels[4 * i] = 255;
			pixels[4 * i + 1] = 255;
			pixels[4 * i + 2] = 255;
			pixels[4 * i + 3] = 255;
		}
		else {
			pixels[4 * i] = 0;
			pixels[4 * i + 1] = 0;
			pixels[4 * i + 2] = 0;
			pixels[4 * i + 3] = 255;
		}
	}
	texture.update(pixels);
}

/* Note: I need to figure out what's the best way to get stuff on the screen
* One way is to some how read all the pixels in array and scale it up to display.
* One possible way is from (https://github.com/jborza/emuchip8/blob/master/main.c)
* Create a sf::Texture, and set the individual pixels on that by using update.
*
* Another naive way is to just draw squares,
*
* Problem: not knowing how to load the program. Cowgod said it programs starts at 0x0200,
* so do we just load prog to 0x0200 or from 0x0000.
* It loads from 0x200 as per Ayman Bagabas' guide (PC starts at 0x200)
*
*
* Graphics is stored as a separate array
* */