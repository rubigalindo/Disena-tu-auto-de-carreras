#include <SFML/Graphics.hpp>
#include <SFML/Audio.hpp>
#include <string>
#include <vector>

bool enAutoMenu = false;
int seleccionado = 0; // índice del auto seleccionado
sf::Texture autosTex[5];
std::vector<sf::Sprite> autos;
sf::RectangleShape marcos[5];
sf::Clock relojLED;
bool colorRojo = true;

int main() {
    sf::RenderWindow ventana(sf::VideoMode({1000, 800}), "Dise\u00F1a tu auto de carreras");

    // Fondo
    sf::Texture fondoTex;
    if (!fondoTex.loadFromFile("recursos/intro.png")) return -1;
    sf::Sprite fondo(fondoTex);
    fondo.setScale(sf::Vector2f(0.7f, 0.7f));
    fondo.setPosition(sf::Vector2f(0.f, 60.f));

    // Fuente y textos
    sf::Font fuente;
    if (!fuente.openFromFile("recursos/arial.ttf")) return -1;

    sf::Text titulo(fuente, sf::String(L"Dise\u00F1a tu auto de carreras"), 50);
    titulo.setFillColor(sf::Color::White);
    titulo.setStyle(sf::Text::Bold);
    titulo.setPosition(sf::Vector2f(
        ventana.getSize().x / 2.f - titulo.getLocalBounds().size.x / 2.f,
        150.f
    ));

    sf::Text botonPlay(fuente, sf::String(L"PLAY"), 70);
    botonPlay.setFillColor(sf::Color::Yellow);
    botonPlay.setPosition(sf::Vector2f(
        ventana.getSize().x / 2.f - botonPlay.getLocalBounds().size.x / 2.f,
        400.f
    ));

    // Texto AutoMenu
    sf::Text textoAutoMenu(fuente, sf::String(L"Selecciona tu auto de carreras"), 40);
    textoAutoMenu.setFillColor(sf::Color::Blue);
    textoAutoMenu.setStyle(sf::Text::Bold);
    textoAutoMenu.setPosition(sf::Vector2f(
        ventana.getSize().x / 2.f - textoAutoMenu.getLocalBounds().size.x / 2.f,
        50.f
    ));

    // Música Intro
    sf::Music musicaIntro;
    if (musicaIntro.openFromFile("recursos/musica_intro.ogg")) {
        musicaIntro.setLooping(true);
        musicaIntro.play();
    }

    // Música AutoMenu
    sf::Music musicaAutoMenu;

    // Sonido burbuja
    sf::SoundBuffer bufferBurbuja;
    if (!bufferBurbuja.loadFromFile("recursos/burbuja.wav")) return -1;
    sf::Sound sonidoBurbuja(bufferBurbuja);

    bool enIntro = true;

    // Cargar autos
    for (int i = 0; i < 5; i++) {
        std::string nombre = "recursos/auto" + std::to_string(i+1) + ".jpg";
        if (!autosTex[i].loadFromFile(nombre)) return -1;

        sf::Sprite sprite(autosTex[i]);
        sprite.setScale(sf::Vector2f(0.2f, 0.2f));

        if (i < 3)
            sprite.setPosition(sf::Vector2f(150.f + i*250.f, 150.f));
        else
            sprite.setPosition(sf::Vector2f(250.f + (i-3)*250.f, 450.f));

        autos.push_back(sprite);

        // Marco LED
        sf::FloatRect bounds = autos[i].getGlobalBounds();
        marcos[i].setSize(sf::Vector2f(bounds.size.x + 10, bounds.size.y + 10));
        marcos[i].setPosition(sf::Vector2f(
            autos[i].getPosition().x - 5,
            autos[i].getPosition().y - 5
        ));
        marcos[i].setFillColor(sf::Color::Transparent);
        marcos[i].setOutlineThickness(5);
        marcos[i].setOutlineColor(sf::Color::Blue);
    }

    while (ventana.isOpen()) {
        while (auto evento = ventana.pollEvent()) {
            if (evento->is<sf::Event::Closed>()) ventana.close();

            // Intro → AutoMenu
            if (enIntro && evento->is<sf::Event::KeyPressed>()) {
                auto keyEvent = evento->getIf<sf::Event::KeyPressed>();
                if (keyEvent && keyEvent->code == sf::Keyboard::Key::Enter) {
                    musicaIntro.stop();
                    if (musicaAutoMenu.openFromFile("recursos/musica_automenu.ogg")) {
                        musicaAutoMenu.setLooping(true);
                        musicaAutoMenu.play();
                    }
                    sonidoBurbuja.play();
                    enIntro = false;
                    enAutoMenu = true;
                }
            }

            if (enIntro && evento->is<sf::Event::MouseButtonPressed>()) {
                auto mouseEvent = evento->getIf<sf::Event::MouseButtonPressed>();
                if (mouseEvent && mouseEvent->button == sf::Mouse::Button::Left) {
                    sf::Vector2f mousePos = ventana.mapPixelToCoords(mouseEvent->position);
                    sf::FloatRect areaPlay = botonPlay.getGlobalBounds();
                    if (areaPlay.contains(mousePos)) {
                        musicaIntro.stop();
                        if (musicaAutoMenu.openFromFile("recursos/musica_automenu.ogg")) {
                            musicaAutoMenu.setLooping(true);
                            musicaAutoMenu.play();
                        }
                        sonidoBurbuja.play();
                        enIntro = false;
                        enAutoMenu = true;
                    }
                }
            }

            // AutoMenu navegación
            if (enAutoMenu && evento->is<sf::Event::KeyPressed>()) {
                auto keyEvent = evento->getIf<sf::Event::KeyPressed>();
                if (keyEvent) {
                    if (keyEvent->code == sf::Keyboard::Key::Left)
                        seleccionado = (seleccionado + 4) % 5;
                    if (keyEvent->code == sf::Keyboard::Key::Right)
                        seleccionado = (seleccionado + 1) % 5;
                    if (keyEvent->code == sf::Keyboard::Key::Enter) {
                        sonidoBurbuja.play();
                        musicaAutoMenu.stop(); // apagar música al pasar a actividad1
                        // Aquí pasas a actividad1
                    }
                }
            }

            if (enAutoMenu && evento->is<sf::Event::MouseButtonPressed>()) {
                auto mouseEvent = evento->getIf<sf::Event::MouseButtonPressed>();
                if (mouseEvent && mouseEvent->button == sf::Mouse::Button::Left) {
                    sf::Vector2f mousePos = ventana.mapPixelToCoords(mouseEvent->position);
                    for (int i = 0; i < 5; i++) {
                        if (autos[i].getGlobalBounds().contains(mousePos)) {
                            seleccionado = i;
                            sonidoBurbuja.play();
                            musicaAutoMenu.stop(); // apagar música al pasar a actividad1
                            // Aquí pasas a actividad1
                        }
                    }
                }
            }
        }

        // Dibujar
        ventana.clear();
        if (enIntro) {
            ventana.draw(fondo);
            ventana.draw(titulo);
            ventana.draw(botonPlay);
        } else if (enAutoMenu) {
            // LED efecto para texto y marcos
            if (relojLED.getElapsedTime().asSeconds() > 0.5f) {
                colorRojo = !colorRojo;
                relojLED.restart();
            }

            textoAutoMenu.setFillColor(colorRojo ? sf::Color::Red : sf::Color::Blue);
            ventana.draw(textoAutoMenu);

            for (int i = 0; i < 5; i++) {
                if (i == seleccionado) {
                    marcos[i].setOutlineColor(colorRojo ? sf::Color::Red : sf::Color::Blue);
                    marcos[i].setOutlineThickness(8);
                } else {
                    marcos[i].setOutlineColor(sf::Color::Blue);
                    marcos[i].setOutlineThickness(5);
                }
                ventana.draw(marcos[i]);
                ventana.draw(autos[i]);
            }
        }
        ventana.display();
    }
    return 0;
}
