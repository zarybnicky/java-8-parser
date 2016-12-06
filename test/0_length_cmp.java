class Main{
  static void run (){
    int b;
    b = test.len("ThisStringHas25characters","Nothing important that should be checked!");
    ifj16.print("Length of string is " + b);
    int a;
    a = test.cmp("same","Same","noCompare",4);
    ifj16.print("\nCompare of string is " + a);
    if (ifj16.length("x\nz") == 3)
      ifj16.print("\nThis should be printed!\n");
  }
}

class test{
  static int len(String a,String b){
    int ret = ifj16.length(a);
    return ret;
  }
  static int cmp(String a,String b, String c, int d){
    return ifj16.compare(a,b);
  }
}