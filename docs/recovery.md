# Backup and recovery procedure

This is the mandatory safety gate before modifying either half. Commands are
shown through the repository wrapper for its pinned official Raspberry Pi
`picotool` build. Install and verify it before connecting a keyboard:

```sh
make setup-picotool
./scripts/picotool version
```

The setup requires a C++ compiler, `pkg-config`, and the libusb-1.0 development
files. On Linux, install `external/picotool/udev/60-picotool.rules` according to
the operating system's udev policy if the device is not accessible as the
current user.

## Non-negotiable rules

- Never insert or remove TRRS while USB is connected to either half.
- Back up and flash only one half at a time, with TRRS disconnected.
- Never use a firmware image unless its provenance, target, and checksum are
  known.
- Do not use `/dev/hidraw9` as a stable device identifier.
- Keep factory images private and outside version control.

Set `KEYBALL44_PRIVATE_BACKUP_DIR` to a directory on encrypted storage outside
the repository. Do not place irreplaceable recovery material in `backups/`
inside a clone, even though that path is ignored as a final safety net.

## Export Vial state

Before entering BOOTSEL, use Vial's Save function to export the current layout.
Store the export next to the private backups and record a checksum for it.

## Back up each half

Perform the following sequence first for the left half and then, from the
beginning, for the right half:

1. Disconnect USB from the entire keyboard.
2. Disconnect the TRRS cable.
3. Enter the selected half's RP2040 BOOTSEL mode using its hardware BOOTSEL
   method while connecting only that half by USB.
4. Confirm exactly one RP2040 device is visible:

   ```sh
   ./scripts/picotool info -a
   ```

5. Save and verify all accessible flash, choosing the correct output filename:

   ```sh
   ./scripts/picotool save -a -v "$KEYBALL44_PRIVATE_BACKUP_DIR/left-factory.uf2"
   ./scripts/picotool save -a -v "$KEYBALL44_PRIVATE_BACKUP_DIR/right-factory.uf2"
   ```

   Run only the command for the physically connected half.

6. Confirm the file exists, is non-empty, and has a stable digest:

   ```sh
   sha256sum "$KEYBALL44_PRIVATE_BACKUP_DIR/left-factory.uf2"
   sha256sum "$KEYBALL44_PRIVATE_BACKUP_DIR/right-factory.uf2"
   ```

7. Record `./scripts/picotool info -a` output, the filename, half, file size,
   date, and SHA-256 digest in a private backup manifest.
8. Disconnect USB before moving to the other half or reconnecting TRRS.

Do not proceed if either backup cannot be read, verified, or unambiguously
associated with its half.

## Flash a tested image

1. Disconnect USB and TRRS.
2. Put only the intended half in BOOTSEL mode and connect it by USB.
3. Use `./scripts/picotool info -a` to confirm the device.
4. Verify the UF2 checksum against the build manifest.
5. Load with verification:

   ```sh
   ./scripts/picotool load -v dist/explicitly-named-image.uf2
   ```

6. Disconnect and reconnect USB to boot the application.
7. Test that half as far as possible before disconnecting USB and repeating for
   the other half.
8. Reconnect TRRS only while both halves are unpowered, then connect USB.

Never create a generic command that guesses the half or automatically chooses
the newest UF2 file.

## Restore a factory image

If a custom build behaves unexpectedly:

1. Disconnect USB and TRRS immediately.
2. Put one half in BOOTSEL and connect only that half.
3. Reconfirm the backup filename and its SHA-256 digest.
4. Restore and verify the corresponding factory image:

   ```sh
   ./scripts/picotool load -v "$KEYBALL44_PRIVATE_BACKUP_DIR/left-factory.uf2"
   ./scripts/picotool load -v "$KEYBALL44_PRIVATE_BACKUP_DIR/right-factory.uf2"
   ```

   Run only the command matching the connected half.

5. Power-cycle that half, then repeat the complete procedure for the other
   half.
6. With both halves unpowered, reconnect TRRS and then USB.
7. Reopen Vial and reload the exported layout only if the stored configuration
   was lost or reset.

The RP2040's BOOTSEL loader resides in read-only ROM. A broken keyboard
application normally cannot overwrite it, so BOOTSEL remains the recovery path
for a non-booting application image.
