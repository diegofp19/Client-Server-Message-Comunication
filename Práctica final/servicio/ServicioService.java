package servicio;
import javax.jws.WebService;
import javax.jws.WebMethod;
import java.util.Random;

@WebService
public class ServicioService{

    @WebMethod
    public String capitalize(String message){
    
        String words[]=message.split("\\s");  
        String capitalizeWord="";  
        for(String w:words){  
            String first=w.substring(0,1);  
            String afterfirst=w.substring(1);  
            capitalizeWord+=first.toUpperCase()+afterfirst+" ";  
        }  
        return capitalizeWord.trim();  
    } 


}