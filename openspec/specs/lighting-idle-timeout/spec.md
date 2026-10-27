# lighting-idle-timeout Specification

## Purpose

Turns the keyboard's lighting off when the board has been left alone, and
brings it back on the next input, without changing any saved lighting
settings.

## Requirements

### Requirement: Lighting turns off after three minutes without input
The firmware SHALL fade all 59 keyboard LEDs to black over 250 ms once
180,000 ms have passed since the last key matrix change or trackball movement
on either half.

#### Scenario: Board left untouched
- **WHEN** no key changes state and the trackball does not move for 180,000 ms
- **THEN** all 59 LEDs fade to black over 250 ms

#### Scenario: Just before the timeout
- **WHEN** 179,999 ms have passed since the last input
- **THEN** the lighting remains at its normal output

#### Scenario: Trackball movement keeps lighting on
- **WHEN** keys are untouched for more than 180,000 ms but the trackball moved
  less than 180,000 ms ago
- **THEN** the lighting remains at its normal output

#### Scenario: Holding a key without other input
- **WHEN** a key is pressed and held with no further key or trackball input
  for 180,000 ms
- **THEN** all 59 LEDs fade to black

### Requirement: Any input restores lighting
The firmware SHALL fade lighting back to its normal output over 250 ms on the
first key matrix change or trackball movement after it went dark for
inactivity. The input SHALL still reach the host as normal.

#### Scenario: Key press wakes lighting
- **WHEN** lighting is dark for inactivity and a key is pressed
- **THEN** lighting fades back to its normal output over 250 ms
- **AND** the key press is sent to the host

#### Scenario: Trackball movement wakes lighting
- **WHEN** lighting is dark for inactivity and the trackball moves
- **THEN** lighting fades back to its normal output over 250 ms
- **AND** the pointer movement is sent to the host

#### Scenario: Input during the fade-out
- **WHEN** input arrives while lighting is fading out for inactivity
- **THEN** lighting reverses toward its normal output from its current
  brightness without a jump

### Requirement: Lighting stays dark for any length of inactivity
Once lighting is dark for inactivity, it SHALL stay dark until the next input,
regardless of how long the board is idle, including beyond the 32-bit
millisecond timer range.

#### Scenario: Idle longer than the timer range
- **WHEN** lighting is dark for inactivity and more than 2^32 ms pass with no
  input
- **THEN** lighting remains dark

### Requirement: Idle timeout is always active
The idle timeout SHALL be enabled on every boot with a fixed 180,000 ms
period. It SHALL NOT depend on the Auto Mouse fade setting, Auto Mouse
enablement, or any Vial setting.

#### Scenario: Auto Mouse fades disabled
- **WHEN** the Auto Mouse fade setting is off and the board is idle for
  180,000 ms
- **THEN** all 59 LEDs fade to black

#### Scenario: Auto Mouse fade still dark when idle input ends
- **WHEN** lighting is dark both for inactivity and because the Auto Mouse
  fade is active, and trackball movement ends the inactivity
- **THEN** lighting stays dark until the Auto Mouse layer is also released

### Requirement: Saved lighting settings are preserved
Going dark for inactivity and restoring SHALL NOT change RGBLight enable
state, hue, saturation, brightness, effect, or speed, and SHALL NOT write
EEPROM. Stock and custom effects SHALL keep running while dark, and restoring
SHALL show their current output.

#### Scenario: Restore after idle
- **WHEN** lighting restores after being dark for inactivity
- **THEN** it uses the current live hue, saturation, brightness, effect, and
  speed, and custom effect animation continues from its current state

#### Scenario: Lighting switched off by the user
- **WHEN** RGB lighting was switched off before or during inactivity and input
  then arrives
- **THEN** lighting remains off

### Requirement: Both halves go dark and restore together
Both halves SHALL go dark and restore within the existing lighting fade
synchronization, including recovery after a failed split update or a reset or
reconnected half.

#### Scenario: Input on the non-master half
- **WHEN** lighting is dark for inactivity and a key on the non-master half is
  pressed
- **THEN** both halves restore together

#### Scenario: Half reconnects while dark
- **WHEN** one half resets or reconnects while lighting is dark for inactivity
- **THEN** that half converges to dark through the existing fade heartbeat
