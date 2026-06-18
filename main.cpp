#define _USE_MATH_DEFINES 
#include <cmath>
#include <ctime>
#include <SFML/Graphics.hpp>
#include <SFML/Audio.hpp>
#include <algorithm>
#include <string>
#include <vector>
#include <optional>
#include <iostream>

enum class GameState {
    Intro,
    AutoMenu,
    Reglas,
    Actividad1,
    Actividad5,
    Rosas,
    Banderas
};

struct ObjetoMecanico {
    sf::Sprite sprite;
    bool esBueno;
    bool enBarra = false;
    ObjetoMecanico(const sf::Sprite& s, bool bueno) : sprite(s), esBueno(bueno) {}
};

class ActividadEquilibrio {
private:
    sf::Texture fondoCiudadTex, tallerTex, basuraTex, barraTex;
    sf::Sprite fondoCiudad{fondoCiudadTex};
    sf::Sprite taller{tallerTex};
    sf::Sprite basura{basuraTex};
    sf::Sprite barra{barraTex};
    
    std::vector<sf::Text> textoErrores;
    float barraAngle = 0.f;

    sf::Texture objetosBuenosTex[9];
    sf::Texture objetosMalosTex[8];
    std::optional<ObjetoMecanico> objetoActual;
    sf::Clock relojObjeto;
    
    float velocidadCaidaObjeto = 1.4f;
    float retardoNuevoObjeto = 1.8f;
    int errores = 0;
    int objetosProcesados = 0;
    const int META_OBJETOS = 50;

    sf::Music musica;
    sf::SoundBuffer bufferFin;
    sf::Sound sonidoFin{bufferFin};

    std::optional<sf::Text> textoFin;
    std::optional<sf::Text> opcionRepetir;
    std::optional<sf::Text> opcionCerrar;
    
    int opcionSeleccionada = 0;
    bool finJuego = false;
    bool completado = false;
    bool cerrarSolicitado = false;

public:
    void inicializar(const sf::Font& fuente, const sf::Font& fuenteError) {

        (void)fondoCiudadTex.loadFromFile("assets/ciudadfondo.png");
        fondoCiudad.setTexture(fondoCiudadTex, true);
        fondoCiudad.setScale(sf::Vector2f(0.8f, 0.8f));

        (void)tallerTex.loadFromFile("assets/taller.png");
        taller.setTexture(tallerTex, true);
        taller.setScale(sf::Vector2f(0.24f, 0.24f));
        taller.setPosition(sf::Vector2f(690.f, 450.f));

        (void)basuraTex.loadFromFile("assets/basura.png");
        basura.setTexture(basuraTex, true);
        basura.setScale(sf::Vector2f(0.24f, 0.24f));
        basura.setPosition(sf::Vector2f(10.f, 500.f));

        (void)barraTex.loadFromFile("assets/barra_equilibrio.png");
        barra.setTexture(barraTex, true);
        barra.setScale(sf::Vector2f(0.35f, 0.35f));
        auto bounds = barra.getLocalBounds();
        barra.setOrigin(sf::Vector2f(bounds.size.x / 2.f, bounds.size.y / 2.f));
        barra.setPosition(sf::Vector2f(500.f, 540.f));
        barraAngle = 0.f;

        textoErrores.clear();
        for (int i = 0; i < 3; i++) {
            sf::Text texto(fuenteError, "X", 100);
            texto.setFillColor(sf::Color::White);
            texto.setOutlineThickness(6);
            texto.setOutlineColor(sf::Color::Black);
            texto.setStyle(sf::Text::Bold);
            auto textBounds = texto.getLocalBounds();
            texto.setOrigin(sf::Vector2f(textBounds.size.x / 2.f, textBounds.size.y / 2.f));
            texto.setPosition(sf::Vector2f(820.f + i * 60.f, 60.f));
            textoErrores.push_back(texto);
        }

        std::string buenos[] = {"motor.png","bateria.png","transmision.png","radiador.png",
                                "alternador.png","suspension.png","escape.png","volante.png","llanta.png"};
        for (int i=0; i<9; i++) (void)objetosBuenosTex[i].loadFromFile("assets/" + buenos[i]);

        std::string malos[] = {"periodico.png","trapo.png","esponja.png","cepillo.png",
                               "aceite.png","clavo.png","pintura.png","aerosol.png"};
        for (int i=0; i<8; i++) (void)objetosMalosTex[i].loadFromFile("assets/" + malos[i]);

        (void)musica.openFromFile("assets/musica_actividad1.ogg");
        musica.setLooping(true);

        (void)bufferFin.loadFromFile("assets/sonido_fin.wav");

        textoFin.emplace(fuente, "Fin Del Juego", 60);
        textoFin->setFillColor(sf::Color::Red);
        textoFin->setPosition(sf::Vector2f(300.f, 200.f));

        opcionRepetir.emplace(fuente, "Repetir", 40);
        opcionRepetir->setPosition(sf::Vector2f(350.f, 400.f));

        opcionCerrar.emplace(fuente, "Cerrar", 40);
        opcionCerrar->setPosition(sf::Vector2f(550.f, 400.f));

        reiniciar();
        musica.stop(); 
    }

    void reiniciar() {
        errores = 0;
        objetosProcesados = 0;
        objetoActual.reset();
        relojObjeto.restart();
        finJuego = false;
        completado = false;
        cerrarSolicitado = false;
        opcionSeleccionada = 0;
        barraAngle = 0.f;
        barra.setRotation(sf::degrees(barraAngle));
        for (auto& txt : textoErrores) txt.setFillColor(sf::Color::White);
        musica.play();
    }

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

    void actualizar() {
        if (finJuego || completado) return;

        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Left)) {
            barraAngle = std::max(-30.f, barraAngle - 0.25f);
            barra.setRotation(sf::degrees(barraAngle));
        }
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Right)) {
            barraAngle = std::min(barraAngle + 0.5f, 30.f);
            barra.setRotation(sf::degrees(barraAngle));
        }

        if (!objetoActual.has_value() && relojObjeto.getElapsedTime().asSeconds() > retardoNuevoObjeto) {
            generarObjeto();
        }

        if (objetoActual.has_value()) {
            auto& obj = *objetoActual;
            if (!obj.enBarra) {
                obj.sprite.move(sf::Vector2f(0.f, velocidadCaidaObjeto));
                if (obj.sprite.getPosition().y > 850.f) {
                    objetoActual.reset();
                    return;
                }
                if (obj.sprite.getGlobalBounds().findIntersection(barra.getGlobalBounds()).has_value()) {
                    obj.enBarra = true;
                    obj.sprite.setPosition(sf::Vector2f(barra.getPosition().x, barra.getPosition().y - 70.f));
                }
            } else {
                float dx = barraAngle * 0.15f;
                obj.sprite.move(sf::Vector2f(dx, 0.f));
                obj.sprite.setPosition(sf::Vector2f(obj.sprite.getPosition().x, barra.getPosition().y - 70.f));

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
                            musica.stop();
                            sonidoFin.play();
                        }
                    } else {
                        objetosProcesados++;
                        if (objetosProcesados >= META_OBJETOS) {
                            completado = true;
                            musica.stop();
                        }
                    }
                    objetoActual.reset();
                }
            }
        }
    }

    void procesarEventos(const sf::Event& evento, sf::RenderWindow& ventana) {
        if (!finJuego) return;

        if (auto key = evento.getIf<sf::Event::KeyPressed>()) {
            if (key->code == sf::Keyboard::Key::Left || key->code == sf::Keyboard::Key::Right)
                opcionSeleccionada = 1 - opcionSeleccionada;
            
            if (key->code == sf::Keyboard::Key::Enter) {
                if (opcionSeleccionada == 0) reiniciar();
                else cerrarSolicitado = true;
            }
        }
        if (auto mouse = evento.getIf<sf::Event::MouseButtonPressed>()) {
            if (mouse->button == sf::Mouse::Button::Left) {
                sf::Vector2f mousePos = ventana.mapPixelToCoords(mouse->position);
                if (opcionRepetir->getGlobalBounds().contains(mousePos)) reiniciar();
                if (opcionCerrar->getGlobalBounds().contains(mousePos)) cerrarSolicitado = true;
            }
        }
    }

    void dibujar(sf::RenderWindow& ventana) {
        ventana.draw(fondoCiudad);
        ventana.draw(taller);
        ventana.draw(basura);
        ventana.draw(barra);
        if (objetoActual.has_value()) ventana.draw(objetoActual->sprite);
        for (const auto& txt : textoErrores) ventana.draw(txt);

        if (finJuego) {
            ventana.draw(*textoFin);
            opcionRepetir->setFillColor(opcionSeleccionada == 0 ? sf::Color::Yellow : sf::Color::White);
            opcionCerrar->setFillColor(opcionSeleccionada == 1 ? sf::Color::Yellow : sf::Color::White);
            ventana.draw(*opcionRepetir);
            ventana.draw(*opcionCerrar);
        }
    }

    bool haCompletado() const { return completado; }
    bool quiereCerrar() const { return cerrarSolicitado; }
    bool estaEnFinJuego() const { return finJuego; }
    void detenerMusica() { musica.stop(); }
};

class ActividadTuercas {
private:
    sf::Texture fondoLlantaTex, tuercaTex;
    sf::Sprite fondoLlanta{fondoLlantaTex};
    std::vector<sf::Sprite> tuercas;
    std::vector<int> clicsTuerca;
    
    sf::Music musica;
    sf::SoundBuffer bufferDrill;
    sf::Sound sonidoDrill{bufferDrill};
    
    std::optional<sf::Text> textoTiempo;
    sf::Clock relojTiempo;
    int tiempoLimite = 20;
    bool finJuego = false;
    bool completado = false;

    std::optional<sf::Text> textoFin;
    std::optional<sf::Text> opcionRepetir;
    std::optional<sf::Text> opcionCerrar;
    int opcionSeleccionada = 0;
    bool cerrarSolicitado = false;

public:
    void inicializar(const sf::Font& fuente) {
        (void)fondoLlantaTex.loadFromFile("assets/fondo_llanta.png");
        fondoLlanta.setTexture(fondoLlantaTex, true);

        auto texSize = fondoLlantaTex.getSize();
        float scaleMax = std::max(4000.f / texSize.x, 4000.f / texSize.y) * 1.05f;
        fondoLlanta.setScale(sf::Vector2f(scaleMax, scaleMax));
        fondoLlanta.setPosition(sf::Vector2f(500.f - (texSize.x * scaleMax)/2.f, 
                                             400.f - (texSize.y * scaleMax)/2.f));

        (void)tuercaTex.loadFromFile("assets/tuerca.png");
        tuercas.clear();
        clicsTuerca.assign(9, 0);

        float cx = 500.f, cy = 400.f, radio = 250.f;
        for (int i = 0; i < 9; ++i) {
            sf::Sprite t(tuercaTex);
            auto b = t.getLocalBounds();
            t.setOrigin(sf::Vector2f(b.size.x / 2.f, b.size.y / 2.f));
            float ang = i * (2.f * M_PI / 9.f);
            t.setPosition(sf::Vector2f(cx + radio * std::cos(ang), cy + radio * std::sin(ang)));
            t.setScale(sf::Vector2f(0.20f, 0.20f));
            tuercas.push_back(t);
        }

        (void)musica.openFromFile("assets/musica_actividad2.ogg");
        (void)bufferDrill.loadFromFile("assets/drill.wav");

        textoTiempo.emplace(fuente, "", 40);
        textoTiempo->setFillColor(sf::Color::White);
        textoTiempo->setPosition(sf::Vector2f(600.f, 60.f));

        textoFin.emplace(fuente, "Fin Del Juego", 60);
        textoFin->setFillColor(sf::Color::Red);
        textoFin->setPosition(sf::Vector2f(300.f, 200.f));

        opcionRepetir.emplace(fuente, "Repetir", 40);
        opcionRepetir->setPosition(sf::Vector2f(350.f, 400.f));

        opcionCerrar.emplace(fuente, "Cerrar", 40);
        opcionCerrar->setPosition(sf::Vector2f(550.f, 400.f));

        reiniciar();
        musica.stop(); 
    }

    void reiniciar() {
        clicsTuerca.assign(9, 0);
        for (auto& t : tuercas) t.setScale(sf::Vector2f(0.20f, 0.20f));
        finJuego = false;
        completado = false;
        cerrarSolicitado = false;
        opcionSeleccionada = 0;
        
        relojTiempo.restart();
        musica.setLooping(true);
        musica.play();
    }

    void actualizar() {
        if (finJuego || completado) return;

        int tiempoRestante = tiempoLimite - static_cast<int>(relojTiempo.getElapsedTime().asSeconds());
        textoTiempo->setString("Tiempo: " + std::to_string(std::max(0, tiempoRestante)));

        bool todasCompletadas = true;
        for (int clics : clicsTuerca) {
            if (clics < 5) { todasCompletadas = false; break; }
        }

        if (todasCompletadas) {
            completado = true;
            musica.stop();
        } else if (tiempoRestante <= 0) {
            finJuego = true;
            musica.stop();
        }
    }

    void procesarEventos(const sf::Event& evento, sf::RenderWindow& ventana) {
        if (finJuego) {
            if (auto key = evento.getIf<sf::Event::KeyPressed>()) {
                if (key->code == sf::Keyboard::Key::Left || key->code == sf::Keyboard::Key::Right)
                    opcionSeleccionada = 1 - opcionSeleccionada;
                
                if (key->code == sf::Keyboard::Key::Enter) {
                    if (opcionSeleccionada == 0) reiniciar();
                    else cerrarSolicitado = true;
                }
            }
            if (auto mouse = evento.getIf<sf::Event::MouseButtonPressed>()) {
                if (mouse->button == sf::Mouse::Button::Left) {
                    sf::Vector2f mousePos = ventana.mapPixelToCoords(mouse->position);
                    if (opcionRepetir->getGlobalBounds().contains(mousePos)) reiniciar();
                    if (opcionCerrar->getGlobalBounds().contains(mousePos)) cerrarSolicitado = true;
                }
            }
            return;
        }

        if (completado) return;

        if (auto mouse = evento.getIf<sf::Event::MouseButtonPressed>()) {
            if (mouse->button == sf::Mouse::Button::Left) {
                sf::Vector2f mousePos = ventana.mapPixelToCoords(mouse->position);
                for (size_t i = 0; i < tuercas.size(); ++i) {
                    if (tuercas[i].getGlobalBounds().contains(mousePos)) {
                        if (clicsTuerca[i] >= 5) break;
                        
                        clicsTuerca[i]++;
                        if (sonidoDrill.getStatus() != sf::SoundSource::Status::Playing) sonidoDrill.play();
                        else { sonidoDrill.stop(); sonidoDrill.play(); }

                        float scale = std::min(tuercas[i].getScale().x + 0.06f, 0.45f);
                        tuercas[i].setScale(sf::Vector2f(scale, scale));
                        break;
                    }
                }
            }
        }
    }

    void dibujar(sf::RenderWindow& ventana) {
        ventana.draw(fondoLlanta);
        for (const auto& t : tuercas) ventana.draw(t);
        ventana.draw(*textoTiempo);

        if (finJuego) {
            ventana.draw(*textoFin);
            opcionRepetir->setFillColor(opcionSeleccionada == 0 ? sf::Color::Yellow : sf::Color::White);
            opcionCerrar->setFillColor(opcionSeleccionada == 1 ? sf::Color::Yellow : sf::Color::White);
            ventana.draw(*opcionRepetir);
            ventana.draw(*opcionCerrar);
        }
    }

    bool haCompletado() const { return completado; }
    bool haPerdido() const { return finJuego; }
    bool quiereCerrar() const { return cerrarSolicitado; }
};

class JuegoMotor {
private:
    sf::RenderWindow ventana;
    GameState estadoActual;
    
    sf::Font fuentePrincipal;
    sf::Font fuenteErrores;

    sf::Texture fondoIntroTex;
    sf::Sprite fondoIntro{fondoIntroTex};
    
    std::optional<sf::Text> titulo;
    std::optional<sf::Text> botonPlay;
    std::optional<sf::Text> textoAutoMenu;
    
    sf::Music musicaIntro, musicaAutoMenu;
    sf::SoundBuffer bufferBurbuja;
    sf::Sound sonidoBurbuja{bufferBurbuja};

    int autoSeleccionado = 0;
    sf::Texture autosTex[5];
    std::vector<sf::Sprite> autos;
    std::vector<sf::RectangleShape> marcos; 
    sf::Clock relojLED;
    bool colorRojo = true;

    std::vector<sf::Texture> cutsceneTex;
    std::optional<sf::Sprite> cutsceneSprite;
    sf::Clock cutsceneReloj;
    int cutsceneIndex = 0;
    sf::Music musicaBanderas;

    ActividadEquilibrio minijuegoEquilibrio;
    ActividadTuercas minijuegoTuercas;

public:
    JuegoMotor() : ventana(sf::VideoMode({1000, 800}), "Disena tu auto de carreras"), estadoActual(GameState::Intro) {
        cargarRecursosGlobales();
        inicializarMenu();
        
        minijuegoEquilibrio.inicializar(fuentePrincipal, fuenteErrores);
        minijuegoTuercas.inicializar(fuentePrincipal);
    }

    void run() {
        while (ventana.isOpen()) {
            procesarEventos();
            actualizar();
            dibujar();
        }
    }

private:
    void cargarRecursosGlobales() {
        (void)fuentePrincipal.openFromFile("assets/arial.ttf");

        if (!fuenteErrores.openFromFile("assets/x.ttf")) {
            (void)fuenteErrores.openFromFile("assets/arial.ttf");
        }
        (void)bufferBurbuja.loadFromFile("assets/burbuja.wav");
    }

    void inicializarMenu() {
        (void)fondoIntroTex.loadFromFile("assets/intro.png");
        fondoIntro.setTexture(fondoIntroTex, true);
        fondoIntro.setScale(sf::Vector2f(0.7f, 0.7f));
        fondoIntro.setPosition(sf::Vector2f(0.f, 60.f));

        titulo.emplace(fuentePrincipal, "Disena tu auto de carreras", 50);
        titulo->setFillColor(sf::Color::White);
        titulo->setStyle(sf::Text::Bold);
        titulo->setPosition(sf::Vector2f(ventana.getSize().x / 2.f - titulo->getLocalBounds().size.x / 2.f, 150.f));

        botonPlay.emplace(fuentePrincipal, "PLAY", 70);
        botonPlay->setFillColor(sf::Color::Yellow);
        botonPlay->setPosition(sf::Vector2f(ventana.getSize().x / 2.f - botonPlay->getLocalBounds().size.x / 2.f, 400.f));

        textoAutoMenu.emplace(fuentePrincipal, "Selecciona tu auto de carreras", 40);
        textoAutoMenu->setStyle(sf::Text::Bold);
        textoAutoMenu->setPosition(sf::Vector2f(ventana.getSize().x / 2.f - textoAutoMenu->getLocalBounds().size.x / 2.f, 50.f));

        marcos.clear();
        for (int i = 0; i < 5; i++) {
            (void)autosTex[i].loadFromFile("assets/auto" + std::to_string(i+1) + ".jpg");
            sf::Sprite sprite(autosTex[i]);
            sprite.setScale(sf::Vector2f(0.2f, 0.2f));
            sprite.setPosition(sf::Vector2f(i < 3 ? 150.f + i*250.f : 250.f + (i-3)*250.f, i < 3 ? 150.f : 450.f));
            autos.push_back(sprite);

            sf::RectangleShape marco(sf::Vector2f(sprite.getGlobalBounds().size.x + 10, sprite.getGlobalBounds().size.y + 10));
            marco.setPosition(sf::Vector2f(sprite.getPosition().x - 5, sprite.getPosition().y - 5));
            marco.setFillColor(sf::Color::Transparent);
            marco.setOutlineThickness(5);
            marcos.push_back(marco);
        }

        (void)musicaIntro.openFromFile("assets/musica_intro.ogg");
        musicaIntro.setLooping(true);
        musicaIntro.play();
    }

    void cargarCutscene(const std::string& prefijo, int cantidad) {
        cutsceneTex.clear();
        cutsceneTex.resize(cantidad);
        for (int i = 0; i < cantidad; i++) {
            (void)cutsceneTex[i].loadFromFile("assets/" + prefijo + std::to_string(i+1) + ".png");
        }
        
        if (!cutsceneSprite.has_value()) {
            cutsceneSprite.emplace(cutsceneTex[0]);
        } else {
            cutsceneSprite->setTexture(cutsceneTex[0], true);
        }
        
        cutsceneSprite->setScale(sf::Vector2f(0.6f, 0.6f));
        cutsceneSprite->setPosition(sf::Vector2f(0.f, 40.f));
        cutsceneIndex = 0;
        cutsceneReloj.restart();
    }

    void procesarEventos() {
        while (auto evento = ventana.pollEvent()) {
            if (evento->is<sf::Event::Closed>()) {
                ventana.close();
                continue;
            }

            switch (estadoActual) {
                case GameState::Intro:
                    if (auto key = evento->getIf<sf::Event::KeyPressed>(); key && key->code == sf::Keyboard::Key::Enter)
                        iniciarAutoMenu();
                    if (auto mouse = evento->getIf<sf::Event::MouseButtonPressed>(); mouse && mouse->button == sf::Mouse::Button::Left)
                        if (botonPlay->getGlobalBounds().contains(ventana.mapPixelToCoords(mouse->position)))
                            iniciarAutoMenu();
                    break;

                case GameState::AutoMenu:
                    if (auto key = evento->getIf<sf::Event::KeyPressed>()) {
                        if (key->code == sf::Keyboard::Key::Left) autoSeleccionado = (autoSeleccionado + 4) % 5;
                        if (key->code == sf::Keyboard::Key::Right) autoSeleccionado = (autoSeleccionado + 1) % 5;
                        if (key->code == sf::Keyboard::Key::Enter) iniciarReglas();
                    }
                    if (auto mouse = evento->getIf<sf::Event::MouseButtonPressed>(); mouse && mouse->button == sf::Mouse::Button::Left) {
                        sf::Vector2f pos = ventana.mapPixelToCoords(mouse->position);
                        for (int i = 0; i < 5; i++) {
                            if (autos[i].getGlobalBounds().contains(pos)) {
                                autoSeleccionado = i;
                                iniciarReglas();
                            }
                        }
                    }
                    break;

                case GameState::Reglas:
                    if (auto key = evento->getIf<sf::Event::KeyPressed>(); key && key->code == sf::Keyboard::Key::Enter) {
                        estadoActual = GameState::Actividad1;
                        minijuegoEquilibrio.reiniciar();
                    }
                    break;

                case GameState::Actividad1:
                    minijuegoEquilibrio.procesarEventos(*evento, ventana);
                    break;

                case GameState::Rosas:
                    if (auto key = evento->getIf<sf::Event::KeyPressed>(); key && key->code == sf::Keyboard::Key::Enter) {
                        estadoActual = GameState::Actividad5;
                        minijuegoTuercas.reiniciar();
                    }
                    break;

                case GameState::Actividad5:
                    minijuegoTuercas.procesarEventos(*evento, ventana);
                    break;

                case GameState::Banderas:
                    if (auto key = evento->getIf<sf::Event::KeyPressed>(); key && key->code == sf::Keyboard::Key::Enter) {
                        musicaBanderas.stop();
                        ventana.close(); 
                    }
                    break;
            }
        }
    }

    void actualizar() {
        switch (estadoActual) {
            case GameState::AutoMenu:
                if (relojLED.getElapsedTime().asSeconds() > 0.5f) {
                    colorRojo = !colorRojo;
                    relojLED.restart();
                }
                break;

            case GameState::Reglas:
            case GameState::Rosas:
            case GameState::Banderas:
                if (cutsceneReloj.getElapsedTime().asSeconds() > 2.0f) {
                    cutsceneIndex = (cutsceneIndex + 1) % cutsceneTex.size();
                    cutsceneSprite->setTexture(cutsceneTex[cutsceneIndex], true);
                    cutsceneReloj.restart();
                }
                if (estadoActual == GameState::Banderas && musicaBanderas.getStatus() == sf::SoundSource::Status::Stopped) {
                    ventana.close();
                }
                break;

            case GameState::Actividad1:
                minijuegoEquilibrio.actualizar();
                if (minijuegoEquilibrio.quiereCerrar()) ventana.close();
                if (minijuegoEquilibrio.haCompletado()) {
                    estadoActual = GameState::Rosas;
                    cargarCutscene("rosas", 3);
                }
                break;

            case GameState::Actividad5:
                minijuegoTuercas.actualizar();
                if (minijuegoTuercas.quiereCerrar()) ventana.close(); // Integración añadida
                if (minijuegoTuercas.haCompletado()) {
                    estadoActual = GameState::Banderas;
                    cargarCutscene("banderas", 6);
                    (void)musicaBanderas.openFromFile("assets/musica_banderas.ogg");
                    musicaBanderas.play();
                }
                break;
        }
    }

    void dibujar() {
        ventana.clear();

        switch (estadoActual) {
            case GameState::Intro:
                ventana.draw(fondoIntro);
                ventana.draw(*titulo);
                ventana.draw(*botonPlay);
                break;

            case GameState::AutoMenu:
                textoAutoMenu->setFillColor(colorRojo ? sf::Color::Red : sf::Color::Blue);
                ventana.draw(*textoAutoMenu);
                for (int i = 0; i < 5; i++) {
                    marcos[i].setOutlineColor(i == autoSeleccionado ? (colorRojo ? sf::Color::Red : sf::Color::Blue) : sf::Color::Blue);
                    marcos[i].setOutlineThickness(i == autoSeleccionado ? 8 : 5);
                    ventana.draw(marcos[i]);
                    ventana.draw(autos[i]);
                }
                break;

            case GameState::Reglas:
            case GameState::Rosas:
            case GameState::Banderas:
                if (cutsceneSprite.has_value()) {
                    ventana.draw(*cutsceneSprite);
                }
                break;

            case GameState::Actividad1:
                minijuegoEquilibrio.dibujar(ventana);
                break;

            case GameState::Actividad5:
                minijuegoTuercas.dibujar(ventana);
                break;
        }

        ventana.display();
    }

    void iniciarAutoMenu() {
        sonidoBurbuja.play();
        musicaIntro.stop();
        (void)musicaAutoMenu.openFromFile("assets/musica_automenu.ogg");
        musicaAutoMenu.setLooping(true);
        musicaAutoMenu.play();
        estadoActual = GameState::AutoMenu;
    }

    void iniciarReglas() {
        sonidoBurbuja.play();
        musicaAutoMenu.stop();
        cargarCutscene("imagen_reglas", 4);
        estadoActual = GameState::Reglas;
    }
};

int main() {
    std::srand(static_cast<unsigned int>(std::time(nullptr))); // Semilla aleatoria
    JuegoMotor juego;
    juego.run();
    return 0;
}