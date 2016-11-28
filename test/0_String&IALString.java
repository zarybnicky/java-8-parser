class Main
{
    static void run() {
        String a = ifj16.readString();
        String b = "hgfeabcd";
	    int comp = ifj16.compare(a, b);
	
	      if (comp != 0){
            ifj16.print("Stringy jsou ruzne! Zkousim seradit a znovu porovnat.\n");
	          comp = ifj16.compare(ifj16.sort(b), ifj16.sort(a));

            if (comp != 0){
    	          ifj16.print("Stringy jsou ruzne! Zkousim hledat zadane jako podretezec.\n");
    		        comp = ifj16.find(b, a);

        		    if (comp < 1){
        		        ifj16.print("Zadany string neni podretezcem nebo je prazdny.\n");
        		        ifj16.print("Bude vam vytisknut vychozi retezec"+b+"\n");
        		    } else {

        		        ifj16.print("Zadany string je podretezcem na pozici "+comp+".\n");
         		    }

            } else {

    	          ifj16.print("Stringy jsou totozne po serazeni vstupu.\n");
    	      }
	    
	      } else {
	
	          ifj16.print("Stringy jsou totozne.\n");
      	}
	
    }
}
