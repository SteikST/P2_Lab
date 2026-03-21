#include <filesystem>
#include "board.h"
#include "candy.h"
#include "controller.h"
#include "game.h"
#include "util.h"

bool test()
{
    // --- 1. Test board 2D container y dimensiones ---
    Board b(10, 10);
    if (b.getWidth() != 10 || b.getHeight() != 10)
    {
        return false; // Fallo si las dimensiones no son correctas
    }

    Candy c(CandyType::TYPE_ORANGE);
    b.setCell(&c, 0, 0);

    // Caso válido
    if (b.getCell(0, 0) != &c)
    {
        return false;
    }

    // Caso inválido: pedir una coordenada fuera del tablero (ej. -1 o 100)
    if (b.getCell(-1, 0) != nullptr || b.getCell(100, 100) != nullptr)
    {
        return false; // El método debería devolver nullptr de forma segura
    }

    // --- 2. Test shouldExplode y explodeAndDrop ---
    {
        Board b_expl(10, 10);
        Candy c1(CandyType::TYPE_BLUE);
        Candy c2(CandyType::TYPE_BLUE);
        Candy c3(CandyType::TYPE_BLUE);
        Candy c4(CandyType::TYPE_RED);

        // Colocamos 3 caramelos azules en fila horizontal y uno rojo suelto
        b_expl.setCell(&c1, 0, 9);
        b_expl.setCell(&c2, 1, 9);
        b_expl.setCell(&c3, 2, 9);
        b_expl.setCell(&c4, 5, 9);

        // Prueba de shouldExplode: Los azules deben explotar, el rojo y el vacío NO.
        if (!b_expl.shouldExplode(0, 9) || !b_expl.shouldExplode(1, 9) || !b_expl.shouldExplode(2, 9))
        {
            return false;
        }
        if (b_expl.shouldExplode(5, 9) || b_expl.shouldExplode(0, 0))
        {
            return false;
        }

        // Prueba de explodeAndDrop
        std::vector<Candy*> exploded = b_expl.explodeAndDrop();

        // Debería habernos devuelto exactamente 3 caramelos
        if (exploded.size() != 3)
        {
            return false;
        }

        // Las casillas donde estaban los azules ahora deben ser nullptr
        if (b_expl.getCell(0, 9) != nullptr || b_expl.getCell(1, 9) != nullptr || b_expl.getCell(2, 9) != nullptr)
        {
            return false;
        }

        // El caramelo rojo intacto en su sitio
        if (b_expl.getCell(5, 9) != &c4)
        {
            return false;
        }
    }

    // --- 3. Dump and load board ---
    {
        Board b2(10, 10);
        if (!b.dump(getDataDirPath() + "dump_board.txt"))
        {
            return false;
        }
        if (!b2.load(getDataDirPath() + "dump_board.txt"))
        {
            return false;
        }
        if (b2.getCell(0, 0)->getType() != c.getType())
        {
            return false;
        }
        std::filesystem::remove(getDataDirPath() + "dump_board.txt");
    }

    // --- 4. Dump and load game (COMENTADO PARA EL PRIMER LLIURAMENT) ---
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

    return true; // ¡Todo perfecto!
}