#ifndef LPS331AP_PARAMS_H
#define LPS331AP_PARAMS_H

#include "board.h"
#include "lps331ap.h"
#include "saul_reg.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief   Set default configuration parameters
 * @{
 */
#ifndef LPS331AP_PARAM_I2C
#define LPS331AP_PARAM_I2C              I2C_DEV(0)
#endif
#ifndef LPS331AP_PARAM_ADDR
#define LPS331AP_PARAM_ADDR             (0x44)
#endif
#ifndef LPS331AP_PARAM_RATE
#define LPS331AP_PARAM_RATE             (LPS331AP_RATE_7HZ)
#endif

#define LPS331AP_PARAMS_DEFAULT         { .i2c  = LPS331AP_PARAM_I2C,  \
                                          .addr = LPS331AP_PARAM_ADDR, \
                                          .rate = LPS331AP_PARAM_RATE }
/**@}*/

/**
 * @brief   Allocate some memory to store the actual configuration
 */
static const lps331ap_params_t lps331ap_params[] =
{
#ifdef LPS331AP_PARAMS_CUSTOM
    LPS331AP_PARAMS_CUSTOM,
#else
    LPS331AP_PARAMS_DEFAULT,
#endif
};

/**
 * @brief   Additional meta information to keep in the SAUL registry
 */
static const saul_reg_info_t lps331ap_saul_info[] =
{
    { .name = "lps331ap" }
};

#ifdef __cplusplus
}
#endif

#endif /* LPS331AP_PARAMS_H */
/** @} */
