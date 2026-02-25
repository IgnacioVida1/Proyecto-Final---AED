//
// Created by ignac on 23/02/2026.
//

#ifndef CONSTANTS_H
#define CONSTANTS_H
#include <stdexcept>

// Tamaño de mundo
constexpr float WORLD_WIDTH = 2000.0f;
constexpr float WORLD_HEIGHT = 2000.0f;

// Límites de spawn y movimiento
constexpr float WORLD_SPAWN_MARGIN = 50.0f;
constexpr float WORLD_BOT_SPAWN_MARGIN = 200.0f;
constexpr float WORLD_PELLET_SPAWN_MARGIN = 50.0f;
constexpr int MAX_SPAWN_ATTEMPTS = 50;

// Tamaño de la ventana
constexpr int VIEWPORT_WIDTH = 1024;
constexpr int VIEWPORT_HEIGHT = 768;

// Parámetros de zoom
constexpr float CAMERA_MIN_ZOOM = 0.5f;
constexpr float CAMERA_MAX_ZOOM = 100.0f;
constexpr float CAMERA_ZOOM_SPEED = 0.05f;
constexpr float CAMERA_FOLLOW_SPEED = 0.07f;
constexpr float CAMERA_IDEAL_PLAYER_SIZE_RATIO = 0.15f;

// Jugador
constexpr float PLAYER_INITIAL_MASS = 2.0f;
constexpr float PLAYER_BASE_RADIUS = 15.0f;
constexpr float PLAYER_MAX_SPEED = 2.0f;
constexpr int PLAYER_ID = 1;
constexpr float SAFE_SPAWN_DISTANCE = 500.0f;

// Bots
constexpr int TOTAL_BOTS = 15;
constexpr float BOT_INITIAL_MASS_MIN = 2.0f;
constexpr float BOT_INITIAL_MASS_MAX = 2.0f;
constexpr float BOT_BASE_RADIUS_MIN = 15.0f;
constexpr float BOT_BASE_RADIUS_MAX = 15.0f;
constexpr float BOT_MAX_SPEED = 2.0f;
constexpr float BOT_DECISION_INTERVAL = 0.5f;
constexpr float BOT_RESPAWN_TIME = 3.0f;

// Radios de comportamiento de bots
constexpr float BOT_VIEW_RADIUS = 300.0f;
constexpr float BOT_FLEE_RADIUS = 200.0f;
constexpr float BOT_CHASE_RADIUS = 250.0f;
constexpr float BOT_SAFE_DISTANCE = 300.0f;
constexpr float BOT_SPAWN_RADIUS = 150.0f;

// Umbrales de prioridad para decisiones
constexpr float BOT_THREAT_PRIORITY_THRESHOLD = 1.5f;
constexpr float BOT_PREY_PRIORITY_THRESHOLD = 1.2f;
constexpr float BOT_FOOD_PRIORITY_MULTIPLIER = 0.5f;
constexpr float BOT_SIZE_COMPARISON_THRESHOLD = 1.2f;

// Pellets
constexpr int TOTAL_PELLETS = 1500;
constexpr float PELLET_INITIAL_MASS_MIN = 0.2f;
constexpr float PELLET_INITIAL_MASS_MAX = 0.4f;
constexpr float PELLET_BASE_RADIUS_MIN = 5.0f;
constexpr float PELLET_BASE_RADIUS_MAX = 7.0f;
constexpr float PELLET_RESPAWN_TIME = 0.5f;

// Factores de movimiento
constexpr float MOVEMENT_FRICTION = 0.9f;
constexpr float MOVEMENT_ACCELERATION = 0.4f;
constexpr float MOVE_TOWARDS_THRESHOLD = 20.0f;
constexpr float MOVE_TOWARDS_STOP_DISTANCE = 0.5f;
constexpr float MOVE_TOWARDS_SLOW_FACTOR = 0.5f;

// Límites de velocidad
constexpr float MAX_SPEED_MULTIPLIER_COLLISION = 2.0f;
constexpr float COLLISION_REBOUND_FORCE = 5.0f;
constexpr float PELLET_MASS_FACTOR = 0.5f;

// Crecimiento al comer
constexpr float EAT_MASS_GAIN_FACTOR = 0.65f;
constexpr float EAT_SCORE_MULTIPLIER = 5.0f;
constexpr float EAT_SIZE_COMPARISON_THRESHOLD = 1.1f;

// Quad
constexpr int QUADTREE_DEFAULT_CAPACITY = 4;
constexpr int QUADTREE_MAX_LEVELS = 8;
constexpr float QUADTREE_COLLISION_PADDING = 5.0f;

// Grid
constexpr float GRID_BASE_SPACING = 50.0f;
constexpr float GRID_ZOOM_LOW_THRESHOLD = 0.7f;
constexpr float GRID_ZOOM_HIGH_THRESHOLD = 1.5f;
constexpr float GRID_SPACING_FAR = 100.0f;
constexpr float GRID_SPACING_CLOSE = 25.0f;

// Texto
constexpr float ENTITY_TEXT_MIN_RADIUS = 15.0f;
constexpr float ENTITY_TEXT_MIN_ZOOM = 0.7f;

// Menu
constexpr float GAME_OVER_DELAY = 3.0f;

// Victoria
constexpr float VICTORY_SIZE_RATIO = 0.75f;
constexpr float VICTORY_RADIUS = (WORLD_WIDTH * VICTORY_SIZE_RATIO) / 2.0f;

constexpr float TEXTURE_BASE_SIZE = 64.0f;
constexpr float TEXTURE_BASE_RADIUS = 32.0f;


template <typename T>
class vector {
private:
    T* datos;           // Puntero a los datos
    size_t capacidad;   // Capacidad total
    size_t longitud;    // Número actual de elementos

    // Redimensionar cuando sea necesario
    void redimensionar() {
        size_t nuevaCapacidad = (capacidad == 0) ? 1 : capacidad * 2;

        T* nuevosDatos = static_cast<T*>(operator new[](nuevaCapacidad * sizeof(T)));

        for (size_t i = 0; i < longitud; i++) {
            new (&nuevosDatos[i]) T(std::move(datos[i]));
            datos[i].~T();
        }

        operator delete[](datos);

        datos = nuevosDatos;
        capacidad = nuevaCapacidad;
    }


public:

    using iterator = T*;
    using const_iterator = const T*;

    // Iteradores
    iterator begin() { return datos; }
    iterator end() { return datos + longitud; }
    const_iterator begin() const { return datos; }
    const_iterator end() const { return datos + longitud; }

    // Constructor
    vector() : datos(nullptr), capacidad(0), longitud(0) {}

    // Destructor
    ~vector() {
        delete[] datos;
    }

    // Constructor de copia
    vector(const vector& otro) {
        capacidad = otro.capacidad;
        longitud = otro.longitud;
        datos = new T[capacidad];
        for (size_t i = 0; i < longitud; i++) {
            datos[i] = otro.datos[i];
        }
    }

    // Constructor con lista
    vector(std::initializer_list<T> lista)
        : datos(nullptr), capacidad(0), longitud(0) {

        reserve(lista.size());

        for (const auto& elemento : lista) {
            push_back(elemento);
        }
    }

    // Operador de asignación
    vector& operator=(const vector& otro) {
        if (this != &otro) {
            delete[] datos;
            capacidad = otro.capacidad;
            longitud = otro.longitud;
            datos = new T[capacidad];
            for (size_t i = 0; i < longitud; i++) {
                datos[i] = otro.datos[i];
            }
        }
        return *this;
    }

    // Añadir elemento al final
    void push_back(const T& valor) {
        if (longitud >= capacidad) {
            redimensionar();
        }
        datos[longitud++] = valor;
    }

    // Construccion directamente en el vector
    template <typename... Args>
    T& emplace_back(Args&&... args) {
        if (longitud >= capacidad) {
            redimensionar();
        }

        new (&datos[longitud]) T(std::forward<Args>(args)...);

        return datos[longitud++];
    }

    // Eliminar último elemento
    void pop_back() {
        if (longitud > 0) {
            longitud--;
        }
    }

    // Borrar elemento especifico
    iterator erase(iterator pos) {
        size_t indice = pos - datos;

        for (size_t i = indice; i < longitud - 1; i++) {
            datos[i] = datos[i + 1];
        }

        longitud--;
        return datos + indice;
    }

    // Reservar capacidad
    void reserve(size_t nuevaCapacidad) {
        if (nuevaCapacidad > capacidad) {
            T* nuevosDatos = new T[nuevaCapacidad];
            for (size_t i = 0; i < longitud; i++) {
                nuevosDatos[i] = datos[i];
            }
            delete[] datos;
            datos = nuevosDatos;
            capacidad = nuevaCapacidad;
        }
    }

    // Acceso por índice
    T& operator[](size_t indice) {
        return datos[indice];
    }

    const T& operator[](size_t indice) const {
        return datos[indice];
    }

    // Tamaño actual
    size_t size() const {
        return longitud;
    }

    // Capacidad actual
    size_t capacity() const {
        return capacidad;
    }

    // ¿Está vacío?
    bool empty() const {
        return longitud == 0;
    }

    // Limpiar vector
    void clear() {
        longitud = 0;
    }
};

template <typename T1, typename T2>
struct pair {
    // Atributos públicos (como en std::pair)
    T1 first;
    T2 second;

    // Constructores
    pair() : first(T1()), second(T2()) {}

    pair(const T1& a, const T2& b) : first(a), second(b) {}

    pair(const pair& otro) : first(otro.first), second(otro.second) {}

    // Asignación
    pair& operator=(const pair& otro) {
        if (this != &otro) {
            first = otro.first;
            second = otro.second;
        }
        return *this;
    }
};

// Ordenar elementos
template <typename Iterator, typename Comparator>
void sort(Iterator begin, Iterator end, Comparator comp) {
    if (begin >= end || begin + 1 >= end) return;

    // QuickSort con pivote al final
    Iterator pivot = end - 1;
    Iterator i = begin;

    for (Iterator j = begin; j < pivot; ++j) {
        if (comp(*j, *pivot)) {
            std::swap(*i, *j);
            ++i;
        }
    }
    std::swap(*i, *pivot);

    // Recursión
    sort(begin, i, comp);
    sort(i + 1, end, comp);
}

// Find
template <typename T>
typename vector<T>::iterator find(vector<T>& vec, const T& valor) {
    for (auto it = vec.begin(); it != vec.end(); ++it) {
        if (*it == valor) {
            return it;
        }
    }
    return vec.end();
}

// Find constante
template <typename T>
typename vector<T>::const_iterator find(const vector<T>& vec, const T& valor) {
    for (auto it = vec.begin(); it != vec.end(); ++it) {
        if (*it == valor) {
            return it;
        }
    }
    return vec.end();
}

template <typename T>
T min(T& a, T& b) {
    return a < b ? a : b;
}

template <typename T>
T &minP(T& a, T& b) {
    return a < b ? a : b;
}

template <typename T>
T max(T& a, T& b) {
    return a > b ? a : b;
}

template <typename T>
T &maxP(T& a, T& b) {
    return a > b ? a : b;
}

#endif //CONSTANTS_H

