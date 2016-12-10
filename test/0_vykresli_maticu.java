class Main{
  static void run(){
    int a = 5;
    Vykresli.matica(a,a*a);
  }
}

class Vykresli{
  static void matica(int row, double iterations){
    int tmp = row;
    while (iterations >= 0){
      row = tmp;
      ifj16.print(iterations + "Matica:\n");
      while(row >= 0){
        Vypis.maticu(iterations,row);
        row = row-1;
      }
      ifj16.print("\n");
      iterations = iterations - 1;
    }
    return;
  }
}

class Vypis{
  static void maticu(double iterations, int row){
    if (row == 5)
      ifj16.print("|E|");
    else if (row == 4)
      ifj16.print("|D|");
    else if (row == 3)
      ifj16.print("|C|");
    else if (row == 2)
      ifj16.print("|B|");
    else if (row == 1){
      ifj16.print("|A|\n");
    }
    // else{
      return;
    // }
  }
}