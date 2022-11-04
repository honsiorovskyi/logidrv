# logidrv
Simple userspace driver for Logitech mice for Macos (tested on M560).

## !TL;DR

I have a Logitech M560 mouse and like quite a lot. But its middle and side buttons don't work on Macos, and official Logitech tools ~are useless~ don't recognize them either.

The only solution I found to make them work is the amazing [SteerMouse](https://www.plentycom.jp/en/steermouse/) app (huge kudos to the author!), but it's a little bit overpowered for my use case (I just need the damn middle click to work!), it isn't an open source solution and it also has [some other serious flaws](https://en.wikipedia.org/wiki/Not_invented_here).

What should I do? Right, create something myself. So, here it is. A simple ~hacky~ app, based on [this StackOverflow question](https://stackoverflow.com/q/30380400), communicating with the HID layer exposed by the mouse and implementing exactly what's missing.

## Usage

Very simple:

* Install Xcode
* Clone the repo
* Run:
```
make
sudo make install

logidrv 
# or even
tmux new -d logidrv
```

## Device support

I have only Logitech M560. For it, I've been able to drive middle button and both side buttons (rocker wheel already works OOB). In theory, it could support other models, but no idea if it does. For more, see **Plans & Contribution** section below.

## Mappings

Currently, with my mouse I have the following mappings (device -> macos) working:
* Left button -> button 0 (left click) — OOB
* Right button -> button 1 (right click) — OOB
* **Middle button -> button 2 (center click) — this app**
* Wheel scroll -> vertical scroll — OOB
* Wheel rocking -> buttons 3 & 4 — OOB
* **Side buttons -> buttons 5 & 6 — this app**

The app doesn't support any custom actions (e.g. Mission Control etc), the only thing it does is generates standard mouse events that can be mapped to whatever you need them to be mapped to using a third party app (Karabiner, BetterTouchTool, Hammerspoon etc). Unix philosophy, hehe.

## Plans & Contribution

### UI
The app is implemented as a CLI tool and I ~don't have even a slightest desire to touch anything inside Xcode~ would love to keep it this way: as simple as possible and not requiring much maintenance.

If you need UI, feel free to fork it (it's MIT-licensed, easy peasy) and implement it the way it works better for you :) If you make some cool UI, I'd be happy to reference it here and contribute there as much as I can, but please don't create UI-related PRs for this project.

### Everything else
Regarding further device support or any other kind of contribution, I'm happy to help and accept PRs, just keep in mind two things:
1. Let's try to keep the project modular, one device-specific functionality shouldn't hurt others.
2. I probably won't be able to test and maintain support for other devices, so be ready to jump in if something will be needed for them in the future.
