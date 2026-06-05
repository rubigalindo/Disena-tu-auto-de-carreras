#include <SFML/Graphics.hpp>
#include <SFML/Audio.hpp>

int main() {
    sf::RenderWindow ventana(sf::VideoMode({800, 600}), "Diseña tu auto de carreras");

    sf::Texture fondoTex;
    fondoTex.openFromFile("recursos/intro.png");
    sf::Sprite fondo(fondoTex);

    sf::Font fuente;
    fuente.openFromFile("recursos/arial.ttf");

    sf::Text titulo(fuente, "Diseña tu auto de carreras", 50);
    titulo.setFillColor(sf::Color::White);
    titulo.setStyle(sf::Text::Bold);
    titulo.setPosition({100.f, 150.f});

    sf::Text botonPlay(fuente, "PLAY", 40);
    botonPlay.setFillColor(sf::Color::Yellow);
    botonPlay.setPosition({350.f, 400.f});

    sf::Music musica;
    if (musica.openFromFile("recursos/musica_intro.ogg")) {
        musica.setRepeating(true);
        musica.play();
    }

    sf::SoundBuffer bufferBurbuja;
    bufferBurbuja.openFromFile("recursos/burbuja.wav");
    sf::Sound sonidoBurbuja(bufferBurbuja);

    bool enIntro = true;

    while (ventana.isOpen()) {
        while (auto evento = ventana.pollEvent()) {
            if (evento->is<sf::Event::Closed>())
                ventana.close();

            if (enIntro && evento->is<sf::Event::KeyPressed>()) {
                auto keyEvent = evento->getIf<sf::Event::KeyPressed>();
                if (keyEvent && keyEvent->code == sf::Keyboard::Key::Enter) {
                    sonidoBurbuja.play();
                    enIntro = false;
                }
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
