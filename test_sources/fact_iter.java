/* Program 1: Vypocet faktorialu (iterativne) */
class Main
{
    static void run()
    {
        int a;
        ifj16.print("Zadejte cislo pro vypocet faktorialu: ");
        a = ifj16.readInt();
        int vysl;
        if (a < 0) { // nacitani zaporneho cisla nemusite podporovat
            ifj16.print("Faktorial nelze spocitat!\n");
        }
        else {
            vysl = 1;
            while (a > 0) {
                vysl = vysl * a;
                a = a - 1;
            }
            ifj16.print("Vysledek je: " + vysl + "\n");
        }
    }
}
