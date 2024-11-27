#ifndef PROTOCOL_H
#define PROTOCOL_H

#include <string>

// Constants
#define MAX_SEQ 7       // Maximum sequence number
#define MAX_PKT 64      // Maximum packet size
#define TIMEOUT 2       // Timeout duration in seconds

// Event types for simulation
typedef enum { frame_arrival, cksum_err, timeout, network_layer_ready } event_type;

// Sequence number type
typedef unsigned int seq_nr;

// Packet structure
typedef struct {
    char data[MAX_PKT]; // Data in the packet
} packet;

// Frame structure
typedef struct {
    packet info;        // Packet being carried
    seq_nr seq;         // Sequence number
    seq_nr ack;         // Acknowledgment number
} frame;

// Function prototypes
void wait_for_event(event_type *event);                  // Wait for an event
void from_network_layer(packet *p);                      // Retrieve packet from network layer
void to_network_layer(packet *p);                        // Pass packet to network layer
void from_physical_layer(frame *r, seq_nr frame_expected); // Retrieve frame from physical layer
void to_physical_layer(frame *s);                        // Send frame to physical layer
void start_timer(seq_nr frame_nr);                       // Start a timer for a frame
void stop_timer(seq_nr frame_nr);                        // Stop a timer for a frame
void send_data(seq_nr frame_nr, seq_nr frame_expected, packet buffer[]); // Send data frame
void increment_frame(seq_nr &seq);                       // Increment a sequence number
bool between(seq_nr a, seq_nr b, seq_nr c);              // Check if b is within [a, c) in a circular manner

#endif
