/**
 *  @file ndsignal.h
 *  @brief Program Signal Header File
 *  @author Eric Fehr (ricofehr@nextdeploy.io, github: ricofehr)
 */

#ifndef NDEPLOYBLINK1_CORE_NDSIGNAL_H_
#define NDEPLOYBLINK1_CORE_NDSIGNAL_H_

/** End the program if setted to 1 */
extern int g_endprog;

/** Init the signal handlers */
void nd_signal_init();

/** The signal handler for ending program */
void nd_signal_end();

#endif // NDEPLOYBLINK1_CORE_NDSIGNAL_H_
