#include <SFML/Graphics.hpp>
#include <SFML/Audio.hpp>
#include <algorithm>
#include <string>
#include <vector>
#include <optional>
#include <iostream>

// Estados del juego
int actividad = 0; // 0=Intro, 1=AutoMenu, 2=Reglas, 3=Actividad1, 4=Actividad2

// AutoMenu
bool enAutoMenu = false;
int seleccionado = 0; // índice del auto seleccionado
sf::Texture autosTex[5];
std::vector<sf::Sprite> autos;
sf::RectangleShape marcos[5];
sf::Clock relojLED;
bool colorRojo = true;

// Reglas
sf::Texture reglasTex[4];
std::optional<sf::Sprite> reglasSprite;
int reglaActual = 0;
sf::Clock relojReglas;

// Música y sonidos
sf::Music musicaIntro;
sf::Music musicaAutoMenu;
sf::SoundBuffer bufferBurbuja;
std::optional<sf::Sound> sonidoBurbuja;

// --- Variables globales para Actividad 1 ---
struct Objeto {
    sf::Sprite sprite;
    bool esBueno;
    bool enBarra = false;
    Objeto(const sf::Sprite &s, bool bueno)
        : sprite(s), esBueno(bueno) {}
};

sf::Texture fondoCiudadTex, tallerTex, basuraTex, barraTex;
sf::Font fuenteX;
sf::Font *fuenteErrorFont = nullptr;
std::optional<sf::Sprite> fondoCiudad, taller, basura, barra;
std::vector<sf::Text> textoErrores;
float barraAngle = 0.f;

sf::Texture objetosBuenosTex[9];
sf::Texture objetosMalosTex[8];
std::optional<Objeto> objetoActual;
sf::Clock relojObjeto;
float velocidadCaidaObjeto = 1.4f;
float retardoNuevoObjeto = 1.8f;

sf::Music musicaActividad1;
sf::SoundBuffer bufferFin;

std::optional<sf::Text> textoFin, opcionRepetir, opcionCerrar;
bool finJuego = false;
int errores = 0;
int objetosProcesados = 0;
const int META_OBJETOS = 20; // cantidad necesaria para pasar
int opcionSeleccionada = 0; // 0=Repetir, 1=Cerrar

std::optional<sf::Sound> sonidoFin;

// --- Inicializar Actividad 1 ---
void cargarActividad1(sf::Font &fuente) {
    if (!fondoCiudadTex.loadFromFile("recursos/ciudadfondo.png")) {
        std::cerr << "Error: no se pudo cargar recursos/ciudadfondo.png\n";
    }
    fondoCiudad.emplace(fondoCiudadTex);
    fondoCiudad->setScale(sf::Vector2f(0.8f, 0.8f));

    if (!tallerTex.loadFromFile("recursos/taller.png")) {
        std::cerr << "Error: no se pudo cargar recursos/taller.png\n";
    }
    taller.emplace(tallerTex);
    taller->setScale(sf::Vector2f(0.24f, 0.24f));
    taller->setPosition(sf::Vector2f(690.f, 450.f));

    if (!basuraTex.loadFromFile("recursos/basura.png")) {
        std::cerr << "Error: no se pudo cargar recursos/basura.png\n";
    }
    basura.emplace(basuraTex);
    basura->setScale(sf::Vector2f(0.24f, 0.24f));
    basura->setPosition(sf::Vector2f(10.f, 500.f));

    if (!barraTex.loadFromFile("recursos/barra_equilibrio.png")) {
        std::cerr << "Error: no se pudo cargar recursos/barra_equilibrio.png\n";
    }
    barra.emplace(barraTex);
    barra->setScale(sf::Vector2f(0.35f, 0.35f));
    auto bounds = barra->getLocalBounds();
    barra->setOrigin(sf::Vector2f(
        bounds.position.x + bounds.size.x / 2.f,
        bounds.position.y + bounds.size.y / 2.f));
    barra->setPosition(sf::Vector2f(500.f, 540.f));
    barraAngle = 0.f;
    barra->setRotation(sf::degrees(barraAngle));

    // Texto de errores XXX
    textoErrores.clear();
    textoErrores.reserve(3);
    for (int i = 0; i < 3; i++) {
        sf::Text texto(*fuenteErrorFont, "X", 100);
        texto.setFillColor(sf::Color::White);
        texto.setOutlineThickness(6);
        texto.setOutlineColor(sf::Color::Black);
        texto.setStyle(sf::Text::Bold);
        auto textBounds = texto.getLocalBounds();
        texto.setOrigin(sf::Vector2f(
            textBounds.position.x + textBounds.size.x / 2.f,
            textBounds.position.y + textBounds.size.y / 2.f));
        texto.setPosition(sf::Vector2f(820.f + i * 60.f, 60.f));
        textoErrores.push_back(texto);
    }

    // Cargar objetos buenos
    std::string buenos[] = {"motor.png","bateria.png","transmision.png","radiador.png",
                            "alternador.png","suspension.png","escape.png","volante.png","llanta.png"};
    for (int i=0;i<9;i++) {
        if (!objetosBuenosTex[i].loadFromFile("recursos/"+buenos[i])) {
            std::cerr << "Error: no se pudo cargar recursos/" << buenos[i] << "\n";
        }
    }

    // Cargar objetos malos
    std::string malos[] = {"periodico.png","trapo.png","esponja.png","cepillo.png",
                           "aceite.png","clavo.png","pintura.png","aerosol.png"};
    for (int i=0;i<8;i++) {
        if (!objetosMalosTex[i].loadFromFile("recursos/"+malos[i])) {
            std::cerr << "Error: no se pudo cargar recursos/" << malos[i] << "\n";
        }
    }

    // Música
    if (!musicaActividad1.openFromFile("recursos/musica_actividad1.ogg")) {
        std::cerr << "Error: no se pudo cargar recursos/musica_actividad1.ogg\n";
    }
    musicaActividad1.setLooping(true);

    // Sonido fin
    if (!bufferFin.loadFromFile("recursos/sonido_fin.wav")) {
        std::cerr << "Error: no se pudo cargar recursos/sonido_fin.wav\n";
    }
    sonidoFin.emplace(bufferFin);

    // Texto fin
    textoFin.emplace(fuente);
    textoFin->setString("Fin Del Juego");
    textoFin->setCharacterSize(60);
    textoFin->setFillColor(sf::Color::Red);
    textoFin->setPosition(sf::Vector2f(300.f, 200.f));

    opcionRepetir.emplace(fuente);
    opcionRepetir->setString("Repetir");
    opcionRepetir->setCharacterSize(40);
    opcionRepetir->setPosition(sf::Vector2f(350.f, 400.f));

    opcionCerrar.emplace(fuente);
    opcionCerrar->setString("Cerrar");
    opcionCerrar->setCharacterSize(40);
    opcionCerrar->setPosition(sf::Vector2f(550.f, 400.f));

    // Reset variables
    errores = 0;
    objetosProcesados = 0;
    objetoActual.reset();
    relojObjeto.restart();
    finJuego = false;
    opcionSeleccionada = 0;
}

// --- Generar objeto aleatorio ---
void generarObjeto() {
    objetoActual.reset();
    bool bueno = rand() % 2;
    sf::Sprite sprite(bueno ? objetosBuenosTex[rand() % 9] : objetosMalosTex[rand() % 8]);
    auto bounds = sprite.getLocalBounds();
    sprite.setScale(sf::Vector2f(0.30f, 0.30f));
    sprite.setOrigin(sf::Vector2f(bounds.size.x / 2.f, bounds.size.y / 2.f));
    sprite.setPosition(sf::Vector2f(500.f, -80.f));
    objetoActual.emplace(sprite, bueno);
    relojObjeto.restart();
}

// --- Actualizar objetos ---
void actualizarObjetos() {
    if (!objetoActual.has_value()) return;
    auto &obj = *objetoActual;

    if (!obj.enBarra) {
        obj.sprite.move(sf::Vector2f(0.f, velocidadCaidaObjeto)); // velocidad caída

        if (obj.sprite.getPosition().y > 850.f) {
            objetoActual.reset();
            return;
        }

        if (obj.sprite.getGlobalBounds().findIntersection(barra->getGlobalBounds()).has_value()) {
            obj.enBarra = true;
            obj.sprite.setPosition(sf::Vector2f(barra->getPosition().x, barra->getPosition().y - 70.f));
            return;
        }
    } else {
        float dx = barraAngle * 0.15f;
        obj.sprite.move(sf::Vector2f(dx, 0.f));
        obj.sprite.setPosition(sf::Vector2f(obj.sprite.getPosition().x, barra->getPosition().y - 70.f));

        bool aBasura = obj.sprite.getPosition().x < 250.f;
        bool aTaller = obj.sprite.getPosition().x > 750.f;
        if (aBasura || aTaller) {
            bool correcto = (aTaller && obj.esBueno) || (aBasura && !obj.esBueno);
            if (!correcto) {
                errores++;
                for (int i = 0; i < 3; i++) {
                    textoErrores[i].setFillColor(i < errores ? sf::Color::Red : sf::Color::White);
                }
                if (errores >= 3) {
                    finJuego = true;
                    musicaActividad1.stop();
                    if (sonidoFin) sonidoFin->play();
                }
            } else {
                objetosProcesados++;
                if (objetosProcesados >= META_OBJETOS) {
                    musicaActividad1.stop();
                    actividad = 4;
                }
            }
            objetoActual.reset();
            return;
        }
    }
}

// --- Dibujar pantalla de fin ---
void dibujarFin(sf::RenderWindow &ventana) {
    ventana.draw(*textoFin);
    if (opcionSeleccionada == 0) {
        opcionRepetir->setFillColor(sf::Color::Yellow);
        opcionCerrar->setFillColor(sf::Color::White);
    } else {
        opcionRepetir->setFillColor(sf::Color::White);
        opcionCerrar->setFillColor(sf::Color::Yellow);
    }
    ventana.draw(*opcionRepetir);
    ventana.draw(*opcionCerrar);
}

// --- Manejar eventos en pantalla de fin ---
void manejarFin(const sf::Event &evento, sf::RenderWindow &ventana, sf::Font &fuente) {
    if (auto key = evento.getIf<sf::Event::KeyPressed>()) {
        if (key->code == sf::Keyboard::Key::Left || key->code == sf::Keyboard::Key::Right) {
            opcionSeleccionada = 1 - opcionSeleccionada;
        }
        if (key->code == sf::Keyboard::Key::Enter) {
            if (opcionSeleccionada == 0) {
                // Repetir → volver al inicio
                musicaActividad1.stop();
                actividad = 0;
                cargarActividad1(fuente); // reset
            } else {
                ventana.close();
            }
        }
    }
    if (auto mouse = evento.getIf<sf::Event::MouseMoved>()) {
        sf::Vector2f mousePos = ventana.mapPixelToCoords(mouse->position);
        if (opcionRepetir->getGlobalBounds().contains(mousePos)) {
            opcionSeleccionada = 0;
        } else if (opcionCerrar->getGlobalBounds().contains(mousePos)) {
            opcionSeleccionada = 1;
        }
    }
    if (auto mouse = evento.getIf<sf::Event::MouseButtonPressed>()) {
        if (mouse->button == sf::Mouse::Button::Left) {
            sf::Vector2f mousePos = ventana.mapPixelToCoords(mouse->position);
            if (opcionRepetir->getGlobalBounds().contains(mousePos)) {
                musicaActividad1.stop();
                actividad = 0;
                cargarActividad1(fuente);
                if (musicaIntro.getStatus() != sf::SoundSource::Status::Playing) {
                    musicaIntro.setLooping(true);
                    musicaIntro.play();
                }
            }
            if (opcionCerrar->getGlobalBounds().contains(mousePos)) {
                ventana.close();
            }
        }
    }
}


int main() {
    sf::RenderWindow ventana(sf::VideoMode({1000, 800}), "Dise\u00F1a tu auto de carreras");

    // Fondo Intro
    sf::Texture fondoTex;
    if (!fondoTex.loadFromFile("recursos/intro.png")) return -1;
    sf::Sprite fondo(fondoTex);
    fondo.setScale(sf::Vector2f(0.7f, 0.7f));
    fondo.setPosition(sf::Vector2f(0.f, 60.f));

    // Fuente y textos
    sf::Font fuente;
    if (!fuente.openFromFile("recursos/arial.ttf")) return -1;
    if (fuenteX.openFromFile("recursos/x.ttf")) {
        fuenteErrorFont = &fuenteX;
    } else {
        fuenteErrorFont = &fuente;
    }

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
    if (musicaIntro.openFromFile("recursos/musica_intro.ogg")) {
        musicaIntro.setLooping(true);
        musicaIntro.play();
    }

    // Sonido burbuja
    if (!bufferBurbuja.loadFromFile("recursos/burbuja.wav")) return -1;
    sonidoBurbuja.emplace(bufferBurbuja);

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
            if (evento->is<sf::Event::Closed>()) {
                ventana.close();
                continue;
            }

            if (actividad == 0) {
                if (auto key = evento->getIf<sf::Event::KeyPressed>(); key) {
                    if (key->code == sf::Keyboard::Key::Enter) {
                        musicaIntro.stop();
                        if (musicaAutoMenu.openFromFile("recursos/musica_automenu.ogg")) {
                            musicaAutoMenu.setLooping(true);
                            musicaAutoMenu.play();
                        }
                        if (sonidoBurbuja) sonidoBurbuja->play();
                        actividad = 1;
                        enAutoMenu = true;
                    }
                }
                if (auto mouse = evento->getIf<sf::Event::MouseButtonPressed>(); mouse) {
                    if (mouse->button == sf::Mouse::Button::Left) {
                        sf::Vector2f mousePos = ventana.mapPixelToCoords(mouse->position);
                        if (botonPlay.getGlobalBounds().contains(mousePos)) {
                            musicaIntro.stop();
                            if (musicaAutoMenu.openFromFile("recursos/musica_automenu.ogg")) {
                                musicaAutoMenu.setLooping(true);
                                musicaAutoMenu.play();
                            }
                            if (sonidoBurbuja) sonidoBurbuja->play();
                            actividad = 1;
                            enAutoMenu = true;
                        }
                    }
                }
            } else if (actividad == 1) {
                if (auto key = evento->getIf<sf::Event::KeyPressed>(); key) {
                    if (key->code == sf::Keyboard::Key::Left)
                        seleccionado = (seleccionado + 4) % 5;
                    if (key->code == sf::Keyboard::Key::Right)
                        seleccionado = (seleccionado + 1) % 5;
                    if (key->code == sf::Keyboard::Key::Enter) {
                        if (sonidoBurbuja) sonidoBurbuja->play();
                        musicaAutoMenu.stop();
                        // Pasar a Reglas
                        for (int i = 0; i < 7; i++) {
                            std::string nombre = "recursos/imagen_reglas" + std::to_string(i+1) + ".png";
                            if (!reglasTex[i].loadFromFile(nombre)) {
                                std::cerr << "Error: no se pudo cargar " << nombre << "\n";
                            }
                        }
                        reglasSprite.emplace(reglasTex[0]);
                        reglasSprite->setScale(sf::Vector2f(0.6f, 0.6f));
                        reglasSprite->setPosition(sf::Vector2f(-5.f, 40.f));

                        reglaActual = 0;
                        actividad = 2;
                    }
                }
                if (auto mouse = evento->getIf<sf::Event::MouseButtonPressed>(); mouse) {
                    if (mouse->button == sf::Mouse::Button::Left) {
                        sf::Vector2f mousePos = ventana.mapPixelToCoords(mouse->position);
                        for (int i = 0; i < 5; i++) {
                            if (autos[i].getGlobalBounds().contains(mousePos)) {
                                seleccionado = i;
                                if (sonidoBurbuja) sonidoBurbuja->play();
                                musicaAutoMenu.stop();
                                // Pasar a Reglas
                                for (int j = 0; j < 7; j++) {
                                    std::string nombre = "recursos/imagen_reglas" + std::to_string(j+1) + ".png";
                                    if (!reglasTex[j].loadFromFile(nombre)) {
                                        std::cerr << "Error: no se pudo cargar " << nombre << "\n";
                                    }
                                }
                                reglasSprite.emplace(reglasTex[0]);
                                reglasSprite->setScale(sf::Vector2f(0.6f, 0.6f));
                                reglasSprite->setPosition(sf::Vector2f(-5.f, 40.f));
                                actividad = 2;
                            }
                        }
                    }
                }
            } else if (actividad == 3 && finJuego) {
                manejarFin(*evento, ventana, fuente);
            }
        }

        // Lógica de juego por pantalla
        if (actividad == 3) {
            if (!finJuego) {
                if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Left)) {
                    barraAngle = std::max(-30.f, barraAngle - 0.25f);
                    barra->setRotation(sf::degrees(barraAngle));
                }
                if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Right)) {
                    if (barraAngle < 30.f) {
                        barraAngle = std::min(barraAngle + 0.5f, 30.f);
                        barra->setRotation(sf::degrees(barraAngle));
                    }
                }
                if (!objetoActual.has_value() && relojObjeto.getElapsedTime().asSeconds() > retardoNuevoObjeto) {
                    generarObjeto();
                }
                if (musicaActividad1.getStatus() != sf::SoundSource::Status::Playing) {
                    musicaActividad1.play();
                }
                actualizarObjetos();
            }
        }

        // Dibujar
        ventana.clear();
        if (actividad == 0) {
            ventana.draw(fondo);
            ventana.draw(titulo);
            ventana.draw(botonPlay);
        } else if (actividad == 1) {
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
        } else if (actividad == 2) {
            if (relojReglas.getElapsedTime().asSeconds() > 2.0f) {
                reglaActual = (reglaActual + 1) % 4;
                reglasSprite->setTexture(reglasTex[reglaActual]);
                relojReglas.restart();
            }
            ventana.draw(*reglasSprite);

            if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Enter)) {
                actividad = 3;
                cargarActividad1(fuente);
            }
        } else if (actividad == 3) {
            if (!finJuego) {
                ventana.draw(*fondoCiudad);
                ventana.draw(*taller);
                ventana.draw(*basura);
                ventana.draw(*barra);
                if (objetoActual.has_value()) ventana.draw(objetoActual->sprite);
                for (int i = 0; i < 3; i++) ventana.draw(textoErrores[i]);
            } else {
                dibujarFin(ventana);
            }
        }

        ventana.display();
    }
    return 0;
}