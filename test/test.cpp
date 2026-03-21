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

    //***4. TEST Dump y Load board***
    {
        //Creamos un tablero vacio
        Board b2(10, 10);
        
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

    //***5. TEST Dump and Load game (COMENTADO PARA EL PRIMER LLIURAMENT)***
    /*
    {
        Game g;
        Controller cont;
        g.update(cont);
        if (!g.dump(getDataDirPath() + "dump_game.txt"))
        {
            return false;
        }
        Game g2;
        if (!g2.load(getDataDirPath() + "dump_game.txt"))
        {
            return false;
        }
        if (g != g2)
        {
            return false;
        }
        std::filesystem::remove(getDataDirPath() + "dump_game.txt");
    }
    */

    //Si pasamos todas las condiciones, devolvemos true, tests superados
    return true;
}