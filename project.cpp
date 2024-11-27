#include<iostream>
#include "bits/stdc++.h"
#include <chrono>
#include "protocol.h"
#include <stdlib.h> // For random numbers
#include <time.h>   // For seeding random
#include <thread>   // For sleep function
using namespace std;

event_type random_event() {
    int random_value = rand() % 4;
    switch (random_value) {
        case 0: return frame_arrival;
        case 1: return cksum_err;
        case 2: return timeout;
        case 3: return network_layer_ready;
        default: return network_layer_ready; // Fallback
    }
}

void wait_for_event(event_type *event) {
    *event = random_event(); // Random event simulation
}

/* Generate a random packet */
void generate_random_packet(packet *p) {
    for (int i = 0; i < MAX_PKT; i++) {
        p->data[i] = 'A' + rand() % 26; // Fill packet with random letters
    }
}

/* Simulate a frame arrival */
void simulate_frame_arrival(frame *r, seq_nr frame_expected) {
    r->seq = (frame_expected + rand() % 3) % (MAX_SEQ + 1); // Simulate random frame sequence
    r->ack = (frame_expected + rand() % (MAX_SEQ + 1)) % (MAX_SEQ + 1); // Random ack
}

/* Fetch a packet from network layer */
void from_network_layer(packet *p) {
    generate_random_packet(p);
}

/* Fetch a frame from physical layer */
void from_physical_layer(frame *r, seq_nr frame_expected) {
    simulate_frame_arrival(r, frame_expected);
}

void to_physical_layer(frame* s) {
    // Simulate sending the frame to the physical layer
    cout << "Sending frame with seq=" << s->seq << " and ack=" << s->ack << endl;
}

// Simple implementation of start_timer
void start_timer(seq_nr frame_num) {
    // Start a simple timer simulation (just for demonstration purposes)
    cout << "Starting timer for frame " << frame_num << endl;
    this_thread::sleep_for(chrono::seconds(TIMEOUT)); // Simulating a delay for timeout
}

// Simple implementation of stop_timer
void stop_timer(seq_nr frame_num) {
    // Stop the timer simulation
    cout << "Stopping timer for frame " << frame_num << endl;
}

/* Send a frame to network layer */
void to_network_layer(packet p) {
    // Placeholder to simulate passing the packet to network layer
    printf("Delivering packet to network layer: %s\n", p.data);
}

/* Check if sequence number is within the window range */
bool between_sequence_window(seq_nr n1, seq_nr n2, seq_nr n3) {
    return (((n1 <= n2) && (n2 < n3)) || ((n3 < n1) && (n1 <= n2)) || ((n2 < n3) && (n3 < n1)));
}

void send_data(seq_nr frame_num, seq_nr frame_expected, packet buffer[]) {
    frame s; /* pointer to frame */
    s.info = buffer[frame_num]; /* Insert packet in the buffer */
    s.seq = frame_num; /* Insert sequence number into frame */
    s.ack = (frame_expected + MAX_SEQ) % (MAX_SEQ + 1); /* Piggyback ACK */
    to_physical_layer(&s); /* Transmit the frame */
    start_timer(frame_num); /* Start the timer running */
}


/* Increment frame number within window */
void increment_frame(seq_nr &seq) {
    seq = (seq + 1) % (MAX_SEQ + 1); // To remain within range 0 --> max_seq     
}

/* Main protocol logic for Go-Back-N */
void protocol_5(void) {
    frame fr1;
    seq_nr next_frame = 0;
    seq_nr expected_frame = 0;
    seq_nr expected_ack = 0;
    packet buffer[MAX_SEQ + 1]; /* Buffer with size of the max sequence to keep all frames in our window */
    seq_nr buffer_count = 0; /* Number of elements inside buffer to be sent */
    event_type E = random_event();
    int total_frames_sent = 0;  // Track how many frames have been sent

    while (true) {
        switch (E) {
            case network_layer_ready:
                if (total_frames_sent < 7) {  // Send only 7 frames
                    from_network_layer(&buffer[next_frame]); /* Fetch new packet */
                    buffer_count++; /* Expand the sender's window */
                    send_data(next_frame, expected_frame, buffer); /* Transmit the frame */
                    increment_frame(next_frame); /* Advance sender's upper window edge */
                    total_frames_sent++;  // Increment the count of frames sent
                } else {
                    cout << "All 7 frames have been sent. Ending transmission." << endl;
                    return;  // Stop the protocol after 7 frames are sent
                }
                break;
            
            case frame_arrival:
                from_physical_layer(&fr1, expected_frame); /* Receive the arrival packet */
                if (fr1.seq == expected_frame) {
                    /* Frames are accepted only in order. */
                    to_network_layer(fr1.info); /* Pass packet to network layer */
                    increment_frame(expected_frame); /* Increment expected number to wait for the next frame to be sent */
                }

                /* Handle piggybacked acknowledgment */
                while (between_sequence_window(expected_ack, fr1.ack, next_frame)) {
                    buffer_count--; /* One frame fewer buffered */
                    stop_timer(expected_ack); /* Frame arrived intact; stop timer */
                    increment_frame(expected_ack); /* Contract sender's window */
                }
                break;
            
            case cksum_err:
                printf("Checksum error occurred. Ignoring frame.\n");
                break;
            
            case timeout:
                /* Timeout occurred; retransmit all outstanding frames */
                printf("Timeout! Retransmitting all frames...\n");
                next_frame = expected_ack; /* Start retransmitting here */
                for (int i = 1; i <= buffer_count; i++) {
                    send_data(next_frame, expected_frame, buffer); /* Resend frame */
                    increment_frame(next_frame); /* Prepare to send the next one */
                }
                break;
        }

        /* Simulate random event occurrence */
        E = random_event();
    }
}

int main() {
    srand(time(NULL)); // Seed random number generator
    protocol_5();   // Run the Go-Back-N protocol simulation
    return 0;
}
