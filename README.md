# Nixie Clock

## Overview

Displays the time fetched via NTP using four Nixie Tubes. Powered by mongoose-os running on an ESP32.

Built using mos tool 2.19.1

## To Build

1. [Download and install mos tool](https://mongoose-os.com/docs/mongoose-os/quickstart/setup.md)
2. Clone this repo.
3. Update you wifi ssid and password in mos.yml (can also be set after flashing using `mos wifi`).
4. Go to project directory and run `mos build`
5. Connect your ESP32 and run `mos flash`

