# LED map provenance and validation

Do not flash the diagnostic until the Vial layout export and both per-half
factory backups exist, have recorded SHA-256 digests, and pass the checks in
[recovery.md](recovery.md).

Build the diagnostic locally with `make diagnostic`. The generated artifact is
`dist/keyball44-vial_right-ledmap-diagnostic-41babbb8.uf2`. Its expected
SHA-256 digest is:

```text
b0f8747a47581ab139f29eb0ccff7ecaa603b05ba3be36f046c0de80008bea71
```

The build shim pins the diagnostic's original Vial build ID to make this
checksum reproducible; the diagnostic remains a local validation tool and is
not a release attachment.

Map one half at a time with USB and TRRS disconnected before every cable
change. Follow the explicit flash procedure in [recovery.md](recovery.md); do
not reconnect the halves while either is powered.

The diagnostic starts at local LED `00` and lights only that LED using the
current RGBLight hue, saturation, and brightness. The last OLED line shows the
half (`L` or `R`) and local index.

- Press the key at local matrix position `0,0` to advance one LED. The index
  wraps only after LED `29` on the left or LED `28` on the right.
- Press the key at local matrix position `0,1` to toggle
  between the diagnostic and the stored stock RGBLight effect.
- Both control keypresses continue through normal QMK processing. The
  diagnostic does not write EEPROM or change the Vial layout.

The populated [led-map.csv](led-map.csv) is derived from the official Keyball44
left and right KiCad PCB/netlist archives at the Yowkees revision recorded in
[`dependencies.lock`](../dependencies.lock); it is not a manual worksheet.
Footprint references provide serial order and LED-die coordinates. Switch and
diode nets provide matrix positions. Coordinates use millimetres with the
origin at the center of the left outer top key, positive X to the right, and
positive Y toward the user. The two halves use a nominal 19.05 mm gap between
their inner key centers; adjust that one gap in firmware if the keyboard is
normally placed farther apart.

Use the diagnostic only to validate the physical revision and jumper routing.
Local index `00` must correspond to PCB `LED1`, then advance monotonically to
`LED30` on the left or `LED29` on the right. Do not transcribe coordinates by
eye. If the sequence differs, record the first mismatching index and stop: the
hardware does not match the currently pinned design data.

Both orientations were validated on the assembled keyboard on 2026-08-30.
On the left, local index `00` is the physical top-right key, matrix `0,0` is
the physical top-left key, and matrix `0,1` is the second key from the left.
This confirms that the left KiCad X-axis must be mirrored for a top-side
assembled view. On the right, local index `00` is the bottom-left underglow,
matrix `0,0` is the physical top-right key, and matrix `0,1` is the second key
from the right. Right indices `10` and `11` are respectively the top-right and
middle-right key LEDs, confirming that the right KiCad X-axis already matches
the top-side assembled view.

After mapping a half, restore its matching verified factory image before
moving to the other half. Stop immediately and follow the failure rule if any
index lights zero or multiple LEDs, the OLED index is wrong, or normal input,
trackball, Vial, or recovery behavior changes.
