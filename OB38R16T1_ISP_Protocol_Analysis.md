# OB38R16T1 ISP Protocol Analysis

## Executive Summary

The OB38R16T1 is an 8051-compatible microcontroller with a built-in bootloader/ISP (In-System Programming) code located at addresses **0x3C80 - 0x3F3C** (701 bytes). The ISP uses a **UART Mode 1 (8-bit, variable baud rate)** interface with **auto-baud detection**.

---

## UART Configuration

### Registers Configuration

| Register | Value | Description |
|----------|-------|-------------|
| **SCON** | 0x50 | Mode 1 UART, 8-bit, REN=1 (receive enabled) |
| **S0RELH** | 0x03 | Baud rate generator reload high byte |
| **PCON.7 (SMOD)** | 1 | Double baud rate enabled |
| **TMOD** | 0x01 | Timer 0 in Mode 1 (16-bit, for timeouts) |
| **AUX** | 0x80 | Auxiliary control, bit 7 set |
| **IEN0** | 0x00 | All interrupts disabled |

### UART Mode

```
SCON = 0x50 = 0101 0000 binary
  ├─ SM0 = 0  ┐
  ├─ SM1 = 1  ┴─→ Mode 1: 8-bit UART, variable baud
  ├─ SM2 = 0  ───→ No multiprocessor communication
  ├─ REN = 1  ───→ Receive enabled
  ├─ TB8 = 0  ───→ 9th transmit bit (unused in Mode 1)
  ├─ RB8 = 0  ───→ 9th receive bit (unused in Mode 1)
  ├─ TI  = 0  ───→ Transmit interrupt flag (clear)
  └─ RI  = 0  ───→ Receive interrupt flag (clear)
```

### Baud Rate

The MCU uses an **internal baud rate generator** (not Timer 1):
- Reload registers: S0RELL (0xAA) and S0RELH (0xBA)
- With SMOD=1: `Baud = Fosc / (16 × (256 - S0RELL))`
- **Auto-baud detection** adjusts S0RELL dynamically during sync

---

## Protocol Overview

### Communication Parameters

- **Data bits**: 8
- **Stop bits**: 1 (implied by Mode 1)
- **Parity**: None
- **Flow control**: None
- **Baud rate**: Auto-detected (adapts to host)

### State Machine

```
┌─────────────────┐
│   POWER ON/     │
│   ISP ENTRY     │
└────────┬────────┘
         ▼
┌─────────────────┐
│  AUTO-BAUD      │──── Send 0xFF, expect 0xF0 response
│  DETECTION      │
└────────┬────────┘
         ▼
┌─────────────────┐
│  WAIT FOR       │──── Looking for 0x55 0xAA sync pattern
│  SYNC PATTERN   │
└────────┬────────┘
         ▼
┌─────────────────┐
│  SEND ACK       │──── Send 0xA5 acknowledge
│  (0xA5)         │
└────────┬────────┘
         ▼
┌─────────────────┐
│  RECEIVE        │──── Get command + parameters
│  COMMAND        │
└────────┬────────┘
         ▼
┌─────────────────┐
│  EXECUTE &      │──── Process, respond, loop back
│  RESPOND        │
└─────────────────┘
```

---

## Packet Format

### Command Packet (Host → MCU)

```
Byte 0: Command Code
Byte 1: Parameter 1 (typically Address High)
Byte 2: Parameter 2 (typically Address Low)
Byte 3: Parameter 3 (typically Length High or data)
Byte 4: Parameter 4 (typically Length Low or data)
[Byte 5+]: Additional data (for write commands)
```

**Note**: Packets are stored in RAM at addresses 0x50-0x54

### Response Packet (MCU → Host)

Simple responses:
- **0xA5**: General acknowledge / sync response
- **0xAA**: Command acknowledge / ready for data
- **0x57**: Error (invalid command)
- **0xEC**: Special mode set response

For read commands, data bytes follow the acknowledge.

---

## Command Set

| Code | Command | Parameters | Description |
|------|---------|------------|-------------|
| 0x00 | Mass Erase | None | Erases flash from 0x0000 to 0x1D00 |
| 0x01 | Page Erase | Addr | Erases page at specified address |
| 0x03 | Write Config | Data | Writes configuration byte |
| 0x06 | Software Reset | None | Triggers MCU software reset |
| 0x07 | Set Mode | Flag | Sets internal operating mode flag |
| 0x08 | Write Flash | Addr, Len, Data | Writes data to flash memory |
| 0x09 | Read Byte | Addr | Reads single byte from address |
| 0x0E | Read Block | Addr, Len | Reads block of data from flash |
| 0x0F | Checksum | Addr, Len | Calculates checksum of region |
| 0x10 | Write+Verify | Addr, Len, Data | Writes data with verification |

---

## Detailed Command Analysis

### Sync Sequence

```
Host → MCU:  0x55
Host → MCU:  0xAA
MCU → Host:  0xA5  (acknowledge)
```

The sync pattern `0x55 0xAA` must be received consecutively. The MCU validates by:
1. Receiving byte, if byte == 0xAA
2. Check if previous byte == 0x55
3. If match, send 0xA5 and enter command mode

### Command 0x00: Mass Erase

```
Host → MCU:  [0x00] [xx] [xx] [xx] [xx]
MCU → Host:  [status]
```

Erases entire user flash (0x0000 to 0x1D00). Uses ISPFC = 0x80 and 0x83 commands internally.

### Command 0x08: Write Flash Data

```
Host → MCU:  [0x08] [AddrH] [AddrL] [LenH] [LenL]
Host → MCU:  [Data0] [Data1] ... [DataN]
MCU → Host:  [status] per byte written
```

Writes data sequentially starting at the specified address.

### Command 0x0E: Read Flash Block

```
Host → MCU:  [0x0E] [AddrH] [AddrL] [LenH] [LenL]
MCU → Host:  0xAA (ready)
MCU → Host:  [Data0] [Data1] ... [DataN]
```

Reads a block of data and streams it back to the host.

---

## Flash Memory Operations

### Unlock Sequence

Before any flash write/erase operation, the flash must be unlocked:

```assembly
MOV TAKEY, #0x55    ; First key
MOV TAKEY, #0xAA    ; Second key
MOV TAKEY, #0x5A    ; Third key
```

### ISP Flash Control Register (ISPFC)

| Value | Operation |
|-------|-----------|
| 0x04 | Read flash byte |
| 0x05 | Write flash byte |
| 0x06 | Read byte (alternate mode) |
| 0x80 | Read/verify mode |
| 0x82 | Config write |
| 0x83 | Page erase |

### Flash Access Procedure

```assembly
; Set address
MOV ISPFAH, #addr_high
MOV ISPFAL, #addr_low

; For write: set data
MOV ISPFDL, #data_byte

; Trigger operation
MOV ISPFC, #operation_code
```

---

## Auto-Baud Detection Algorithm

Located at address 0x3E21:

```
1. Initialize R6 = 0xFF (baud rate trial value)
2. Check if ISP mode enabled (ISPFC bit 4)
3. Loop:
   a. Set S0RELL = R6
   b. Transmit 0xFF
   c. Wait for response
   d. If response == 0xF0: baud matched, exit
   e. Decrement R6, repeat
4. Once synced, use Timer 0 for additional timing verification
```

This allows the MCU to adapt to any standard baud rate within its operating range.

---

## Implementation Example (Python)

```python
import serial
import time

class OB38R16T1_ISP:
    def __init__(self, port, baudrate=9600):
        self.ser = serial.Serial(port, baudrate, timeout=1)
        
    def sync(self):
        """Synchronize with bootloader"""
        # Send sync pattern
        self.ser.write(bytes([0x55, 0xAA]))
        time.sleep(0.01)
        
        # Wait for acknowledge
        response = self.ser.read(1)
        if response == bytes([0xA5]):
            return True
        return False
    
    def send_command(self, cmd, params=[]):
        """Send command with parameters"""
        packet = [cmd] + params
        # Pad to 5 bytes if needed
        while len(packet) < 5:
            packet.append(0x00)
        self.ser.write(bytes(packet))
        
    def read_flash(self, address, length):
        """Read flash memory"""
        addr_h = (address >> 8) & 0xFF
        addr_l = address & 0xFF
        len_h = (length >> 8) & 0xFF
        len_l = length & 0xFF
        
        self.send_command(0x0E, [addr_h, addr_l, len_h, len_l])
        
        # Wait for ready signal
        response = self.ser.read(1)
        if response != bytes([0xAA]):
            raise Exception("Read not acknowledged")
            
        # Read data
        return self.ser.read(length)
    
    def mass_erase(self):
        """Erase all user flash"""
        self.send_command(0x00)
        response = self.ser.read(1)
        return response
    
    def software_reset(self):
        """Trigger software reset"""
        self.send_command(0x06)
```

---

## Memory Map

```
0x0000 ┌─────────────────────────┐
       │                         │
       │     User Flash          │
       │     (Application)       │
       │                         │
0x1D00 ├─────────────────────────┤ ← Mass erase boundary
       │     Reserved/Config     │
       │                         │
0x3C80 ├─────────────────────────┤ ← ISP Code Start
       │                         │
       │     ISP Bootloader      │
       │     (701 bytes)         │
       │                         │
0x3F3C └─────────────────────────┘ ← ISP Code End
```

---

## Key Findings

1. **UART Type**: Standard 8051 Mode 1 UART (8-N-1)
2. **Baud Rate**: Auto-detected using internal baud rate generator
3. **Protocol Type**: Proprietary binary protocol (not standard like STM bootloader or Intel HEX)
4. **Security**: Flash unlock requires 3-byte key sequence (0x55, 0xAA, 0x5A)
5. **Entry**: ISP mode entered when ISPFC bit 4 is set at boot
6. **Sync Pattern**: 0x55 0xAA → responds with 0xA5

This protocol is typical of ELAN/Generalplus family microcontrollers (OB series) and is similar to other 8051-derived MCU ISP implementations.
