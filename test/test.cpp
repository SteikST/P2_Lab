/**
 * FITXER test.cpp
 * AUTOR Ferran Sanchez Bargas i Sabir Allouch El Imrani
 * DATA 02/06/2026
 * VERSIO 2.0 (Segon Lliurament)
 * Implementacio dels tests unitaris per verificar el funcionament de Board i Game.
 */

#include <filesystem>
#include "board.h"
#include "candy.h"
#include "controller.h"
#include "game.h"
#include "util.h"

bool test()
{
    //***1. TEST board, getWidth, getHeight y Constructor***
    //Mediante el constructor hemos definido el tamaño del board
    Board b(10, 10);

    //Si getWidth o getHeight no devuelve las dimensiones correctas, devuelve false
    if (b.getWidth() != 10 || b.getHeight() != 10)
    {
        return false;
    }

    //Comprobamos que el constructor empieza vacío (con nullptr)
    // Miramos la primera y la última casilla para asegurarnos.
    if (b.getCell(0, 0) != nullptr || b.getCell(9, 9) != nullptr)
    {
        return false;
    }

    //***2. TEST setCell y getCell***
    //Creamos un caramelo naranja
    Candy c(CandyType::TYPE_ORANGE);

    //Lo ponemos en la esquina superior izquierda
    b.setCell(&c, 0, 0);

    //Caso válido, getCell debe devolver el caramelo naranja
    //Si el caramelo es diferente, devuelve false
    if (b.getCell(0, 0) != &c)
    {
        return false;
    }

    //Caso inválido, pedir una coordenada fuera del tablero (ej. -1 o 100)
    //Nos hemos asegurado que devuelva nullptr en este caso, si devuelve un 
    //caramelo, devolvemos false
    if (b.getCell(-1, 0) != nullptr || b.getCell(100, 100) != nullptr)
    {
        return false; // El método debería devolver nullptr de forma segura
    }

    //***3. TEST shouldExplode y explodeAndDrop***
    {
        //Creamos un board y caramelos para comprobar la explosion
        Board b_expl(10, 10);
        Candy c1(CandyType::TYPE_BLUE);
        Candy c2(CandyType::TYPE_BLUE);
        Candy c3(CandyType::TYPE_BLUE);
        Candy c4(CandyType::TYPE_RED);

        //Colocamos 3 caramelos azules en fila horizontal y uno rojo suelto
        b_expl.setCell(&c1, 0, 9);
        b_expl.setCell(&c2, 1, 9);
        b_expl.setCell(&c3, 2, 9);
        b_expl.setCell(&c4, 5, 9);

        //Prueba de shouldExplode: Los azules deben explotar, el rojo y el vacio NO
        //Si uno de ellos no explota, devuelve false
        if (!b_expl.shouldExplode(0, 9) || !b_expl.shouldExplode(1, 9) || !b_expl.shouldExplode(2, 9))
        {
            return false;
        }
        //Si el rojo explota, devuelve false
        if (b_expl.shouldExplode(5, 9) || b_expl.shouldExplode(0, 0))
        {
            return false;
        }

        //Prueba de explodeAndDrop
        //La funcion explodeAndDrop devuelve la cantidad de caramelos
        //que han explotado en un vector
        std::vector<Candy*> exploded = b_expl.explodeAndDrop();

        //Debería habernos devuelto exactamente 3 caramelos (los azules)
        //Si no nos devuelve los caramelos, devuelve false
        if (exploded.size() != 3)
        {
            return false;
        }

        //Las casillas donde estaban los azules ahora deben ser nullptr,
        //si no son nullptr, devuelve false
        if (b_expl.getCell(0, 9) != nullptr || b_expl.getCell(1, 9) != nullptr || b_expl.getCell(2, 9) != nullptr)
        {
            return false;
        }

        //El caramelo rojo debe de seguir en su sitio (no ha explotado)
        //Si se ha movido, devuelve false
        if (b_expl.getCell(5, 9) != &c4)
        {
            return false;
        }
    }

    //***3.5. TEST Reacción en cadena***
    {
        Board b_chain(10, 10);
        Candy cb1(CandyType::TYPE_BLUE), cb2(CandyType::TYPE_BLUE), cb3(CandyType::TYPE_BLUE);
        Candy cr1(CandyType::TYPE_RED), cr2(CandyType::TYPE_RED), cr3(CandyType::TYPE_RED);

        // Fila del suelo: 1 rojo a la izquierda, y 3 azules a su derecha
        b_chain.setCell(&cr1, 0, 9);
        b_chain.setCell(&cb1, 1, 9);
        b_chain.setCell(&cb2, 2, 9);
        b_chain.setCell(&cb3, 3, 9);

        // Fila de arriba: 2 rojos esperando a caer en las columnas 1 y 2
        b_chain.setCell(&cr2, 1, 8);
        b_chain.setCell(&cr3, 2, 8);

        // Al explotar los 3 azules, los 2 rojos caen al suelo.
        // Al caer, se juntan con el rojo de la posición (0,9) formando una nueva línea de 3.
        // El bucle 'do-while' del código detectará esta nueva línea y también la explotará.
        std::vector<Candy*> exploded_chain = b_chain.explodeAndDrop();

        // Tienen que haber explotado 6 caramelos en total (3 azules + 3 rojos)
        if (exploded_chain.size() != 6)
        {
            return false;
        }
    }

    //***4. TEST Dump y Load board***
    {
        //Creamos un tablero vacio
        Board b2(10, 10);

        //Comprobar que pasa si intentamos leer un fichero que no existe
        // El metodo load debe devolver false para proteger el programa.
        if (b2.load(getDataDirPath() + "archivo_inventado_que_no_existe.txt") != false)
        {
            return false;
        }

        //getDataDirPath busca el archivo de texto en la carpeta correcta
        //Iniciamos la funcion de escritura dump en el archivo de texto
        //Si por algun motivo no se pudo realizar la escritura, devuelve false
        if (!b.dump(getDataDirPath() + "dump_board.txt"))
        {
            return false;
        }

        //Si por algun motivo no se pudo realizar la lectura, devuelve false
        if (!b2.load(getDataDirPath() + "dump_board.txt"))
        {
            return false;
        }

        //Miramos que tipo de caramelo hay en 0,0
        //Si es distinto al caramelo de color naranja del inicio
        //la informacion se ha perdido y devuelve fals
        if ((*b2.getCell(0, 0)).getType() != c.getType())
        {
            return false;
        }

        //Borramos el archivo temporal de escritura y lectura
        std::filesystem::remove(getDataDirPath() + "dump_board.txt");
    }

    //***5. TEST Dump and Load game***
    {
        //Creamos un nuevo juego y un controlador vacio
        Game g;
        Controller cont;

        //Actualizamos el juego una vez para que inicie su estado
        g.update(cont);

        //Iniciamos la funcion de escritura dump en el archivo de texto
        //Si por algun motivo no se pudo realizar la escritura, devuelve false
        if (!g.dump(getDataDirPath() + "dump_game.txt"))
        {
            return false;
        }

        //Creamos un segundo juego vacio donde cargaremos la partida
        Game g2;

        //Iniciamos la funcion de lectura load del archivo de texto
        //Si por algun motivo no se pudo realizar la lectura, devuelve false
        if (!g2.load(getDataDirPath() + "dump_game.txt"))
        {
            return false;
        }

        //Usamos el operator== para comprobar si ambos juegos son identicos
        //Si hay alguna diferencia en el estado, devuelve false
        if (g != g2)
        {
            return false;
        }

        //Borramos el archivo temporal de escritura y lectura
        std::filesystem::remove(getDataDirPath() + "dump_game.txt");
    }

    //Si pasamos todas las condiciones, devolvemos true, tests superados
    return true;
}