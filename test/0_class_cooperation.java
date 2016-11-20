class test{
    static String s = "ahoj";

    static int testing(String a, String b){
        int ret = ifj16.compare(a, b);
        return ret;
    }
}

class Main{
    static void run(){
        String a = "ahoj";
        int comp1 = ifj16.compare(a, test.s);
        int comp2 = test.testing(a, test.s);

        if (comp1 == comp2){
            ifj16.print("Dopadlo to dobre.\n");
        } else {
            ifj16.print("Toto by nemelo nastat!\n");
        }
    }
}