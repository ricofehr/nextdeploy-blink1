/**
 *  @file ndblink1.c
 *  @brief ND Blink1 Implementation
 *  @author Eric Fehr (ricofehr@nextdeploy.io, github: ricofehr)
 */

#include "ndeployblink1/blink1/ndblink1.h"

static blink1_device* d = NULL;

void nd_blink1_on()
{
    if (d == NULL) {
        d = blink1_open();
        blink1_enableDegamma();
    }
}

void nd_blink1_color(uint8_t r, uint8_t g, uint8_t b)
{
    int rc;

    nd_blink1_on();
    if (d == NULL) {
        printf("Blink1 seems not connected with the computer\n");
        return;
    }

    rc = blink1_fadeToRGB(d, 3000, r,g,b);
    if(rc == -1) {
        printf("Blink1 issues during rgb color\n");
    }
}

void nd_blink1_off()
{
    if (d != NULL) {
        nd_blink1_color(0, 0, 0);
        blink1_close(d);
    }
}
