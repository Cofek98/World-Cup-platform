package bgu.spl.net.srv;

import java.util.concurrent.ConcurrentHashMap;

public class User {
    
    //fields:
    private int connectionId;
    private String userName;
    private String password;
    private boolean connected;
    public ConcurrentHashMap<Integer, String> subsById;
    public ConcurrentHashMap<String, Integer> subsByTopic;


    //methods:

    //constructor
    public User(int con, String name, String pass) {
        connectionId = con;
        userName = name;
        password = pass;
        connected = false;
        subsById = new ConcurrentHashMap<Integer, String>();
        subsByTopic = new ConcurrentHashMap<String, Integer>();

    }

    public int getConnectionId(){
        return connectionId;
    }

    public void setConnectionId(int conId){
        connectionId = conId;
    }

    public String getName(){
        return userName;
    }
    public String getPassword(){
        return password;
    }

    public boolean isConnected(){
        return connected;
    }

    public void connect(){
        connected = true;
    }

    public void disconnect(){
        connected = false;
        subsById.clear();
        subsByTopic.clear();
    }

    public void subscribe(String topic, int sub){
        subsById.put(sub, topic);
        subsByTopic.put(topic, sub);
    }

    public String unsubscribe(int sub){
        String topic = subsById.remove(sub);
        if (topic == null){
            return null;
        }
        subsByTopic.remove(topic);
        return topic;
        
    }

    public boolean isSubscribedTo(String topic){
        return subsById.contains(topic);
    }

}
