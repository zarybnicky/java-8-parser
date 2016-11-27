class Main{
    static void run(){

        int b = 5;
        ifj16.print(b);
        ifj16.print("ahoj svet tu som ja\n");

        //b = ifj16.readInt();
        // ifj16.print(b);

        double g = 5.2;
        ifj16.print(g);

        String str = "Hello World!\n";
        ifj16.print(str);

        // ifj16.print("test"+"je tu length\n");
        ifj16.print(ifj16.length("hellooo"));
        ifj16.print("ahoj\n");


        String substr = ifj16.substr("hellooo",1,3);

        ifj16.print(substr+"\n");

        String s_a = "Ahoj";
        String s_aa = "Ahoj";

        String s_b = "Marek";

        ifj16.print("1 - compare\n");

        if(ifj16.compare(s_a, s_aa)){
            ifj16.print("rovnake\n");
        }
        else{
            ifj16.print("rozne\n");
        }

        ifj16.print("2 - compare\n");


        if(ifj16.compare(s_a, s_b)){
            ifj16.print("rovnake\n");
        }
        else{
            ifj16.print("rozne\n");
        }

        String f_a = "Ahojhojhoj";
        String f_b = "oj";

        int i;

        i = ifj16.find(f_a,f_b);

        ifj16.print(i);
        ifj16.print("\n");



    }
}
