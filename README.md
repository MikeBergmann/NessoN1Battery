# Nesso N1 Battery Monitor

A small Arduino sketch for the **Arduino Nesso N1** that shows the battery
status on the built-in display and keeps charging enabled.

It's meant for **storage**: when a Nesso N1 is sitting unused, its LiPo cell
slowly self-discharges and can drop into deep-discharge, which damages the
battery. Flash this sketch, glance at the screen now and then, and plug in USB
to top it up before it gets too low.

## What it shows

- A battery symbol with a fill bar and the charge level in percent
- Battery voltage (V)
- Charge status: `Pre-Charge`, `Charging...`, `Full`, or `Not charging`

The display refreshes every 2 seconds. Charging is enabled on startup, so
connecting USB power will recharge the cell automatically.

## Requirements

- Arduino Nesso N1
- The `Arduino_Nesso_N1` library

## Usage

1. Open `Battery/Battery.ino` in the Arduino IDE.
2. Select the Nesso N1 board.
3. Upload.

The display comes up with a short splash, then switches to the live battery
view. Serial output (115200 baud) logs the level and voltage each cycle.

## Note

This is a minimal utility sketch, not a full battery-management tool. Check on
a stored device periodically and recharge when the level gets low to avoid
deep-discharge.
