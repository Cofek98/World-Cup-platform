package bgu.spl.net.srv;

//import java.lang.reflect.Array;
//import java.io.IOException;
import java.util.concurrent.ConcurrentHashMap;
import java.util.concurrent.ConcurrentLinkedQueue;
//import java.util.function.BiConsumer;

public class MyConnections<T> implements Connections<T>{

    //fields:
    private ConcurrentHashMap<String, User> usersByName;
    private ConcurrentHashMap<Integer, User> usersByConId;
    private ConcurrentHashMap<Integer, ConnectionHandler<T>> conHands;
    private ConcurrentHashMap<String, ConcurrentLinkedQueue<Integer>> topics;
    Integer msgId;


    //methods:

    //constructor
    public MyConnections(){
        usersByName = new ConcurrentHashMap<String, User>();
        usersByConId = new ConcurrentHashMap<Integer, User>();
        conHands = new ConcurrentHashMap<Integer, ConnectionHandler<T>>();
        topics = new ConcurrentHashMap<String, ConcurrentLinkedQueue<Integer>>();
        msgId = 1;
    }

    public void subscribe(String topic, int conId){
        topics.computeIfAbsent(topic, k -> new ConcurrentLinkedQueue<>()).add(conId);
    }

    public void unsubscribe(String topic, int conId){
        ConcurrentLinkedQueue<Integer> thisTopic = topics.get(topic);
        if (thisTopic != null){
            thisTopic.remove(conId);
        }
    }

    public ConcurrentHashMap<String, User> getUsersByName(){
        return usersByName;
    }

    public ConcurrentHashMap<Integer, User> getUsersByConId(){
        return usersByConId;
    }

    public ConcurrentHashMap<Integer, ConnectionHandler<T>> getConHands(){
        return conHands;
    }


    public void addUser(User user){
        usersByName.put(user.getName(), user);
        usersByConId.put(user.getConnectionId(), user);
        user.connect();
    }

    public void addConnection(Integer conId, ConnectionHandler<T> hand){   
        conHands.put(conId, hand);
    }



    public boolean send(int connectionId, T msg){
        ConnectionHandler<T> hand = conHands.get(connectionId);
        synchronized(hand){
            hand.send(msg);
        }
        return true;
    }

    public void send(String channel, T msg){
        
        for (Integer con : topics.get(channel)){
            send(con, msg);
        }
                
    }

    public ConcurrentHashMap<String, ConcurrentLinkedQueue<Integer>> getTopics(){
        return topics;
    }

    public int getMsgId(){
        
        int currMsgId;
        synchronized(msgId){
            currMsgId = msgId;
            msgId ++;
        }
        return currMsgId;   
    }

    public void disconnect(int connectionId){
        topics.forEach((name, queue) -> unsubscribe(name, connectionId));
    }



}