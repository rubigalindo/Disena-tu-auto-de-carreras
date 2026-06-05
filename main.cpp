#include <SFML/Graphics.hpp>
#include <SFML/Audio.hpp>

int main() {
    sf::RenderWindow ventana(sf::VideoMode(800, 600), "Diseña tu auto de carreras");

    sf::Texture fondoTex;
    fondoTex.loadFromFile("recursos/intro.png");
    sf::Sprite fondo(fondoTex);

    sf::Font fuente;
    fuente.loadFromFile("recursos/arial.ttf");

    sf::Text titulo("Diseña tu auto de carreras", fuente, 50);
    titulo.setFillColor(sf::Color::White);
    titulo.setStyle(sf::Text::Bold);
    titulo.setPosition(100, 150);

    sf::Text botonPlay("PLAY", fuente, 40);
    botonPlay.setFillColor(sf::Color::Yellow);
    botonPlay.setPosition(350, 400);

    sf::Music musica;
    if (musica.openFromFile("recursos/musica_intro.ogg")) {
        musica.setLoop(true);
        musica.play();
    }

    sf::SoundBuffer bufferBurbuja;
    bufferBurbuja.loadFromFile("recursos/burbuja.wav");
    sf::Sound sonidoBurbuja(bufferBurbuja);

    bool enIntro = true;

    while (ventana.isOpen()) {
        sf::Event evento;
        while (ventana.pollEvent(evento)) {
            if (evento.type == sf::Event::Closed)
                ventana.close();

            if (enIntro && evento.type == sf::Event::KeyPressed && evento.key.code == sf::Keyboard::Enter) {
                sonidoBurbuja.play();
                enIntro = false; 
            }
        }

        ventana.clear();
        if (enIntro) {
            ventana.draw(fondo);
            ventana.draw(titulo);
            ventana.draw(botonPlay);
        } else {
            // Aquí irá la fase AutoMenu
        }
        ventana.display();
    }

    return 0;
}
