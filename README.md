# ble_wheelchair_control

Note the following:
Define: NRF_LOG_USES_RTT=1 *** not UART
Also: 
0> RAM START ADDR 0x20002080 should be adjusted to 0x20001FE8
 0> RAM SIZE should be adjusted to 0x2018 
 0> sd_ble_enable: RAM START at 0x20002080
 0> sd_ble_enable: app_ram_base should be adjusted to 0x20001FE8
 0> ram size should be adjusted to 0x2018
