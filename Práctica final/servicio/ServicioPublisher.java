
package servicio;
import javax.xml.ws.Endpoint;



public class ServicioPublisher {


    public static void main(String[ ] args) {
        final String url = "http://localhost:8888/rs";
        System.out.println("Publishing UpperCaseService at endpoint " + url);
        Endpoint.publish(url, new ServicioService());
    }
}