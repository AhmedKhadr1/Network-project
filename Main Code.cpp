
#include <thread>
#include <chrono>
#include <cstdlib>
#include <ctime>
#include "D:\project\protocol.h"
#include <iostream>

using namespace std;

packet sender_buffer[MAX_SEQ + 1];
packet receiver_buffer[MAX_SEQ + 1];
bool received_acks[MAX_SEQ + 1] = { false };
seq_nr receiver_last_ack = MAX_SEQ;
static string senderx[7] = { "Khaled mohamed","Khaled mohamed","Ahmed mohamed khadr","John shoukry","CSE351","Computer Networks","ASU" };
static string receiverx[7] = { "Recieved 1","Received 2","Received 3","Received 4","Received 5","Received 6 ","Received 7" };
void from_network_layer(packet* p) {
    
    int random_index = rand() % 7;
    strcpy_s(p->data, senderx[random_index].c_str());

}


void send_to_sender(frame* ack) {
    int random_index = rand() % 7;
    std::cout << "[RECEIVER] Sending frame with seq=" << ack->seq
        << ", ack=" << ack->ack
        << ", data: " << string(receiverx[random_index].c_str()) << endl;


    this_thread::sleep_for(chrono::milliseconds(500));


    std::cout << "[SENDER] Received acknowledgment for seq=" << ack->ack << endl;
    received_acks[ack->ack] = true;
}
void send_to_receiver(frame* s) {
    std::cout << "[SENDER] Sending frame with seq=" << s->seq
        << ", ack=" << s->ack
        << ", data: " << string(s->info.data) << endl;


    this_thread::sleep_for(chrono::milliseconds(500));


    frame received_frame = *s;
    if (rand() % 10 < 8) {
        std::cout << "[RECEIVER] Received frame with seq=" << received_frame.seq
            << ", ack=" << received_frame.ack
            << ", data: " << string(received_frame.info.data) << endl;


        strcpy_s(receiver_buffer[received_frame.seq].data, received_frame.info.data);


        frame ack_frame;
        ack_frame.ack = received_frame.seq;
        receiver_last_ack = received_frame.seq;
        ack_frame.seq = (received_frame.seq + 1) % (MAX_SEQ + 1);
        strcpy_s(ack_frame.info.data, "");
        send_to_sender(&ack_frame);
    }
    else {
        std::cout << "[RECEIVER] Frame with seq=" << received_frame.seq << " lost!" << endl;
    }
}




void start_timer(seq_nr frame_nr) {
    std::cout << '\n' << "[TIMER] Starting timer for frame " << frame_nr << endl;
    this_thread::sleep_for(chrono::seconds(TIMEOUT));

    if (!received_acks[frame_nr]) {
        std::cout << '\n' << "[TIMER] Timeout for frame " << frame_nr << ". Retransmitting..." << endl;

        frame s;
        s.seq = frame_nr;
        s.ack = receiver_last_ack;
        s.info = sender_buffer[frame_nr];
        send_to_receiver(&s);
    }
}

    void stop_timer(seq_nr frame_nr) {
        if (received_acks[frame_nr]) {
            cout << "[TIMER] Stopping timer for frame " << frame_nr << endl;
        }
    }


    void protocol() {
        seq_nr next_frame_to_send = 0;
        seq_nr frame_expected = 0;


        for (int i = 0; i <= MAX_SEQ; i++) {
            from_network_layer(&sender_buffer[i]);
        }


        for (int i = 0; i < MAX_SEQ; i++) {
            frame s;
            s.seq = next_frame_to_send;
            s.ack = frame_expected;
            s.info = sender_buffer[next_frame_to_send];

            thread timer_thread(start_timer, next_frame_to_send);


            send_to_receiver(&s);


            timer_thread.detach();


            next_frame_to_send = (next_frame_to_send + 1) % (MAX_SEQ + 1);

            this_thread::sleep_for(chrono::milliseconds(1000));
        }

        this_thread::sleep_for(chrono::seconds(10));

        cout << "[SENDER] All frames sent and acknowledged. Exiting protocol." << endl;
    }


int main() {
    cout << "Under Supervision of Prof.Hossam Fahmy, We gladly show our test case for Network Project:"<<'\n';
    cout << "_________________________________________________________________________________________" << '\n';
    srand(time(0));
    protocol();
    return 0;
}