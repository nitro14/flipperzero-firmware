#include <api-hal-power.h>
#include <main.h>
#include <bq27220.h>
#include <bq25896.h>

void HAL_RCC_CSSCallback(void) {
    LL_RCC_ForceBackupDomainReset();
    LL_RCC_ReleaseBackupDomainReset();
    NVIC_SystemReset();
}

void api_hal_power_init() {
    bq27220_init();
    bq25896_init();
}

uint8_t api_hal_power_get_pct() {
    return bq27220_get_state_of_charge();
}

bool api_hal_power_is_charging() {
    return bq25896_is_charging();
}

void api_hal_power_off() {
    bq25896_poweroff();
}

void api_hal_power_enable_otg() {
    bq25896_enable_otg();
}

void api_hal_power_disable_otg() {
    bq25896_disable_otg();
}

float api_hal_power_get_battery_voltage() {
    return (float)bq27220_get_voltage() / 1000.0f;
}

float api_hal_power_get_battery_current() {
    return (float)bq27220_get_current() / 1000.0f;
}

void api_hal_power_dump_state(string_t buffer) {
    BatteryStatus battery_status;
    OperationStatus operation_status;
    if (bq27220_get_battery_status(&battery_status) == BQ27220_ERROR
        || bq27220_get_operation_status(&operation_status) == BQ27220_ERROR) {
        string_cat_printf(buffer, "Failed to get bq27220 status. Communication error.\r\n");
    } else {
        string_cat_printf(buffer,
           "bq27220: CALMD: %d, SEC0: %d, SEC1: %d, EDV2: %d, VDQ: %d, INITCOMP: %d, SMTH: %d, BTPINT: %d, CFGUPDATE: %d\r\n",
            operation_status.CALMD, operation_status.SEC0, operation_status.SEC1,
            operation_status.EDV2, operation_status.VDQ, operation_status.INITCOMP,
            operation_status.SMTH, operation_status.BTPINT, operation_status.CFGUPDATE
        );
        // Battery status register, part 1
        string_cat_printf(buffer,
           "bq27220: CHGINH: %d, FC: %d, OTD: %d, OTC: %d, SLEEP: %d, OCVFAIL: %d, OCVCOMP: %d, FD: %d\r\n",
            battery_status.CHGINH, battery_status.FC, battery_status.OTD,
            battery_status.OTC, battery_status.SLEEP, battery_status.OCVFAIL,
            battery_status.OCVCOMP, battery_status.FD
        );
        // Battery status register, part 2
        string_cat_printf(buffer,
           "bq27220: DSG: %d, SYSDWN: %d, TDA: %d, BATTPRES: %d, AUTH_GD: %d, OCVGD: %d, TCA: %d, RSVD: %d\r\n",
            battery_status.DSG, battery_status.SYSDWN, battery_status.TDA,
            battery_status.BATTPRES, battery_status.AUTH_GD, battery_status.OCVGD,
            battery_status.TCA, battery_status.RSVD
        );
        // Voltage and current info
        string_cat_printf(buffer,
            "bq27220: Full capacity: %dmAh, Remaining capacity: %dmAh, State of Charge: %d%%\r\n",
            bq27220_get_full_charge_capacity(), bq27220_get_remaining_capacity(),
            bq27220_get_state_of_charge()
        );
        string_cat_printf(buffer,
            "bq27220: Voltage: %dmV, Current: %dmA, Temperature: %dC\r\n",
            bq27220_get_voltage(), bq27220_get_current(), (bq27220_get_temperature() - 2731)/10
        );
    }

    string_cat_printf(buffer,
        "bq25896: VBUS: %d, VSYS: %d, VBAT: %d, Current: %d, NTC: %dm%%\r\n",
        bq25896_get_vbus_voltage(), bq25896_get_vsys_voltage(),
        bq25896_get_vbat_voltage(), bq25896_get_vbat_current(),
        bq25896_get_ntc_mpct()
    );
}