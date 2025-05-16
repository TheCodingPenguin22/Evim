#ifndef EDITORMOTIONS_H
#define EDITORMOTIONS_H

/*
 *
 * This file is for having all the vim motions is a single place.
 * I don't know if this is the best way to do this but it's the one
 * I though of.
 *
 * The motions work thourgh a buffer.
 * If a user presses a key that has more key assosiated with it,
 * such as d, Eg: dd, dw de. Then it adds that char to the buffer.
 * When the buffer matches a existing combo command then it executes it.
 *
 * Then it is processed in the normal mode input processing function.
 *
 */

void processVimMotionBuffer();
void freeVimMotionBuffer();
void appendToVimMotionBuffer(char c);
void vimMotionO();
void vimMotionDD(int at);
#endif
