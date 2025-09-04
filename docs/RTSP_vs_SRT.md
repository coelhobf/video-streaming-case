# RTSP vs SRT Analysis

## RTSP (Real-Time Streaming Protocol)

**Purpose**: Application-layer protocol for controlling streaming media servers
**Transport**: TCP for control, UDP/TCP for data (RTP/RTCP)

**When to use RTSP:**
- Camera integration (IP cameras natively support RTSP)
- Local networks with reliable connectivity
- Real-time applications requiring low latency
- Existing RTSP infrastructure

**Advantages:**
- Low latency, near real-time streaming
- Universal support across cameras and players
- Direct RTP streaming without overhead
- Built-in control features (play, pause, seek)

**Limitations:**
- Firewall issues, multiple ports, NAT traversal problems
- Poor performance over unreliable networks
- Basic retransmission mechanisms
- Not ideal for internet streaming

## SRT (Secure Reliable Transport)

**Purpose**: Transport protocol optimized for live video over unpredictable networks
**Transport**: UDP-based with intelligent retransmission

**When to use SRT:**
- Internet streaming over long distances
- Content distribution between facilities
- Unreliable network conditions
- Firewall traversal requirements

**Advantages:**
- Network resilience with adaptive bitrate and packet recovery
- Single UDP port, easier NAT traversal
- Built-in encryption for security
- Intelligent congestion control

**Limitations:**
- More complex than RTSP for simple scenarios
- Limited camera support (newer protocol)
- Higher CPU usage than RTSP
- Higher latency than RTSP in ideal conditions

## Decision Matrix

| Scenario | Recommended Protocol | Reason |
|----------|---------------------|---------|
| IP Camera to Local Server | RTSP | Direct, low latency, standard support |
| Server to Internet Distribution | SRT | Network resilience, firewall traversal |
| LAN Monitoring | RTSP | Minimal overhead, real-time performance |
| Remote Broadcasting | SRT | Reliability over distance, error recovery |
| Multi-location Streaming | SRT | Consistent quality across varied networks |

## Pipeline Architecture

```
Pipeline 1: File → RTSP (local ingestion)
Pipeline 2: RTSP → SRT (network distribution)  
Pipeline 3: SRT → WebRTC/HLS (browser delivery)
```

**Design rationale:**
- RTSP for local ingestion from files or cameras
- SRT for reliable network distribution
- WebRTC/HLS for browser compatibility

This approach uses each protocol where it performs best.
