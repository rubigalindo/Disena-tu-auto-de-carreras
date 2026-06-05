#include <SFML/Graphics.hpp>
#include <SFML/Audio.hpp>

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

     sf::Text botonPlay(fuente, "PLAY", 70); // tamaño controlado aquí
    botonPlay.setFillColor(sf::Color::Yellow);
    botonPlay.setPosition(sf::Vector2f(
        ventana.getSize().x / 2.f - botonPlay.getLocalBounds().size.x / 2.f,
        400.f
    ));


    // Música de fondo
    sf::Music musica;
    if (musica.openFromFile("recursos/musica_intro.ogg")) musica.play();

    // Sonido burbuja
    sf::SoundBuffer bufferBurbuja;
    if (!bufferBurbuja.loadFromFile("recursos/burbuja.wav")) return -1;
    sf::Sound sonidoBurbuja(bufferBurbuja);

    bool enIntro = true;

    while (ventana.isOpen()) {
        while (auto evento = ventana.pollEvent()) {
            if (evento->is<sf::Event::Closed>()) ventana.close();

            // Tecla Enter
            if (enIntro && evento->is<sf::Event::KeyPressed>()) {
                auto keyEvent = evento->getIf<sf::Event::KeyPressed>();
                if (keyEvent && keyEvent->code == sf::Keyboard::Key::Enter) {
                    sonidoBurbuja.play();
                    enIntro = false;
                }
            }

            // Clic en botón PLAY
            if (evento->is<sf::Event::MouseButtonPressed>()) {
                auto mouseEvent = evento->getIf<sf::Event::MouseButtonPressed>();
                if (mouseEvent && mouseEvent->button == sf::Mouse::Button::Left) {
                    sf::FloatRect areaPlay = botonPlay.getGlobalBounds();
                    sf::Vector2f mousePos(
                        static_cast<float>(mouseEvent->position.x),
                        static_cast<float>(mouseEvent->position.y)
                    );
                    if (areaPlay.contains(mousePos)) {
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

}
return 0;
    
}
