class Main{
  static void abc(int x){
    ifj16.print(x+"\n");
  }
  static void run(){
    int c= 0;
    for (int i; c < 5; c = c + 1){
        abc(c);
    }
    ifj16.print("\nEND FOR:\n START WHILE:");
    while(c == 5){
      abc(c);
      c = c - 1;
    }
    ifj16.print("\nEND WHILE:\n START DOWHILE:\n");
    do{
      abc(c);
      c = c + 2;
      ifj16.print(c + "\n");
    }while(c==4);

    ifj16.print("END DO WHILE:\n START CONDITION:\n");
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
