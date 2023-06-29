#include <SFML/Graphics.hpp>
#include <SFML/Config.hpp>
#include <SFML/System.hpp>

#include <iostream>
#include <array>

constexpr auto WIDTH = 3;
constexpr auto HEIGHT = 3;

void SetPixelsToTexture(std::array<uint8_t, 9Ui64>& states, sf::Texture& texture);

int main2() {
	sf::RenderWindow window(sf::VideoMode(600, 600), "SFML works!");
	window.setFramerateLimit(60);

	std::array<uint8_t, WIDTH* HEIGHT> states =
	{
		1, 0, 1,
		0, 1, 0,
		1, 0, 1
	};

	sf::Texture texture;
	texture.create(WIDTH, HEIGHT);

	sf::Sprite sprite;
	sprite.setScale(200.0, 200.0);

	bool useState = true;

	while (window.isOpen()) {
		sf::Event event;
		while (window.pollEvent(event)) {
			if (event.type == sf::Event::Closed)
				window.close();
		}

		SetPixelsToTexture(states, texture);
		sprite.setTexture(texture);

		window.clear();
		window.draw(sprite);
		window.display();
	}

	return 0;
}

void SetPixelsToTexture(std::array<uint8_t, 9Ui64>& states, sf::Texture& texture) {
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