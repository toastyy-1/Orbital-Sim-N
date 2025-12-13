# Orbital Simulation Software N

A real-time gravitational physics simulator built in C with SDL3.

## Overview

This program simulates gravitational interactions between celestial bodies using Newton's law of universal gravitation.

## Features

### Gravitational Physics
- Real-time gravitational force calculations using F = GMm/r²
- Verlet Integration
- Total system energy tracking with error monitoring
- Collision Detection
- Adjustable simulation speed

###  Spacecraft Systems
  - Thrust  with configurable specific impulse
  - Fuel consumption based on mass flow rate
  - Variable throttle control (0-100%)

- **Burn Planning**:
  - Schedule multiple burns with precise start times and durations
    - **Tangent**: retrograde or prograde burns
    - **Normal**: burns perpendicular to the orbit
    - **Absolute**: burns relative to the inertial observer (space)
  - Each of these burn types can be configured with a rotation heading relative to each respective axis

## Usage
### Configuration Files

The simulation is configured via `simulation_data.json`:

#### Adding Celestial Bodies

```json
{
  "bodies": [
    {
      "name": "Earth",
      "mass": 5.972e24,
      "pos_x": 0,
      "pos_y": 0,
      "vel_x": 0,
      "vel_y": 0,
      "radius": 6371000
    }
  ]
}
```

**Parameters:**
- `name`: Display name
- `mass`: Mass in kilograms
- `pos_x`, `pos_y`: Position in meters (m) from the "center" (center of the screen)
- `vel_x`, `vel_y`: Initial velocity in meters per second
- `radius`: Body radius in meters

#### Adding Spacecraft

```json
{
  "spacecraft": [
    {
      "name": "Example Craft",
      "pos_x": 6671000.0,
      "pos_y": 0.0,
      "vel_x": 0.0,
      "vel_y": 7700.0,
      "dry_mass": 5500.0,
      "fuel_mass": 18000.0,
      "thrust": 450000.0,
      "specific_impulse": 314.0,
      "mass_flow_rate": 146.0,
      "attitude": 0.0,
      "moment_of_inertia": 20000.0,
      "nozzle_gimbal_range": 0.105,
      "burns": [
        {
          "burn_target": "Earth",
          "burn_type": "tangent",
          "start_time": 3600.0,
          "duration": 1000.0,
          "heading": 0.0,
          "throttle": 1.0
        }
      ]
    }
  ]
}
```

**Spacecraft Parameters:**
- **Position/Velocity**: Same as bodies (m, m/s)
- **Mass Properties**:
  - `dry_mass`: Spacecraft mass without fuel (kg)
  - `fuel_mass`: Available propellant (kg)
- **Propulsion**:
  - `thrust`: Maximum engine thrust (N)
  - `specific_impulse`: Engine efficiency (s)
  - `mass_flow_rate`: Fuel consumption rate (kg/s)
- **Attitude**:
  - `attitude`: Initial orientation (radians)
  - `moment_of_inertia`: Rotational inertia (kg⋅m^2)
  - `nozzle_gimbal_range`: Thrust vectoring limit (radians)

**Burn Parameters:**
- `burn_target`: Name of reference body (must match body name exactly -- case sensitive)
- `burn_type`: `"tangent"`, `"normal"`, or `"absolute"`
- `start_time`: Burn start time (seconds from T+0)
- `duration`: Burn length (seconds)
- `heading`: Direction offset (radians)
- `throttle`: Engine power (0.0 to 1.0)

### Controls

| Key/Action                     | Function |
|--------------------------------|----------|
| **Space**                      | Pause/Resume simulation |
| **R**                          | Reset simulation to initial state |
| **Right Mouse Drag**           | Pan viewport |
| **Mouse Wheel**                | Zoom in/out |
| **Hover + Scroll** (Speed Box) | Adjust time step |
| **Craft View Button**          | Toggle spacecraft telemetry window |
| **Show Stats Button**          | Toggle statistics panel |

### Compilation Dependencies
- **SDL3**: Graphics rendering and window management
- **SDL3_ttf**: TrueType font rendering
- **cJSON**: JSON parsing library

### Font Files
- `CascadiaCode.ttf` (or modify `main.c` to use an alternative font)
