class Main {
    static void run() {

        boolean a = true;
        boolean b = false;

        boolean c = (5 > 3);


        if (a)  // negates int
            ifj16.print("1. true\n");


        if (b)
            ifj16.print("2. false\n");


        if (a && 5 == 5)
            ifj16.print("3. true\n");


        if (a && !(5 < 3))
            ifj16.print("4. true\n");


        if (a && (5 >= 6))
            ifj16.print("5. false\n");


        if (a && (5 >= 6))
            ifj16.print("6. false\n");


        if (a && (a || b))
            ifj16.print("7. true\n");


        if (a && (a || b))
            ifj16.print("8. true\n");


        if (b || (a || b))
            ifj16.print("9. true\n");


        if ((b && a) || b)
            ifj16.print("10. false\n");


        if (!((b && a) || b))
            ifj16.print("11. true\n");


        if(c)
            ifj16.print("12. true\n");


        c = !(5 < 4);

        if(c)
            ifj16.print("13. true\n");



    }
}
