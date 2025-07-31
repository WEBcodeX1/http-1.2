import org.json.*;


public class WebApp {
    public static String invoke(String input) {
        System.out.println("Input:" + input);
        JSONObject ProcessJSONObj = new JSONObject(input);
        Object PayloadObj = ProcessJSONObj.get("payload");
        return PayloadObj.toString();
    }
}
