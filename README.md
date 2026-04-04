⚡ xsonicland
Sovereign High-Performance XWayland Bridge for the ssX Ecosystem.

---

🛡️ JESTERMAN'S CREED
This repository is a sovereign expression of technical freedom. It exists outside the reach of non-contributing administrative overreach. The creator's intent is the absolute law of this tree. Unauthorized metadata interference will be met with immediate history redaction.

---

## 📦 Architectural Core

### XLibre Core
Surgically integrated fb/ and mi/ optimization layers for raw 2D rendering efficiency.

### Sovereign Shims
Custom ssx_accel hardware shims that bypass standard Wayland buffer stalls.

### Zero-Leak Metadata
The codebase is 100% "Sonic-Clean" purged of all legacy "Archon" naming.

---

## ⚡ The Latency-First Stack

### VSync: DISABLED by default
xsonicland prioritizes input-to-photon speed over artificial frame-locking.

### TearFree Logic
Optimized tearing control is available for 2D-pipes but remains secondary to raw throughput.

### Sub-Millisecond Path
Implements a "Fast-Path" rendering loop for X11 applications targeting sub-3ms latency targets.

---

## 🔗 THE UNIFIED BRIDGE: XSONICLAND ↔ SONICMESA
The circle is complete. XSonicLand now utilizes the **0x504E4943 (SONIC)** ring to inject 2D primitives directly into the SonicMesa hardware queue.

### **The Architecture of Speed:**
1. **The Pulse:** XSonicLand batches X11/Xwayland 2D requests (BitBlt, SolidFill) into the `0x504E4943` io_uring.
2. **The Cache:** All commands are 64-byte aligned, ensuring the 5800X3D L3 cache remains the primary workspace.
3. **The Bypass:** We skip the generic "Archon" acceleration paths. The Xserver speaks directly to the driver's XAA frontend.
4. **The Threshold:** A hard-coded batch threshold of 16 commands minimizes SQPOLL overhead, delivering near-instantaneous UI responsiveness on 144Hz+ displays.

**"The Xserver is no longer a middleman. It is a high-velocity command injector."**

### ⚡ KEY FEATURES (Updated)
- **SQPOLL io_uring:** Asynchronous batch submission via dedicated kernel thread
- **16-Command Batch Threshold:** Tuned for 5800X3D's latency profile
- **64-Byte Aligned Commands:** L3 cache-resident command buffers
- **Zero-Copy Submission:** Fixed buffers for direct hardware injection
- **XAA Hardware Bridge:** ssx_xaa kernel-to-driver interface
- **Fallback to L3:** CPU fallbacks run entirely in 5800X3D L3 cache when GPU busy

---

## 🔧 System Orchestration

### sonicd Native
Full support for READY=1 signaling and high-priority orchestration within the ssX ecosystem.

---

## 📜 License

This software is provided under the terms of the X.Org license.
See the LICENSE file for details.
