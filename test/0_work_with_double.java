class Main{
  static double abc(){
    double a = 5.6;
    double b = 4.4;
    return a+b;
  }
  static void run (){
    double n;
    //uninitialized
    n  = abc();
  }
}