# SMART AIR-SHIELD — Safety Guidelines & Explicit Non-Claims

> [!IMPORTANT]
> The following safety statements and non-claims apply verbatim to the SMART AIR-SHIELD project, its firmware, hardware attachments, and promotional materials:

1. **Particulate Filtration Scope:** This device reduces particulate (PM2.5/PM10) exposure; it does not claim to remove CO, NOx, or all VOCs.
2. **Medical & Respiratory Disclaimer:** It is not a certified respirator or medical device.
3. **Non-Structural Attachment Rule:** The attachment must be removable and non-structural — firmware/hardware must never require drilling, cutting, or permanently modifying the helmet shell.
4. **Emergency & Operational Clearance:** Must not obstruct the visor, chin strap, retention system, or emergency-release mechanism.
5. **Mandatory Helmet Certification:** Riders must always wear a BIS-certified helmet; this device is a supplementary accessory only.

---

## Mechanical & Electrical Safeguards

- **Thermal Protections:** The 2S Li-ion battery pack is equipped with a dedicated Hardware Battery Management System (BMS) offering short-circuit, over-current, over-voltage ($8.4\text{V}$), and under-voltage ($6.0\text{V}$) protection.
- **Fail-Safe Blower Behavior:** In the event of sensor communication timeouts, hardware watchdog triggers, or low battery ($<10\%$), the firmware automatically falls back to safe fixed speeds ($50\%$) and raises acoustic/visual alerts without abrupt stoppage.
- **Detachable Clamp System:** The mounting bracket relies on external non-destructive clamping lined with vibration-damping neoprene rubber, preventing any abrasion or stress concentration on the helmet outer shell.
