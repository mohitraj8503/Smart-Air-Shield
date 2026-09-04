# SMART AIR-SHIELD Mechanical & Enclosure Specifications

This directory contains design specifications, CAD templates, and mounting interface guidelines for the SMART AIR-SHIELD physical prototype.

---

## 1. Physical Enclosure Dimensions (from Project Design Sheet)

```text
    ┌─────────────────────────── 120 mm ───────────────────────────┐
    │                                                              │ ▲
    │   ┌─────────────────────┐       ┌──────────────────────┐     │ │
    │   │  Ambient PM Inlet   │       │  Quick-Release Latch │     │ │
    │   │  (Protected Grill)  │       │  for Filter Bay      │     │ │
    │   └─────────────────────┘       └──────────────────────┘     │ 72 mm
    │                                                              │ │
    │   ┌────────────────────────────────────────────────────┐     │ │
    │   │  Internal Layout:                                  │     │ │
    │   │  [Pre-Filter -> HEPA H13 -> Carbon] -> Blower 5015 │     │ │
    │   │  [7.4V 2S Li-ion Battery 2500mAh] + [Control PCB]  │     │ │
    │   └────────────────────────────────────────────────────┘     │ ▼
    └─────────────────────────────┬────────────────────────────────┘
                                  │
                                  └── Clean Air Duct (10 mm ID) ──► Outlet (28 x 24 mm)
```

| Parameter | Specification | Target Constraint |
| :--- | :--- | :--- |
| **External Dimensions** | $120\text{ mm (W)} \times 72\text{ mm (H)} \times 45\text{ mm (D)}$ | Fits comfortably on helmet rear/side lower rim |
| **Total Assembly Weight** | $220 - 250\text{ grams}$ | Must not cause rider neck fatigue or imbalance |
| **Material Recommendation** | PETG or ABS (UV-resistant, impact-resistant) | High thermal stability (-10°C to 50°C) |
| **Infill & Wall Thickness** | 3 perimeters, 25% gyroid infill | Optimized strength-to-weight ratio |
| **Duct Tubing** | $10\text{ mm}$ inner diameter, flexible silicone / TPU | Route along lower helmet rim into chin area |
| **Delivered Air Outlet** | $28\text{ mm (W)} \times 24\text{ mm (H)}$ contoured diffuser | Multi-hole curved air knife near nose/mouth |

---

## 2. Three-Stage Filter Cartridge Cavity
The enclosure features an external quick-swap access door so the rider can inspect or replace the filter without detaching the module from the helmet:
1. **Stage 1 (Pre-Filter)**: 40-mesh washable nylon or stainless-steel mesh to trap road dust, insects, and hair.
2. **Stage 2 (HEPA H13 Media)**: Mini-pleated fiberglass/PTFE paper rated $\ge 95\%$ particulate removal at $0.3\,\mu\text{m}$.
3. **Stage 3 (Activated Carbon)**: Honeycomb-structured carbon pellets to adsorb organic odors, fuel vapors, and basic VOCs.

---

## 3. Helmet Mounting Rules (Mandatory Safety Constraints)
1. **Zero Structural Modifications**: Do **NOT** drill, cut, screw into, or puncture the helmet shell.
2. **No Chemical Degradation**: Do not use solvent-based adhesives (e.g. cyanoacrylate) that compromise the polycarbonate/ABS shell integrity.
3. **Removable Clamp Bracket**: Use a dual-screw rim clamp with 2mm soft neoprene lining that grips the lower edge/bead of the helmet.
4. **Unobstructed Operation**: The mounting position must never obstruct:
   - Full rotation of the visor.
   - The chin strap retention mechanism.
   - Peripheral vision ($> 105^\circ$ lateral field of view).
   - Emergency cheek-pad release pull-tabs.

---

## 4. CAD Placeholder Files
Student 1 (Mechanical team member) should export and commit CAD files into this directory using standard formats:
- `enclosure_main_body.step` / `.stl`
- `filter_cartridge_tray.step` / `.stl`
- `helmet_clamp_bracket.step` / `.stl`
- `chin_outlet_diffuser.step` / `.stl`
