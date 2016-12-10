class Main {
    static boolean b = true;
    static void run() {

        boolean a = false;
        boolean c;
        while (false) {}
        if (true)
            c = foo(true, a, " TF");
        if (!c)
            if (false)
                a = true;
            else if (true)
                foo(false,b," FT");
    }

    static boolean foo(boolean a, boolean b, String c) {
        ifj16.print(a+b+c+"\n");
        return (a && b);
    }
}
