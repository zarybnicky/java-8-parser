class Main{
  static void run(){
    int i = 0;

    for (int c; i < 10; i=i+1){
      ifj16.print("\nFibonnaci:" + Main.fibonacci(i));
    }

  }
  static int fibonacci(int i){
    if (i == 0)
      return 0;
    if (i == 1)
      return 1;
    return Main.fibonacci(i-1) + Main.fibonacci(i-2);
  }
}