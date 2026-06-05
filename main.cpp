#include <SFML/Graphics.hpp>
#include <SFML/Audio.hpp>
#include <iostream>

// Clase Boton
class Boton {
private:
    sf::Text texto;
public:
    Boton(const sf::Font& fuente, const std::string& label, unsigned int size, sf::Vector2f posicion, sf::Color color) {
        texto.setFont(fuente);
        texto.setString(label);
        texto.setCharacterSize(size);
        texto.setFillColor(color);
        texto.setPosition(posicion);
    }

    void draw(sf::RenderWindow& ventana) {
        ventana.draw(texto);
    }

    bool isClicked(sf::RenderWindow& ventana, sf::Vector2i mousePixel) {
        sf::Vector2f mousePos = ventana.mapPixelToCoords(mousePixel);
        return texto.getGlobalBounds().contains(mousePos);
    }
};

// Clase IntroMenu
class IntroMenu {
private:
    sf::Sprite fondo;
    sf::Text titulo;
    Boton botonPlay;
    sf::Music musica;
    sf::Sound sonidoBurbuja;
    bool activo;

public:
    IntroMenu(sf::RenderWindow& ventana, const sf::Font& fuente, sf::Texture& fondoTex, sf::SoundBuffer& bufferBurbuja)
        : botonPlay(fuente, "PLAY", 70,
            sf::Vector2f(ventana.getSize().x / 2.f - 100.f, 400.f),
            sf::Color::Yellow) {

        fondo.setTexture(fondoTex);
        fondo.setScale(sf::Vector2f(0.7f, 0.7f));
        fondo.setPosition(sf::Vector2f(0.f, 60.f));

        titulo.setFont(fuente);
        titulo.setString(L"Diseña tu auto de carreras");
        titulo.setCharacterSize(50);
        titulo.setFillColor(sf::Color::White);
        titulo.setStyle(sf::Text::Bold);
        titulo.setPosition(sf::Vector2f(
            ventana.getSize().x / 2.f - titulo.getLocalBounds().width / 2.f,
            150.f
        ));

        if (musica.openFromFile("recursos/musica_intro.ogg")) {
            musica.setLoop(true);
            musica.play();
        }

        sonidoBurbuja.setBuffer(bufferBurbuja);
        activo = true;
    }

    bool estaActivo() const { return activo; }

    void manejarEvento(sf::RenderWindow& ventana, const sf::Event& evento) {
        if (!activo) return;

        if (evento.type == sf::Event::KeyPressed && evento.key.code == sf::Keyboard::Enter) {
            sonidoBurbuja.play();
            activo = false;
        }

        if (evento.type == sf::Event::MouseButtonPressed && evento.mouseButton.button == sf::Mouse::Left) {
            if (botonPlay.isClicked(ventana, {evento.mouseButton.x, evento.mouseButton.y})) {
                sonidoBurbuja.play();
                activo = false;
            }
        }

        if (evento.type == sf::Event::Resized) {
            sf::View view(sf::FloatRect(0.f, 0.f, evento.size.width, evento.size.height));
            ventana.setView(view);
        }
    }

    void draw(sf::RenderWindow& ventana) {
        if (!activo) return;
        ventana.draw(fondo);
        ventana.draw(titulo);
        botonPlay.draw(ventana);
    }
};

// Programa principal
int main() {
    sf::RenderWindow ventana(sf::VideoMode(1000, 800), "Diseña tu auto de carreras");

    sf::Texture fondoTex;
    if (!fondoTex.loadFromFile("recursos/intro.png")) return -1;

    sf::Font fuente;
    if (!fuente.openFromFile("recursos/arial.ttf")) return -1;

    sf::SoundBuffer bufferBurbuja;
    if (!bufferBurbuja.loadFromFile("recursos/burbuja.wav")) return -1;

    IntroMenu intro(ventana, fuente, fondoTex, bufferBurbuja);

    while (ventana.isOpen()) {
        sf::Event evento;
        while (ventana.pollEvent(evento)) {
            if (evento.type == sf::Event::Closed) ventana.close();
            intro.manejarEvento(ventana, evento);
        }

        ventana.clear();
        if (intro.estaActivo()) {
            intro.draw(ventana);
        } else {
            // Aquí irá la fase AutoMenu
        }
        ventana.display();
    }
    return 0;
}


