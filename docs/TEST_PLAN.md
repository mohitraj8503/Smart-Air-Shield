# SMART AIR-SHIELD — Experimental Test & Validation Plan

This document outlines the experimental procedures, bench testing methodologies, and road evaluation protocols required for the Vishwakarma Awards 2026–27 submission.

---

## 1. Bench Airflow & Pressure-Drop Characterization

### Objective
Measure the volume flow rate ($2 - 8\text{ L/min}$) and pressure drop ($\Delta P$) across the 3-stage filter cartridge at variable blower PWM duty cycles.

### Equipment
- Hot-wire anemometer or mini flow meter (calibrated for $0.5 - 15\text{ L/min}$)
- Digital differential manometer ($0 - 500\text{ Pa}$ range)
- Precision DC bench power supply ($7.4\text{V}$, current monitoring)
- Sealed test duct tube ($10\text{ mm}$ ID)

### Test Procedure
1. Mount the 3-stage cartridge (Pre-filter + HEPA H13 + Activated Carbon) inside the enclosure duct.
2. Insert static pressure taps immediately upstream of the filter and downstream of the blower.
3. Step through PWM duty cycles from $20\%$ to $100\%$ in increments of $10\%$.
4. At each step, record:
   - Volume flow rate ($Q$ in $\text{L/min}$)
   - Static pressure drop ($\Delta P$ in $\text{Pa}$)
   - Motor current consumption ($I$ in $\text{mA}$)
5. Plot Flow Rate vs Duty Cycle and Pressure Drop vs Airflow.

---

## 2. Controlled Aerosol & Particulate Challenge Test

### Objective
Quantify single-pass filtration efficiency ($\eta$) under heavy particle loads ($300 - 800\,\mu\text{g}/\text{m}^3$ PM2.5).

### Test Setup
- Enclosed acrylic testing chamber ($500 \times 500 \times 500\text{ mm}$)
- Controlled particulate generator (standard incense stick / smoldering aerosol source)
- Dual calibrated PMS7003 sensors (Ambient in-chamber, Outlet at delivery nozzle)

### Test Procedure
1. Place the SMART AIR-SHIELD inside the sealed chamber.
2. Ignite the particle source until chamber concentration stabilizes between $300 - 600\,\mu\text{g}/\text{m}^3$.
3. Activate the prototype in Auto mode.
4. Log ambient ($C_{\text{amb}}$) and delivered ($C_{\text{out}}$) concentrations at 1 Hz for 15 minutes.
5. Compute instantaneous and mean filtration efficiency:
   $$\eta = \left(1 - \frac{C_{\text{out}}}{C_{\text{amb}}}\right) \times 100\%$$
6. **Pass Criterion:** Measured $\eta \ge 95\%$ for PM2.5 and PM10 across all active duty cycles.

---

## 3. Acoustic Noise Evaluation

### Objective
Ensure operating noise inside and outside the helmet conforms to the $< 35\text{ dB(A)}$ requirement at medium speed and does not interfere with traffic sound perception.

### Test Setup
- Sound Level Meter (Type 2 / Class 2, A-weighting, fast response)
- Full-face helmet mounted on an artificial head form
- Low-noise acoustic environment ($< 28\text{ dB(A)}$ ambient baseline)

### Test Procedure
1. Position microphone at $1\text{ meter}$ distance in the lateral direction.
2. Position a second probe inside the ear cavity of the helmet.
3. Record SPL at:
   - Low mode ($30\%$ duty)
   - Medium mode ($60\%$ duty)
   - High mode ($90\%$ duty)
4. **Pass Criterion:** Lateral SPL $< 35\text{ dB(A)}$ at $1\text{ meter}$ for Medium ($60\%$) duty cycle.

---

## 4. Paired On-Road Commuter Test Protocol (ON vs OFF)

### Objective
Demonstrate actual in-helmet exposure reduction during real-world peak commuter traffic in Indian urban conditions.

### Test Protocol
1. **Route Selection:** A standard $10\text{ km}$ urban commute route incorporating traffic junctions, bus stops, and construction stretches.
2. **Paired Sampling Design:**
   - **Leg A (System OFF):** Ride the designated route with the SMART AIR-SHIELD powered OFF; both ambient and outlet sensors log baseline exposure in the visor zone.
   - **Leg B (System ON):** Ride the same route under comparable traffic conditions with the system in Auto mode.
3. **Data Analysis:**
   - Export CSV session files from the companion dashboard.
   - Compare integrated particulate exposure doses:
     $$\text{Exposure Dose} = \int_{0}^{T} C(t)\,dt$$
   - Calculate total particulate reduction percentage for both PM2.5 and PM10.

---

## 5. Battery Endurance Test

### Objective
Verify that the $7.4\text{V } 2500\text{mAh}$ Li-ion battery pack delivers $6 - 8\text{ hours}$ of continuous operation under realistic commuter conditions.

### Test Procedure
1. Fully charge battery pack until $V_{\text{pack}} \ge 8.35\text{V}$.
2. Run prototype under simulated cycling load ($50\%$ duty average).
3. Record battery terminal voltage and state-of-charge percentage every 5 minutes until low-voltage cutoff ($6.0\text{V}$).
4. **Pass Criterion:** Total operational runtime $> 6.0\text{ hours}$.
