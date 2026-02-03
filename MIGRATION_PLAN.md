# Phased Migration to ArduinoMqttClient

## Overview
Implement ESPRazorBlade using ArduinoMqttClient from Phase 2 onwards (no PubSubClient). Incremental phases build up functionality, with each phase testable on hardware before proceeding. Phase 1 is WiFi-only to establish a solid foundation.

## Phase Breakdown

### Phase 1: WiFi Connection Verification
**Goal**: Verify existing WiFi connection code works correctly  
**Changes**: None (already implemented)  
**Test**: Simple sketch that connects to WiFi and prints status

**Deliverables**:
- `examples/Phase1_WiFi/Phase1_WiFi.ino` - Minimal WiFi test sketch
- Verify: WiFi connects, IP address displayed, reconnection works

**Success Criteria**:
- WiFi connects successfully
- IP address printed to Serial
- Reconnection works after WiFi disconnect

---

### Phase 2: Basic MQTT Connection with ArduinoMqttClient
**Goal**: Add MQTT connection using ArduinoMqttClient (no PubSubClient - fresh implementation)  
**Changes**:
- Add `#include <ArduinoMqttClient.h>` 
- Add MQTT client member variable using ArduinoMqttClient
- Implement basic connection logic using ArduinoMqttClient API
- Keep RTOS MQTT task simple (just connection, no publishing yet)

**Deliverables**:
- Updated `ESPRazorBlade.h` - ArduinoMqttClient include and client declaration
- Updated `ESPRazorBlade.cpp` - Basic MQTT connection with ArduinoMqttClient
- `examples/Phase2_MQTT_Connection/Phase2_MQTT_Connection.ino` - Test sketch

**Test Sketch Features**:
- Initialize library
- Connect to WiFi
- Connect to MQTT broker
- Print connection status
- Verify keepalive works (wait 2+ minutes)

**Success Criteria**:
- MQTT connects successfully
- Connection status reported correctly
- Keepalive maintains connection for 2+ minutes
- Reconnection works after broker disconnect

**API Differences to Handle**:
- ArduinoMqttClient uses `begin()` instead of `setServer()`
- Connection uses `connect()` with different parameters
- No `loop()` method - uses `poll()` instead
- Different state checking methods

---

### Phase 3: Basic Publish Functionality
**Goal**: Implement and test basic publish operations  
**Changes**:
- Implement `publish()` methods using ArduinoMqttClient API
- Add thread-safe publishing with mutex
- Test with simple string and numeric values

**Deliverables**:
- Updated `ESPRazorBlade.cpp` - Publish implementations
- `examples/Phase3_Publish/Phase3_Publish.ino` - Test sketch

**Test Sketch Features**:
- Connect to WiFi and MQTT
- Publish string message
- Publish integer value
- Publish float value
- Verify messages appear on MQTT broker

**Success Criteria**:
- All publish overloads work correctly
- Messages appear on broker with correct topics
- Thread-safe publishing (if RTOS task reintroduced)

---

### Phase 4: RTOS Task Integration
**Goal**: Reintroduce MQTT RTOS task with ArduinoMqttClient  
**Changes**:
- Create MQTT RTOS task that calls `poll()` periodically
- Implement thread-safe publish operations
- Handle connection state in RTOS context

**Deliverables**:
- Updated `ESPRazorBlade.cpp` - RTOS MQTT task implementation
- `examples/Phase4_RTOS/Phase4_RTOS.ino` - Test sketch

**Test Sketch Features**:
- Verify MQTT runs in background task
- Test publish from main loop while task runs
- Verify non-blocking operation
- Test connection recovery

**Success Criteria**:
- MQTT task runs independently
- Main loop not blocked
- Publish works from both task and main loop
- Connection recovery works automatically

---

### Phase 5: Telemetry Callback System
**Goal**: Implement telemetry callback registration and execution  
**Changes**:
- Implement `registerTelemetry()` method
- Add telemetry processing in MQTT task
- Handle callback execution and publishing

**Deliverables**:
- Updated `ESPRazorBlade.cpp` - Telemetry processing
- `examples/Phase5_Telemetry/Phase5_Telemetry.ino` - Test sketch

**Test Sketch Features**:
- Register multiple telemetry callbacks
- Verify callbacks execute at correct intervals
- Test with different interval values
- Verify data published correctly

**Success Criteria**:
- Callbacks execute at specified intervals
- Multiple callbacks work simultaneously
- Data published to correct topics
- No memory leaks or crashes

---

### Phase 6: Keepalive and Reconnection Testing
**Goal**: Verify robust connection management  
**Changes**:
- Test keepalive handling
- Test WiFi reconnection scenarios
- Test MQTT reconnection scenarios
- Stress test with rapid publish operations

**Deliverables**:
- `examples/Phase6_StressTest/Phase6_StressTest.ino` - Comprehensive test
- Documentation of edge cases handled
**Status**: Implemented (stress test example available in `examples/Phase6_StressTest/`)

**Test Scenarios**:
- WiFi disconnect/reconnect
- MQTT broker disconnect/reconnect
- Rapid publish operations
- Long-running operation (30+ minutes)
- Multiple telemetry callbacks

**Success Criteria**:
- All reconnection scenarios handled gracefully
- No crashes or memory leaks
- Keepalive maintains connection
- Performance acceptable under load

---

## Implementation Notes

### ArduinoMqttClient API Differences

1. **Initialization**:
   ```cpp
   // Old (PubSubClient)
   PubSubClient mqttClient(wifiClient);
   mqttClient.setServer(broker, port);
   
   // New (ArduinoMqttClient)
   ArduinoMqttClient mqttClient;
   mqttClient.begin(broker, port, wifiClient);
   ```

2. **Connection**:
   ```cpp
   // Old
   mqttClient.connect(clientId, username, password);
   
   // New
   mqttClient.connect(clientId, username, password);
   // Returns int (0 = success)
   ```

3. **Loop/Poll**:
   ```cpp
   // Old
   mqttClient.loop();
   
   // New
   mqttClient.poll();
   ```

4. **Publish**:
   ```cpp
   // Old
   mqttClient.publish(topic, payload, retained);
   
   // New
   mqttClient.beginMessage(topic, retained, qos);
   mqttClient.print(payload);
   mqttClient.endMessage();
   ```

5. **Connection State**:
   ```cpp
   // Old
   mqttClient.connected()
   
   // New
   mqttClient.connected()
   ```

### File Structure

```
ESPRazorBlade/
├── ESPRazorBlade.h
├── ESPRazorBlade.cpp
├── Configuration.h (user config)
├── Configuration.h.example
├── library.properties
├── README.md
├── MIGRATION_PLAN.md
└── examples/
    ├── Phase1_WiFi/
    │   └── Phase1_WiFi.ino
    ├── Phase2_MQTT_Connection/
    │   └── Phase2_MQTT_Connection.ino
    ├── Phase3_Publish/
    │   └── Phase3_Publish.ino
    ├── Phase4_RTOS/
    │   └── Phase4_RTOS.ino
    ├── Phase5_Telemetry/
    │   └── Phase5_Telemetry.ino
    └── Phase6_StressTest/
        └── Phase6_StressTest.ino
```

## Testing Checklist Per Phase

- [ ] Code compiles without errors
- [ ] Uploads to ESP32-C6 successfully
- [ ] Serial output shows expected messages
- [ ] WiFi connects (Phase 1+)
- [ ] MQTT connects (Phase 2+)
- [ ] Publish works (Phase 3+)
- [ ] RTOS tasks run correctly (Phase 4+)
- [ ] Telemetry callbacks execute (Phase 5+)
- [ ] Reconnection works (Phase 6)
- [ ] No crashes during extended operation (Phase 6)

## Rollback Plan

Each phase should be committed separately. If a phase fails:
1. Document the issue
2. Revert to previous working phase
3. Fix issue before proceeding

## Dependencies

- **Phase 1**: ESP32 Arduino Core, WiFi library (built-in)
- **Phase 2+**: ArduinoMqttClient library (install via Library Manager)
- ESP32 Arduino Core
- WiFi library (built-in)

**Note**: ArduinoMqttClient should be installed before starting Phase 2.
