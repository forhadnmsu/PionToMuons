# A Program to Run Event Display on Simulated Data

This program was separated from `SimChainDev` just now.
Please do not use it since no one has yet confirmed that it runs fine.
A further update is expected in the near future.

## (Anticipated) Usage

```
ssh spinquestgpvm01.fnal.gov
cd /path/to/EventDisplaySim
source setup.sh
root EventDisplay4Sim.C
```

## Note

 - The detector info (i.e. `PHG4Reco`) is configured in the same way as `SimChainDev/Fun4Sim.C`.
   Details should be found there.
 - The event generator is configured in the same way as `SimChainDev/Fun4Sim.C`.
   Details should be found there.
 - Most likely `EventDisplay4Sim.C` will be modified not to generate events but instead read them from `DST.root`, which should be generated via `SimChainDev/Fun4Sim.C`.
