class Main{
  static void run (){
    int b;
    b = test.len("ThisStringHas25characters","Nothing important that should be checked!");
    ifj16.print("Length of string is" + b);
    int a;
    a = test.cmp("same","Same","noCompare",4);
    ifj16.print("Compare of string is " + a);
    if (ifj16.length("x\nz") == 3)
      ifj16.print("This should be printed!");
  }
}

class test{
  static int len(String a,String b){
    return ifj16.length(a);
  }
  static int cmp(String a,String b, String c, int d){
    return ifj16.compare(a,b);
  }
}