class Main{
  static void run(){
    ifj16.print("LEAST UPDATE 29.11.2016 of function compare lets see\n");
    String s1 = "abc";
    String s2 = "ABC";
    int length = ifj16.compare(s1,s2);
    ifj16.print("\nCOMPARE RESULT :\n"+length + "\n");

    ifj16.print("------------------ START OF PROGRAM -----------------\n");
    ifj16.print("Lets first check if ifj16 is working well");
    ifj16.print("READ int:\n");
    int a = ifj16.readInt();
    ifj16.print("RESULT readInt function: \n" + a + "\nREAD double:\n");
    double b = ifj16.readDouble();
    ifj16.print("RESULT readDouble function: \n" + b + "\nREAD string:\n");
    String c = ifj16.readString();
    ifj16.print("RESULT readString function: \n" + c + "\nWORK with string\n");
    ifj16.print("length\n");
    int l;
    l = ifj16.length(c);
    ifj16.print("RESULT length function: \n" + l + "\nNOW SubString\n");
    //retype DOUBLE!?!?
    int retype = 5;
    String sub = ifj16.substr(c,retype,a-6);
    ifj16.print("RESULT substring function: \n" + sub + "\nNow compare\n");
    l = ifj16.compare(c,sub);
    ifj16.print("RESULT compare function: \n" + l + "\nNow find\n");
    l = ifj16.find(c,sub);
    ifj16.print("RESULT find function: \n" + l + "\nNow sort\n");
    String new = ifj16.sort(c);
    ifj16.print("RESULT sort funciton: \n" + new + "\nEND of all builtin EVERYTHING WENT WELL GOOD JOB\n");
    ifj16.print("------------------- END OF PROGRAM ------------------\n");
  }
}