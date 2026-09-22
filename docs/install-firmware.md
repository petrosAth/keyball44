# Install firmware

Before installing firmware, export the Vial layout and make a verified
full-flash backup of each half by following the [backup procedure](recovery.md).
Use only a tested KeebNERD Keyball44 image whose provenance, target, and
checksum are known. Never use an image for Holykeebs, a generic Keyball44, or
other hardware.

Never insert or remove TRRS while USB is connected to either half. Flash only
one half at a time, with TRRS disconnected, and use an explicitly named image.

1. Disconnect USB and TRRS.
2. Connect only the intended half by USB and put it in BOOTSEL mode.
3. Use `./scripts/picotool info -a` to confirm the device.
4. Verify the UF2 checksum against the build manifest. The UF2 and
   `SHA256SUMS` from the same release must be in the current directory:

   ```sh
   sha256sum -c SHA256SUMS
   ```

5. Load with verification, replacing the example with the explicit filename
   of the verified UF2:

   ```sh
   ./scripts/picotool load -v explicitly-named-image.uf2
   ```

6. Disconnect and reconnect USB to boot the application.
7. Test that half as far as possible before disconnecting USB and repeating for
   the other half.
8. Reconnect TRRS only while both halves are unpowered, then connect USB.

Never create a generic command that guesses the half or automatically chooses
the newest UF2 file.
