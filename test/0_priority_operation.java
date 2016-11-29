class Main{
  static double abc(){
    double a = 5.0;
    double b = 5.0;
    return a*a+b*b;
    //should return 50 priority check!!
  }
  static void run (){
    double n;
    //uninitialized
    n  = abc();
    ifj16.print(n);
  }
}