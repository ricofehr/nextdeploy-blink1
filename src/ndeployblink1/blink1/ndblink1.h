/**
 *  @file blink1.h
 *  @brief Blink1 Header File
 *  @author Eric Fehr (ricofehr@nextdeploy.io, github: ricofehr)
 */

#ifndef NDEPLOYBLINK1_BLINK1_NDBLINK1_H_
#define NDEPLOYBLINK1_BLINK1_NDBLINK1_H_

#include <stdio.h>

#include "ndeployblink1/blink1/lib/blink1-lib.h"

/**
 *  Open the blink1 device
 */
void nd_blink1_on();

/**
 *  Send to the blink1 device the color r,g,b
 *  @param r red level
 *  @param g green level
 *  @param b blue level
 */
void nd_blink1_color(uint8_t r, uint8_t g, uint8_t b);

/**
 *  Turn off the blink1 device
 */
void nd_blink1_off();

#endif // NDEPLOYBLINK1_BLINK1_NDBLINK1_H_
