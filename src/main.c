
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>

#include "ndeployblink1/core/ndconfig.h"
#include "ndeployblink1/core/ndsignal.h"
#include "ndeployblink1/blink1/ndblink1.h"
#include "ndeployblink1/ndeploy/ndapiclient.h"
#include "ndeployblink1/ndeploy/ndmodel.h"

/* Global config defined into ndconfig.h */
struct ndconfig *g_ndconfig;

/* Global exit flag defined into ndsignal.h */
int g_endprog;

int main(int argc, char* argv[])
{
    int nd_status;
    uint8_t r = 0, g = 0, b = 0;

    nd_config_init_config();
    nd_apiclient_auth(g_ndconfig->endpoint, g_ndconfig->username, g_ndconfig->password);
    nd_signal_init();

    while(1) {
        nd_status = nd_apiclient_check_status(g_ndconfig->endpoint);

        r = g = b = 0;
        switch (nd_status) {
            case ND_IDLE: g = 255; break;
            case ND_WORKING: b = 255; break;
            case ND_VM_ERROR: r = 255; break;
            case ND_VM_DOWN: r = 255; g = 153; break;
            case JSON_ERROR: r = 255; g = 255; break;
            default: g = 255; break;
        }

        nd_blink1_color(r, g, b);

        usleep(20 * 1000 * 1000);

        if (g_endprog) {
            break;
        }
    }

    nd_blink1_off();
    nd_config_destroy_global();
}
