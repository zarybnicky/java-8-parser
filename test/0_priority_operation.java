class Main{
    static void run () {
        double a = 3.0;
        double b = 5.0;
        ifj16.print("a = 3\n");
        ifj16.print("b = 5\n");
        ifj16.print("a * a + b * b = 34? " + (a * a + b * b) + "\n");
        ifj16.print("a * (a + b) * b = 120? " + (a * (a + b) * b) + "\n");
        ifj16.print("(-a) = -3? " + (-a) + "\n");
        ifj16.print("-a = -3? " + -a + "\n");
        ifj16.print("--a = 2? " + --a + "\n");
        ifj16.print("a++ = 2? " + a++ + "\n");
        ifj16.print("a+5 = 8? " + (a + 5) + "\n");
        ifj16.print("!false && 5 != 1 + 3 * 1 = true? " + (!false && 5 != 1 + 3 * 1) + "\n");
    }
}
