#include <SFML/Graphics.hpp>
#include <SFML/Audio.hpp>

int main() {
    sf::RenderWindow ventana(sf::VideoMode({800, 600}), "Diseña tu auto de carreras");

    // Fondo
    sf::Texture fondoTex;
    if (!fondoTex.loadFromFile("recursos/intro.png")) {
        return -1;
    }
    sf::Sprite fondo(fondoTex);

    // Fuente y textos
    sf::Font fuente;
    if (!fuente.openFromFile("recursos/arial.ttf")) {
        return -1;
    }

    sf::Text titulo(fuente, "Diseña tu auto de carreras", 50);
    titulo.setFillColor(sf::Color::White);
    titulo.setStyle(sf::Text::Bold);
    titulo.setPosition({100.f, 150.f});

    sf::Text botonPlay(fuente, "PLAY", 40);
    botonPlay.setFillColor(sf::Color::Yellow);
    botonPlay.setPosition({350.f, 400.f});

    // Música de fondo
    sf::Music musica;
    if (musica.openFromFile("recursos/musica_intro.ogg")) {
        musica.play();
    }

    // Sonido burbuja
    sf::SoundBuffer bufferBurbuja;
    if (!bufferBurbuja.loadFromFile("recursos/burbuja.wav")) {
        return -1;
    }
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
