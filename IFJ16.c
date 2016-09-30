import java.io.*;

class ifj16
{
  private static BufferedReader in = new BufferedReader(new InputStreamReader(System.in));
  private static int readErrCode = 7;
  private static int runErrCode = 10;

  public static void main(String[] args)
  {
    Main.run();  // ignore command-line arguments since IFJ16 does not support arrays in the basic specification
  }

  public static void print(String message)
  {
    System.out.print(message);
  }

  public static void print(int i)
  {
    System.out.print(i);
  }

  public static void print(double d)
  {
    System.out.print(d);
  }

  public static int readInt() 
  {
    int i = 0;
    try {
      i = Integer.parseInt(in.readLine());
    } 
    catch (NumberFormatException e) {
      System.err.println("Input error - Invalid value for an int.");
      System.exit(readErrCode);
      return 0;
    }
    catch (NullPointerException e) {
      System.err.println("Input error - null result from readLine; the result will be 0.");
      System.exit(readErrCode);
      return 0;
    }
    catch (IOException e) {
      System.err.println("Input error - IOException occured; the result will be 0.");
      System.exit(readErrCode);
      return 0;
    }
    return i;
  }

  public static double readDouble()
  {
    double d = Double.NaN;
    try {
      d = Double.parseDouble(in.readLine());
    } 
    catch (NumberFormatException e) {
      System.err.println("Input error - Invalid value for a double.");
      System.exit(readErrCode);
      return 0.0;
    }
    catch (NullPointerException e) {
      System.err.println("Input error - null result from readLine.");
      System.exit(readErrCode);
      return 0.0;
    }
    catch (IOException e) {
      System.err.println("Input error - IOException occured.");
      System.exit(readErrCode);
      return 0.0;
    }
    return d;
  }

  public static String readString()
  {
    String s;
    try {
      s = in.readLine();
    } 
    catch (IOException e) {
      System.err.println("Input error - IOException occured; the result will be the empty string.");
      return "";
    }
    if (s == null) {
      System.err.println("Input error - null result from readLine; the result will be empty string.");
      return "";
    }
    return s;
  }
  
  public static int length(String s)
  {
    return s.length();
  }
  
  public static String substr(String s, int i, int n)
  {
    try {
      return s.substring(i, i+n);
    }
    catch (IndexOutOfBoundsException e) {
      System.err.println("Substr error - IndexOutOfBoundsException occured.");
      System.exit(runErrCode);
      return "";
    }
  }
 
  public static int compare(String s1, String s2)
  {
    return s1.compareTo(s2);
  }
 
  public static int find(String s, String search)
  {
    return s.indexOf(search);
  }
  
  public static String sort(String s)
  {
    char[] c = s.toCharArray();
    java.util.Arrays.sort(c);
    return new String(c);
  } 

}

/* Zde bude nasledovat program jazyka IFJ16 */
