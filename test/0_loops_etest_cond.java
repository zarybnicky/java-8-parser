class Main{
  static void abc(int x){
    ifj16.print(x);
  }
  static void run(){
    int c= 0;
    for (int i; c < 5; c = c + 1){
        abc(c);
    }
    while(c == 5){
      abc(c);
      c = c - 1;
    }
    do{
      abc(c);
      c = c + 2;
    }while(4==c);

    if (c>6){
      abc(c);
      ifj16.print("c > 6");
      c = c - 1;
    }
    else{
      //ifj16.readDouble();
      ifj16.print("abc");
    }
  }
}
