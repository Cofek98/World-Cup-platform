#include <Kthread.h>

Kthread::Kthread(ConnectionHandler& hand, StompProtocol& protocol) : 
hand(&hand),
protocol(&protocol)
{}

Kthread::~Kthread(){
    delete(hand);
    delete(protocol);
}

Kthread::Kthread(const Kthread& other) : 
hand(other.hand),
protocol(other.protocol)
{}

Kthread& Kthread::operator=(const Kthread& other) { 
    hand = other.hand;
    protocol = other.protocol;
    return(*this);
}

void Kthread::run(){
    while (1) {
        
        const short bufsize = 1024;
        char buf[bufsize];
        std::cin.getline(buf, bufsize);
		std::string line(buf);
        string frame = protocol->process(line);
        if (frame != ""){
            hand->sendFrameAscii(frame, '\0');
        }
    } 
}