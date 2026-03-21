#include "board.h"
#include <memory>
#include <iostream>
#include <fstream>

using namespace std;

Board::Board(int width, int height)
{
    m_width = width;
    m_height = height;

    //.assign sirve para rellenar m_grid, estamos metiendo en cada punto 
    //de la fila (m_width) una columna de largo m_height con casillas vacias (nullptr)
    m_grid.assign(m_width, vector<Candy*>(m_height, nullptr));
}

Board::~Board()
{
    //Aun no hace falta hacerlo
}

//Candy* significa que devuelve un puntero de candy
//m_grid[x][y] contiene un puntero candy

Candy* Board::getCell(int x, int y) const
{
    Candy* out = nullptr;
    
    //Verificamos que la posicion sea valida
    if (x >= 0 && x < m_width && y >= 0 && y < m_height)
    {
        out = m_grid[x][y];
    }
        
    return out;
}

void Board::setCell(Candy* candy, int x, int y)
{
    //Verificamos que la poscion sea valida
    if (x >= 0 && x < m_width && y >= 0 && y < m_height)
    {
        m_grid[x][y] = candy;
    }
}


int Board::getWidth() const
{
    return m_width;
}


int Board::getHeight() const
{
    return m_height;
}

//countInDirecton metodo auxiliar propio creado como ayuda para shouldExplode
//CanyType es una clase enumerada (una lista para darle nombres a los colores de los caramelos
//situada en candy.h. La clase asigna numeros a las  palabras RED=0, BLUE=1...
//dx y dy definen la direccion en la que nos moveremos: dx=1, dy = 0 derecha, etc..
int Board::countInDirection(int startX, int startY, int dx, int dy, CandyType type) const
{
    
    //Cuenta la cantidad de caramelos iguales despues del inicial en la
    //direccion indicada por dx y dy
    int comptador = 0;

    //Nos movemos una posicion a partir de la posicion inicial
    int currentX = startX + dx;
    int currentY = startY + dy;

    //Miramos el tipo de caramela en la posicion actual
    Candy* c = getCell(currentX, currentY);

    //Verificamos si el caramelo es el mismo que el de la posicion inicial
    while (c != nullptr && (*c).getType() == type)
    {
        comptador++;

        //Nos movemos una posicion a partir de la posicion actual
        currentX += dx;
        currentY += dy;

        //Miramos el tipo de caramelo en la posicion actual
        c = getCell(currentX, currentY);
    }

    return comptador;
}

bool Board::shouldExplode(int x, int y) const
{
    //Por defecto no explota
    bool out = false;

    //Obtenemos el tipo de caramelo por referencia de la casilla central
    Candy* centerCandy = getCell(x, y);

    //Si en la casilla central no hay caramelo, se acaba, sino seguimos
    if (centerCandy != nullptr)
    {
        //Conseguimos el tipo de caramelo, para el metodo countInDirection
        CandyType type = (*centerCandy).getType();

        //Calculamos las cuatro líneas posibles, el +1 es para contar el caramelo central tambien
        int horizontal = countInDirection(x, y, -1, 0, type) + countInDirection(x, y, 1, 0, type) + 1;
        int vertical = countInDirection(x, y, 0, -1, type) + countInDirection(x, y, 0, 1, type) + 1;
        int diagDownUp = countInDirection(x, y, -1, -1, type) + countInDirection(x, y, 1, 1, type) + 1;
        int diagUpDown = countInDirection(x, y, -1, 1, type) + countInDirection(x, y, 1, -1, type) + 1;

        //Si alguna de las líneas llega al mínimo para explotar, ponemos la salida en true
        if (horizontal >= SHORTEST_EXPLOSION_LINE || vertical >= SHORTEST_EXPLOSION_LINE ||
            diagDownUp >= SHORTEST_EXPLOSION_LINE || diagUpDown >= SHORTEST_EXPLOSION_LINE)
        {
            out = true;
        }
    }

    return out;
}

//Retornamos un vector con la cantidad de caramelos explotados:
//vector<Candy*>
vector<Candy*> Board::explodeAndDrop()
{
    
    //Declaracion del vector que retornamos con la cantidad de caramelos explotados
    vector<Candy*> explodedTotal;

    //Detecta si ha habido una explosion
    bool hadExplosions;

    do
    {
        //Al inicio no hay ninguna explosion = false
        hadExplosions = false;

        //Declaramos un vector dentro de un vector que contiene valores de true (explota)
        //o false (no explota) con el tamaño del tablero que es por defecto 10x10
        vector<vector<bool>> toExplode(m_width, vector<bool>(m_height, false));

        //Este for identifica las casillas que deben explotar pasando por todas ellas
        for (int x = 0; x < m_width; x++)
        {
            for (int y = 0; y < m_height; y++)
            {
                //Usamos el metodo shoulExplode, devuelve true si hay 3
                //caramelos o mas del mismo tipo y por lo tanto deben explotar
                if (shouldExplode(x, y))
                {
                    //Ponemos en true la casilla donde el caramelo explota
                    //y true hadExplosions porque ha habido una explosion
                    toExplode[x][y] = true;
                    hadExplosions = true;
                }
            }
        }

        //Si ha habido al menos una explosion se ejecuta
        if (hadExplosions)
        {
            //Este for guarda la cantidad de caramelos que han explotado y
            //vacia sus casillas
            for (int x = 0; x < m_width; x++)
            {
                for (int y = 0; y < m_height; y++)
                {
                    //Si explota se ejecuta
                    if (toExplode[x][y])
                    {
                        //Guarda la referencia del caramelo explotado
                        Candy* c = getCell(x, y);

                        //Si la casilla no esta vacia, guarda el caramelo en
                        //explodedTotal, ampliando el vector y añadiendo su refencia
                        if (c != nullptr)
                        {
                            explodedTotal.push_back(c);
                            //Eliminamos el caramelo que debe explotar
                            setCell(nullptr, x, y);
                        }
                    }
                }
            }

            for (int x = 0; x < m_width; x++)
            {
                //'writeY' es el "hueco destino". Empezamos asumiendo que la posición 
                //más baja disponible para que caiga un caramelo es el fondo del tablero.
                int writeY = m_height - 1;

                //'readY' es nuestro "explorador". Empieza en el fondo y va subiendo 
                //casilla a casilla (restando 1) buscando caramelos que sigan vivos.
                for (int readY = m_height - 1; readY >= 0; readY--)
                {
                    // Miramos qué hay en la casilla que el explorador está revisando
                    Candy* c = getCell(x, readY);

                    // Solo hacemos algo si encontramos un caramelo real (ignoramos los huecos vacíos)
                    if (c != nullptr)
                    {
                        // Si readY y writeY coinciden, el caramelo ya está en el punto más bajo posible.
                        // Solo lo movemos si está "flotando" (readY es distinto a writeY).
                        if (readY != writeY)
                        {
                            // Vaciamos la casilla original donde estaba flotando
                            setCell(nullptr, x, readY);
                            // Teletransportamos el caramelo al hueco destino más bajo
                            setCell(c, x, writeY);
                        }

                        // Independientemente de si el caramelo cayó o si ya estaba bien colocado al fondo,
                        // acabamos de asegurar un caramelo en la posición 'writeY'. Por lo tanto, 
                        // el nuevo hueco libre estará un piso más arriba.
                        writeY--;
                    }
                }
            }
        }
    // Si hubo explosiones, el do-while vuelve a empezar para buscar combos
    } while (hadExplosions); 

    return explodedTotal;
}

bool Board::dump(const string& output_path) const
{
    //Devuelve true si se pudo realizar la escritura, false en caso contrario
    bool success = false;

    //Creamos la variable y luego abrimos el archivo
    ofstream fitxer;
    fitxer.open(output_path);

    //Comprobamos si el archivo se ha abierto correctamente
    if (fitxer.is_open())
    {
        //Recorremos el tablero fila por fila
        for (int y = 0; y < m_height; y++)
        {
            for (int x = 0; x < m_width; x++)
            {
                Candy* c = getCell(x, y);

                if (c == nullptr)
                {
                    fitxer << "-1 ";
                }
                else
                {
                    //Obtenemos el tipo de caramelo y lo transformamos en un numero
                    fitxer << (int)(*c).getType() << " ";
                }
            }
            //Salto de linea al acabar la fila
            fitxer << "\n";
        }

        //Guardamos los cambios cerrando el archivo
        fitxer.close();

        //Ponemos true, ya que todo se pudo realizar la escritura correctamente
        success = true;
    }

    return success;
}

bool Board::load(const string& input_path)
{
    //Devuelve true si se pudo realizar la lectura, false en caso contrario
    bool success = false;

    //Abrimos el archivo en modo lectura
    ifstream fitxer;
    fitxer.open(input_path);

    //Comprobamos si el archivo se ha abierto bien
    if (fitxer.is_open())
    {
        //Recorremos el tablero fila por fila
        for (int y = 0; y < m_height; y++)
        {
            for (int x = 0; x < m_width; x++)
            {
                int typeValue;

                //Leemos el siguiente número del archivo y lo metemos en typeValue
                //El operador >> se salta los espacios y saltos de línea solo
                if (fitxer >> typeValue)
                {
                    //Si el numero es -1, la casilla debe estar vacía
                    if (typeValue == -1)
                    {
                        setCell(nullptr, x, y);
                    }
                    else
                    {
                        //Si es un número de color (0, 1, 2...), ponemos un caramelo nuevo.
                        //Usamos CandyType para convertir el numero al tipo de caramelo del juego
                        Candy* nuevoCaramelo = new Candy((CandyType)typeValue);
                        setCell(nuevoCaramelo, x, y);
                    }
                }
            }
        }

        //Guardamos los cambios cerrando el archivo
        fitxer.close();

        //Ponemos true, ya que todo se pudo realizar la lectura correctamente
        success = true;
    }

    return success;
}
