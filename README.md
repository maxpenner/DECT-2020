# DECT-2020
This is a C++ library of the ETSI standard DECT-2020. DECT-2020 New Radio is a Radio Interface Technology (RIT) designed to provide a slim but powerful technology foundation for wireless applications deployed in various use cases and markets.

[DECT Introduction](https://www.etsi.org/technologies/dect)

The standard consists of four parts which can be found on the DECT Technical Committee (TC) page.

[DECT committee page](https://www.etsi.org/committee/1394-dect)

## Structure
- lib/include/dect2020/mac: MAC layer functionality based on sections_part3 and sections_part4
- lib/include/dect2020/phy: PHY functionality based on sections_part3 and sections_part4
- lib/include/dect2020/sections_part3: function of [part 3](https://www.etsi.org/committee/1394-dect)
- lib/include/dect2020/sections_part4: function of [part 4](https://www.etsi.org/committee/1394-dect)
- lib/include/dect2020/utils: utility functions
- lib/test/: various programs to test library

The channel coding of DECT-2020 has similarities to LTE, which is why we utilize [srsRAN](https://github.com/srsRAN/srsRAN). Its functions for channel coding and decoding have been adapted accordingly.

## Results
### Channel Coding
These are results for various transport block sizes (TBS) and up to one HARQ retransmission. Similar results simulated in Matlab can be found [here](https://arxiv.org/abs/2104.08549). 

<p align="center">
  <img src="./pics/PER_HARQ.png" width="700">
</p>
